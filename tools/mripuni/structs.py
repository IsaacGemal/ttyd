"""Usage (repo root, after a build): python3 tools/mripuni/scan.py tools/mripuni/access.json, then
python3 tools/mripuni/structs.py rels/mri/include/mri_puni.h."""
"""Render PuniWork and PuniData from access.json: every accessed offset becomes a field (Vec for three 4-byte
fields with float use, f32/s32/s16 by access), existing names kept, padding in between."""
import json
a = json.load(open('tools/mripuni/access.json'))
KEEP = {'W': {0x328: ('s32', 'unk328'), 0x444: ('BOOL', 'showPuniCounter'), 0x448: ('BOOL', 'unk448'),
              0x44C: ('s32', 'puniCounterAlpha')},
        'D': {0x4: ('s32', 'unk4'), 0xC: ('u16', 'unkC')}}
SIZE = {'W': 0x4D0, 'D': 0x5A8}


def fields(k):
    acc = {int(o, 16): v['kinds'] for o, v in a[k].items()}
    out, o = {}, 0
    offs = sorted(set(acc) | set(KEEP[k]))
    i = 0
    while i < len(offs):
        o = offs[i]
        if o in KEEP[k]:
            out[o] = (*KEEP[k][o], 4 if KEEP[k][o][0] in ('s32', 'BOOL') else 2); i += 1; continue
        kinds = acc[o]
        w = max(int(x[0]) for x in kinds)
        trip = all((o + d) in acc and (o + d) not in KEEP[k] and max(int(x[0]) for x in acc[o + d]) == 4 for d in (4, 8))
        mixed = lambda q: any('f' in x for x in acc[q]) and any('i' in x for x in acc[q])
        if w == 4 and trip and any(mixed(o + d) for d in (0, 4, 8)):
            out[o] = ('Vec', f'field_0x{o:X}', 12); i += 3; continue
        if w == 8:
            t = 'f64' if any('d' in x for x in kinds) else 'u8'
            out[o] = (t, f'field_0x{o:X}', 8) if t == 'f64' else ('u8', f'field_0x{o:X}[8]', 8)
        elif w == 4:
            out[o] = ('f32' if any('f' in x for x in kinds) and not any('i' in x for x in kinds) else 's32', f'field_0x{o:X}', 4)
        elif w == 2:
            out[o] = ('u16' if any('u' in x for x in kinds) else 's16', f'field_0x{o:X}', 2)
        else:
            out[o] = ('u8', f'field_0x{o:X}', 1)
        i += 1
    return out


def render(name, k):
    f = fields(k)
    lines, pos = [f'typedef struct {name} {{'], 0
    for o in sorted(f):
        assert o >= pos, (name, hex(o), hex(pos))
        if o > pos:
            lines.append(f'    u8 unk{pos:X}[0x{o:X} - 0x{pos:X}];'.ljust(40) + f'// 0x{pos:X}')
        t, n, w = f[o]
        lines.append(f'    {t} {n};'.ljust(40) + f'// 0x{o:X}')
        pos = o + w
    if pos < SIZE[k]:
        lines.append(f'    u8 unk{pos:X}[0x{SIZE[k]:X} - 0x{pos:X}];'.ljust(40) + f'// 0x{pos:X}')
    lines.append(f'}} {name};  // 0x{SIZE[k]:X}')
    return '\n'.join(lines)


import re, sys
h = open(sys.argv[1]).read()
for name, k in (('PuniWork', 'W'), ('PuniData', 'D')):
    m = re.search(r'typedef struct ' + name + r' \{.*?\} ' + name + r';', h, re.S)
    h = h[:m.start()] + render(name, k) + h[m.end():]
h = re.sub(r'\} (PuniWork|PuniData);  // (0x[0-9A-F]+)', r'} \1; // \2', h)
open(sys.argv[1], 'w').write(h)
