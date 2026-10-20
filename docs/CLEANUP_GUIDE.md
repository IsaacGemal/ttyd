# Cleanup guide

How to make matched code readable without changing a byte of the build. For cleanup workers and their reviewers.
Read it before docs/CONTEXT.md; CONTEXT holds the per-file precedents, this file holds the rules they add up to.

Each rule is tagged with where it comes from:

- **[M]** Melee practice: doldecomp/melee's CONTRIBUTING style guide, its cleanup index
  (`tools/cleanup_index.py`, rendered at doldecomp.github.io/melee/cleanup/) and its merged cleanup PRs. Local
  copy: `~/melee-cleanup` (sparse). The rules here are restated in our own words; Melee has no repository licence.
- **[T]** our own precedent, mostly from the 2026-10-19/20 Opus reviews recorded in docs/CONTEXT.md.
- **[M+T]** both projects arrived at it.

## 1. The invariant

- The linked build stays byte-identical (`build/G8MJ01/ok` is recreated), and every changed C file still compiles
  without `-DMUST_MATCH`. [M+T: Melee keeps a non-matching build too, which is why its hacks sit behind
  `MUST_MATCH`.]
- The assembly is ground truth. A name, type or comment that disagrees with what the code does is a bug, even if it
  builds. [M+T]
- A wrong name is worse than a placeholder. Leaving `field_0x1C` alone is always an acceptable outcome. [M+T: Melee's
  "don't name what you haven't understood" rule; its policy goes further and asks contributors not to let AI name
  struct fields or globals at all. We do let models name, so every name carries its evidence in CONTEXT and a
  reviewer checks it.]

## 2. What to fix first

Melee counts a *site* once by its outermost form, so `*(f32*)((u8*)p + 0x10)` is one site, not three, and groups
sites into three families. The order below is the one that pays off soonest for a reader.

1. **Move a pointer by a constant** [M]: `((u8*)p + 0x10)`, `((T*)p)[n]`, offset macros such as `WW_F32(w, 0x10)`,
   absolute addresses. Each is an unnamed struct field. Fix by naming the field in the header and accessing it.
   Our index weight: 2 (`raw_offset`).
2. **Placeholder names on things the code already explains** [T]: `field_0xNN`, `unk_*`, `dat_8xxxxxxx`/`lbl_`/
   `func_8xxxxxxx` address names. Melee's index doesn't count names at all; ours does (weight 1), because most of
   our remaining decoding cost is names.
3. **Retype a pointer** [M]: `(Struct*)p`, `(u8*)p`, `*(T*)p`, `(u32)p`, function-pointer casts. Often disappears on
   its own once the field in step 1 has the right type (a `void*` field that is always an `EffEntry*` becomes
   `EffEntry*`, and every cast on it goes). Our index counts only `fn_ptr_cast` (weight 2) of this family today.
4. **Match hacks** [M]: `#ifdef MUST_MATCH` regions, pragmas, `goto`, `PAD_STACK`. Remove one only when the clean
   form compiles to the same object; most must stay. Never add one during cleanup.

Within a family, pick the files the index ranks worst (`python3 tools/cleanup_index.py --top 20`) and finish a struct
at a time rather than scattering single fields across many files: one well-named header removes sites in every user.

## 3. Naming

### General

- **Follow the original first.** Names that came from the game (non-address names in
  `config/G8MJ01/**/symbols.txt`, strings in the binary) are never renamed, and they set the style of their file.
  [M+T: Melee's guide puts names recovered from binary strings above its own conventions.]
- **Match the file's existing style** when it has one; otherwise use the defaults below. [T]
- **Name from the consumer, not the first store.** A field written 0 at init and later passed to
  `animPoseSetLocalTime` is a pose time, whatever the init looked like. [T]
- **One name, one meaning.** Don't name a union member, a reused stack slot or a shared work field from a single
  motion or caller. If `0x2B8` is a jump height in one motion and a sound handle in another, it stays a placeholder,
  or each use gets its own union member named by its own user. [T]
- **No value-only or numbered names.** `colorWhite`, `vec3_zero3`, `float_2p5`, `fadeColor7` restate the initialiser
  or invent a series that isn't there. The address name stays. [T]
- **Don't name a constant after the one local or call it feeds** (`tex5Scale` for a `{1,1,1}` Vec used once). [T]
- **Content names for strings are fine:** `str_MOBJ_` for `"MOBJ_"`, `str_relPathFormat` for `"%s/rel/%s.rel"`. A
  name that is the literal can't mislead. [T]
- **Prefer an enum constant to a raw number** wherever the value is one of a known set (motion ids, states, flags,
  GX enums). Melee's largest recent cleanup series is exactly this: raw motion-state literals replaced by named
  constants, fighter by fighter. [M+T]
- **Abbreviations:** use the common ones (`pos`, `vel`, `rot`, `idx`, `num`, `cur`, `prev`, `dir`, `cb`, `anim`) and
  nothing invented. [M: Melee keeps a glossary of accepted short forms.]

### Defaults by kind

| kind | style | example | source |
|---|---|---|---|
| struct fields | camelCase | `scrollPos`, `animStep` | [T]; Melee uses snake_case, ours follows TTYD's own symbols |
| dol functions | the original's name; else `module` + camelCase verb | `winMgrSeq`, `marioCtrlOff` | [T] |
| REL and script (evt) helpers | snake_case | `unitwincoin_disp_onoff` | [T] |
| file-local statics | camelCase, no prefix needed | `winTpl` | [M+T] |
| enums | SCREAMING_SNAKE, one shared prefix per enum | `FILE_STATE_FREE` | [M] |
| locals and parameters | the file's style; descriptive over `temp_r31` | `entry`, `frame` | [M+T] |

Renames of symbols (functions, globals, statics) go through `tools/rename_symbol.py`, never by hand: the linker,
objdiff and the dol's FORCEACTIVE list all read the name. Fields, locals and parameters are plain source edits.

### When the meaning is unknown

- Fields: `field_0xNN` at the field's offset. Padding that is truly never touched: `u8 pad_0xNN[size]`, only for real
  gaps. Don't invent `unk1`, `x2C`, `temp` or `misc` placeholders. [T; Melee's equivalent is `xNN`/`pad_NN`, same
  idea, different spelling: keep ours.]
- Symbols: keep the address name. Melee's interim form for a function it understands only by location is
  `module_ADDR` (`grGreens_80213B1C` instead of `fn_80213B1C`); we have no reason to do that, since the address name
  already says as much.
- `.sbss2` GXColor templates (zero or white colours copied and then overwritten) stay unnamed: every effect has one,
  and the name could only restate the value. [T]
- Pool constants (`float_<value>_<addr>`, `double_*`, `zero_*`) stay as they are: many have to stay named to keep
  the pool layout, and a value-only name collides across addresses. Our index gives them weight 0. [T]

## 4. Types and layout

- **Header structs carry their offsets** on every field, named or not, and a size check where the project has one.
  Ours trail as `// 0xNN`; Melee leads with `/* +NN */`. Same rule, keep our form. [M+T]
- **A struct's comments say what the code proves:** writer and reader, units, range. Keep them to a line. [T]
- **Type the field once instead of casting at every use.** If every user casts `field_0x40` to `EffEntry*`, the field
  is an `EffEntry*`. [M: e.g. a blaster's effect-history array went from `s32[6]` with casts to typed effect
  pointers.]
- **Unions for a field with several proven meanings,** one member per meaning, each named by its user. Melee replaced
  an attribute pointer that every item cast differently with an explicit union of per-item layouts. [M+T]
- **Bitfields vs masks:** use whichever the asm shows (`rlwinm` extraction of one bit → a bitfield may match; `& 0x10`
  tests → a mask with a named `#define`/enum). Don't convert one into the other without a byte check. [M]
- **Data tables get their real element type**, not `u8[]` or `s32[]` with casts at the reader: a stage table becomes
  `BattleStageData[]`, a list of `{name, x, y}` records becomes an array of a struct. Melee has a tool that rewrites
  item state tables to their typed form; we've done the same by hand for btlobject and stage data. [M+T]
- **No struct copied from another project.** Layouts come from our own code and asm. [M]
- **Explicit `NULL`** for pointers: `p != NULL`, `= NULL`, not `!p`/`= 0`, where the result is identical. [M]

```c
/* before */
*(f32*)((u8*)work + 0x2C) += *(f32*)((u8*)work + 0x30);
((EffEntry*)work->field_0x40)->flags |= 2;

/* after: the header names 0x2C/0x30 and types 0x40 */
work->angle += work->angleSpeed;
work->eff->flags |= 2;
```

## 5. Pointer arithmetic and casts

- `((u8*)p + 0xNN)` that lands on a named field becomes that field. `uv run --with libclang tools/offset_to_field.py
  --list PATH`, then `--apply`, does the mechanical part. [T]
- An offset into an unnamed region: add the field to the header (with its offset comment), then convert every user,
  in every file, in the same change. [M+T]
- Stepping a pointer by the element size (`p = (u8*)p + 0x60`) becomes `++p` once `p` has the element type. [M+T]
- A string passed as "symbol + offset" (`(char*)table + 0xCC`) becomes the string literal when the build allows. Melee
  removed exactly this kind of hack along with the data-ordering stub that existed only to place the string. [M]
- Offset macros (`WW_S32(w, off)`) are a stopgap; each call counts as a raw offset. Replace calls with field
  accesses and drop the macro when it has no callers. A macro whose only parameter is the base (a named accessor)
  is fine as it is. [T]
- Leave a cast that the match needs. If removing it changes the object, it stays (inside `MUST_MATCH` if the clean
  form differs). [M+T]

## 6. Match hacks

- **`MUST_MATCH`**: the hack goes under `#ifdef MUST_MATCH`, the natural code under `#else`. Keep both branches in
  step: a rename or retype applies to both. A region can go only when the natural form gives the same object in the
  matching build. Melee classifies its regions as data-order stubs, asm, stack padding, macros, helper functions and
  plain code, and has been removing the data-order ones first; we have almost none of those (`pool_plan.py` handles
  pool order). [M+T]
- **`PAD_STACK(n)`** (`include/matching.h`): Melee's single largest category (about 2,000 sites); we have no uses. Don't
  introduce it in cleanup. [M]
- **Pragmas**: every pragma that changes code generation is a hack site, guarded or not (Melee counts all of them
  except `push`/`pop`). Never add one in cleanup; one being removed needs the same object without it. `#pragma once`
  is not a hack. [M+T]
- **`goto`**: may go only when the structured rewrite compiles to the identical object. Melee removed over 200 of its
  gotos this way in two PRs, turning `goto done` chains into `else if` and nested blocks. Don't reshape control flow
  for any other reason. [M+T]
- **Inline asm**: stays. [M+T]
- **Pool constants**: a named pool constant may become a literal only where the unit's object stays identical
  (`tools/obj_equiv.py` reads them as the same target); constants inside `MUST_MATCH` conditionals stay as written.
  [T]

## 7. Inlines, macros and accessors

- An inline that exists in the original (shown by an assert, a shared code shape across units, or a debug string) can
  be written as an inline; don't invent inlines to tidy a function. [M+T]
- A `static inline` shared by several files goes in the module's header; a file-local one stays at the top of its
  `.c`. [M]
- Accessor macros are fine when they name something (one whose only parameter is the base); a macro that hides an offset is a raw offset
  with a nicer spelling (see section 5). [M+T]
- Don't expand loops, add temporaries or split expressions to make code "clearer" if the original didn't have them:
  you'll change register allocation or scheduling, and the match is the point. [T]

## 8. Don't

- Rename a symbol that looks original. [M+T]
- Change control flow, expression types, access widths or signedness for readability. [T]
- Name from one store, one caller or one motion when other users disagree. [T]
- Give a constant a name that only restates its value or its one call. [T]
- Number a series that isn't one (`zeroVec0..6`). [T]
- Use placeholders other than `field_0xNN` / `pad_0xNN` / the address name. [M+T]
- Write a comment explaining what was removed or why an earlier name was wrong. [T]
- Edit a header without updating every user (run the compiles with a higher `-maxerrors`, with and without
  `-DMUST_MATCH`). [T]
- Copy code or text from another project into ours. [M+T]

## 9. Review checklist

1. Build: `build/G8MJ01/ok` recreated; changed C files compile without `MUST_MATCH`; `tools/obj_equiv.py` for any unit
   where a constant became a literal.
2. Every new name has evidence in the CONTEXT entry: the consumer that proves it, and for a union member, every
   other user of that offset.
3. No value-only, numbered or single-call names; strings named by content; `.sbss2` colour templates untouched.
4. Style matches the file; renames went through `rename_symbol.py`; no original names touched.
5. Index before and after (`python3 tools/cleanup_index.py`), same script on both sides.
6. Nothing added to the hack family (no new pragma, `MUST_MATCH`, `goto`, `PAD_STACK`).

## 10. Melee and us, side by side

| topic | Melee | us |
|---|---|---|
| what the index counts | casts, offsets, hacks; names not counted | names, raw offsets, hacks; most casts not counted |
| how | raw site count, one per outermost form, three families | weighted count per kLOC |
| placeholder field | `xNN`, padding `pad_NN` | `field_0xNN`, padding `pad_0xNN` |
| offset comments | leading `/* +NN */` | trailing `// 0xNN` |
| field style | snake_case | camelCase |
| unknown function | `module_ADDR` | address name kept |
| AI naming | discouraged for fields and globals | allowed, with evidence and Opus review |
| pool constants | mostly inline literals | named where the pool layout needs them |
| non-matching build | yes, behind `MUST_MATCH` | yes, behind `MUST_MATCH` |
