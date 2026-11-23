# Banked forms

Best forms recorded for functions that do not match yet, or that match only through a form the project rules do not accept (held). None of these files is landable as it stands; each one's first line gives its score and what it still needs, and `data/attempts.csv` has the full history per address.

Files named `ADDR_*.cpp` are function bodies or whole source files to splice into `src/`; `.h`/`.hh` files are header overlays that the form needs. Score lines describe the tree they were measured on, so re-check against current `main` before relying on them.

| file | first line |
|---|---|
| `0x00443C60.cpp` | // best 78.6% raw (6743/6759 bytes), no overlay; needs #include "airctype.h" (placed just above the function, or after temporal.h) and _$E298 renumbered to _$E3 |
| `0x00443C60_exit.cpp` | // best 99.8% raw, 99.8% normalised (6759/6759 bytes), no overlay; needs #include "airctype.h" after temporal.h (it renumbers _$E298 to _$E300); splice before 0 |
| `0x00443C60_techno.cpp` | // best 99.8% raw, 99.8% normalised (6759/6759 bytes), no overlay; needs #include "airctype.h" after temporal.h (it renumbers _$E298 to _$E300); splice before 0 |
| `0x004666E0_ballistic.cpp` | // BulletClass::AI best natural 80.0% raw (92.2% normalised, 6403/6422 bytes, frame 0x198) with overlay 0x004666E0_bullet_velocity.h (78.7% raw without it); dro |
| `0x004666E0_bullet.cpp` | // BulletClass::AI best natural 72.7% raw (86.7% normalised, 6388/6422 bytes, frame 0x198 as the exe) with overlay 0x004666E0_bullet_velocity.h (include/velocit |
| `0x004666E0_bullet_velocity.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x0046DFD0.cpp` | // 70.3% (5528/5648), whole src/campscor.cpp; natural F8F0 (calls F810, still MATCH) breaks Show to 65.3%; outer cases reordered 4ED,497+INIT,TIMER,HSCROLL,COMM |
| `0x00485AF0_cell.cpp` | // best 72.2% (2185/2191 B), no overlay; insert after 0x00485590 (Redraw_Veins) in src/cell.cpp; Get_Vein_Frame is called directly (no wrapper needed now); this |
| `0x00485AF0_cell_diag.cpp` | // DIAGNOSTIC, never lands: 90.4% (2201/2191 B = exe + the 10-byte probe); without the probe line this natural form scores 71.9% (2185/2191 B) |
| `0x00485AF0_near.cpp` | // best natural 72.2% raw, 82.6% normalised (2185/2191 B), no overlay; insert after 0x00485590 (Redraw_Veins) in src/cell.cpp. The 6 missing bytes are a zero th |
| `0x004AFF60_loco.cpp` | // DriveLocomotionClass::Draw_Matrix MATCH (1189/1189) only with overlay 0x004AFF60_loco_timer.h (timer.h with ProgressTimerClass::Progress as one statement: re |
| `0x004AFF60_loco_timer.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x004AFF60_twins.cpp` | // DriveLocomotionClass::Draw_Matrix MATCH 100.0% (1189/1189) only with overlay 0x004AFF60_twins_timer.h (timer.h with ProgressTimerClass::Progress as one state |
| `0x004AFF60_twins_timer.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x0050AF10_present.patch` | diff --git a/src/house.cpp b/src/house.cpp |
| `0x0051B350_tunnel.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x00552D60_misc.cpp` | // 43.2% raw / 87.7% normalised (5023/5010), no overlay: build/batch/r_misc/0x00552D60.cpp ported to today's Fancy_Text_Print_Wide (it returns the Point2D); who |
| `0x00565C10.cpp` | // 99.7% (5374/5374) with no header change: the tube shift is a file-local two-statement inline (Shift_Tube), which leaves both TPoint2D<short>::operator+= call |
| `0x00565C10_tube.cpp` | // 99.7% raw, 99.6% normalised (5374/5374) with the overlay 0x00565C10_tube.h (tube.h plus an inline TubeClass::Shift holding both +=; the member name is a gues |
| `0x00565C10_tube.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x0056D100_grm.cpp` | // best 93.5% (290/290 bytes), no overlay; splice before 0x0056D230 in src/map.cpp; tail registers differ (exe this=esi loaded after the push of to, mzone=edi,  |
| `0x0056D100_ra2b.cpp` | // best 93.5% (290/290 bytes), no overlay; splice before 0x0056D230 in src/map.cpp. Only the tail registers differ (exe: this esi loaded after the push of to, m |
| `0x0056D6E0_zone.cpp` | // best 54.8% raw (73.0% normalised, 806/804 bytes), no overlay, body unchanged from the near_mapx bank; insert before 0x0056DA10 in src/map.cpp. Exe keeps cptr |
| `0x0056D6E0_zone2.cpp` | // best 54.8% raw (73.0% normalised, 806/804 bytes), no overlay, body unchanged from 0x0056D6E0_zone.cpp; insert before 0x0056DA10 in src/map.cpp (with the Zone |
| `0x0056D6E0_zone_pertype.cpp` | // 53.4% raw (73.3% normalised, 856/804 bytes), no overlay: lower raw score and 52 bytes long, banked because the exe's block order supports it; insert before 0 |
| `0x005824A0_zone.cpp` | // best 40.6% raw (55.0% normalised, 2194/2184 bytes), no overlay, body unchanged from the near_map2 bank; insert before 0x00582D30 in src/map.cpp. Left: regist |
| `0x00584550_zone.cpp` | // best 60.1% raw (71.1% normalised, 2290/2290 bytes), no overlay, body unchanged from the near_map2 bank; insert before 0x00584E50 in src/map.cpp. Left: from t |
| `0x00584550_zone2.cpp` | // best 60.1% raw (71.1% normalised, 2290/2290 bytes), no overlay; 0x00584550_zone.cpp with the final pass written through Subzone_Of (same score); insert befor |
| `0x00587410.cpp` | // best 97.1% raw (2109/2109), no overlay; insert before 0x00587C50 in src/map.cpp; left: search at [esp+0x2c] (any form that assigns search after the From/To i |
| `0x00587410_bridge.cpp` | // best flag-free 99.5% raw, 100.0% normalised (2109/2109), no overlay; insert before 0x00587C50 in src/map.cpp; while (true) with a `search = next` join gives  |
| `0x00587410_bridge_flag.cpp` | // MATCH 100% (2109/2109) but held: the loop test `while (!broken)` is always true when tested (flag form, as on branch single-587410b); kept for reference besi |
| `0x0058C800_mapgen.cpp` | // 0x0058C800 natural best 86.4% raw, 93.4% normalised (1704/1678), no overlay; insert before 0x0058CE90. TibSun's My_In_Radar for the side cell scores the same |
| `0x0058C800_mapgen2.cpp` | // 0x0058C800 natural best 86.4% raw (1704/1678), no overlay; insert before 0x0058CE90; unchanged from 0x0058C800_mapgen.cpp. |
| `0x0058C800_mapgen2_diag.cpp` | // 0x0058C800 DIAGNOSTIC, not landable: MATCH (1678/1678), whole file 307/307 otherwise unchanged; one dead call of a 40-term inline before `int processed_count |
| `0x0058C800_mapgen_diag.cpp` | // 0x0058C800 DIAGNOSTIC, not landable: MATCH (1678/1678) with the natural body (build/batch/near_gen/0x0058C800.cpp, which compiles the same as 0x0058C800_mapg |
| `0x0058C800_ra2a.cpp` | // 0x0058C800 MATCH (1678/1678; whole mapgen.cpp 307/307), NOT landable: two single-use TU-local helpers (Create_Seed_Region, Is_Region_Side_In_Radar), no secon |
| `0x0058C800_ra2a_zone.cpp` | // 0x0058C800 99.8% (1678/1678), whole file otherwise unchanged; side test through the file's existing In_Zone_Map_Bounds (two callers), Create_Seed_Region stil |
| `0x0058D620_mapgen.cpp` | // 0x0058D620 natural best 70.9% raw, 88.7% normalised (4061/4012), no overlay; insert before 0x0058E5D0, replace Increase_Height 0x0058D5A0 with the body below |
| `0x0058D620_mapgen2.cpp` | // 0x0058D620 natural best 70.9% raw (4061/4012), no overlay; replace Increase_Height 0x0058D5A0 with these two functions and shift every _$E marker from 206 up |
| `0x005981F0_mapgen.cpp` | // 0x005981F0 best 33.4% raw, 47.7% normalised (1829/1889) with the exe's file.Close(), no overlay; insert before 0x00599530. Unchanged from build/batch/near_ge |
| `0x005981F0_mapgen2.cpp` | // 0x005981F0 natural best 33.4% raw (1829/1889) with the exe's file.Close(), no overlay; insert before 0x00599530; unchanged from 0x005981F0_mapgen.cpp. |
| `0x00598960.cpp` | // Best natural form of 0x00598960 Generate_Random_Map: 93.6% (3088/3018 bytes), no overlay. Every repeated block is a call (Init_Regions, Destroy_Cell_Regions, |
| `0x00598960_diag.cpp` | // DIAGNOSTIC, not source: 0x00598960 Generate_Random_Map MATCH (3018/3018) and 0x005AC290 MATCH, but Cleanup 0x00599530 falls to 60.4% (230/276: its own copy s |
| `0x00598960_grm.cpp` | // best 93.6% (3088/3018 bytes), no overlay; splice over Generate_Random_Map and Cleanup in src/mapgen.cpp; Cleanup 0x00599530 and 0x005AC290 MATCH beside it. |
| `0x00598960_mapgen2.cpp` | // 0x00598960 natural best 93.6% (3088/3018), no overlay; same form as 0x00598960_grm.cpp; splice over Generate_Random_Map and Cleanup in src/mapgen.cpp (Cleanu |
| `0x00598960_mapgen2_diag.cpp` | // 0x00598960 DIAGNOSTIC, not landable: MATCH (3018/3018) with Cleanup 0x00599530, 0x005AC290 and the whole file (307/307) matching; two dead calls of a 34-term |
| `0x00598960_ra2a.cpp` | // 0x00598960 natural best 93.6% (3088/3018), Cleanup 0x00599530 and 0x005AC290 MATCH; the middle region teardown is MapRegionClass::Destroy_Cell_Regions() inst |
| `0x00598960_ra2a_diag.cpp` | // 0x00598960 DIAGNOSTIC, not landable: MATCH (3018/3018) with Cleanup, 0x005AC290 and the whole file 307/307; one dead 40-term inline call in Cleanup before De |
| `0x005B01C0_move.cpp` | // 82.5% raw, 93.8% normalised (5604/5602 bytes), no overlay, no helper; full copy of src/mech.cpp with 0x005B01C0 added. Walk's tube-block form (Cell(tube->Ent |
| `0x005B01C0_move2.cpp` | // 89.1% raw, 94.1% normalised (5604/5602 bytes), no overlay, no helper; full copy of src/mech.cpp with 0x005B01C0 added. From 0x005B01C0_move.cpp: the cell blo |
| `0x005B20F0.cpp` | // Projectile_Motion 99.7% (2101/2101 bytes) with overlay 0x005B20F0_velocity.h (include/velocity.h with only Speed() written as sqrt((X * X) + (Y * Y) + (Z * Z |
| `0x005B20F0_bullet.cpp` | // Projectile_Motion best natural 99.7% (2101/2101 bytes) with overlay 0x005B20F0_vec_velocity.h (Speed() with each square parenthesised); source unchanged from |
| `0x005B20F0_vec.cpp` | // Projectile_Motion best natural 99.7% (2101/2101 bytes) with overlay 0x005B20F0_vec_velocity.h (include/velocity.h with only Speed() written as sqrt((X * X) + |
| `0x005B20F0_vec_diag.cpp` | // Projectile_Motion DIAGNOSTIC ONLY, never landed: MATCH (2101/2101 bytes) with overlay 0x005B20F0_vec_diag_velocity.h, which adds a second constructor TVeloci |
| `0x005B20F0_vec_diag_velocity.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x005B20F0_vec_velocity.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x005B20F0_velocity.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x005E3D10_score.cpp` | // best 77.3% raw / 97.0% normalised (8723/8702 B), no overlay; insert before 0x005E6230. Every inlining decision now equals the exe's (GenericNode::Unlink is c |
| `0x005EA3E0_score.cpp` | // best 59.2% raw / 95.0% normalised (1259/1277 B), no overlay; insert before 0x005EA8E0. Inlining decisions equal the exe's. Left: in the full path the exe giv |
| `0x005EC9F0_score.cpp` | // best 86.4% (2168/2168 B), no overlay; insert before 0x005ED270; needs #include "swap.h" after tiprequest.h and FUN_005E94D0 in the count-loop form below (it  |
| `0x0060A5B0.cpp` | // best 44.1% (917/1184) with src changes: Find_Data moved above Typed_Char_Sound; Typed_Char_Sound and Is_Typed_Control use Find_Data (both MATCH); Is_Typed_Co |
| `0x0060A5B0_odh.cpp` | // best natural 45.4% (890/1184) on 23 Nov src (after 0x00612B70 landed); every other ownrdraw function MATCH with it. Inline budget is the blocker (tools/inlin |
| `0x0060A5B0_odh2.cpp` | // best natural 31.5% raw / 57.9% normalised (1159/1184 bytes) on 23 Nov src (main 1ea946b); register-masked diff 76 lines (old _odh bank: 228). Every other own |
| `0x0060B950.cpp` | // best 99.8% (1895/1895 bytes), no overlay, re-scored 22 Nov: only the w/old_w compare before MoveWindow differs (ours cmp ecx,eax, exe cmp eax,ecx); with `w ! |
| `0x0060B950_odraw.cpp` | // best 99.8% (1895/1895 bytes, 99.6% normalised), no overlay, re-scored 22 Nov on the flattened WinData (branch n22-odraw): only the w/old_w compare before Mov |
| `0x00610CA0_odh.cpp` | // best 48.4% (7216/7265), normalised 78.1%, on 23 Nov src (after 0x00612B70 landed); no overlay; tools/inlinebudget.py reports no differing inlining decisions  |
| `0x006153E0.cpp` | // best 84.0% (4032/4032), all 244 other ownrdraw.cpp functions MATCH with it in place; insert before 0x006163A0, add "class MovieHandle; class ShapeSet;" befor |
| `0x00617250_odh.cpp` | // best 65.6% (6923/6895), normalised 84.0%, on 23 Nov src (after 0x00612B70 landed); no overlay. tools/inlinebudget.py now reports no differing inlining decisi |
| `0x00618D40.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler: 57.0% raw, 72.9% normalised, 14666/14666 bytes, frame 0x2D2C (22 Nov). |
| `0x00618D40_lbh.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler: 52.9% raw, 73.8% normalised, 14562/14666 bytes, frame 0x2D2C (23 Nov, natural form; file checks 253/254 with the five m |
| `0x00618D40_lbh_diag.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler DIAGNOSTIC, never landable: 49.0% raw, 75.2% normalised, 14778/14666 bytes, frame 0x2D30 (23 Nov; file checks 253/254 wi |
| `0x00618D40_odraw.cpp` | // 0x00618D40 OwnerDraw::ListBoxHandler: 57.0% raw, 73.1% normalised, 14666/14666 bytes, frame 0x2D2C, re-scored 22 Nov on the flattened WinData (branch n22-odr |
| `0x0062C6E0_ballistic.cpp` | // ParticleClass::Spark_Behavior_AI MATCH (1072/1072) with overlay 0x0062C6E0_ballistic.h (Ballistic_Move gains a TechnoClass * payback parameter; bounce.cpp an |
| `0x0062C6E0_ballistic.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x0064BDD0_mixed.cpp` | // 0x0064BDD0 36.0% raw / 52.2% normalised (1450/1455 B), no overlay; 0x0064BDD0_plan_exit.cpp plus a named type local (unsigned char type = event->Type) that g |
| `0x0064BDD0_plan.cpp` | // 0x0064BDD0 best 48.4% raw / 64.8% normalised (1448/1455 B), no overlay; splice after 0x0064B660 in src/queue.cpp and rename FUN_0064BDD0 (declaration and cal |
| `0x0064BDD0_plan_exit.cpp` | // 0x0064BDD0 29.4% raw / 50.5% normalised (1450/1455 B), no overlay; same splice as 0x0064BDD0_plan.cpp. Leaving the loop when nothing is left over (exe 0x0064 |
| `0x006622C0_rocket.cpp` | // best 95.2% raw / 97.5% normalised (3432/3432 B), no overlay, a full rocket.cpp; block layout, every stack slot and the EDI/EBX allocation match; left: one ti |
| `0x006622C0_rocket_diag.cpp` | // DIAGNOSTIC, not source: 98.1% raw (3632/3432 B); the 95.2% bank plus 20 dead stores to distinct statics before the last return; with them the only differing  |
| `0x0069F670_loco.cpp` | // ShipLocomotionClass::Draw_Matrix MATCH (1189/1189) only with overlay 0x0069F670_loco_timer.h (timer.h with ProgressTimerClass::Progress as one statement: ret |
| `0x0069F670_loco_timer.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x0069F670_twins.cpp` | // ShipLocomotionClass::Draw_Matrix MATCH 100.0% (1189/1189) only with overlay 0x0069F670_twins_timer.h (timer.h with ProgressTimerClass::Progress as one statem |
| `0x0069F670_twins_timer.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x006A7780_side.cpp` | // best natural 99.0% raw, 99.2% normalised (1428/1428 bytes), no overlay; insert after FUN_006A7760 in src/sidebar.cpp WITH StripClass::Page (0x006A89E0) calli |
| `0x006A7780_side_diag.cpp` | // MATCH (1428/1428 bytes) as a diagnostic only, not landable: src/sidebar.cpp checks 115 of 115 with the function of 0x006A7780_side.cpp inserted after FUN_006 |
| `0x006A7780_side_sidebar.cpp` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x006A7780_team.cpp` | // best 98.5% raw, 98.7% normalised (1428/1428 bytes), no overlay; insert after FUN_006A7760 in src/sidebar.cpp, which is otherwise unchanged; two of the four i |
| `0x006A7780_team_diag.cpp` | // MATCH (1428/1428 bytes) as a diagnostic only, not landable: the unused read of SidebarRect.Y numbers that global before the first inlined row count; insert a |
| `0x006A9540.cpp` | // best natural form: 76.5% raw, 97.3% normalised (4276/4280 bytes), no overlay; insert after FUN_006A94F0 in src/sidebar.cpp. Not a match. |
| `0x006A9540_side.cpp` | // best natural 76.5% raw, 97.3% normalised (4276/4280 bytes), no overlay; insert after FUN_006A94F0 in src/sidebar.cpp. Not a match; the body is 0x006A9540.cpp |
| `0x006BB9A0.cpp` | // best 92.5% natural (10261/10261 bytes): whole src/startup.cpp, TibSun-form WinMain with per-block HRESULT result, retval/factory names. A forced form MATCHes |
| `0x006BB9A0_forced.cpp` | // MATCH, forced (not landable): whole src/startup.cpp; needs single-caller inline Read_Setup_Options, one-use aliases netptr, zbuf (chained) and mouse. All 502 |
| `0x006CC390.cpp` | // best 99.4% (6884/6884 bytes), fragment for src/super.cpp before 0x006CDE80; needs include/cell.h's "private:" before OccupierPtr changed to "public:" so the  |
| `0x006D3D10_tact.cpp` | // best 31.6% raw, 55.5% normalised (3597/3643 B), no overlay; append at the end of src/tactical.cpp. Unchanged from the near_misc bank, re-checked: all 58 call |
| `0x006D7840_near.cpp` | // best natural 83.8% raw, 92.8% normalised (952/954 B), no overlay; splice before Tile_Center in src/tactical.cpp (after 0x006D7560). Still one operand: the ex |
| `0x006D7840_tact.cpp` | // best natural 83.8% raw, 92.8% normalised (952/954 B), no overlay; splice before Tile_Center in src/tactical.cpp (after 0x006D7560), no _$E renumbering needed |
| `0x006D7840_tact_diag.cpp` | // DIAGNOSTIC, never lands: 93.0% raw, 99.7% normalised (955/954 B). 'short pitch' makes the pitch operand a non-leaf, and then every instruction but its own lo |
| `0x006D7F20_tact.cpp` | // best 65.0% raw, 76.8% normalised (809/824 B), no overlay; splice before Set_Ground_Position (above 0x006D8260) in src/tactical.cpp. Site 1 now matches (pixel |
| `0x006FDD50_techno.cpp` | // best 56.8% raw, 86.7% normalised (7112/7167 bytes), no overlay; insert before 0x006FD800 in src/techno.cpp. The cast.h helpers (As_Building, As_Foot) in the  |
| `0x00705E00_exit.cpp` | // best 90.0% raw, 93.7% normalised (2112/2100 bytes), no overlay; needs #include "msanim.h" in techno.cpp; splice before 0x00706640. Body unchanged from 0x0070 |
| `0x00705E00_techno.cpp` | // best 90.0% raw, 93.7% normalised (2112/2100 bytes), no overlay; needs #include "msanim.h" in techno.cpp; splice before 0x00706640. The dirty rectangle now ma |
| `0x00710AF0_ra2d.cpp` | // 0x00710AF0 MATCH with overlay 0x00710AF0_ra2d_ability.hh (TibSun loop ctor) + 0x00710AF0_ra2d_techtype.h (UnusedField at 0x3C0); together they add declaratio |
| `0x00710AF0_ra2d_ability.hh` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x00710AF0_ra2d_techtype.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x00741970_techno_face.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x00748C30_gfx.cpp` | // best 92.5% raw / 92.2% normalised (923/923 B), no overlay; append to src/unvqtblc.cpp. NOT landable: still needs `pcode`, a dead pointer to code (only a poin |
| `0x00748C30_misc.cpp` | // best 91.8% raw / 91.2% normalised (923/923 B), no overlay; append to src/unvqtblc.cpp. NOT landable as it stands: `pcode` is a dead pointer whose only job is |
| `0x00754CB0.cpp` | // 0x00754CB0 Init_Voxel_Matrices best natural 97.0% (3290/3290 bytes), whole voxel.cpp, no overlay: TibSun's references with Cross_Product reading LightDirecti |
| `0x0075AEC0.cpp` | // best natural 83.4% raw, 93.3% normalised (4931/4988), no overlay; full copy of today's src/walk.cpp. Guard inversion at the end (IsUnderEMP and arrival as if |
| `0x0075AEC0_returns.cpp` | // 83.0% raw, 93.7% normalised (4936/4988), no overlay; full copy of src/walk.cpp. Variant of 0x0075AEC0_tube.cpp (83.3% raw, 93.8% normalised, still the best b |
| `0x0075AEC0_tube.cpp` | // 83.3% raw, 93.8% normalised (4932/4988), no overlay once tube.h has Get_Exit and Get_Count; full copy of src/walk.cpp. 0x0075AEC0.cpp with the tube height st |
| `0x0076C290_misc_wdtnet.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x0076D4D0_wdt.cpp` | // MATCH (1531/1531) with every wdtsel.cpp function matching, no overlay, but NOT landable as is: it needs (1) the main loop's cursor built one inline level dow |
| `0x0076D4D0_wdt2.cpp` | // MATCH (1531/1531), not landed: with 0x0076D4D0_wdt2_vcursor.h as include/vcursor.h and 0x0076D4D0_wdt2_wdtmap.diff applied, wdtsel.cpp is 68/68 and wdtmap.cp |
| `0x0076D4D0_wdt2_vcursor.h` | // SPDX-License-Identifier: GPL-3.0-or-later |
| `0x0076D4D0_wdt2_wdtmap.diff` | --- src/wdtmap.cpp |
| `0x00787770_ra2c.cpp` | // 0x00787770 Join_WOL_Lobby 56.2% raw / 87.8% normalised (2713/2568 bytes) on main 3d41729, no overlay; splice after Login_WOL (0x00787010), before #include "_ |
| `0x00787770_wol3.cpp` | // 0x00787770 Join_WOL_Lobby 55.9% (2705/2568 bytes) on main 08cfecb, no overlay; splice after Login_WOL (0x00787010), before #include "_mixfile.h". inlinebudge |
| `0x00789B60_wol.cpp` | // 0x00789B60 FUN_00789B60 (create game): 99.8% (2312/2312 bytes), whole src/wonline.cpp, every other function MATCH (469). One instruction differs: lea ebx,[ed |
| `0x00789B60_wol2.cpp` | // 0x00789B60 FUN_00789B60 (create game): 99.8% (2312/2312 bytes), whole src/wonline.cpp = the n24-wol2 worktree src (stand-in trio unified, all 469 others MATC |
| `0x0078AC10.cpp` | // 0x0078AC10 WOL_Main_Dialog_Proc: 81.3% (9430/9478 bytes; 66 differing code lines vs 97 in the 81.4% bank), whole src/wonline.cpp; every other function MATCH  |
| `0x0078AC10_wol3.cpp` | // 0x0078AC10 WOL_Main_Dialog_Proc 81.6% (9442/9478 bytes) on main 08cfecb, full wonline.cpp. Natural change: Apply_WOL_Settings calls FUN_0077D7A0(music) again |
| `0x00793D20_nat.cpp` | // 0x00793D20 Start_Talking_To 51.3% (2685/2699 bytes, norm 78.8%) on main 8a10b56, no overlay; splice before '// FUNCTION: GAMEMD 0x007947B0'. Natural port of  |
| `0x00793D20_wol3.cpp` | // 0x00793D20 Start_Talking_To 53.1% (2685/2699 bytes) on main 08cfecb, no overlay, with the unbacked 'int next = trying + 1; trying = next;' (not landable; nat |
| `0x0079E520_wol3.cpp` | // 0x0079E520 buddy dialog proc 48.0% (3283/2939 bytes) on main 08cfecb, no overlay; splice before 0x0079F0A0. inlinebudget: 25 differing decisions in two clust |
| `0x0079F140_wol.cpp` | // 0x0079F140 buddy-list loop: 75.9% (2711/2763 bytes, normalised 90.8%), no overlay, all 469 other wonline functions MATCH; insert before 0x0079FC10. Needs the |
| `0x0079F140_wol2.cpp` | // 0x0079F140 buddy-list loop: 75.9% (2711/2763 bytes), no overlay, against the n24-wol2 worktree src (SimpleWonlineDialogControl unified; F2/F3 are SimpleDialo |
| `0x007BAB90_near.cpp` | // best natural 87.4% raw, 94.1% normalised (507/511 B), no overlay; insert after 0x007BAB80 in src/xsurface.cpp. The exe keeps the doubled dy in esi (home [esp |
| `0x007BAB90_team.cpp` | // best 87.4% raw, 94.1% normalised (507/511 bytes), no overlay; insert after 0x007BAB80 in src/xsurface.cpp; the exe doubles dx after abs() and gives the non-s |
