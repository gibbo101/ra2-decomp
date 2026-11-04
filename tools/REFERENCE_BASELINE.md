To protect resolved relocation sites, run the full gate in a trusted checkout with `RA2_SAVE_REFERENCES=/absolute/path/base.json` (a new file). Run the contribution gate with `RA2_REQUIRE_REFERENCES=/absolute/path/base.json` using that same file. A previously resolved field must still resolve to the same target at the same function offset, with the same relocation type and addend. Existing unknowns can become verified. Capture and verification also check that the retail executable agrees.

Capture the baseline with a plain verification after recording progress.
