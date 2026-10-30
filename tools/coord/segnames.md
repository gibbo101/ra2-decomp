# Names for the former seg_ ranges

Measured 28 October 2026 against the 62 `seg_` ranges (1,316 functions) and five VQA `?`
rows of the committed `data/modules.csv`. **309 of those 1,316 functions now have real
module names**: 30 ranges are named whole, 7 in part, 25 stay `seg_`. Two VQA `?` functions
joined `vqalib/drawer`.

Run against the current tree, `uv run tools/modules.py` gives 1,011 `seg_` functions where
the old code gives 1,320 on the same tree. The old code's 1,320 is 4 more than the committed
1,316 because batch agents added empty `src/_milsectmr.cpp`, `src/sprite.cpp`,
`src/_winfix.cpp` and `src/diskswap.cpp` during the run. The "src/ file holding none of its
range" rule then refuses those four names: `_milsectmr` and `sprite` become
`seg_0040D310` and `seg_006B8EE0`, and `_winfix` and `diskswap` fold into `_weapon` and
`disklaser`. The names come back once those files hold their functions. None of my changes
caused this.

## What modules.py learned

Every rule below is general, and the module docstring describes each one.

- **L, initialised objects:** a static initialiser that writes a global whose name (from
  symbols.csv or names.csv) only one TibSun file defines at file scope ties to that file.
- **L, RTTI class:** the class of the RTTI vtables holding a function, including nested
  names (`History@WorldDominationTour`), ties to the TibSun file that defines it, when only
  one file does.
- **L, string tables:** besides string literals the function holds directly, the strings
  in a table of two or more string pointers that it holds.
- **Y, vtables:** if only code in one segment holds a class's vtable, those functions get
  that class (constructors and destructors).
- **Y, vector templates:** VectorClass and DynamicVectorClass code over `X *` gets class X,
  but only in a segment where no other class is named.
- **U:** code that no TibSun file claims goes to the module beside it in its segment when
  every caller, and every function holding its address, is in that module, and no data
  holds its address.
- **D:** failing U, code goes to the module beside it when it holds a data address that, of
  all other code, only that module's code holds. An address 256 bytes or less past another
  address the same code holds is skipped, because it may be the end of that object (quat's
  code holds the end of quarry's name table).

Like B and C, U and D count as weak evidence, so a module named only by them is not
certain. U and D only extend a module beside the code. They cannot create a name.

## names.csv rows added (21)

Each row is proven by a matched caller whose source makes that call, or that passes or uses
that address, at that place. Each row's evidence column gives the caller.

- `BlowPipe::Key` 0x004381D0
- `Buffer::Buffer` 0x0043AD00 (with its decorated symbol)
- `Buffer::~Buffer` 0x0043AE50 (with its decorated symbol)
- `CrateClass::Create_Crate` 0x004A17C0
- `CrateClass::Get_Crate` 0x004A1AA0
- `CreditClass::AI` 0x004A2600
- `LCW_Comp` 0x00551E50
- `lzo1x_decompress` 0x0055C0E0
- `MSSfx::MSSfx`, `MSSfx::~MSSfx`, `MSSfx::Play` 0x005D5820, 0x005D5850, 0x005D5860
- `RawFileClass::~RawFileClass` 0x0065CA00 (with its decorated symbol)
- `RLEEngine::Line_Compress` 0x00661590
- `SHAPipe::Result` 0x0069E9E0
- `StorageClass::operator+=`, `operator-=`, `First_Used_Slot` 0x006C9740, 0x006C97E0,
  0x006C9820
- `UnVQ2_4x4_Table`, `UnVQ2_4x2_Table`, `UnVQ1_4x4_Table`, `UnVQ1_4x2_Table` 0x007483B0,
  0x00748750, 0x00748AF0, 0x00748C30

check.py also reads names.csv. The calls to these functions in matched code were
unverified before. They now resolve to these addresses.

## Each former range

"Stays" means the range keeps its `seg_` name.

| Former range (fns) | Between | Now | Evidence, or why it stays |
|---|---|---|---|
| seg_00401000 (99) | (start) .. seg_ | stays | YR audio (`AudioIDXData`, YRpp). A name sorting before `_astar` cannot come from a class, and no TibSun file fits. |
| seg_00403CE0 (89) | seg_ .. seg_ | stays | A header run, then `AudioController` (YRpp). Same problem. |
| seg_00406920 (175) | seg_ .. _astar | stays | A header run, then audio streams (`AudioStream::PlayWAV`, `Audio::ReadWAVFile`, "AUDIO.MIX"). Same problem. |
| seg_0040B350 (40) | _astar .. _command | stays | A header run, BombListClass calls, and vectors of `BombClass *` and `TechnoClass *`. This looks like a YR `_` instance file (`_bomb`?), but TibSun has no such file and nothing proves the name. |
| seg_0040D300 (1) | _map .. _milsectmr | _map | U: only `_map` calls it. |
| seg_0040DA00 (4) | _palette .. _rtti | **_rect** | L: initialisers of RECT_NONE, TacticalRect (0,0,640,400), VisibleRect (0,0,640,400) and SidebarRect, which are TibSun `_rect.cpp`'s four objects. The tie is through the names.csv row for VisibleRect. |
| seg_00412660 (33) | ahandle .. aircraft | stays | A header run, then `AircraftTrackerClass` (YRpp). `aircrafttracker` sorts after `aircraft`, so the file is not named after the class. |
| seg_00431A30 (1) | bench .. bfiofile | bfiofile | D: the `BufferIOFileClass(char const *)` constructor, which stores the class's RTTI vtable. Only bfiofile's code also holds that vtable. TibSun's first bfiofile function. |
| seg_00432670 (30) | bfiofile .. bitfont | stays | A header initialiser, then movie-player helpers called by movie, movies and msanim (a Bink wrapper would sort here). No class, string or TibSun file. |
| seg_004381D0 (1) | blowpipe .. blwstraw | blowpipe | L: names row `BlowPipe::Key`. |
| seg_0043AD00 (3) | brain .. buff | buff | L: names row for TibSun's first `Buffer` constructor. The other two, byte-identical, lie between it and `Buffer(int)`. |
| seg_0043AE50 (2) | buff .. buffer | buff 1, seg_0043AE80 1 | L: names row `Buffer::~Buffer`. 0x0043AE80 is `Buffer::Reset` by TibSun's order, but nothing references it. |
| seg_00473940 (2) | carry .. category | category | D: these read `_categories` (0x0081B7C8), which only category's `Category_From_Name` also reads. They are TibSun's `Description_From_Category` and `Name_From_Category`. |
| seg_004796E0 (2) | CD .. cdcntrl | stays | No callers, names or shared data. |
| seg_0047A6F0 (2) | cdcntrl .. cdfile | cdcntrl | U |
| seg_0048BB20 (4) | combuf .. Connect | stays | A header initialiser, then `IndexClass<int, CommandClass const *>` code used by Options, Init and MainLoop. No TibSun or OpenTS file sorts there. |
| seg_004A17C0 (3) | crate .. crc | crate | L: names rows `Create_Crate` and `Get_Crate`. `Put_Crate` lies between them. |
| seg_004A2060 (1) | crcpipe .. crcstraw | stays | A CRC result finisher (likely `CRCPipe::Result`), unreferenced. |
| seg_004A2600 (1) | Credits .. cstraw | Credits | L: names row `CreditClass::AI`. |
| seg_004CABF0 (1) | factory .. fastmath | factory | D: a `VectorClass<TechnoTypeClass const *>` constructor. Factory instantiates it first. |
| seg_004CBEF0 (19) | findpath .. flasher | stays | A header run, then `TrajectoryHelper` (a YRpp namespace, not a class). |
| seg_004E23C0 (2) | GameDlg .. gametime | gametime 1, seg_004E23C0 1 | D: 0x004E2440 sets gametime's start time (0x008B3F14). 0x004E23C0 is unreferenced. |
| seg_004E5D60 (4) | GDlgSupp .. getcd | GDlgSupp 3, seg_004E6030 1 | D: 0x004E5ED0 shares 0x008B3FC8 with GDlgSupp only, which takes in the two before it. 0x004E6030 is called from four modules. |
| seg_004E6140 (1) | getcd .. getcpu | stays | A 1-byte `ret` called by globals. |
| seg_004F2C50 (6) | grphmenu .. grphmimg | stays | A header initialiser, then `GraphicMenuAnimItem` (RTTI). The class name does not sort there. |
| seg_004F4C20 (9) | gscreen .. hicolor | stays | Palette's `HashString` helpers. HashString is YRpp's name for a char[256] struct, not a proven class, and only one function carries it. |
| seg_00510B00 (19) | House .. houstype | stays | A header run, then `HouseClass` statics (`FindByIndex`, `<Player @ A>`). This is a second HouseClass file sorting after `house`, name unknown (like progress2). |
| seg_00517340 (1) | hover .. hsv | stays | Held only by data. |
| seg_00551E50 (1) | lcw .. lcwpipe | lcw | L: names row `LCW_Comp` (TibSun lcw.cpp). |
| seg_00556790 (37) | link .. list | stays | A header run, then `LineTrail` (YRpp and RTTI). `linetrail` sorts before `link`. |
| seg_0055BB40 (1) | logic .. lzo1x_c | logic | U |
| seg_0055BC00 (2) | lzo1x_c .. lzopipe | lzo1x_c 1, **lzo1x_d** 1 | U: TibSun's static `do_compress`. L: names row `lzo1x_decompress`. |
| seg_0058B110 (14) | map .. seg_ | stays | Header initialisers only: a module with no code of its own. |
| seg_0058B320 (14) | seg_ .. MapGen | stays | The same. Two such segments and one TibSun candidate (`mapedit`). RA's `mapeddlg`, `mapedplc`, `mapedsel` and `mapedtm` would sort here too. |
| seg_005BB560 (1) | ModemHst .. modemreg | ModemHst | U |
| seg_005C61D0 (21) | mpmanbattle .. mpmath | mpmath | D (0x0082E174) and U. These are the XMP helpers called by pk and mpmath. |
| seg_005D5820 (3) | msgloop .. mstimer | **mssfx** | L: names rows for `MSSfx` (TibSun mssfx.cpp). |
| seg_005D8D80 (28) | multiplayerteam .. NetDlg | stays | A header run, then firewall and NAT code ("FirewallSettings", "mangler1.westwood.com"). No class or TibSun file. |
| seg_005EEF60 (3) | netshare .. nullconn | netshare | U |
| seg_00625780 (5) | ownrdraw .. packet | ownrdraw | U |
| seg_00631B20 (2) | PhoneEd .. pipe | PhoneEd | U |
| seg_00631D40 (8) | pipe .. pk | **pixelfx** 7, seg_00631D40 1 | Y: only these functions hold the `PixelFXClass` RTTI vtable. 0x00631D40 is called from Tactical. |
| seg_00645A70 (5) | ptype .. quat | ptype 1, seg_ 1, **quarry** 2, seg_ 1 | U for 0x00645A70. L: the name table of `Quarry_From_Name` and `Name_From_Quarry` holds TibSun quarry.cpp's strings. 0x00645B10 (a vector destructor) and 0x00645BB0 (YR's threat-from-quarry) stay. |
| seg_0065C630 (1) | ramfile .. random | stays | Unreferenced. |
| seg_0065CA00 (1) | random .. rawfile | rawfile | L: names row `RawFileClass::~RawFileClass`. |
| seg_006614F0 (8) | rgb .. rndstraw | seg_ 2, **rle** 2, seg_ 4 | L: names row `RLEEngine::Line_Compress`, and U. 0x006614F0 and 0x00661510 are a 3-byte RGB stream Save and Load. 0x006615D0 to 0x00661700 have no proof. |
| seg_006635F0 (48) | rocket .. rules | stays | A header run, then lobby code with `OwnerTalkClass` vectors ("mphmd.exe"). `ownertalk` does not sort there. |
| seg_0067FDC0 (67) | saveload .. savever | saveload | U: TibSun saveload's `Save_Vector`/`Load_Vector` template instances, then `SavegameInformation`. Only saveload calls its `Read` and `Write`. Its constructor is also called by LoadDlg, but lies between them. |
| seg_0068BE90 (11) | Scenario .. scheme | Scenario | U on the last four. The rest lie between Scenario's labels. |
| seg_0069D770 (3) | Session .. sha | Session | U |
| seg_0069E9E0 (2) | shapipe .. shastraw | shapipe 1, seg_0069E9F0 1 | L: names row `SHAPipe::Result`. 0x0069E9F0 is `SHAStraw::Get` by its body, but it is unreferenced. |
| seg_006C96E0 (5) | storage .. straw | storage | L: names rows for three of TibSun's five StorageClass operators, which are in TibSun's order. |
| seg_006C99B0 (36) | straw .. Super | **subtitle** | Y: `DynamicVectorClass<SubTitle *>` (RTTI) code. YRpp calls the class `SubTitleClass`, and "subtitlemd.txt" is loaded here. |
| seg_0072A690 (274) | txtlabel .. UICmnds | stays | A header run, then the UI shape and palette cache ("WAITCYLO.SHP"). The only names are YRpp's `OwnerDraw::Get*Convert`, a namespace that spans ownrdraw too. |
| seg_00734430 (32) | UICmnds .. Unit | stays | A header run, then `StringTable` (YRpp, CSF loading). `stringtable` does not sort there. |
| seg_007483B0 (4) | unvqtblc .. utracker | unvqtblc | L: names rows for the four UnVQ functions (TibSun unvqtblc.cpp). |
| seg_0074BB20 (2) | vanimtype .. vector | vanimtype 1, vector 1 | U. D: `BooleanVectorClass`'s first constructor. |
| seg_0074F6F0 (1) | vein .. version | vein | U |
| seg_00751C30 (1) | voc .. volinfo | stays | A CD-volume query patched by EA, called by CD. No proof. |
| seg_007678F0 (21) | WDTGameOptions .. seg_ | **wdthistory** | L: the `History@WorldDominationTour` RTTI vtable. TibSun wdthistory.cpp defines History. |
| seg_00767C80 (82) | seg_ .. wdtnet | **wdtmap** | L: the `Map@WorldDominationTour` RTTI vtable. TibSun wdtmap.cpp defines Map. |
| seg_0077B070 (17) | WOLPersonaInformation .. wonline | stays | A header run, then an intro and theme routine using WOLOptions. No class. |
| ? (2) | vqalib/drawer .. dstream | vqalib/drawer | U |
| ? (2), ? (1) | vqalib | stay `?` | No evidence. |

## Named rows whose range changed

All 31 grew into the unassigned gap beside them. None shrank, moved off its old functions,
or changed its certainty.

| Module | Old range (fns) | New range (fns) |
|---|---|---|
| _map | 0x0040CF80-0x0040D2F0 (25) | 0x0040CF80-0x0040D300 (26) |
| bfiofile | 0x00431B20-0x00432663 (16) | 0x00431A30-0x00432663 (17) |
| blowpipe | 0x00438060-0x004381C2 (2) | 0x00438060-0x0043820F (3) |
| buff | 0x0043ADC0-0x0043AE42 (3) | 0x0043AD00-0x0043AE77 (7) |
| category | 0x00473980-0x004739C9 (1) | 0x00473940-0x004739C9 (3) |
| cdcntrl | 0x00479710-0x0047A6E2 (21) | 0x00479710-0x0047A9C0 (23) |
| crate | 0x004A1540-0x004A17BF (15) | 0x004A1540-0x004A1C02 (18) |
| Credits | 0x004A2140-0x004A25F3 (16) | 0x004A2140-0x004A2770 (17) |
| factory | 0x004C96A0-0x004CABE8 (56) | 0x004C96A0-0x004CAC39 (57) |
| gametime | 0x004E2450-0x004E24D4 (4) | 0x004E2440-0x004E24D4 (5) |
| GDlgSupp | 0x004E3110-0x004E5D58 (64) | 0x004E3110-0x004E602F (67) |
| lcw | 0x00551C60-0x00551E43 (1) | 0x00551C60-0x00551FE6 (2) |
| logic | 0x0055AD30-0x0055BB31 (23) | 0x0055AD30-0x0055BB89 (24) |
| lzo1x_c | 0x0055BB90-0x0055BBF6 (1) | 0x0055BB90-0x0055C0D4 (2) |
| ModemHst | 0x005B75D0-0x005BB554 (50) | 0x005B75D0-0x005BB57D (51) |
| mpmath | 0x005C69A0-0x005C91CA (33) | 0x005C61D0-0x005C91CA (54) |
| netshare | 0x005E2410-0x005EEF58 (164) | 0x005E2410-0x005EF0F9 (167) |
| ownrdraw | 0x00600320-0x00625773 (254) | 0x00600320-0x00625AA6 (259) |
| PhoneEd | 0x00630980-0x00631B11 (25) | 0x00630980-0x00631C71 (27) |
| ptype | 0x006449F0-0x00645A67 (31) | 0x006449F0-0x00645B03 (32) |
| rawfile | 0x0065CA70-0x0065D42B (19) | 0x0065CA00-0x0065D42B (20) |
| saveload | 0x0067C480-0x0067FDB8 (23) | 0x0067C480-0x0068214E (90) |
| Scenario | 0x00683070-0x0068BE8E (74) | 0x00683070-0x0068C32B (85) |
| Session | 0x00696D00-0x0069D768 (138) | 0x00696D00-0x0069D8B9 (141) |
| shapipe | 0x0069E070-0x0069E9D5 (16) | 0x0069E070-0x0069E9EF (17) |
| storage | 0x006C93F0-0x006C96DE (19) | 0x006C93F0-0x006C9840 (24) |
| unvqtblc | 0x00748220-0x007483A1 (3) | 0x00748220-0x00748FCA (7) |
| vanimtype | 0x0074AB70-0x0074BB1D (30) | 0x0074AB70-0x0074BB9F (31) |
| vector | 0x0074BBF0-0x0074C167 (15) | 0x0074BBA0-0x0074C167 (16) |
| vein | 0x0074C170-0x0074F6E8 (62) | 0x0074C170-0x0074F720 (63) |
| vqalib/drawer | 0x007BE660-0x007BF210 (11) | 0x007BE660-0x007BF233 (13) |

## Landed functions outside their src file's module

None of these have been moved.

| src file | Module now | Fns | Addresses |
|---|---|---|---|
| src/ahandle.cpp | seg_00412660 | 14 | 0x00412660-0x00412850 (header run) |
| src/bfiofile.cpp | seg_00432670 | 1 | 0x00432670 (header initialiser) |
| src/combuf.cpp | seg_0048BB20 | 1 | 0x0048BB20 (header initialiser) |
| src/grphmenu.cpp | seg_004F2C50 | 1 | 0x004F2C50 (header initialiser) |
| src/house.cpp | seg_00510B00 | 11 | 0x00510B70-0x00510CF0 (header run) |
| src/link.cpp | seg_00556790 | 14 | 0x00556790-0x00556980 (header run) |
| src/straw.cpp | subtitle | 1 | 0x006C99B0 (header initialiser) |
| src/txtlabel.cpp | seg_0072A690 | 1 | 0x0072A690 (header initialiser) |
| src/uicmnds.cpp | seg_00734430 | 14 | 0x00734430-0x00734620 (header run) |
| src/wdtgameoptions.cpp | wdthistory | 1 | 0x007678F0 (header initialiser) |
| src/wolpersonainformation.cpp | seg_0077B070 | 1 | 0x0077B070 (header initialiser) |
| src/progress2.cpp | progress | 11 | 0x006433D0-0x00643E90 (not a former seg_ range) |

In every former-seg_ case, the header initialisers that begin the next module were landed in
the src file of the module before it.

## Leads not resolved

- **SubTitleClass in straw.** YRpp's `SubTitleClass` ctor, dtor and setters (0x006C9900 to
  0x006C9960) are landed in `src/straw.cpp` and lie inside straw's range, before
  subtitle's header initialiser at 0x006C99B0. Either they are header inlines that straw
  happens to use first, or a module with no header sorts between straw and subtitle.
- **progress2.** No general evidence separates it from progress: it has no header
  initialisers, and the class is the same as progress's. It stays inside `progress`.
- **wonline.** Its range (477 functions, anchored by `__FILE__`) was not split. Nothing new
  marks a boundary inside it.
- **alphalighting.** The 0x00420120 lead is already settled in the committed map: that
  range sits inside `alphalighting`.
- **Remaining large ranges.** Each is a YR-only file whose name cannot be derived. Either
  the class name sorts on the wrong side of a neighbour (AircraftTracker, LineTrail,
  StringTable), there is no class at all (the firewall code, the UI shape cache, lobby
  talk), or the code sits before `_astar`, where no class-derived name can sort (the audio
  ranges).
