# Banked forms

Best forms recorded for functions that do not match yet, or that match only through a form the project rules do not accept (held). None of these files is landable as it stands; each one's first line gives its score and what it still needs, and `data/attempts.csv` has the full history per address.

Files named `ADDR_*.cpp` are function bodies or whole source files to splice into `src/`; `.h`/`.hh` files are header overlays that the form needs. Score lines describe the tree they were measured on, so re-check against current `main` before relying on them.

| file | first line |
|---|---|
| `0x00443C60.cpp` | // best 99.8% raw (6759/6759 bytes), no overlay; the Fable stop partial unchanged (needs #include "airctype.h" after temporal.h, renumbering _$E298 to _$E300; s |
| `0x00443C60_exit.cpp` | // best 99.8% raw, 99.8% normalised (6759/6759 bytes), no overlay; needs #include "airctype.h" after temporal.h (it renumbers _$E298 to _$E300); splice before 0 |
| `0x00443C60_gauge.cpp` | // 0x00443C60 99.8% raw / 99.8% normalised (6759/6759, 3 differing lines: add ecx,edx at 0x00444E42), gauge5 27 Nov: the 0x00443C60.cpp form unchanged (needs #i |
| `0x00443C60_order.cpp` | // 0x00443C60 99.8% (6759/6759), natural (the refinery statement is TibSun's), no overlay; needs #include "airctype.h" after temporal.h (renumber _$E298 to _$E3 |
| `0x00443C60_techno.cpp` | // best 99.8% raw, 99.8% normalised (6759/6759 bytes), no overlay; needs #include "airctype.h" after temporal.h (it renumbers _$E298 to _$E300); splice before 0 |
| `0x00481180.cpp` | // best 70.2% (764/795 bytes), no overlay; insert after 0x00481130 in src/cell.cpp. Only diff: TPoint2D::Length inside the inlined Spot_Index stays a call (inli |
| `0x004BC750.cpp` | // best 94.2% raw / 98.8% normalised (6070/6064 B), no overlay; splice before 0x004BDF00 in src/dsurface.cpp; slots, loops A/B/D match (loop D's lidx/oidx produ |
| `0x004BC750_gauge.cpp` | // best 94.2% raw / 98.8% normalised (6070/6064 B, 40 diff lines), no overlay; splice before 0x004BDF00 in src/dsurface.cpp; source identical to 0x004BC750.cpp. |
| `0x004BC750_n77.cpp` | // 94.3% raw / 98.9% normalised (6054/6064 B, 37 diff lines), no overlay; splice before 0x004BDF00 in src/dsurface.cpp. 0x004BC750.cpp with loop C's z += zwrap  |
| `0x00547CF0.cpp` | // best 38.8% raw / 72.3% normalised (6896/6967 B), no overlay; splice after the _$E214 marker before 0x00549830 in src/isotype.cpp; start columns clamp with ma |
| `0x00547CF0_n75.cpp` | // best 15.1% raw / 35.3% normalised (6993/6967 B), no overlay; independent fresh write from the exe and TibSun (row-local zrow/destrow/arow, early returns with |
| `0x00547CF0_n77.cpp` | // best 39.0% raw / 72.8% normalised (6898/6967 B, 261 diff lines), no overlay; splice after the _$E214 marker before 0x00549830 in src/isotype.cpp. 0x00547CF0. |
| `0x00565C10.cpp` | // 99.7% raw, 99.6% normalised (5374/5374) with overlay 0x00565C10_coord.h (coord.h plus Cell::operator+= forwarding to BASECLASS::operator+=, point.h's TPoint3 |
| `0x00565C10_coord.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x00565C10_gauge.cpp` | // 99.7% raw, 99.6% normalised (5374/5374), 12 differing lines (occupy-loop list->X in dx, exe cx); body identical to 0x00565C10_robin.cpp. Gauge: localprobe +0 |
| `0x00565C10_robin.cpp` | // 99.7% (5374/5374), no header change: Robin's static inline Shift_Tube (ra2-1001-decomp src/m85/src/map.cpp:1155, name, parameters and body as written, placed |
| `0x00565C10_shift_tube.cpp` | // 99.7% (5374/5374) with no header change: the tube shift is a file-local two-statement inline (Shift_Tube), which leaves both TPoint2D<short>::operator+= call |
| `0x00565C10_tube.cpp` | // 99.7% raw, 99.6% normalised (5374/5374) with the overlay 0x00565C10_tube.h (tube.h plus an inline TubeClass::Shift holding both +=; the member name is a gues |
| `0x00565C10_tube.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x00565C10_weight.patch` | --- a/include/coord.h |
| `0x0056D100.cpp` | // best natural 93.5% (290/290 B), no overlay; splice after Zone_Span (0x0056CB90, ends before 0x0056D3A0) in src/map.cpp; rechecked 24 Nov. Same bytes as 0x005 |
| `0x0056D100_gauge.cpp` | // 93.5% raw, 94.3% normalised (290/290), 16 differing lines (tail registers: exe this=esi after push of to, mzone=edi, to-zone=ebp); same body as 0x0056D100.cp |
| `0x0056D100_grm.cpp` | // best 93.5% (290/290 bytes), no overlay; splice before 0x0056D230 in src/map.cpp; tail registers differ (exe this=esi loaded after the push of to, mzone=edi,  |
| `0x0056D100_ra2b.cpp` | // best 93.5% (290/290 bytes), no overlay; splice before 0x0056D230 in src/map.cpp. Only the tail registers differ (exe: this esi loaded after the push of to, m |
| `0x0056D100_regs.cpp` | // best natural 93.5% (290/290 B), no overlay; same source as 0x0056D100_grm.cpp (splice before 0x0056D230 in src/map.cpp). Identical bytes under the RTM C2 (81 |
| `0x0058D620.cpp` | // 0x0058D620 76.7% raw, 95.7% normalised (4004/4012), no overlay beyond the header line below; every inlining decision matches the exe and the other 306 functi |
| `0x0058D620_mapgen.cpp` | // 0x0058D620 natural best 70.9% raw, 88.7% normalised (4061/4012), no overlay; insert before 0x0058E5D0, replace Increase_Height 0x0058D5A0 with the body below |
| `0x0058D620_mapgen2.cpp` | // 0x0058D620 natural best 70.9% raw (4061/4012), no overlay; replace Increase_Height 0x0058D5A0 with these two functions and shift every _$E marker from 206 up |
| `0x0058D620_mapnm.cpp` | // 0x0058D620 64.2% raw, 90.8% normalised (4006/4012), no overlay; same splice as 0x0058D620_mapgen.cpp (replace Increase_Height 0x0058D5A0 with the two functio |
| `0x005981F0.cpp` | // 0x005981F0 best 65.4% raw, 92.1% normalised (1904/1889) WITH the exe's file.Close(), no overlay; insert before 0x00599530 (the form left in fable4). Same inl |
| `0x005981F0_mapgen.cpp` | // 0x005981F0 best 33.4% raw, 47.7% normalised (1829/1889) with the exe's file.Close(), no overlay; insert before 0x00599530. Unchanged from build/batch/near_ge |
| `0x005981F0_mapgen2.cpp` | // 0x005981F0 natural best 33.4% raw (1829/1889) with the exe's file.Close(), no overlay; insert before 0x00599530; unchanged from 0x005981F0_mapgen.cpp. |
| `0x005981F0_n75.cpp` | // 0x005981F0 n75 fresh form 34.6% raw, 50.9% normalised (1854/1889), with file.Close(); does not beat 0x005981F0.cpp (65.4/92.1). Decisions: op= called at list |
| `0x00598960.cpp` | // 0x00598960 94.2% (3042/3018 bytes), Cleanup 0x00599530 and ??_GMapRegionClass 0x005AC290 MATCH, mapgen.cpp 233 MATCH as before. Splice over src's Generate_Ra |
| `0x00598960_diag.cpp` | // DIAGNOSTIC, not source: 0x00598960 Generate_Random_Map MATCH (3018/3018) and 0x005AC290 MATCH, but Cleanup 0x00599530 falls to 60.4% (230/276: its own copy s |
| `0x00598960_grm.cpp` | // best 93.6% (3088/3018 bytes), no overlay; splice over Generate_Random_Map and Cleanup in src/mapgen.cpp; Cleanup 0x00599530 and 0x005AC290 MATCH beside it. |
| `0x00598960_mapgen1.cpp` | // 0x00598960 lead 61.4% raw (3018/3018 bytes, norm 97.8%); best raw stays 0x00598960_robin2.cpp (93.6%). Splice over src's Generate_Random_Map..Cleanup (helper |
| `0x00598960_mapgen2.cpp` | // 0x00598960 natural best 93.6% (3088/3018), no overlay; same form as 0x00598960_grm.cpp; splice over Generate_Random_Map and Cleanup in src/mapgen.cpp (Cleanu |
| `0x00598960_mapgen2_diag.cpp` | // 0x00598960 DIAGNOSTIC, not landable: MATCH (3018/3018) with Cleanup 0x00599530, 0x005AC290 and the whole file (307/307) matching; two dead calls of a 34-term |
| `0x00598960_ra2a.cpp` | // 0x00598960 natural best 93.6% (3088/3018), Cleanup 0x00599530 and 0x005AC290 MATCH; the middle region teardown is MapRegionClass::Destroy_Cell_Regions() inst |
| `0x00598960_ra2a_diag.cpp` | // 0x00598960 DIAGNOSTIC, not landable: MATCH (3018/3018) with Cleanup, 0x005AC290 and the whole file 307/307; one dead 40-term inline call in Cleanup before De |
| `0x00598960_robin2.cpp` | // natural best 93.6% (3088/3018), same body as _spend.cpp (splice over src's Generate_Random_Map..Cleanup; Cleanup 0x00599530 MATCH). inlinebudget on it: only  |
| `0x00598960_spend.cpp` | // 0x00598960 natural best 93.6% (3088/3018), Cleanup 0x00599530 and 0x005AC290 MATCH, whole mapgen.cpp 306/307; _ra2a.cpp with own-size-only respellings. Splic |
| `0x0060A5B0.cpp` | // best natural 84.0% raw / 93.4% normalised (1140/1184 bytes) on main 0f9483d; whole ownrdraw.cpp, every other function MATCH (TrackBarHandler included, GetKey |
| `0x0060A5B0_cast724.cpp` | // natural 31.3% raw / 72.4% normalised (1128/1184) on main 0e48e42; whole ownrdraw.cpp, every other function MATCH except TrackBarHandler 0x0061D950 (99.2, dec |
| `0x0060A5B0_n77.cpp` | // best natural 84.0% raw / 93.4% normalised (1140/1184), 54 diff lines, n77 twin sweep 27 Nov: whole ownrdraw.cpp = the 0x0060A5B0.cpp bank with 'extern Dynami |
| `0x0060A5B0_od.cpp` | // best natural 36.2% raw / 52.4% normalised (864/1184 bytes) on 23 Nov src (main 6a3d9df); every other ownrdraw function MATCH with it. Raw-score best is still |
| `0x0060A5B0_odh.cpp` | // best natural 45.4% (890/1184) on 23 Nov src (after 0x00612B70 landed); every other ownrdraw function MATCH with it. Inline budget is the blocker (tools/inlin |
| `0x0060A5B0_odh2.cpp` | // best natural 31.5% raw / 57.9% normalised (1159/1184 bytes) on 23 Nov src (main 1ea946b); register-masked diff 76 lines (old _odh bank: 228). Every other own |
| `0x0060A5B0_s441.cpp` | // best 44.1% (917/1184) with src changes: Find_Data moved above Typed_Char_Sound; Typed_Char_Sound and Is_Typed_Control use Find_Data (both MATCH); Is_Typed_Co |
| `0x0060A5B0_s728.cpp` | // best natural 44.1% raw / 72.8% normalised (1100/1184 bytes) on main 5f49d41, ShapeSet + Get_Count (today's WinData); whole module file. Previous whole-file b |
| `0x0060A5B0_shp724.cpp` | // best natural 31.3% raw / 72.4% normalised (1128/1184 bytes) on main 56eb922; whole module file, every other ownrdraw function (249) MATCH with it. Previous 4 |
| `0x0060B950.cpp` | // 0x0060B950 99.8% (1895/1895), no overlay, re-scored 23 Nov on main: only the w/old_w compare before MoveWindow differs (exe mov eax,[w]; mov ecx,[old_w]; cmp |
| `0x0060B950_close.cpp` | // 0x0060B950 99.8% (1895/1895), no overlay, re-scored 23 Nov: body unchanged from 0x0060B950_order.cpp; replace the FUN_0060B950 forward declaration in src/own |
| `0x0060B950_gauge.cpp` | // 0x0060B950 99.8% raw / 99.6% normalised (1895/1895 B), 4 diff lines, gauge 27 Nov: no window (flipsweep local 0-63, expr 0-40, call 0-40 one diff set; declpr |
| `0x0060B950_n77.cpp` | // 0x0060B950 99.8% raw / 99.6% normalised (1895/1895), 4 diff lines, n77 twin sweep 27 Nov: body identical to 0x0060B950.cpp, re-scored on today's src (splice  |
| `0x0060B950_odraw.cpp` | // best 99.8% (1895/1895 bytes, 99.6% normalised), no overlay, re-scored 22 Nov on the flattened WinData (branch n22-odraw): only the w/old_w compare before Mov |
| `0x0060B950_order.cpp` | // 0x0060B950 99.8% (1895/1895), no overlay, re-scored 23 Nov on main: only the w/old_w compare before MoveWindow (exe mov eax,[w]; mov ecx,[old_w]; cmp eax,ecx |
| `0x00610CA0.cpp` | // best 54.7% (7058/7265 bytes), 25 Nov src; splice in place of the file-level ActiveWindows/ProcessedMessages statics and their SYNTHETIC markers (before 0x006 |
| `0x00610CA0_n75.cpp` | // best 54.7% raw (54.746, 7235/7265 bytes; normalised 81.5), 26 Nov src; beats the 54.690 bank on raw only (bank 91.4 normalised). Splice as the bank does, wit |
| `0x00610CA0_n77.cpp` | // best 54.7% raw / 91.4% normalised (7069/7265 on today's src), 231 diff lines, n77 twin sweep 27 Nov: body identical to 0x00610CA0.cpp (splice the same way: l |
| `0x00610CA0_odh.cpp` | // best 48.4% (7216/7265), normalised 78.1%, on 23 Nov src (after 0x00612B70 landed); no overlay; tools/inlinebudget.py reports no differing inlining decisions  |
| `0x00617250.cpp` | // best 99.0% (6895/6895 bytes), normalised 98.9%, 50 changed full-context lines, on 25 Nov src (fable4); no overlay; every other ownrdraw function stays MATCH. |
| `0x00617250_n77.cpp` | // best 99.0% raw / 98.9% normalised (6895/6895), 48 diff lines, n77 twin sweep 27 Nov: body identical to 0x00617250.cpp (splice as that bank says; 255 others M |
| `0x00617250_odh.cpp` | // best 65.6% (6923/6895), normalised 84.0%, on 23 Nov src (after 0x00612B70 landed); no overlay. tools/inlinebudget.py now reports no differing inlining decisi |
| `0x00618D40.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler: 57.0% raw, 72.9% normalised, 14666/14666 bytes, frame 0x2D2C (22 Nov). |
| `0x00618D40_c1h.cpp` | // 0x00618D40 ListBoxHandler, 52.6% raw, 14478/14666 (26 Nov, c1h): c1 (cell->surf read at each use, both char * text locals kept) plus OD_SUBCLASSED's item hei |
| `0x00618D40_c1l2.cpp` | // 0x00618D40 ListBoxHandler, 52.4% raw, 14478/14666 (26 Nov, c1l2): w1 with the SURFACE cell read as cell->surf at each use (the exe re-reads [cell+0x10] three |
| `0x00618D40_lbh.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler: 52.9% raw, 73.8% normalised, 14562/14666 bytes, frame 0x2D2C (23 Nov, natural form; file checks 253/254 with the five m |
| `0x00618D40_lbh2.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler: 49.0% raw, 72.3% normalised, 14558/14666 bytes (23 Nov, lbh2: Find_Data prologue plus every evidence-backed Win32 spell |
| `0x00618D40_lbh_diag.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler DIAGNOSTIC, never landable: 49.0% raw, 75.2% normalised, 14778/14666 bytes, frame 0x2D30 (23 Nov; file checks 253/254 wi |
| `0x00618D40_n77.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler: 58.4% raw, 80.0% normalised, 14718/14666 bytes, 237 diff lines (27 Nov, n77 twin sweep; from 57.2/79.4/14746): the 0x00 |
| `0x00618D40_odraw.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler: 57.0% raw, 73.1% normalised, 14666/14666 bytes, frame 0x2D2C, re-scored 22 Nov on the flattened WinData (branch n22-odr |
| `0x0064BDD0.cpp` | // 0x0064BDD0 99.8% raw / 99.8% normalised (1455/1455 B), no overlay; splice after 0x0064B660 (Add_Compressed_Events) in src/queue.cpp and rename FUN_0064BDD0 t |
| `0x0064BDD0_gauge.cpp` | // 0x0064BDD0 99.8% raw / 99.8% normalised (1455/1455 B), 2 diff lines, gauge 27 Nov: no window (localprobe/flipsweep local 0-63, expr 0-40, call 0-40 one diff  |
| `0x0064BDD0_mixed.cpp` | // 0x0064BDD0 36.0% raw / 52.2% normalised (1450/1455 B), no overlay; 0x0064BDD0_plan_exit.cpp plus a named type local (unsigned char type = event->Type) that g |
| `0x0064BDD0_n77.cpp` | // 0x0064BDD0 99.8% raw / 99.8% normalised (1455/1455 B), 2 diff lines, n77 twin sweep 27 Nov: body identical to 0x0064BDD0.cpp (splice after 0x0064B660, rename |
| `0x0064BDD0_plan.cpp` | // 0x0064BDD0 best 48.4% raw / 64.8% normalised (1448/1455 B), no overlay; splice after 0x0064B660 in src/queue.cpp and rename FUN_0064BDD0 (declaration and cal |
| `0x0064BDD0_plan_exit.cpp` | // 0x0064BDD0 29.4% raw / 50.5% normalised (1450/1455 B), no overlay; same splice as 0x0064BDD0_plan.cpp. Leaving the loop when nothing is left over (exe 0x0064 |
| `0x006D3D10.cpp` | // best 58.5% raw, 84.7% normalised (3557/3643 B), no overlay; insert after Render_Terrain (0x006D3AC0) in src/tactical.cpp. Every other function in tactical.cp |
| `0x006D3D10_n75.cpp` | // 60.3% raw, 93.0% normalised (3645/3643 B, frame 0xFC), 75 differing normalised lines against the main bank's 102; same header needs and $E renumbering as 0x0 |
| `0x006D3D10_n77.cpp` | // best 73.1% raw, 93.5% normalised (3613/3643 B, 182 diff lines), frame 0xF4; same header needs and $E renumbering as 0x006D3D10.cpp (insert after Render_Terra |
| `0x006D3D10_tact.cpp` | // best 31.6% raw, 55.5% normalised (3597/3643 B), no overlay; append at the end of src/tactical.cpp. Unchanged from the near_misc bank, re-checked: all 58 call |
| `0x006D7F20.cpp` | // best 98.0% raw (824/824 B), natural, no probes or overlay; splice before Set_Ground_Position (above 0x006D8260) in src/tactical.cpp. Budget fixed by 'Coord c |
| `0x006D7F20_gauge.cpp` | // best 98.0% raw/normalised (824/824 B, 10 diff lines), natural, no overlay; = 0x006D7F20_n77.cpp, splice before Set_Ground_Position (above 0x006D8260) in src/ |
| `0x006D7F20_n77.cpp` | // best 98.0% raw/normalised (824/824 B, 10 diff lines), natural, no overlay; splice before 0x006D8260 in src/tactical.cpp. 0x006D7F20.cpp with the twins' head  |
| `0x006D7F20_tact.cpp` | // best 65.0% raw, 76.8% normalised (809/824 B), no overlay; splice before Set_Ground_Position (above 0x006D8260) in src/tactical.cpp. Site 1 now matches (pixel |
| `0x0075AEC0.cpp` | // 86.6% raw, 96.6% normalised (4977/4988), no overlay, no helper; full copy of src/walk.cpp. Mech's matched layout (one IsNewNavCom store at the end, guard ret |
| `0x0075AEC0_gauge.cpp` | // 86.6% raw, 96.6% normalised (4977/4988, 114 diff lines), no overlay; full copy of src/walk.cpp, = 0x0075AEC0.cpp. Gauge: +0..4 one code, +5 another (84.0, ct |
| `0x0075AEC0_mech_inline.cpp` | // 86.1% raw, 97.5% normalised (4968/4988), no overlay, no helper; full copy of src/walk.cpp. 0x0075AEC0.cpp with Coord(Cell(tube_ptr->Exit), 0) for HeadToCoord |
| `0x0075AEC0_n77.cpp` | // 86.6% raw, 96.6% normalised (4977/4988, 114 diff lines), no overlay; full copy of src/walk.cpp, unchanged from 0x0075AEC0.cpp: no twin difference beat it (Ro |
| `0x0075AEC0_returns.cpp` | // 83.0% raw, 93.7% normalised (4936/4988), no overlay; full copy of src/walk.cpp. Variant of 0x0075AEC0_tube.cpp (83.3% raw, 93.8% normalised, still the best b |
| `0x0075AEC0_tube.cpp` | // 83.3% raw, 93.8% normalised (4932/4988), no overlay once tube.h has Get_Exit and Get_Count; full copy of src/walk.cpp. 0x0075AEC0.cpp with the tube height st |
| `0x00789B60.cpp` | // 0x00789B60 99.8% raw (2312/2312 bytes, ctor 0x007AC610 now called as the exe does; only lea ebx,[edx+edi] left), agent-789b60 24 Nov. Base: today's main wonl |
| `0x00789B60_g27.cpp` | // 0x00789B60 99.8% raw (2312/2312, only lea ebx,[edx+edi] left), agent-789B60o 26 Nov: whole src/wonline.cpp = the 0x00789B60.cpp bank spliced at its address ( |
| `0x00789B60_gauge.cpp` | // 0x00789B60 99.8% raw / 98.6% normalised (2312/2312, 1 differing line: lea ebx,[edx+edi]), gauge5 27 Nov: whole src/wonline.cpp, identical to the 0x00789B60_n |
| `0x00789B60_n10.cpp` | // 0x00789B60 61.9% raw / 98.3% normalised natural (2321/2312), n10 agent 23 Nov. Base: today's main wonline.cpp with (a) the stand-in trio FUN_007EF720/FUN_007 |
| `0x00789B60_n77.cpp` | // 0x00789B60 99.8% raw / 98.6% normalised (2312/2312, only lea ebx,[edx+edi] left), n77 twin sweep 27 Nov: whole src/wonline.cpp; every other wonline function  |
| `0x00789B60_order.cpp` | // 0x00789B60 61.9% natural on 23 Nov main (2321/2312): replace FUN_00789B60 in src/wonline.cpp with this (marker added, FUN_0077D720(user) called as the exe in |
| `0x00789B60_wol.cpp` | // 0x00789B60 FUN_00789B60 (create game): 99.8% (2312/2312 bytes), whole src/wonline.cpp, every other function MATCH (469). One instruction differs: lea ebx,[ed |
| `0x00789B60_wol2.cpp` | // 0x00789B60 FUN_00789B60 (create game): 99.8% (2312/2312 bytes), whole src/wonline.cpp = the n24-wol2 worktree src (stand-in trio unified, all 469 others MATC |
| `0x00789B60_wolcls.cpp` | // 0x00789B60 61.9% raw (2321/2312), wolcls agent 23 Nov. Base: the n32-wolcls package (stand-in trio unified as SimpleWonlineDialogControl at the top, #include |
| `0x00789B60_wolib.cpp` | // 0x00789B60 61.9% natural (2321/2312), unchanged from 0x00789B60_order.cpp; splice as that bank says. Two independent blockers. (1) inlinebudget: exe calls Wo |
| `0x007BAB90.cpp` | // best natural 97.2% raw (511/511 B), no overlay; insert after 0x007BAB80 in src/xsurface.cpp. Two stores sit in the wrong block: the exe stores x before the s |
| `0x007BAB90_gauge.cpp` | // best natural 97.2% raw (511/511 B, 8 diff lines), no overlay; insert after 0x007BAB80 in src/xsurface.cpp; source identical to 0x007BAB90.cpp. Gauge flat: lo |
| `0x007BAB90_late.cpp` | // 91.4% raw (513/511 B), no overlay; insert after 0x007BAB80 in src/xsurface.cpp. Second family: every store in the exe's block and every slot right; the one d |
| `0x007BAB90_n77.cpp` | // best natural 97.2% raw / 97.8% normalised (511/511 B, 8 diff lines), no overlay; insert after 0x007BAB80 in src/xsurface.cpp. 0x007BAB90.cpp with pitch decla |
| `0x007BAB90_near.cpp` | // best natural 87.4% raw, 94.1% normalised (507/511 B), no overlay; insert after 0x007BAB80 in src/xsurface.cpp. The exe keeps the doubled dy in esi (home [esp |
| `0x007BAB90_regs.cpp` | // best natural 87.4% raw, 94.1% normalised (507/511 B), no overlay; same source as 0x007BAB90_ties.cpp (insert after 0x007BAB80). Identical under RTM C2 / 8168 |
| `0x007BAB90_team.cpp` | // best 87.4% raw, 94.1% normalised (507/511 bytes), no overlay; insert after 0x007BAB80 in src/xsurface.cpp; the exe doubles dx after abs() and gives the non-s |
| `0x007BAB90_ties.cpp` | // best natural 87.4% raw, 94.1% normalised (507/511 B), no overlay; same source as 0x007BAB90_near.cpp (insert after 0x007BAB80). 23 Nov: flat over declprobe 0 |
| `0x007BB350.cpp` | // best natural 39.6% (1474/1478 B), masked 244/483, frame 0x38, no overlay; insert before Draw_Circle in src/xsurface.cpp. From W1 (38.0%, kept as 0x007BB350_3 |
| `0x007BB350_374.cpp` | // best natural 37.4% (1472/1478 B), fine 17675 (was 18350), normalised 44.6%, no overlay; frame 0x38; insert after Fill_Rect_Trans in src/xsurface.cpp. Changes |
| `0x007BB350_380.cpp` | // best natural 38.0% (1474/1478 B), masked 244/483, frame 0x38, no overlay; insert before Draw_Circle in src/xsurface.cpp. From the 37.4% bank (kept as 0x007BB |
| `0x007BB350_n75.cpp` | // 39.6% (1474/1478 B, normalised 44.8), ties the bank: bank form with no named bottom/top (VC6 CSEs point.Y +/- rady identically). Insert before Draw_Circle in |
