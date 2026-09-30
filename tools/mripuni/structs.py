"""Usage (repo root, after a build): python3 tools/mripuni/scan.py tools/mripuni/access.json, then
python3 tools/mripuni/structs.py rels/mri/include/mri_puni.h."""
"""Render PuniWork and PuniData from access.json: every accessed offset becomes a field (Vec for three 4-byte
fields with float use, f32/s32/s16 by access), existing names kept, padding in between."""
import json
a = json.load(open('tools/mripuni/access.json'))
KEEP = {'W': {0x328: ('s32', 'unk328'), 0x444: ('BOOL', 'showPuniCounter'), 0x448: ('BOOL', 'unk448'),
              0x44C: ('s32', 'puniCounterAlpha'),
              # from the functions written in wave 35: four PuniData pointers
              0x364: ('struct PuniData*', 'field_0x364'), 0x368: ('struct PuniData*', 'field_0x368'),
              0x36C: ('struct PuniData*', 'field_0x36C'), 0x370: ('struct PuniData*', 'field_0x370')},
        'D': {0x4: ('s32', 'unk4'), 0xC: ('u16', 'unkC'),
              # from the functions written in wave 35: hitCheckFilter results and a surface normal
              0x210: ('struct HitEntry*', 'field_0x210'), 0x214: ('struct HitEntry*', 'field_0x214'),
              0x560: ('Vec', 'field_0x560')}}
WIDTH = lambda t: 12 if t == 'Vec' else 2 if t in ('s16', 'u16') else 1 if t in ('u8', 's8') else 4
SIZE = {'W': 0x4D0, 'D': 0x5A8}
VEC = {k: {int(o, 16) for o in a['vec'][k]} for k in 'WD'}


def fields(k):
    acc = {int(o, 16): v['kinds'] for o, v in a[k].items()}
    w4 = lambda q: q in acc and q not in KEEP[k] and max(int(x[0]) for x in acc[q]) == 4
    mixed = lambda q: any('f' in x for x in acc[q]) and any('i' in x for x in acc[q])
    # Vecs: first where a PSVEC call proves one, then non-overlapping triples copied as words and read as floats
    vec = {o for o, (t, _) in KEEP[k].items() if t == 'Vec'}
    taken = lambda o: any(v <= o + 8 and o <= v + 8 for v in vec)
    for o in sorted(VEC[k]):
        if all(w4(o + d) or (o + d) not in acc for d in (0, 4, 8)) and not taken(o):
            vec.add(o)
    # a triple whose three members are all copied and read as floats wins over one that only touches one of them
    cands = [o for o in acc if all(w4(o + d) for d in (0, 4, 8)) and any(mixed(o + d) for d in (0, 4, 8))]
    for o in sorted(cands, key=lambda o: (-sum(mixed(o + d) for d in (0, 4, 8)), o)):
        if not taken(o):
            vec.add(o)
    out = {}
    for o in sorted(set(acc) | set(KEEP[k])):
        if any(v < o < v + 12 for v in vec):
            continue
        if o in KEEP[k]:
            out[o] = (*KEEP[k][o], WIDTH(KEEP[k][o][0]))
        elif o in vec:
            out[o] = ('Vec', f'field_0x{o:X}', 12)
        else:
            kinds = acc[o]
            w = max(int(x[0]) for x in kinds)
            if w == 8:
                out[o] = ('f64', f'field_0x{o:X}', 8) if any('d' in x for x in kinds) else ('u8', f'field_0x{o:X}[8]', 8)
            elif w == 4:
                out[o] = ('f32' if any('f' in x for x in kinds) else 's32', f'field_0x{o:X}', 4)
            elif w == 2:
                out[o] = ('u16' if any('u' in x for x in kinds) else 's16', f'field_0x{o:X}', 2)
            else:
                out[o] = ('u8', f'field_0x{o:X}', 1)
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
    m = re.search(r'typedef struct ' + name + r' \{.*?\} ' + name + r';[^\n]*', h, re.S)
    h = h[:m.start()] + render(name, k) + h[m.end():]
open(sys.argv[1], 'w').write(h)
