import struct
from types import SimpleNamespace
import unittest

from rtti import Rtti


class Image:
    base = 0x400000

    def __init__(self):
        self.image = bytearray(0x1000)
        self.sizes = {}
        self.pe = SimpleNamespace(sections=[
            SimpleNamespace(Name=b".text", VirtualAddress=0x100, Misc_VirtualSize=0x100),
            SimpleNamespace(Name=b".rdata", VirtualAddress=0x300, Misc_VirtualSize=0x500),
        ])
        self.write(0x340, 0, 0, 0, self.base + 0x900, self.base + 0x380)
        self.write(0x380, 0, 0, 1, self.base + 0x3A0)
        self.write(0x3A0, self.base + 0x3C0)
        self.write(0x3C0, self.base + 0x900, 0, 0, 0, 0, 0)
        self.image[0x908:0x913] = b".?AVTest@@\0"
        self.write(0x3FC, self.base + 0x340, self.base + 0x100, 0)

    def write(self, offset, *words):
        struct.pack_into("<" + "I" * len(words), self.image, offset, *words)

    def u32(self, address):
        return struct.unpack_from("<I", self.image, address - self.base)[0]


class RttiBoundsTests(unittest.TestCase):
    def test_valid_hierarchy_preserves_virtual_ownership(self):
        image = Image()
        rtti = Rtti(image, None)
        self.assertEqual(rtti.owner(image.base + 0x100), ".?AVTest@@")
        self.assertEqual(rtti.unique_primary_tables, {"Test@@": image.base + 0x400})
        self.assertEqual(rtti.class_of("?Run@Test@@UAEXXZ"), (".?AVTest@@", "U"))

    def test_unreferenced_null_hierarchy_lookalike_is_ignored(self):
        image = Image()
        image.write(0x500, 0, 0, 0, image.base + 0x900, 0)
        rtti = Rtti(image, None)
        self.assertNotIn(image.base + 0x500, rtti.cols)
        self.assertEqual(len(rtti.vtables), 1)

    def test_referenced_null_hierarchy_fails(self):
        image = Image()
        image.write(0x350, 0)
        with self.assertRaisesRegex(ValueError, "Invalid RTTI locator"):
            Rtti(image, None)

    def test_truncated_hierarchy_fails(self):
        image = Image()
        image.write(0x350, image.base + len(image.image) - 12)
        with self.assertRaisesRegex(ValueError, "outside the image"):
            Rtti(image, None)

    def test_base_array_must_be_fully_readable(self):
        image = Image()
        image.write(0x388, 2, image.base + len(image.image) - 4)
        with self.assertRaisesRegex(ValueError, "outside the image"):
            Rtti(image, None)

    def test_base_descriptor_must_be_fully_readable(self):
        image = Image()
        image.write(0x3A0, image.base + len(image.image) - 20)
        with self.assertRaisesRegex(ValueError, "outside the image"):
            Rtti(image, None)

    def test_unknown_type_descriptor_fails(self):
        image = Image()
        image.write(0x3C0, image.base + 0xA00)
        with self.assertRaisesRegex(ValueError, "Unknown RTTI type"):
            Rtti(image, None)

    def test_unterminated_live_type_name_fails(self):
        image = Image()
        name = b".?AVTest@@"
        image.image[0x908:] = name + b"A" * (len(image.image) - 0x908 - len(name))
        with self.assertRaisesRegex(ValueError, "Unterminated RTTI type"):
            Rtti(image, None)

    def test_missing_vtables_fails_instead_of_disabling_class_checks(self):
        image = Image()
        image.write(0x3FC, 0, 0)
        with self.assertRaisesRegex(ValueError, "No valid RTTI vtable slots"):
            Rtti(image, None)

    def test_invalid_base_counts_fail_without_partial_acceptance(self):
        for count in (0, 257, 0xFFFFFFFF):
            with self.subTest(count=count):
                image = Image()
                image.write(0x388, count)
                with self.assertRaisesRegex(ValueError, "Invalid RTTI base count"):
                    Rtti(image, None)

    def test_descriptor_at_exact_image_boundary_is_valid(self):
        image = Image()
        image.write(0x3A0, image.base + len(image.image) - 24)
        image.write(len(image.image) - 24, image.base + 0x900, 0, 0, 0, 0, 0)
        self.assertEqual(len(Rtti(image, None).vtables), 1)

    def test_vtable_at_rdata_boundary_is_valid(self):
        image = Image()
        image.write(0x3FC, 0, 0)
        image.write(0x7F8, image.base + 0x340, image.base + 0x100)
        rtti = Rtti(image, None)
        self.assertEqual(rtti.vtables[image.base + 0x7FC][2], [image.base + 0x100])


if __name__ == "__main__":
    unittest.main()
