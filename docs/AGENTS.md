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
  files you created, by name. Never run a delete over the whole scratchpad.
- Before you finish, delete the scratch you made that nobody needs (probes, logs, variant
  copies) with `rm` on files or `find DIR -type f -delete`, never a recursive force
  delete. Keep your best source and its header overlay.

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
   Random2Class, ColorScheme, and the shared
   ones batch 1 gave YR's layout (RulesClass, HouseClass, CellClass, FootClass, the file
   classes, the gadgets, the surfaces). Include them
   rather than writing stand-ins; `uv run tools/headercheck.py <Class>` proves any header you fix in
   your overlay.
3. Mark the definition with `// FUNCTION: <ADDR>` on the line before it. The function
   must not be `static`: an unused static is discarded and the checker finds nothing.
   A function a header defines, which the module's object emits out of line (an inline
   constructor, a compiler-generated destructor), takes `// FUNCTION: <ADDR> Class::Name`
   with no definition after it. A template instance takes its mangled class name
   (`// FUNCTION: 0x0063E540 ?$DynamicVectorClass@H::Add`) and `template class X<T>;`
   to force emission; VC6 ignores `template bool X<int>::f(...)`.
4. `uv run tools/check.py build/spike/<ADDR>/<module>.cpp <ADDR> --overlay build/spike/<ADDR>/include`
5. Read the diff, change the source, repeat.

Stop at MATCH, or after 30 checks without the score rising, or after 45 minutes (for a
function over 2,000 bytes: 60 checks or 90 minutes).
Every compile counts as a check, including those your own scripts run.

## Rules

- The flags are fixed (`/O2 /Ob2 /Gr /GR /MT /DWIN32`). No inline assembly, `__emit`,
  `#pragma optimize`, hard-coded addresses or `volatile` tricks.
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
  direct initialisation or `c = c + r` does not.
- A sum used once stays inline in its expression; a local for it reschedules the code.

### Branches and returns
- `return strcmp(a, b) == 0;` gives `sete`; the branching form is
  `if (strcmp(a, b) != 0) return false; else return true;`.
- Early returns that merged into `setne`: write nested `if` blocks. Conversely
  `return (A && B && C) ? 1 : 0;` keeps every temporary alive, each in its own slot.
- A `goto` to the shared tail and a `return` compile differently.
- `if (a || b) X; else Y;` has one fixed block order however it is written. For the
  other order, make both tests reach one block: `if (a) goto none; if (b) { none: X }`.
- `!p ? 0 : p->Count` and `p ? p->Count : 0` give opposite branch polarity.
- `setcc; test; jne` on one register is an inlined function returning bool.

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

### Stack frame
- Locals are laid out by size, not declaration order. To fix two objects' order, make
  them members of one local struct.
- A buffer declared inside the loop body gives `push ecx`; one at function level does not.
- One variable shared by two branches shares a slot; one per branch gets its own.
- A class returned by value: a temporary and a named local give different frames
  (Wstring in 0x0046DFD0 needed the temporary).
- `Coord c = p->F();` copies from the return pointer; a `const &` argument does not.

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
- Each function has an inline budget, spent from the last member destructor backwards:
  moving one member's cleanup into a file-local inline helper shifts which stay calls.
- Nested inline decisions depend on how many expansions a function already holds; a
  code-neutral inline helper can flip them.
- `member.Type::Type();` in a constructor body builds a member with no null test and
  after the statements before it; placement new adds the test.

### Constructs
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
- A TS layout can agree with YR's for a prefix only: run headercheck before porting a
  function that reaches past a class's proven part.
- Where YRpp groups members into a struct and TS has flat fields, use a nested struct
  with TS's field names (ScenarioClass's lighting).
- A YRpp row can merge two classes (MapSeedClass with the generator): trust the
  constructor and the size passed to `new` over data/layouts.csv.
- Porting a stub-based match: drop the overlay and map stub members onto the real
  header by offset, keeping TS names. Most then match in one or two checks.

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
- `DynamicVectorClass<T>`'s constructor and Add are called for some T and inlined for
  others in the original; no instantiation order tried reproduces it.

## Report

Return: the result (`MATCH`, `partial` or `failed`), the best percentage the checker
printed, how many checks you ran, the path of your best source, the kind of blocker if
not matched (for example "register allocation", "unknown callee", "header layout"),
and what you learned that would help the next function, in a sentence or two.
