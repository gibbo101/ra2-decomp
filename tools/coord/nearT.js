export const meta = {
  name: 'batch-0-small-functions',
  description: 'Match 168 small functions in 12 jobs over 66 named modules, alongside batch 120 into src/: Sonnet, a header merge, then Opus',
  phases: [
    { title: 'Opus', detail: 'one agent per module or group of modules, on its banked near misses', model: 'opus' },
    { title: 'Header merge', detail: 'one agent lands every requested shared-header change and the functions it gates', model: 'opus' },
  ],
}

const FN = {
  type: 'object',
  properties: {
    address: { type: 'string' },
    result: { type: 'string', enum: ['MATCH', 'partial', 'failed'] },
    best_percent: { type: 'number' },
    checks: { type: 'integer' },
    blocker: { type: 'string' },
    note: { type: 'string' },
  },
  required: ['address', 'result', 'best_percent', 'checks', 'blocker', 'note'],
}
const RESULT = {
  type: 'object',
  properties: {
    module: { type: 'string' },
    functions: { type: 'array', items: FN },
    header_changes: { type: 'string' },
    needs_other_headers: { type: 'string' },
    notes: { type: 'string' },
  },
  required: ['module', 'functions', 'header_changes', 'needs_other_headers', 'notes'],
}
const MERGE = {
  type: 'object',
  properties: {
    headers_changed: { type: 'string' },
    landed: { type: 'array', items: { type: 'string' } },
    functions: { type: 'array', items: FN },
    checkall: { type: 'string' },
    notes: { type: 'string' },
  },
  required: ['headers_changed', 'landed', 'functions', 'checkall', 'notes'],
}

const REPO = 'REPO_PATH'
const files = mods => mods.map(m => `src/${m.toLowerCase()}.cpp`).join(', ')
const brief = (name, mods, extra, big) => {
  const names = Object.keys(mods)
  const one = names.length === 1
  const all = Object.values(mods).flat()
  return `Repository: ${REPO} (matching decompilation of Red Alert 2: Yuri's Revenge). Read CLAUDE.md, README.md ` +
  `(licence header) and docs/AGENTS.md first: its loop, rules, "What has worked" and "Known dead ends" apply, ` +
  `with these differences for this batch:\n` +
  `- Other agents work on other modules at the same time, including other batches on the OTHERS modules. ` +
  (one ? `You own module "${names[0]}".` : `You own the modules ${names.map(m => `"${m}"`).join(', ')} (several small ones, given to one agent).`) +
  ` Write each module's matches straight into src/<module>.cpp, lowercase (${files(names)}; create one if missing: README licence header, then the ` +
  `#include lines of reference/tibsun/code/<module>.cpp, or of the nearest TibSun file when it has none). Keep functions in address order, each ` +
  `with "// FUNCTION: GAMEMD 0x..." on the line before it (reccmp format; TEMPLATE/SYNTHETIC markers and name lines as docs/AGENTS.md step 3 says). Functions already annotated in those files must stay MATCH.\n` +
  `- Static initialisers (_$E<n>) that build header constants (CELL_LEPTON_DIAG through COORD_NONE, CELL_CENTER_COORD, RECT_NONE) are the module's own header run: ` +
  `#include "geometry.h" (and rect.h) and write bodiless SYNTHETIC markers with the numbers check.py reports. Never write local ShortPair/IntTriple/IntQuad ` +
  `structs, DAT_ doubles, FUN_004CAC40(pow(...)) or extern DAT_007E17xx for them, and never a cast over a real object, a local struct standing in for a real class, or a DAT_ alias of Session/Options: the coordinator rejects those.\n` +
  `- Check with: uv run tools/check.py src/<module>.cpp <ADDR>   (no overlay: what lands in src/ ` +
  `must match against include/ as it is). Do not run tools/checkall.py: other agents edit files while it runs.\n` +
  `- Headers: you may adopt (uv run tools/adopt_header.py <file>.h) and edit only the header TibSun's ` +
  `<module>.cpp mainly implements (usually <module>.h) for the modules you own; prove a class you change ` +
  `with uv run tools/headercheck.py <Class>, and keep every function already in src/ that uses it MATCH. Any ` +
  `other header change: put it in build/batch/${name}/include/, use --overlay to explore, do not land ` +
  `functions that need it, and describe the exact change and the evidence for it in needs_other_headers. A ` +
  `merge step will land those changes and the functions they gate, so a function that matches with your ` +
  `overlay should be banked as an overlay match, not worked further.\n` +
  `- Only checker-proven MATCHes stay in src/. A function you cannot match: remove it from src/, save your best ` +
  `attempt as build/batch/${name}/<ADDR>.cpp with a one-line comment of its best score (and whether it ` +
  `matches with the overlay), and report it.\n` +
  `- A scratch root copies what it changes and never symlinks orig/, toolchain/, src/, include/ or data/.\n` +
  `- No git commands of any kind, not even read-only: a stash or checkout takes the work of every running agent with it. Never a recursive delete of any kind; a project hook blocks them, and a block is a stop sign, never something to route around: ` +
  `delete only files you created, by name or a glob in one directory, then rmdir. Never delete another agent's files or anything in tools/coord/. Delete your own scratch before finishing, in the session scratchpad too, keeping the ` +
  `banked attempts and any overlay.\n\n` +
  `Your functions${big ? '' : ' (all under 1.5 KB)'}: ${names.map(m => `module ${m}: ${mods[m].join(', ')}`).join('; ')}.\n` +
  `Work through them in address order; start each with uv run tools/ctx.py <ADDR> and the TibSun source. Each is a near miss from an earlier round: read the note for its best score, blocker and banked attempt first.\n` +
  (extra || '') +
  `\nBefore finishing run uv run tools/check.py on each of ${files(names)}: every annotated function in ` +
  `them must print MATCH. Report every function you were given (${all.length}), with module set to "${name}".`
}

phase('Opus')
const sonnet = (await parallel(args.jobs.map(job => () =>
  agent(brief(job.name, job.modules, job.note ? `\n${job.note}\n` : '', !!job.model), { label: `${job.model || 'sonnet'} ${job.name}`, phase: 'Opus', model: job.model || 'sonnet', effort: job.effort || 'medium', schema: RESULT })
    .then(r => ({ module: job.name, mods: job.modules, sonnet: r }))))).filter(Boolean)

const open = sonnet.flatMap(r => (r.sonnet ? r.sonnet.functions : [])
  .filter(f => f.result !== 'MATCH').map(f => ({ ...f, module: r.module })))
const requests = sonnet.filter(r => r.sonnet && r.sonnet.needs_other_headers && r.sonnet.needs_other_headers.trim())
  .map(r => `### ${r.module}\n${r.sonnet.needs_other_headers}`)
log(`Sonnet matched ${sonnet.reduce((n, r) => n + (r.sonnet ? r.sonnet.functions.filter(f => f.result === 'MATCH').length : 0), 0)} of ${args.jobs.flatMap(j => Object.values(j.modules).flat()).length}; ${requests.length} modules asked for header changes`)

phase('Header merge')
let merged = null
if (requests.length) {
  merged = await agent(
    `Repository: ${REPO} (matching decompilation of Red Alert 2: Yuri's Revenge). Read CLAUDE.md, README.md and ` +
    `docs/AGENTS.md first. Other batches (OTHERS) may be running at the same time, their own header merges included. Before editing a shared header, ` +
    `re-read it and keep edits that are not yours; make small Edit calls, never rewrite a whole header. If checkall fails only in ` +
    `those batches' files, report it rather than fix it.\n\n` +
    `This batch's first tier ran one Opus agent per module or group of modules on earlier near misses; each could edit only its own module's header. They asked ` +
    `for these changes to shared headers (their overlays are in build/batch/<job>/include/, their banked ` +
    `attempts in build/batch/<job>/<ADDR>.cpp, where a job is one module or a group of small ones):\n\n${requests.join('\n\n')}\n\n` +
    `Unmatched functions from that tier: ${open.map(f => `${f.address} (${f.module}, ${f.best_percent}%, ${f.blocker})`).join('; ')}.\n\n` +
    `Your job:\n` +
    `1. Land each requested header change in include/ properly: verify it against the exe (tools/headercheck.py ` +
    `<Class>, tools/ctx.py, the disassembly) before taking it, reconcile requests that conflict, and give a class ` +
    `YR's real layout rather than a patch that fits one function. Never guess.\n` +
    `2. For each banked function that matched (or nearly) with an overlay: drop the overlay, check it against ` +
    `include/ with uv run tools/check.py src/<module>.cpp <ADDR> after moving it into its src/ module in address ` +
    `order with its // FUNCTION: GAMEMD marker (or TEMPLATE/SYNTHETIC), and keep it only if it prints MATCH. Delete a banked file once its ` +
    `function lands. Leave every other file in build/batch/ alone, including overlays from earlier batches: ` +
    `a banked attempt still needs its overlay.\n` +
    `3. Run uv run tools/checkall.py, but first check with pgrep -f 'tools/checkall.py|tools/refaudit.py' that no other batch's checkall or refaudit is running, and wait for it if one is: two at once overwrite each other's output and can hang Wine's compiler. No regressions against data/progress.csv are allowed. Do not run --record ` +
    `and do not edit data/; the coordinator records. Then, after your checkall has finished, run uv run tools/refaudit.py: for any symbol it lists that ` +
    `is one real object (not per-file constant copies, overloads, template instances or function-local statics), fix ` +
    `the offset error and report its address and evidence so the coordinator can add a data/names.csv row.\n` +
    `No git commands of any kind, not even read-only: a stash or checkout takes the work of every running agent with it. Never a recursive delete of any kind (a project hook blocks them; never route around it); delete only files you created, by name, including your own files in the session ` +
    `scratchpad, and never anything in tools/coord/. MATCH means the checker printed MATCH, with no inline asm, ` +
    `#pragma optimize, hard-coded addresses or volatile tricks, except Westwood's own, copied unchanged from a TibSun archive header.\n` +
    `Report: headers changed (with headercheck results), the addresses you landed, a row for every unmatched ` +
    `function above with its current best, and the checkall summary line.`,
    { label: 'header merge', phase: 'Header merge', model: 'opus', effort: 'high', schema: MERGE })
}
const landed = new Set(merged ? merged.landed.map(a => a.toUpperCase()) : [])

const count = r => (r.sonnet ? r.sonnet.functions.filter(f => f.result === 'MATCH').length : 0)
log(`Opus ${sonnet.reduce((n, r) => n + count(r), 0)}, merge ${landed.size}`)
return { merged, modules: sonnet }
