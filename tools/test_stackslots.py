import unittest
from unittest import mock

import stackslots
from stackslots import decode, slots, listing_names


class StackTests(unittest.TestCase):
    def test_different_reservations_same_entry_slot(self):
        for size in (8, 12):
            ins = decode(bytes([0x83, 0xec, size, 0x8b, 0x44, 0x24, size, 0xc3]), 0x401000)
            self.assertEqual(slots(ins, lambda ins: None)[0x401003], ("entry", 0))

    def test_join_with_conflicting_depth_is_unknown(self):
        ins = decode(bytes.fromhex("7401 50 8b442404 c3"), 0x401000)
        self.assertIsNone(slots(ins, lambda ins: None)[0x401003])

    def test_names_on_wrapped_listing_rows(self):
        text = "sym PROC NEAR\n  00006\t81 ec 8c 00 00\n\t00  mov eax, _pitch$23[esp+12]\nsym ENDP\n"
        self.assertEqual(listing_names(text, "sym"), {6: "pitch"})

    def test_listing_reads_no_source_files(self):
        # A source-annotated listing opens every file a #line names, which src/ cannot provide.
        with mock.patch.object(stackslots.check, "Original"), \
                mock.patch.object(stackslots, "CoffObject"), \
                mock.patch.object(stackslots.check, "compile_source", return_value=(None, "stop")) as compile_source:
            with self.assertRaises(ValueError):
                stackslots.diagnose("a.cpp", 0x401000, None, b"")
        listing_flags = [f for f in compile_source.call_args.kwargs["extra"] if f.startswith("/FA")]
        self.assertTrue(listing_flags)
        self.assertTrue(all("s" not in f[3:] for f in listing_flags))
