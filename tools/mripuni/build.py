#!/usr/bin/env python3
"""Rebuild rels/mri/src/mri_puni.c and its header as a full skeleton, keeping upstream's work.
Run from a worktree root. Steps: rel_gen's skeleton (all data, verified equal; every function at a TODO placeholder),
then upstream's typed tables and .bss pointers in place of the raw ones, upstream's six written functions at their
placeholders, and a header that keeps upstream's structs and prototypes and declares every mri_puni function other
mri scripts CALL as USER_FUNC (their `extern s32 NAME[]` stand-ins elsewhere are removed separately). Then run order.py; groups.py plans a wave."""
import glob, re, subprocess

UPSTREAM = '54b0fff'  # main before this rebuild: upstream's partial mri_puni
up_c = subprocess.run(['git', 'show', f'{UPSTREAM}:rels/mri/src/mri_puni.c'], capture_output=True, text=True, check=True).stdout
up_h = subprocess.run(['git', 'show', f'{UPSTREAM}:rels/mri/include/mri_puni.h'], capture_output=True, text=True, check=True).stdout
subprocess.run(['python3', 'tools/rel_gen.py', 'mri', 'mri_puni'], check=True)
g, gen_h = open('rels/mri/src/mri_puni.c').read(), open('rels/mri/include/mri_puni.h').read().split('\n')
names = set(re.findall(r'^\.fn (\w+),', open('build/G8MJ01/mri/asm/mri_puni.s').read(), re.M))
called = set()
for f in glob.glob('rels/mri/src/**/*.c', recursive=True):
    called |= {n for n in re.findall(r'\bCALL\((\w+)', open(f).read()) if n in names}
called |= {n for n in re.findall(r'\bCALL\((\w+)', g) if n in names}

# header
others = {}
for h in glob.glob('rels/mri/include/*.h') + glob.glob('include/**/*.h', recursive=True):
    if h.endswith('mri_puni.h'):
        continue
    t = open(h, errors='replace').read()
    for n in re.findall(r'^[^\n#/]*?\b(\w+)\s*\([^;{]*\)\s*;', t, re.M):
        others.setdefault(n, h)
real = {'puniGetTex': 'PuniTexInfo* puniGetTex(void);',
        '_make_texbuf': 'PuniTexInfo* _make_texbuf(PuniTexInfo* texbuf, s32 count, const char* a3, const char* a4, f32 a5);'}
add = []
for l in gen_h:
    m = re.match(r'(?:void (\w+)\(void\);  // TODO: real signature|USER_FUNC\((\w+)\);|EVT_DEF\((\w+)\);)$', l)
    if not m:
        continue
    n = m.group(1) or m.group(2) or m.group(3)
    if re.search(r'\b' + n + r'\b', up_h):
        continue
    if n in called:
        add.append(f'USER_FUNC({n});')
    elif n in others:
        continue  # declared with its real signature elsewhere (mri.h)
    else:
        add.append(real.get(n, l))
header = up_h.rstrip('\n') + '\n\n' + '\n'.join(add) + '\n'
missing = [n for n in sorted(called) if not re.search(r'\b' + n + r'\b', header)]
header += ''.join(f'USER_FUNC({n});\n' for n in missing)
# event functions the scripts don't CALL but that take the event and return EVT_RETURN_*, and a display callback
for n in ['evt_puni_rnd_pos', 'evt_puni_get_systemmode', 'evt_puni_debug_info', 'evt_puni_init_teki', 'evt_puni_get_anim',
          'evt_puni_setsudan', 'evt_puni_fade_onoff', 'puni_disp_info_kassencheck']:
    header = header.replace(f'void {n}(void);  // TODO: real signature', f'USER_FUNC({n});')
header = header.replace('void evt_icon_disp_func(void);  // TODO: real signature',
                        'void evt_icon_disp_func(CameraId camId, void* param);')
header = header.replace('#include "evt/evt_cmd.h"\n', '#include "evt/evt_cmd.h"\n#include "driver/camdrv.h"\n', 1)
open('rels/mri/include/mri_puni.h', 'w').write(header)

# source: upstream's typed data by name
for m in re.finditer(r'^((?:u16|u16\*|void\*|PuniTexInfo\*|PuniWork\*|PuniData\*) (\w+)(\[\d*\])?(?: = \{.*?\};|;))\n', up_c, re.M | re.S):
    name, text = m.group(2), m.group(1)
    pat = re.compile(r'(?:// TODO: type\n)?^(?:static )?u(?:8|16|32) ' + re.escape(name) + r'\[\d+\] = \{\n.*?^\};\n', re.M | re.S)
    t = pat.search(g)
    assert t, name
    g = g[:t.start()] + text + '\n' + g[t.end():]
# upstream's written functions at their placeholders
for fn in ['puniGetPunidat', 'puniGetPunigp', 'puniGetTex', '_make_texbuf', 'evt_make_texbuf', 'evt_puni_mem_init']:
    m = (re.search(r'^[^\n]*\b' + re.escape(fn) + r'\([^;{]*\)\s*\{\n.*?^\}\n', up_c, re.M | re.S)
         or re.search(r'^USER_FUNC\(' + fn + r'\) \{\n.*?^\}\n', up_c, re.M | re.S))
    t = re.search(r'^// TODO: write ' + re.escape(fn) + r' \([^\n]*\n(?://[^\n]*\n)*', g, re.M)
    assert m and t, fn
    g = g[:t.start()] + m.group(0) + g[t.end():]
g = g.replace('#include "mri_puni.h"\n', '#include "mri_puni.h"\n#include "mri.h"\n', 1)
for inc in re.findall(r'^#include [<"][^>"]+[>"]\n', up_c, re.M):
    if inc not in g:
        g = g.replace('#include "mri.h"\n', '#include "mri.h"\n' + inc, 1)
g = g.replace('#pragma force_active on\n', '#pragma force_active on\nextern void* Punimap;\nextern PuniTexInfo* Punitex;\n'
              'extern PuniWork* Punigp;\nextern PuniData* Punidat;\n', 1)
open('rels/mri/src/mri_puni.c', 'w').write(g)
print(len(add) + len(missing), 'declarations added;', g.count('// TODO: write'), 'functions to write;', g.count('// TODO: type'), 'tables raw')
