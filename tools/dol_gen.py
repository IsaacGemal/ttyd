"""Write the data of a DOL unit as readable source: event scripts as EVT macros (ttydasm), other objects as u32 arrays
marked `// TODO: type`, strings inline as literals in the order the original pool has them.

    python3 tools/dol_gen.py UNIT [--force-active] [--append FILE] > src/UNIT.c

UNIT is the split's name without .c (battle/battle_event_subset); reads build/G8MJ01/asm/UNIT.s, so build first.
With --append FILE the unit's existing source gets the data added after its functions (includes and declarations the
data needs are inserted after FILE's #includes). Modeled on tools/rel_gen.py, which does the same for RELs.

The .data and .sdata objects are emitted in an interleaving that makes each new string appear where the original
pool has it (MWCC pools string literals in order of first use); check the result with a full build. Tables are u32
arrays because most battle structs are not typed yet; give them real types when they are.
"""
import re, sys, os, subprocess, glob, struct

args = [a for a in sys.argv[1:] if not a.startswith('--')]
UNIT = args[0]
FORCE = '--force-active' in sys.argv
APPEND = sys.argv[sys.argv.index('--append') + 1] if '--append' in sys.argv else None
if APPEND:
    args = [a for a in args if a != APPEND]
asm = open(f'build/G8MJ01/asm/{UNIT}.s', errors='replace').read()

objs = []
for m in re.finditer(r'# \.(\w+):0x[0-9A-F]+ \| (0x[0-9A-F]+) \| size: (0x[0-9A-F]+)\n\.(obj|fn) (\S+?),? (\w+)\n(.*?)^\.end(?:obj|fn) ', asm, re.M | re.S):
    sec, addr, size, kind, name, scope, body = m.groups()
    objs.append(dict(sec='.' + sec, addr=int(addr, 16), size=int(size, 16), kind=kind, name=name, scope=scope, body=body))
fns = [o for o in objs if o['kind'] == 'fn']
names = {o['name'] for o in objs}


def is_gap(o):
    return o['name'].startswith('gap_')


strings = {}      # object name -> raw text
order = {'.rodata': [], '.sdata2': []}   # original pool order of strings (by name)
for o in objs:
    if o['sec'] in order and o['kind'] == 'obj':
        s = re.findall(r'\.string "((?:[^"\\]|\\.)*)"', o['body'])
        if s:
            strings[o['name']] = s[0]
            order[o['sec']].append(o['name'])
        elif re.fullmatch(r'\s*\.4byte 0x00000000\s*', o['body']):   # an empty string (or padding after the last one)
            strings[o['name']] = ''
            order[o['sec']].append(o['name'])


def cstr(s):
    b = s.encode('latin1', 'backslashreplace').decode('unicode_escape').encode('latin1')
    b = b.split(b'\0')[0]
    t = b.decode('shift_jis')
    return '"' + t.replace('\\', '\\\\').replace('"', '\\"').replace('\n', '\\n') + '"'


def content(name):
    return cstr(strings[name])


NOTSCRIPT = set()   # objects that look like scripts but ttydasm cannot read
isscript = lambda o: o['name'] not in NOTSCRIPT and bool(re.match(r'\s*\.4byte 0x000[0-9A-F]{5}', o['body'])) and \
    bool(re.search(r'\.4byte 0x00000001(\s*\.4byte 0x00000000)*\s*$', o['body']))


def wrap_ptr(line):
    """PTR() on pointer arguments the macros don't cast (a CALL's arguments after the first, SET_VALUE's value...)."""
    if not re.match(r'\s*[A-Z_0-9]+\(', line):
        return line
    parts = re.split(r'(STRING\("(?:[^"\\]|\\.)*"\))', line)
    for i in range(0, len(parts), 2):
        p = parts[i]
        m = re.match(r'(\s*CALL\()(\w+)(.*)', p) if i == 0 else None
        if m:
            head, rest = m.group(1) + m.group(2), m.group(3)
        elif i == 0:
            hm = re.match(r'(\s*[A-Z_0-9]+\()?', p).group(0)
            head, rest = hm, p[len(hm):]
        else:
            head, rest = '', p
        rest = re.sub(r'(?<![\w"])([A-Za-z_][A-Za-z0-9_]*)(?![\w(])',
                      lambda mm: mm.group(1) if mm.group(1) == 'EVTDAT_POINTER_MAX' else f'PTR({mm.group(1)})', rest)
        parts[i] = head + rest
    s = ''.join(parts)
    return re.sub(r'\b(CALL_CHILD|SPAWN_EVENT)\(PTR\((\w+)\)\)', r'\1(\2)', s)


def render(o):
    """Source text of one data object, and the strings (contents, in order of use) it introduces."""
    st = '' if o['scope'] == 'global' else 'static '
    if o['sec'] == '.data' and o['size'] <= 48:
        st += '__declspec(section ".data") '   # too small for .data by size; the original keeps it there
    used = []
    if isscript(o):
        t = subprocess.run(['tools/ttydasm/target/release/ttydasm', 'extract', '-m', 'MarioSt', '-s', o['name'],
                            '-a', hex(o['addr']), '-p', '.'], capture_output=True, text=True)
        if t.returncode:
            print(f"ttydasm failed on {o['name']}; written as a table", file=sys.stderr)
            NOTSCRIPT.add(o['name'])
            return render(o)
        lines = [l for l in t.stdout.rstrip('\n').split('\n') if l.strip() != 'END']
        body = [lines[0]] + [wrap_ptr(l) for l in lines[1:]]
        text = st + '\n'.join(body)
        used = re.findall(r'STRING\(("(?:[^"\\]|\\.)*")\)', text)
        return text, used
    words = []
    for m in re.finditer(r'^\s*\.(4byte|float|string|byte|2byte|double|rel|skip) (.*)$', o['body'], re.M):
        k, v = m.group(1), m.group(2).strip()
        if k == '4byte':
            if v.startswith('0x'):
                words.append(v)
            elif v in strings:
                words.append(f'(u32){content(v)}')
                used.append(content(v))
            else:
                words.append(f'(u32)&{v}' if v in names else f'(u32){v}')
        elif k == 'float':
            words.append('0x%08X' % struct.unpack('>I', struct.pack('>f', float(v)))[0])
        elif k == 'skip' and int(v, 0) % 4 == 0:
            words += ['0x00000000'] * (int(v, 0) // 4)
        elif k == 'rel':
            sym, lbl = [x.strip() for x in v.split(',')]
            tgt = [x for x in objs if x['name'] == sym]
            if not tgt or not lbl.startswith('.L_'):
                sys.exit(f"{o['name']}: unsupported .rel {v}")
            words.append(f'(u32)((u8*)&{sym} + {int(lbl[3:], 16) - tgt[0]["addr"]:#x})')
        else:
            sys.exit(f"{o['name']}: unsupported .{k}")
    if len(words) * 4 != o['size']:
        sys.exit(f"{o['name']}: {len(words) * 4} bytes vs {o['size']}")
    text = f"// TODO: type\n{st}u32 {o['name']}[{len(words)}] = {{\n    " + \
        ',\n    '.join(', '.join(words[j:j + 4]) for j in range(0, len(words), 4)) + ",\n};"
    return text, used


data = [o for o in objs if o['sec'] in ('.data', '.sdata') and o['kind'] == 'obj' and not is_gap(o)]
queues = {'.data': [o for o in data if o['sec'] == '.data'], '.sdata': [o for o in data if o['sec'] == '.sdata']}
rendered = {o['name']: render(o) for o in data}
# expected next strings per pool, by content
expected = {p: [content(n) for n in order[p]] for p in order}
which = {}
for p in order:
    for n in order[p]:
        which.setdefault(content(n), p)
seen = set()


def fits(o):
    """Do the new strings this object introduces come next in their pools (or is it string-free)?"""
    pos = {p: len([1 for c in expected[p] if c in seen]) for p in expected}
    tmp = set()
    for c in rendered[o['name']][1]:
        if c in seen or c in tmp:
            continue
        p = which.get(c)
        if p is None:
            continue
        # the next unseen string of that pool
        nxt = [e for e in expected[p] if e not in seen and e not in tmp]
        if not nxt or nxt[0] != c:
            return False
        tmp.add(c)
    return True


emit = []
while queues['.data'] or queues['.sdata']:
    pick = None
    for q in ('.data', '.sdata'):
        if queues[q] and fits(queues[q][0]):
            pick = q
            break
    if pick is None:
        pick = '.data' if queues['.data'] else '.sdata'
    o = queues[pick].pop(0)
    emit.append(o)
    seen.update(rendered[o['name']][1])

src_parts = []
placed = set()


def placeholder(f):
    return (f"// TODO: write {f['name']} ({'global' if f['scope'] == 'global' else 'static'}, {f['size']:#x} bytes) here:\n"
            f"// its asm is in build/G8MJ01/asm/{UNIT}.s")


for o in emit:
    if not APPEND:
        for f in fns:
            if f['name'] not in placed and re.search(rf'\b{re.escape(f["name"])}\b', o['body']):
                src_parts.append(placeholder(f))
                placed.add(f['name'])
    src_parts.append(rendered[o['name']][0])
if not APPEND:
    for f in fns:
        if f['name'] not in placed:
            src_parts.append(placeholder(f))
src = '\n\n'.join(src_parts)
defined = {o['name'] for o in emit}
own_fns = {f['name'] for f in fns}

# forward declarations for objects used before their definition
fwd = []
for o in emit:
    n = o['name']
    dpos = min([p for p in (src.find(f'EVT_BEGIN({n})'), src.find(f' {n}[')) if p >= 0] or [0])
    if re.search(rf'[(&]\s*{re.escape(n)}\b', src[:dpos]):
        st = '' if o['scope'] == 'global' else 'static '
        fwd.append(f"{st}s32 {n}[{o['size'] // 4}];" if isscript(o) else f"{st}u32 {n}[{o['size'] // 4}];")

# names the data uses that the unit doesn't define: find the headers that declare them
idents = set(re.findall(r'\b([A-Za-z_]\w*)\b', re.sub(r'"(?:[^"\\]|\\.)*"', '', re.sub(r'//[^\n]*', '', src)))) - defined - own_fns
hdrs = set()
hdrtext = {h: open(h, errors='replace').read() for h in glob.glob('include/**/*.h', recursive=True)}
for h, t in hdrtext.items():
    for n in idents:
        if re.search(rf'(USER_FUNC|EVT_DEF)\({n}\)|\b{n}\s*\(|\b{n}\[\]', t):
            hdrs.add(h[8:])
hdrs |= {'evt/evt_cmd.h'}
skip = re.compile(r'^(LW|LF|GW|GF|GSW|GSWF|LSW|LSWF|UW|UF|FLOAT|STRING|PTR|EVT_BEGIN|EVT_END|EVTDAT_POINTER_MAX|extend_data|static|u8|u32|s32|u64|f32|f64|TODO|type|__declspec|section)$')
missing = sorted(n for n in idents if not n.isupper() and not skip.match(n) and
                 not any(re.search(rf'(USER_FUNC|EVT_DEF)\({n}\)|\b{n}\s*\(|\b{n}\[\]', hdrtext['include/' + h]) for h in hdrs))
ext = [f'extern s32 {n}[];' for n in missing]

pre = ''.join(f'#include "{h}"\n' for h in sorted(hdrs))
decl = '\n'.join(ext + fwd)
if APPEND:
    text = open(APPEND).read()
    m = list(re.finditer(r'^#include [^\n]*\n', text, re.M))[-1]
    inc = ''.join(f'#include "{h}"\n' for h in sorted(hdrs) if f'#include "{h}"' not in text)
    text = text[:m.end()] + inc + text[m.end():]
    body = ('\n#pragma force_active on\n' if FORCE else '\n') + (decl + '\n\n' if decl else '') + '// clang-format off\n' + src + '\n// clang-format on\n' + ('#pragma force_active off\n' if FORCE else '')
    sys.stdout.write(text.rstrip('\n') + '\n' + body)
else:
    out = pre + ('\n#pragma force_active on\n' if FORCE else '\n') + (decl + '\n\n' if decl else '') + '// clang-format off\n' + src + '\n// clang-format on\n' + ('#pragma force_active off\n' if FORCE else '')
    sys.stdout.write(out)
print(f'{UNIT}: {len(emit)} objects, {sum(1 for o in emit if isscript(o))} scripts, {len(ext)} externs', file=sys.stderr)
