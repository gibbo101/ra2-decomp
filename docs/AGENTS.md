# Matching one function

You are given one address in `gamemd.exe` (Yuri's Revenge 1.001). Write C++ that the
original compiler turns into that function's exact bytes. MATCH means
`tools/check.py` printed MATCH; nothing else counts.

## Workspace

- Work only in `build/spike/<ADDR>/` (ADDR as `0x00410A60`). Put your source there,
  named after the module `tools/ctx.py` reports (`abstype.cpp`).
- A header you need to change: copy it from `include/` if it is there, else from
  `reference/tibsun/code/`, into `build/spike/<ADDR>/include/` and edit the copy.
- Never edit `src/`, `include/`, `reference/`, `data/`, `tools/` or `docs/`. Other
  agents work at the same time; the coordinator merges results.
- Keep helper scripts and variant files in `build/spike/<ADDR>/` too: the scratchpad is
  shared. No two files may differ only in case: under Wine they write one `.obj`, and
  the compile fails with only the file name in the log.
- The session scratchpad is shared with the coordinator and other agents: delete only
  files you created, one by name.
- A scratch root copies the files it changes; never symlink `orig/`, `toolchain/`, `src/`,
  `include/` or `data/` into it: a write or delete through a link reaches the real repo.
- No recursive delete of any kind, anywhere. The project hook
  (`.claude/hooks/no-recursive-delete.py`) blocks `rm -r`, `find -delete` (except
  `-type d -empty`), `find -exec rm`, `xargs rm`, `rmtree`, `git clean` and
  `rsync --delete`. A blocked delete is a stop sign: never route around it.
- No git commands at all, not even read-only ones; `.claude/hooks/no-git-for-agents.py`
  blocks them. The tree is shared: one agent's stash takes every agent's uncommitted work.
- Before you finish, delete the scratch you made that nobody needs (probes, logs, variant
  copies): `rm` on the files by name or by a glob in one directory, then `rmdir` the
  empty directory. Keep your best source and its header overlay.

## Loop

1. `uv run tools/ctx.py <ADDR>`: extent, module, names, annotated disassembly,
   callers, and the TibSun file to start from.
2. If a TibSun file is listed, find the function there that does what the disassembly
   does (same calls, strings, shape) and start from it, with its `#include` lines. YR
   changed many classes: member offsets are in `data/layouts.csv` (YRpp's names), and
   `include/abstract.h` and `include/abstype.h` show how YR headers differ from TS.
   No TibSun file listed: grep `reference/tibsun/code/` for the callees and strings
   anyway (ctx.py has missed one), then write the function from the disassembly.
   Headers in `include/` for these classes are proven against the exe (size, member
   offsets, every vtable slot): AbstractClass, AbstractTypeClass, ObjectClass,
   MissionClass, RadioClass, TechnoClass, BuildingClass, ObjectTypeClass,
   TechnoTypeClass, BuildingTypeClass, BaseClass, TeamClass, AnimClass, AnimTypeClass,
   UnitTypeClass, UnitClass, InfantryTypeClass, AircraftTypeClass, TeamTypeClass,
   HouseTypeClass, WarheadTypeClass, BombClass, ParasiteClass, LineTrail, ScenarioClass,
   Random2Class, ColorScheme, AircraftClass, BulletClass, WeaponTypeClass,
   BulletTypeClass, EventClass, the spawn, slave, capture and temporal managers,
   AirstrikeClass, EBolt, and the shared
   ones batch 1 gave YR's layout (RulesClass, HouseClass, CellClass, FootClass, the file
   classes, the gadgets, the surfaces). Include them
   rather than writing stand-ins; `uv run tools/headercheck.py <Class>` proves any header you fix in
   your overlay.
3. Mark the definition with `// FUNCTION: GAMEMD <ADDR>` on the line before it (reccmp's
   annotation format). The function must not be `static`: an unused static is discarded
   and the checker finds nothing. A function with no definition in the file takes its name
   on the comment line after the marker, with nothing after it:
   - a header inline the object emits out of line: `// FUNCTION: GAMEMD <ADDR>` then
     `// Class::Name`;
   - a template instance: `// TEMPLATE: GAMEMD <ADDR>` then its decorated symbol
     (`// ?Add@?$DynamicVectorClass@H@@QAE_NABH@Z`, as check.py prints it) or the short
     form `// ?$DynamicVectorClass@H::Add`, plus `template class X<T>;` to force
     emission; VC6 ignores `template bool X<int>::f(...)`;
   - a compiler-generated destructor: `// SYNTHETIC: GAMEMD <ADDR>` then
     `// ??_GClass@@UAEPAXI@Z`; a static initialiser: `// SYNTHETIC: GAMEMD <ADDR> SYMBOL`
     then `// _$E12`.
4. `uv run tools/check.py build/spike/<ADDR>/<module>.cpp <ADDR> --overlay build/spike/<ADDR>/include`
   check.py merges the archive's headers, include/ and the overlay into one directory,
   so an overlay header replaces its include/ copy for nested includes too.
5. Read the diff, change the source, repeat.

Stop at MATCH, or after 30 checks without the score rising, or after 45 minutes (for a
function over 2,000 bytes: 60 checks or 90 minutes).
Every compile counts as a check, including those your own scripts run.

## Rules

- The flags are fixed (`/O2 /Ob2 /Gr /GR /MT /DWIN32`). No inline assembly, `__emit`,
  `#pragma optimize` or `auto_inline`, hard-coded addresses or `volatile` tricks, except
  Westwood's own inline asm, copied unchanged from the TibSun archive (header or .cpp).
- Name callees and globals as `tools/ctx.py` names them (YRpp or TibSun names); an
  unnamed one is `FUN_<ADDR>` / `DAT_<ADDR>`. `data/symbols.csv` labels several one-line
  UI functions `UI::GetResource`; 0x00622B50 is `UI::StandardWndProc`.
- Declare each callee with the calling convention the disassembly shows. A member
  function is thiscall already; writing `__thiscall` is error C4234. A free or static
  function is `__fastcall` by default under /Gr (first two arguments in ECX and EDX), so
  a CRT function or a `qsort` comparator must be declared `__cdecl`. ECX alone does not
  prove a member: a free `__fastcall` function also takes its first argument there.
- `WRONG TARGETS` means the bytes match but a call or global is declared as the wrong
  thing: fix the declaration, not the code.

## What has worked

Each entry: what the diff showed, then the source change that fixed it.

### Reading the diff
- The percentage compares disassembled instructions. A frame-size change alters every
  `[esp+N]`, and a length change disassembles jump tables as code, so better source can
  score lower. In a large function judge by byte length and by the instruction shapes.
- Write the whole body, every repeated block included, before tuning registers.
- `FUN_`/`DAT_` names are not checked against their address, so a MATCH can still have
  two globals swapped. Read the reference lines (`+0x.. NAME: original 0x..`).
- Inlined code often has an out-of-line copy or a twin elsewhere; match that first.
  VoxelAnimClass::VoxelAnimClass (0x007493B0) has AnimClass's bounce code.

### Declarations
- A callee returning a class by value takes a hidden buffer: a virtual pushes only the
  buffer (`virtual Coord F(void)`); a free `__fastcall` one gets it in ECX.
- A varargs function is always `__cdecl`. Caller-popped pushes around an ECX call may
  be the arguments of a following varargs call, not of the ECX one.
- A callee returning a double in st0 is `double __cdecl F(double)`. A pushed double
  ending `...60000000` (`0x3FF6666660000000`) is a widened float literal: write `1.4f`.
- `setne` on a callee's result means the callee returns `unsigned char`, not `bool`.
  `cmp; sbb; inc` is an unsigned compare: the value is unsigned.
- An 8-byte Point2D argument is passed on the stack. `or ecx,-1 / or eax,-1 / push`
  comes from a by-value default argument, `F(Point2D p = Point2D(-1, -1))`.
- `obj.F()` on an object, not a pointer or reference, calls a virtual directly; so does a
  virtual on `this` inside a constructor. Call through a pointer to keep it virtual.
- `DAT_` globals packed together are often one object's fields (SessionClass::Instance
  near 0xA8B238): declare the object, with real offsets, as a static member.
- `#undef LoadString` after `<windows.h>`. Include `<stddef.h>` and `"always.h"` before
  `stringtable.h` so `wchar_t` is defined.

### Evaluation order
- Arguments are evaluated right to left. To call a nested callee first (GetDlgItem, an
  INI read), put its result in a temporary.
- For `a * b` on doubles the right operand loads first: write the operands in the
  reverse of the disassembly's load order.
- Within one expression VC6 makes a call before plain loads. If the original loads values
  into callee-saved registers before a call, load them in earlier statements:
  `int key = p->Key; int old = t[i]; t[i] = old + p->F(key);`. One such statement can
  fix the whole function's register allocation.
- A load order no rewording moves: look at the inline constructor's parameter types.
  `CellStruct(int x, int y)` keeps adds 32-bit (`lea`); short parameters narrow them.
- `(*this)[Count++] = x` through an inline `operator[]`, or
  `int n = Count; Count = n + 1; V[n] = x;`, stores the count first; `V[Count++]` does not.
- `c = Coord(pt, 0); c += r;` gives a zeroed-register Z store and load/add/store order;
  direct initialisation or `c = c + r` does not. A named `Coord pt` added with `+=`
  fixed a double X store that `predicted - (Coord(pp, 0) + coord)` gave.
- x87 loads of a Coord difference in Z, Y, X order: the difference in a named local
  inside a file-local helper. `Distance_To` and `(a - b).Length()` load Y, Z, X.
- `Map[Adjacent_Cell(Get_Cell(), face)]` with no named Cell local keeps the original's
  add order; a named local loads the adjacent offset first.
- A sum used once stays inline in its expression; a local for it reschedules the code.
- A field stored twice, or stored through a register: VC6's aliasing of writes through
  casts, e.g. `(Point2D &)rect = ...; ((Point2D *)&rect)[1] = ...;` (0x006D2DE0).

### Branches and returns
- `return strcmp(a, b) == 0;` gives `sete`; the branching form is
  `if (strcmp(a, b) != 0) return false; else return true;`.
- Early returns that merged into `setne`: write nested `if` blocks. Conversely
  `return (A && B && C) ? 1 : 0;` keeps every temporary alive, each in its own slot.
- A `goto` to the shared tail and a `return` compile differently. A tail emitted twice
  where the original has one: TibSun's early returns turned into `goto done`.
- An instruction one place out of order around a ternary: move the shared operation into
  each arm (`c ? a - h : b - h`, not `(c ? a : b) - h`; 0x004D9C60).
- Part of a computation tail-merged across branches: compute it inside each branch,
  so the common half merges and the rest stays (0x0051FB00).
- `if (a || b) X; else Y;` has one fixed block order however it is written. For the
  other order, make both tests reach one block: `if (a) goto none; if (b) { none: X }`.
- `!p ? 0 : p->Count` and `p ? p->Count : 0` give opposite branch polarity.
- `setcc; test; jne` on one register is an inlined function returning bool.
- A sparse switch that the original compiles to a byte-index jump table: list every
  in-range case and send the no-op ones to a label after the switch with `goto`;
  `break` lets VC6 drop them and emit a compare chain.
- `bool b = unit->F() != 0;` for a bool virtual that compiles to `setne; test`.
- Write identical tails out in full in each branch, null tests included: VC6 merges them
  itself and then allocates registers as the original does (0x006F6AC0).
- A clamp that merges two stores or lands in the wrong register: `MAX(v, 0)` where the
  ternary `(v <= 0) ? 0 : v` failed (0x0047F250).

### Registers
- ESI and EDI swapped with every other byte identical: add one real use of a variable
  and see whether the allocation flips; if it does, look for the use the original had.
- Before blaming register allocation, compare the tail and the length: one missing
  statement (an `else` branch) moves every register after it.
- VC6 ranks variables by use count, weighted by branch; one use more or fewer moves a
  variable into or out of EBX/EBP. Re-read a member instead of a local, or the reverse.
- An induction variable that starts at a member offset (`mov ebp,0x15d8 ... cmp
  ebp,0x1618`) comes from writing `Type->Arr[i]` in full at each use. A cached pointer
  gives a zero-based index plus a displacement. Global tables likewise: `T[k].X` each time.
- A search loop matched with `Node *n = &Nodes[i];` and the loop-invariant hoisted into
  a local, where repeated `Nodes[i]` did not.
- Use parameters directly instead of copying them to locals: VC6 then packs a later
  local into the dead argument slot, as the original does.
- Block structure moves registers far from the change: one loop filling two arrays
  instead of two loops, or unrolled `EnableWindow(GetDlgItem(...))` calls instead of a loop.
- Moving a few statements into a small function that /Ob2 inlines changes allocation
  around them; try it when the original treats a block as separate.
- Declare variables used across `case`s once at function scope, TibSun style.
- A global the original loads before a branch: copy it to a local with two uses. One use,
  or the global read directly, leaves the load after the branch (0x005FF850).

### Stack frame
- Locals are laid out by size, not declaration order. To fix two objects' order, make
  them members of one local struct.
- A buffer declared inside the loop body gives `push ecx`; one at function level does not.
- One variable shared by two branches shares a slot; one per branch gets its own.
- A frame too big by a fixed amount with every `[esp+N]` matching: a local buffer has the
  wrong size (name[200], not [256], in 0x00542620).
- A call's return temporary gets a function-level slot; one inside an inlined body is
  scoped to it and can share a slot (0x004361D0's 8-byte excess).
- A class returned by value: a temporary and a named local give different frames
  (Wstring in 0x0046DFD0 needed the temporary).
- `Coord c = p->F();` copies from the return pointer; a `const &` argument does not.
- A local can share a dead argument slot depending on which statement it is declared
  before (0x006CB560: `bool charged` before `UIName = ...`).
- VC6 never schedules a store above the end of an inlined constructor's stores: wrap
  fields whose store order is off in a small struct with one (Tactical's two bools).
- A frame 8 bytes too big is often a Coord temporary the original overlaps with a dead
  slot: pass it by value into a file-local inline helper (`Coord_Pixel` in 0x0073CEC0).

### Inlining
- A TibSun out-of-line function may be inline in YR, or the reverse (a function placed
  after its neighbours). If ours calls what the original inlines, define it `inline` first.
- What the original calls (constructors, `Add`) stays a call when declared without a body.
- The original mixes call and inline forms of one function: wrap one use in a one-line
  inline helper. Helpers nested too deep stop inlining and become real calls.
- A nested call left out of line under an inlined wrapper (`Cell_Terrain()` around
  `Cell_Find_Object`) is the inline budget: call the wrapper, as TibSun does. If both
  levels still inline, wrap other calls in the function in file-local inline helpers
  until it stays; each helper must contain a call (Preview_Cell_Color needed three).
- A callee the original inlines that lives in the same module is defined in that file
  (`Do_AnimTypes`, `Read_X`); define it there and VC6 inlines it.
- Each function has an inline budget, spent from the end of the function backwards:
  moving one member's cleanup into a file-local inline helper shifts which stay calls,
  and a late expression's shape decides whether an earlier call inlines
  (`Clear_Occupy_Bit(Coord(Position))`, not `PositionCoord`, in 0x0051DBD0).
- Nested inline decisions depend on how many expansions a function already holds; a
  code-neutral inline helper can flip them.
- A plain function defined above its callers is inlined and also emitted; at /Ob2 VC6
  can also inline a member defined later in the same file, so a newly landed neighbour
  can break a matched caller (0x006CB560). One too big
  to inline: a file-local `static inline` helper holds the body, and the member is a
  one-line plain wrapper (`_Conscript` in team.cpp).
- An inline helper written as a template can crash VC6 (internal compiler error); write
  one overload per type.
- The shape of repeated inlines matters, not only their count: a nested ternary left
  room for a later timer constructor that a multi-statement body pushed out of line.
- Calls in an unused local spend the inline budget too: three `Count()` calls kept the
  `Cell(int, int)` constructor a call in 0x004684E0.
- `member.Type::Type();` in a constructor body builds a member with no null test and
  after the statements before it; placement new adds the test.

### Constructs
- `(x & 2) >> 1` that the exe computes as `and eax,2; shr eax,1`: write
  `(char)(x & 2) >> 1`; without the cast VC6 folds it differently.
- A double expression compared with a float member: cast it to `(float)` for the
  exe's `fcomp dword ptr [mem]`; without the cast VC6 emits fld/fcompp.
- A non-void function the original lets fall off its end (ObjectClass::IronCurtain is a
  bare `ret`): an empty body, with warning 4716 disabled in a push/pop around the class.
- An inline constructor the original calls out of line in one function only: route the
  call through a file-local inline helper (foot.cpp's Action_Line_Rect).
- Load-all-then-store-all of a struct copy (an RGB colour): copy through a named
  temporary (`RGBClass c = ...; x->Color = c;`); a direct assignment interleaves.
- A loop whose inline helpers keep their own null tests: put the test in the `while`
  condition; a `break` inside the loop lets VC6 fold them.
- A clamp the original writes as compare-and-store (`if (x < 0) x = 0; return x;`);
  an early `return 0` gives setl/dec/and.
- Deleting destructor (`test flags,1`, then `operator delete`; callers `push 1` to a
  vtable slot): declare the destructor virtual and annotate `~Class`; check.py tries
  `??1`, `??_G` and `??_E`. `??_G` is emitted only if something `delete`s the class.
- A destructor that stores no vptr before calling the base destructor means the class
  declares none: remove the declaration. Even an empty declared one adds the store.
- Dialog procedures: `LOWORD(wparam)`/`HIWORD(wparam)` keep compares unmasked and put
  msg and wparam in the original's registers. Jump tables land in the function's tail.
- A for-loop variable outlives its loop (old scope rules): give each loop its own name.
- A constructor storing two vptrs (base, then derived) does its work in the base
  constructor; the derived one is empty. Init list versus body moves the vptr store.
- A function-local static whose guard is re-tested at each use sits in an inline accessor.
- The vptr a constructor or destructor stores last names its class: read the RTTI at
  vtable minus 4 (`Exe.vtables()` in headercheck.py) before naming one.
- Static initialisers and atexit stubs are named `$E<n>`, numbered across the whole
  file: after any header or file change, re-read the number from the checker's listing.
  Name each one exactly (`// _$E2` with SYMBOL); two claims on one function fail checkall.
- A template instance's element type comes from its vtable's RTTI, not from look-alike
  code: palette's vectors are `HSVClass`, which compiles the same as `RGBClass`.
- A free fastcall `operator==` whose body jumps to itself comes from a member operator==
  that is non-const: a const operand falls through to the free one (HashString).
- A file-static helper the compiler emits only for its callers (team's `_Is_It_Breathing`)
  lands with its first caller, not before.
- A large function that many small ones wait on is worth a dedicated attempt: Init_Commands
  matched first time and brought 298 emitted command-class functions with it.
- A class's constructor, destructors and virtuals are emitted where the class is built:
  never write a function that `new`s it to emit them (the command classes wait on Init_Commands).
- A local struct cast over a real object (`((ScenMapOps *)&Map)->...`) is a stand-in too:
  declare the member in its own class's header, with TibSun's name where TibSun has it.
- So is a `DAT_` alias or byte offset into a known object (`DAT_00A8DA90` for Session.Slots):
  reach it through the object's members.
- Label every template instance from the RTTI of the vtable that holds it: two addresses
  under one symbol (`VectorClass<int>::Clear`) mean one label has the wrong element type.
  For a virtual, that is the vtable whose slot points at it: a derived template's deleting
  destructor stores only the base vptr (TypeList's is byte-identical to VectorClass's).
- check.py fails a template label whose address sits in a vtable of another instance, by
  RTTI; a non-template class mismatch is a warning, and checkall counts them.
- A module's header initialisers (geometry.h's run, the 23-byte initialiser) start its
  range: the run after a module's last function belongs to the next file.
- Byte-identical template instances with no vtable (`_com_ptr_t::_Release`) can be folded:
  label one only from a caller whose type proves it, or leave it unlabelled.
- A null test then `push 1; call` on a class with no virtual destructor is `delete p`:
  annotate the callee as SYNTHETIC `??_GClass@@QAEPAXI@Z`, never a new member.
- One address called with two argument types is two overloads the linker folded: declare
  both (HouseClass::Get_First_Ownable takes unit and building type vectors at 0x00505310).
- A class's RTTI also names its real base: LoadProgressMgr derives from INoticeSink, which
  retired a stand-in callback class that only matched through a cast.
- A call kept out of line only by a declared, never-defined overload (a `User const *`
  copy of wonline's FUN_0077D4C0) is a stand-in: bank the function.
- `return (a && b);` in a bool function gives `mov eax,1` / `xor eax,eax`; `if` with
  `return 1;` and `return 0;` gives `mov al,1` (MPUnholyAllianceClass::vt_entry_14).
- YRpp's enum values can be wrong for 1.001 (ACTION_REPAIR is 10, EATEN 11): prove an enum
  value from a switch or compare in the exe before trusting it.
- An early `return` that the original tail-merges with a real branch: assign the result in
  each branch and keep one `return(action);` after the if/else, as TibSun writes it.
- Never chain inline wrappers to run out VC6's inline depth (Find_Data0..7): only helpers
  the original plausibly had are source, so such a match is banked, not landed.
- The reverse, when inlined tails merge into the wrong arm's copy: end each arm with its
  own `return;`, which moves the shared copy to the last arm (ZBuffer::Pan).
- VC6 spends the inline budget from the end of a function backwards, and every inlined call
  costs the same: write the accessor calls the original made (planmgr's Get_Count, Set_Event).
- An inline call the original leaves out of line goes out of line when wrapped in a file-local
  inline helper one level deeper (bullet's Coord_Dist, building's _Usable).
- An unnamed `seg_` range that opens with a header run is a module of its own: it lands in
  `src/<previous module>2.cpp` (3 for a second such range), a provisional name that keeps the
  linker's alphabetical order. One with no header run is the end of the module before it and
  lands in that module's file.
- A function start missing from data/functions.csv is fixed in tools/functions.py, then the
  CSV regenerated; never edit the CSV by hand.
  A score where every byte matches up to a `ret` and only a short tail after it differs is
  that case: the next function hides in this one's extent (saveload's Get_Player_Name).
- A run of about 13 small initialisers per file (CELL_LEPTON_DIAG through COORD_NONE)
  is `include/geometry.h`'s per-file constants: include it where the exe's `_$E` numbers
  put it (usually right after `always.h`; after `wdtnet.h` in wdtconflict and wdtstate),
  before any `extern` of the same names, and mark them; never write `Init_` or
  `ShortPair`/`IntTriple` stand-ins.
- TibSun's `dynamic_cast<ObjectClass *>` is `AbstractFlags & 2` in YR.
- An atexit stub with no `$S` guard byte destroys a plain global, not a static member
  (`Kamikaze KamikazeInstance;`, not `Kamikaze::Instance`).
- Consecutive static members with destructors share one `$S` guard byte; a function
  definition between two of them starts a new byte. Order definitions to match the
  original's guard bits (0x006A4C80).
- A range named for one class can hold another's code (the side range is ship
  locomotion, the ramfile range RadSiteClass): read the vtable and RTTI first.
- A pushed `__FILE__` and line: `#line 215 "D:\\ra2mdpost\\AbsType.cpp"` before the
  statement. A string's first-character test: `if (*name)`, not `if (name[0])`.

### Headers
- headercheck matches members by offset, not name: leave padding gaps undeclared. A
  member named `pad`/`align`, or inside YRpp's `align_*` bytes, is flagged; to fill a
  gap use unnamed bitfields (`unsigned char : 8`, `unsigned int : 32`).
- MSVC never reuses a base's tail padding. Where the CSV and the exe disagree, the
  offsets the constructor and Read_INI store to are the authority.
- A bool YR keeps in a 4-byte field becomes `BOOL`. `__declspec(align)` and anonymous
  unions do not compile; use a plain struct for a union-shaped member.
- Constness differing from the base (`Read_INI(CCINIClass const &)` against
  `CCINIClass &`) adds a slot instead of overriding: count one high, address mismatch.
- "inherited in the exe, overridden in ours": delete our override; "overridden in the
  exe, inherited in ours": add one. A TS virtual with no YR slot becomes non-virtual.
- The linker folded every `return 0` function into `_purecall`, so an exe slot holding
  it may be a `return 0` body, not a pure virtual.
- TS sizes YR changed: StageClass 24 to 28 bytes (`stage.h`), BANIM_COUNT 13 to 21
  (`banim.hh`), attached particle systems 5 to 8, weapon slots 3 to 18.
- Keep TS member names at YR's offsets; use YRpp names only for members TS lacks.
- YR signatures that differ from TS: Take_Damage has 7 arguments, Is_Moving_Onto_Bridge
  and Destination_Coord take a `TechnoClass *`; MissionClass slot 0x1E8 is
  `void QueueMission(MissionType, bool)` and 0x1EC is `bool Commence(void)`.
- MSVC lays out overloaded virtuals of one name in reverse declaration order: declare
  them in the reverse of the exe's slots (RadioClass's three Transmit_Message).
- An unknown virtual: `headercheck.py <Class>` prints every slot with its name; look the
  offset up there before adding a declaration.
- YRpp's parameter order can be wrong: trust the original's push order
  (VocClass::PlayGlobal takes volume before panning).
- A callee overloaded in the exe but only one overload referenced in a file: give both
  overloads a names.csv row with their decorated symbol, or check.py resolves by name.
- A function that matches YRpp's address may be TS's under another name (FindInfantry-
  Subposition is Closest_Free_Spot): compare with TibSun before adding a member.
- An unverified reference proves nothing: a wrong offset into a global object whose
  address check.py does not know passes. Give each object you reach into a names.csv
  row; `uv run tools/refaudit.py` lists symbols whose references imply different bases.
- Code ported from TibSun brings Westwood's `/* ** */` comments and commented-out code:
  cut them to the comment rules (two `//` lines of behaviour) before landing.
- `#define NDEBUG` before the includes is how a file whose original has its asserts
  compiled out is built (ownrdraw.cpp, wonline.cpp).
- A match that flips with unrelated header declarations is VC6 ordering spill reloads by
  heap address: sweep dummy declarations and keep the form that fails at fewest offsets.
- When no body form helps, moving real declarations the file already uses to before or
  after the function shifts its heap offsets too (wonline 0x00788180); never add unused ones.
  One move may not be enough: move several together (six held it against a new inline).
  Removing declarations nothing uses works too (nine held it); results jump with set size.
- A shared-header layout change can flip it as well: an equally true form with fewer new
  names (`int X[64][2]` rather than a new pair struct) held wonline 0x00788180.
- A named reference to an object indexed more than once (`CellClass & center = Map[cell]`)
  can pin the order: it took Generate_Rural_Buildings from 24 of 64 failing offsets to 0.
- Never remove a function data/progress.csv records from src/: report the stand-in instead.
- An unverified global can be the wrong object: give each vector a function reads a
  names.csv row (Good_LZ read `Technos` where the exe walks `Objects`, 0x00A8E360).
- One class, one definition: include the header that defines it (SHPStruct is msanim.h's).
- A TS layout can agree with YR's for a prefix only: run headercheck before porting a
  function that reaches past a class's proven part.
- Where YRpp groups members into a struct and TS has flat fields, use a nested struct
  with TS's field names (ScenarioClass's lighting).
- A YRpp row can merge two classes (MapSeedClass with the generator): trust the
  constructor and the size passed to `new` over data/layouts.csv.
- Porting a stub-based match: drop the overlay and map stub members onto the real
  header by offset, keeping TS names. Most then match in one or two checks.
- Byte-identical twins exist (Wstring and WideWstring's constructors, 0x7B4620 and
  0x7B66C0): the neighbouring functions (char vs wchar_t) decide which class owns which.
  Byte-identical overloads in one file (Get_Field's char& and unsigned char&) each take
  the next unclaimed symbol in file order, so mark them in their source order.
- YR made some TS text members wide: OwnerDraw's CellData holds `WideWstring`, so
  `include/ownrdraw.h` overrides the TibSun copy.
- A function that lands makes its callers' references to it checkable: re-run checkall
  after `--record` (Seed_Land called 0x0057A0C0 under Build_All_Cliffs' name; Input_Name
  calls 0x0054F530, not To_ASCII).
- A new header with a static object renumbers another file's `$E` markers (empulse.h
  moved startup.cpp's by one): renumber the markers rather than undo the header.
- Wide string literals are pooled across files (0x0082083C is `L" "`): a constant
  stored into a temporary is not necessarily a vtable.
- `volatile` is allowed only on data another thread writes (the mouse thread's flags), where
  the exe re-reads it; never on a local or to force a match (CLAUDE.md lists the exceptions).
- Westwood's inline asm from the archive stays where Westwood put it, unchanged (blitblit.h,
  xsurface.cpp's surface_quick_fill); check.py refuses any asm block not found verbatim there.
- The message list is `Session.Messages`, not a global; Add_Message takes YR's 7 arguments.
- A static member with a constructor added to a widely included header (`dsurface.h`)
  renumbers the `$E` markers of every file that includes it: declare it extern in the user.
- YR's PassabilityType has PASSABLE_BEACH at 3 (8 values); Draw_Voxel takes 10 arguments;
  Record_The_Kill takes `HouseClass *`.
- Inside a member function, a global the class shadows needs `::` (`::Buildings` in a
  HouseClass member); otherwise the member is read and only the offsets differ.
- YR signatures: Exit_Coord takes an int index; Shroud_The_Map and Reveal_The_Map take a
  `HouseClass *`; EventClass's MegaMission constructor has 6 arguments.
- LightSourceClass has a DetailLevel at +0x34, compared with `Options.DetailLevel`.
- Every static data member a header declares, even a `static int`, uses up one `$E` number
  in each file including it; static member functions do not. Statics only the class's
  own .cpp uses stay out of the shared header: that file declares the full class itself.
- An inline constructor or destructor in a shared header on a class with vector members
  shifts `$E` and `$S` numbers in every includer: declare it, define it inline in its file.
- A header that needs a type only by reference forward-declares it (`class Wstring;`):
  a new `#include` there flipped wonline 0x00788180; the .cpp includes the full header.
- An `#include` a new function needs goes after the file's last global definition: in
  the include block its headers' static members renumber every `$E` marker after it.
- A class only one module uses (MSFrameAnim in wdtmap) is declared in that .cpp, which
  keeps it out of shared headers whose changes flip heap-sensitive matches.
- Read a matched caller's reference lines before declaring a destructor: wdtmap's
  0x0040D300 is BasicTimerClass<FrameTimerClass>'s, not StageClass's.

## Known dead ends

- Reordering local declarations, or `const` on parameters: never moved anything.
- Loop forms (unsigned, short, `do`/`while`, a slot pointer) to rebase an induction
  variable.
- Parameter types (int, unsigned, short, `__int64`, struct) to push registers instead of
  immediates.
- Rewording an expression to change its load order.
- A varargs member to explain an ECX argument plus caller-popped pushes.
- `delete this` in a deleting destructor: it adds a virtual call and a null test.
- For 0x00421EA0's x87 order: removing uses of 0, Vector3 constructor or member forms.
- An EBP frame pointer in a function with no floating point (0x0047EFE0): nothing tried
  made VC6 emit one.
- Keeping a small same-file callee out of line where other callers inline it (txtlabel2
  0x0072D730, 0x0072E450, 0x0072F540): helpers, budget fillers and call forms all failed.
- `DynamicVectorClass<T>`'s constructor and Add are called for some T and inlined for
  others in the original; no instantiation order tried reproduces it.

## Report

Return: the result (`MATCH`, `partial` or `failed`), the best percentage the checker
printed, how many checks you ran, the path of your best source, the kind of blocker if
not matched (for example "register allocation", "unknown callee", "header layout"),
and what you learned that would help the next function, in a sentence or two.
