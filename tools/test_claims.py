import unittest

from coord.claims import issue_claims

STARTS = [0x00401000, 0x00402000, 0x00403000, 0x00404000]


def comment(login, when, body):
    return {"author": {"login": login}, "createdAt": when, "body": body}


class IssueClaimsTests(unittest.TestCase):
    def claims(self, comments, body=""):
        return issue_claims({"body": body, "comments": comments}, "owner", STARTS)

    def test_contributor_ranges_and_body_claim(self):
        found = self.claims([comment("dk", "2026-11-02T00:00:00Z", "Claiming 0x00401000-0x00402FFF")],
                            body="Batches running: 0x00404000")
        self.assertEqual(found, {0x00401000, 0x00402000, 0x00404000})

    def test_maintainer_comment_never_claims(self):
        found = self.claims([comment("owner", "2026-11-09T00:00:00Z", "Heads-up on 0x00403000")])
        self.assertEqual(found, set())

    def test_maintainer_heads_up_does_not_release(self):
        found = self.claims([comment("dk", "2026-11-02T00:00:00Z", "Claiming 0x00401000"),
                             comment("owner", "2026-11-09T00:00:00Z", "A twin for 0x00401000")])
        self.assertEqual(found, {0x00401000})

    def test_take_back_releases_only_earlier_claims_it_lists(self):
        found = self.claims([
            comment("owner", "2026-11-10T00:00:00Z", "We're taking back: 0x00401000, 0x00402000"),
            comment("dk", "2026-11-02T00:00:00Z", "Claiming 0x00401000-0x00403FFF"),
            comment("dk", "2026-11-11T00:00:00Z", "Claiming 0x00402000 again"),
        ])
        self.assertEqual(found, {0x00402000, 0x00403000})


if __name__ == "__main__":
    unittest.main()
