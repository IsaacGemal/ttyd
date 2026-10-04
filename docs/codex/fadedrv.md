# fadedrv link attempts (2026-10-04)

Baseline: all 27 functions 100%; sections: .text 100%, .data 99.804306%, .rodata 99.37888%, .sdata 100%, .sdata2 95.83333%, .sbss/.sbss2 100%. Object sizes still omit trailing padding (.data -4, .rodata -4, .sdata -28, .sbss -4, .sbss2 -4); objdiff percentages alone do not prove layout. Checksum: not linked (baseline build uses original object).

1. Run pool_plan before editing. Its named-prefix plan regresses fadeEntry (99.73%), fadeMain (99.86%), disp_texture (96.68%), z (76.50%), disp_maku (91.98%), disp_tec (96.21%). Source and object restored; fresh report confirms 27/27 at 100%. No plan applied. Checksum: not attempted for the rejected plan.

2. Insert an ordinary deferred unused helper between disp_maku and disp_tec in source order, returning 3.141592f. It emits after disp_tec and before disp_maku, so pi is pooled first without changing disp_maku. Result: 27/27 functions 100%; .sdata2 100% and byte-identical; other section scores unchanged. Checksum: not yet attempted (remaining padding).

3. Restore .sdata with u8/u16/u32[7] zero padding and .sbss with an extra word, then flip Matching. All functions stay 100%; .sdata bytes are identical but objdiff scores 6.060606% because padding symbols add unmatched objects. Link fails: lbl_802C1538 is undefined, referenced from hitCheckFilter. Checksum: no (link failed). The original .sbss2 zero is already a compiler-generated GXColor template, so explicitly adding it duplicates a word; retain only the gap.

4. Try i4tex[32]={0xFF}: same identical bytes and all functions 100%, but .sdata still 6.060606% because the original object is one byte. Rejected. Try a packed named 31-byte gap: globals still align it to four, making .sdata 36 bytes and 5.555556%; functions 100%. Rejected. Checksum: no (still missing rodata symbol).

5. Probe named rodata strings plus an immediately generated conversion helper midway through source. All strings need explicit .rodata placement; more importantly, switching defer_codegen off flushes previously deferred functions. This moved fadeEntry early and regressed it to 73.406044%; other functions remained 100%. Reverted. Checksum: no (rejected before link).

6. Switch defer_codegen off at end of source before defining the .rodata and .data zero words. Result: .data byte-identical (1024 bytes), .sdata/.sdata2 byte-identical, all 27 functions 100%. The const .rodata zero still needs diagnosis. Checksum: pending linked-build diagnosis.

7. The end-of-file defer_codegen pragma alone does not flush until a function is generated. Add an immediately generated real u32-to-f32 helper before force_active and the tail declarations. Its conversion double reuses the existing pool entry. Result: .rodata/.data/.sdata/.sdata2 bytes identical, .sbss/.sbss2 sizes identical, all 27 functions 100%. Both helper bodies are absent from the final ELF; lbl_802C1538 resolves to exactly 0x802C1538. Checksum: YES, all 32 files OK; link_diag reports zero DOL bytes differ.

8. Once the flushing helper is present, make lbl_802C1538 const again: the word stays at the tail and the compiler's section-permission warning disappears. Result: 27/27 functions 100%; .text/.data/.rodata/.sdata2/.sbss 100%; .sdata 6.060606%, .sbss2 66.66667% are symbol-layout scoring artifacts despite exact bytes and sizes. Checksum: YES, all 32 files OK. The linked-unit checksum is authoritative; CI intentionally skips data-section percentages once a unit is linked.

Final verification: `ninja` passes all 32 checksums; `tools/ci.py --allow-pragma` passes (including natural compilation without MUST_MATCH, data bytes, and no-regression ratchet); `tools/reloc_value_check.py fadedrv --all` reports zero findings; `tools/sdata2_diff.py driver/fadedrv` reports both pools IDENTICAL; `tools/link_diag.py driver/fadedrv` reports zero differing DOL bytes. The final ELF contains neither pool helper and places lbl_802C1538 at its original address. No tools were edited; no commit or push was made. Scratch probes removed.
