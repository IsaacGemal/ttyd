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
hdrtext = {h: open(h, errors='replace').read() for h in glob.glob('include/**/*.h', recursive=True)}
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



_EXT = None


def ext_strings():
    """name -> raw text of every string object in the DOL asm (for strings this unit does not own)."""
    global _EXT
    if _EXT is None:
        _EXT = {}
        for f in glob.glob('build/G8MJ01/asm/**/*.s', recursive=True):
            t = open(f, errors='replace').read()
            for m in re.finditer(r'\.obj ((?:str|dat)_\w+), \w+\n\t\.string "((?:[^"\\]|\\.)*)"', t):
                _EXT[m.group(1)] = m.group(2)
    return _EXT


USED_EXT = []


def link_external_strings(text, body, literal, wrap):
    """Replace inline literals by the name of the string object another unit owns, in the order the body refers to them."""
    for n in re.findall(r'\.4byte (\w+)', body):
        if n in strings or n in names:
            continue
        raw = ext_strings().get(n)
        if raw is None:
            continue
        lit = cstr(raw)
        pat = literal(lit)
        if pat in text:
            text = text.replace(pat, wrap(n), 1)
            if n not in USED_EXT:
                USED_EXT.append(n)
    return text


def content(name):
    return cstr(strings[name])


SCALARS = set()
if APPEND:
    SCALARS = set(re.findall(r'(?m)^\s*(?:s32|u32|f32|s16|u16|s8|u8|BOOL)\s+(\w+)\s*;', open(APPEND).read()))
NOTSCRIPT = set()   # objects that look like scripts but ttydasm cannot read
isbytes = lambda o: bool(re.search(r'^\s*\.byte ', o['body'], re.M)) and not re.search(r'^\s*\.(4byte|2byte|float|string|double|rel|skip) ', o['body'], re.M)
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


def join_halves(halves):
    """One `.4byte`-style word from two consecutive `.2byte` values (big-endian), as the words list wants it."""
    hi, lo = (int(h, 0) for h in halves)
    if not (0 <= hi <= 0xFFFF and 0 <= lo <= 0xFFFF):
        raise ValueError(f"not a halfword: {halves}")
    return '0x%04X%04X' % (hi, lo)


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
        for sc in SCALARS:   # a variable (not an array) the source defines: the script holds its address
            text = re.sub(rf'PTR\({re.escape(sc)}\)', f'PTR(&{sc})', text)
        text = link_external_strings(text, o['body'], lambda lit: f'STRING({lit})', lambda n: f'STRING((void*){n})')
        used = re.findall(r'STRING\(("(?:[^"\\]|\\.)*")\)', text)
        return text, used
    if isbytes(o):   # a table of `.byte` lines only: u8 elements
        vals = [v.strip() for l in re.findall(r'^\s*\.byte (.*)$', o['body'], re.M) for v in l.split(',')]
        if len(vals) != o['size']:
            sys.exit(f"{o['name']}: {len(vals)} bytes vs {o['size']}")
        return f"// TODO: type\n{st}u8 {o['name']}[{len(vals)}] = {{\n    " + \
            ',\n    '.join(', '.join(vals[j:j + 8]) for j in range(0, len(vals), 8)) + ",\n};", used
    words = []
    half = []   # a pending `.2byte`: two of them make one word (a struct's s16 pair, as in battle_seq_end's _rank_up_data)
    for m in re.finditer(r'^\s*\.(4byte|float|string|byte|2byte|double|rel|skip) (.*)$', o['body'], re.M):
        k, v = m.group(1), m.group(2).strip()
        if half and k != '2byte':
            sys.exit(f"{o['name']}: a lone .2byte before .{k}")
        if k == '2byte':
            half.append(v)
            if len(half) == 2:
                words.append(join_halves(half))
                half = []
        elif k == '4byte':
            if v.startswith('0x'):
                words.append(v)
            elif v in strings:
                words.append(f'(u32){content(v)}')
                used.append(content(v))
            else:
                words.append(f'(u32)&{v}' if v in names and v not in {f_['name'] for f_ in fns} else f'(u32){v}')
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
    if half:
        sys.exit(f"{o['name']}: a lone .2byte at the end")
    if len(words) * 4 != o['size']:
        sys.exit(f"{o['name']}: {len(words) * 4} bytes vs {o['size']}")
    _t = 's32' if (o['name'] in NOTSCRIPT or re.search(rf'EVT_DEF\({re.escape(o["name"])}\)', ' '.join(hdrtext.values()))) else 'u32'
    words = [w for w in words]
    if _t == 's32':
        words = [w.replace('(u32)', '(s32)') for w in words]
    text = f"// TODO: type\n{st}{_t} {o['name']}[{len(words)}] = {{\n    " + \
        ',\n    '.join(', '.join(words[j:j + 4]) for j in range(0, len(words), 4)) + ",\n};"
    text = link_external_strings(text, o['body'], lambda lit: f'(s32){lit}' if _t == 's32' else f'(u32){lit}', lambda n: f'(s32)(void*){n}' if _t == 's32' else f'(u32)(void*){n}')
    return text, used


data = [o for o in objs if o['sec'] in ('.data', '.sdata') and o['kind'] == 'obj' and not is_gap(o)]
if APPEND:
    # data the source already defines is left alone; compiler-made objects (jump tables) come from the functions
    _src = open(APPEND).read()
    _defd = lambda n: bool(re.search(rf'(?m)^(?!\s*(//|extern))[^\n(]*\b{re.escape(n)}\b\s*(\[[^\]]*\]\s*)*=|EVT_BEGIN\({re.escape(n)}\)', _src))
    data = [o for o in data if not _defd(o['name']) and not o['name'].startswith('jumptable_')]
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


PRAGMA_STATE = re.compile(r'#pragma\s+(\w+)\s+(on|off|reset)\b(.*)')


def split_source(text):
    """(preamble, [(function name, text)], epilogue): the existing source cut into its function definitions.

    A chunk is one function together with the preprocessor block that wraps it (an `#ifdef` ... `#endif` around a helper
    ends at its `#endif`; a function with `#ifdef`/`#else` bodies counts only the first branch's braces). A `#pragma NAME
    on|off|reset` pair is not kept as text between chunks, where reordering would separate it: the pragma state in effect
    at each chunk is recorded and re-emitted around that chunk alone (`off`/`on` before, `reset` after), so the chunks can
    be moved without changing which functions the pragma applies to."""
    lines = text.split('\n')
    first = None
    for i, l in enumerate(lines):
        code = re.sub(r'//.*', '', l)
        if re.match(r'^[A-Za-z_][\w\s\*]*\(.*\)\s*\{?\s*$', code) and not code.startswith(('typedef', 'extern')) and '=' not in code and not code.rstrip().endswith(';'):
            first = i
            break
    if first is None:
        return text, [], ''
    # a preprocessor block (#if ... ) that opens right above the first function belongs to it
    lead = first
    while first > 0 and (lines[first - 1].startswith('//') or lines[first - 1].startswith('#')):
        first -= 1
    pre_lines = lines[:first]
    body_lines = lines[first:]
    chunks, cur = [], []
    depth = 0
    pp = []          # one entry per open #if: True while inside its first branch
    state = {}
    cur_state = None
    continued = False
    for l in body_lines:
        code = re.sub(r'//.*', '', l)
        m = PRAGMA_STATE.match(l.strip())
        if m and m.group(1) != 'force_active':
            k, v, rest = m.groups()
            state[k] = (v != 'reset', v, rest.strip())
            if not cur and cur_state is None:
                pass
            continue
        if cur_state is None and code.strip():
            cur_state = dict(state)
        cur.append(l)
        t = l.strip()
        was_continued, continued = continued, l.rstrip().endswith('\\')
        if was_continued or (t.startswith('#') and continued):
            continue   # the lines of a multi-line #define hold braces that are not the function's
        if re.match(r'#\s*if', t):
            pp.append(True)
        elif re.match(r'#\s*(else|elif)', t) and pp:
            pp[-1] = False
        elif re.match(r'#\s*endif', t) and pp:
            pp.pop()
        elif not t.startswith('#') and (not pp or pp[-1]):
            depth += code.count('{') - code.count('}')
        if depth == 0 and not pp and ('}' in code or re.match(r'#\s*endif', t)) and any(re.search(r'\{', re.sub(r'//.*', '', c)) for c in cur):
            chunks.append((cur, cur_state or {}))
            cur, cur_state = [], None
    out = []
    for c, st in chunks:
        body = '\n'.join(c).strip('\n')
        wrap_pre = [f'#pragma {k} {v}{(" " + r) if r else ""}' for k, (act, v, r) in sorted(st.items()) if act]
        wrap_post = [f'#pragma {k} reset' for k, (act, v, r) in sorted(st.items()) if act]
        full = '\n'.join(wrap_pre + [body] + wrap_post)
        m2 = re.search(r'(?:USER_FUNC|EVT_DEF)\((\w+)\)|\b(\w+)\s*\(', re.sub(r'(?m)^\s*(//.*|#.*)$', '', body))
        out.append(((m2.group(1) or m2.group(2)) if m2 else '', full))
    tail = '\n'.join(cur)
    return '\n'.join(pre_lines), out, tail


if APPEND:
    pre_text, fchunks, tail_text = split_source(open(APPEND).read())
    fn_addr = {f['name']: f['addr'] for f in fns}
    pool = {o['name']: o['sec'] for o in objs if o['sec'] in ('.rodata', '.sdata2') and o['kind'] == 'obj' and not is_gap(o)
            and '$' not in o['name']}
    # MWCC pools a string in a data initializer, and an aggregate constant (a Vec = {0,0,0}) in a function, when it parses
    # them, in source order; the strings inside functions come afterwards, at codegen, in text order; .sdata2 constants
    # are also pooled at codegen. Only the parse-time entries constrain how functions and data interleave.
    rd = lambda n: pool.get(n) == '.rodata'
    data_strs = {n for o in data for n in re.findall(r'\b(\w+)\b', o['body']) if rd(n) and n in strings}
    fn_aggr = {n for f in fns for n in re.findall(r'\b(\w+)\b', f['body']) if rd(n) and n.startswith('vec3_')}
    parse_pool = data_strs | fn_aggr
    pool_order = {'.rodata': [o['name'] for o in objs if o['sec'] == '.rodata' and o['name'] in parse_pool]}
    refs_of = lambda body, fn=False: {n for n in re.findall(r'\b(\w+)\b', body) if n in parse_pool and (n not in strings if fn else True)}
    helpers = [c for c in fchunks if c[0] not in fn_addr]
    funcs = sorted([c for c in fchunks if c[0] in fn_addr], key=lambda c: -fn_addr[c[0]])   # deferred codegen: reverse of the text
    fbody = {f['name']: f['body'] for f in fns}
    items = {'F': [dict(name=c[0], text=c[1], refs=refs_of(fbody[c[0]], True)) for c in funcs],
             'D': [dict(name=o['name'], text=rendered[o['name']][0], refs=refs_of(o['body'])) for o in data if o['sec'] == '.data'],
             'S': [dict(name=o['name'], text=rendered[o['name']][0], refs=refs_of(o['body'])) for o in data if o['sec'] == '.sdata']}
    seen_p = set()
    _emitted = {o['name'] for o in data}
    for o in objs:   # data the source already defines sits in the preamble: its pool entries come first
        if o['sec'] in ('.data', '.sdata') and o['kind'] == 'obj' and not is_gap(o) and o['name'] not in _emitted \
                and not o['name'].startswith('jumptable_'):
            seen_p |= refs_of(o['body'])

    def ok(item):
        new = item['refs'] - seen_p
        for p, lst in pool_order.items():
            need = [n for n in lst if n not in seen_p]
            k = len([n for n in new if pool[n] == p])
            if set(need[:k]) != {n for n in new if pool[n] == p}:
                return False
        return True

    merged = []
    while any(items.values()):
        pick = None
        for q in ('F', 'D', 'S'):
            if items[q] and ok(items[q][0]):
                pick = q
                break
        if pick is None:
            print('pool order: no item fits; taking', [items[q][0]['name'] for q in items if items[q]], file=sys.stderr)
            for q in items:
                if items[q]:
                    new_ = sorted(items[q][0]['refs'] - seen_p, key=lambda n: (pool[n], pool_order[pool[n]].index(n)))
                    print('   ', q, items[q][0]['name'], 'needs', new_[:6], '| next expected', {pp: [n for n in l if n not in seen_p][:3] for pp, l in pool_order.items()}, file=sys.stderr)
            pick = next(q for q in ('F', 'D', 'S') if items[q])
        it = items[pick].pop(0)
        merged.append(it)
        seen_p |= it['refs']
    emit = [o for o in data if o['name'] in {m['name'] for m in merged}]
    src_parts = [c[1] for c in helpers] + [m['text'] for m in merged]
    src = '\n\n'.join(src_parts)
    defined = {o['name'] for o in emit}
    own_fns = {f['name'] for f in fns}
    APPEND_PRE, APPEND_TAIL = pre_text, tail_text
else:
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
    APPEND_PRE = APPEND_TAIL = ''

import glob as _g
# forward declarations for objects used before their definition, and for the unit's own functions the data calls
fwd = []
HDR_ADD = []
if APPEND:
    _dtext = '\n'.join(rendered[o['name']][0] for o in emit)
    _called = set(re.findall(r'\bCALL\((\w+)', _dtext))
    for f in sorted(fns, key=lambda f: -f['addr']):   # declaration order sets function order: same as the definitions
        _nm = re.escape(f['name'])
        _declared = re.search(rf'(USER_FUNC|EVT_DEF)\({_nm}\)|\b{_nm}\s*\(', APPEND_PRE) or any(re.search(rf'(USER_FUNC|EVT_DEF)\({_nm}\)|\b{_nm}\s*\(', t) for t in hdrtext.values())
        if re.search(rf'\b{_nm}\b', _dtext) and not _declared:
            st = '' if f['scope'] == 'global' else 'static '
            _sig = next((c[1] for c in fchunks if c[0] == f['name']), None)
            if _sig is not None:   # the definition's own signature is the declaration
                _head = re.sub(r'(?m)^\s*(//.*|#pragma.*)$', '', _sig.split('{')[0]).strip() + ';'
            else:
                _head = f"USER_FUNC({f['name']});" if f['name'] in _called else f"void {f['name']}(void);  // TODO: real signature"
            if f['scope'] == 'global':
                HDR_ADD.append(_head)   # a non-static prototype belongs in the unit's header (CI rejects it in a .c)
            else:
                fwd.append(_head if _head.startswith('static') else 'static ' + _head)
for o in emit:
    n = o['name']
    dpos = min([p for p in (src.find(f'EVT_BEGIN({n})'), src.find(f' {n}[')) if p >= 0] or [0])
    if re.search(rf'[(&]\s*{re.escape(n)}\b', src[:dpos]):
        st = '' if o['scope'] == 'global' else 'static '
        if isbytes(o):
            fwd.append(f"{st}u8 {n}[{o['size']}];")
            continue
        fwd.append(f"{st}s32 {n}[{o['size'] // 4}];" if (isscript(o) or n in NOTSCRIPT or re.search(rf'EVT_DEF\({re.escape(n)}\)', ' '.join(hdrtext.values()))) else f"{st}u32 {n}[{o['size'] // 4}];")

# names the data uses that the unit doesn't define: find the headers that declare them
idents = set(re.findall(r'\b([A-Za-z_]\w*)\b', re.sub(r'"(?:[^"\\]|\\.)*"', '', re.sub(r'//[^\n]*', '', '\n'.join(rendered[o['name']][0] for o in emit))))) - defined - own_fns - names
hdrs = set()
for h, t in hdrtext.items():
    for n in idents:
        if re.search(rf'(USER_FUNC|EVT_DEF)\({n}\)|\b{n}\s*\(|\b{n}\[\]', t):
            hdrs.add(h[8:])
hdrs |= {'evt/evt_cmd.h'}
if HDR_ADD:
    _c = glob.glob(f'include/**/{os.path.basename(UNIT)}.h', recursive=True)
    hdrs.add((_c[0] if _c else f'include/{UNIT}.h')[8:])
skip = re.compile(r'^(LW|LF|GW|GF|GSW|GSWF|LSW|LSWF|UW|UF|FLOAT|STRING|PTR|EVT_BEGIN|EVT_END|EVTDAT_POINTER_MAX|extend_data|static|u8|u32|s32|u64|f32|f64|TODO|type|__declspec|section)$')
missing = sorted(n for n in idents if n not in USED_EXT and not n.isupper() and not skip.match(n) and
                 not any(re.search(rf'(USER_FUNC|EVT_DEF)\({n}\)|\b{n}\s*\(|\b{n}\[\]', hdrtext.get('include/' + h, '')) for h in hdrs))
def owner_decl(n):
    """(header path, declaration) for a name another source file defines, so the declaration goes in its header."""
    for path in _g.glob('src/**/*.c', recursive=True):
        text = open(path).read()
        m = re.search(rf'^USER_FUNC\({re.escape(n)}\)', text, re.M)
        decl = f'USER_FUNC({n});' if m else None
        if not m:
            m = re.search(rf'^(?:const\s+)?(\w+)\s+{re.escape(n)}\s*\[(\d*)\]\s*=', text, re.M)
            if m:
                decl = f'EVT_DEF({n});' if m.group(1) == 's32' else f'extern {m.group(1)} {n}[{m.group(2)}];'
        if decl:
            hdr = 'include/' + path[4:-2] + '.h'
            if os.path.exists(hdr):
                return hdr, decl
    return None


_ext_names = []
for n in missing:
    o = owner_decl(n)
    if o is None:
        print(f'WARNING: {n} is declared by no header and no header-owning source defines it: writing a stand-in extern', file=sys.stderr)
        _ext_names.append(n)
        continue
    hdr, decl = o
    if decl not in open(hdr).read():
        open(hdr, 'a').write(('' if open(hdr).read().endswith('\n') else '\n') + decl + '\n')
    hdrs.add(hdr[8:])
missing = _ext_names
ext = [f'extern s32 {n}[];' for n in missing] + [f'extern const char {n}[];' for n in USED_EXT if not re.search(rf'\b{n}\s*\[', APPEND_PRE if APPEND else '')]

pre = ''.join(f'#include "{h}"\n' for h in sorted(hdrs))
decl = '\n'.join(ext + fwd)
if APPEND:
    text = APPEND_PRE
    m = list(re.finditer(r'^#include [^\n]*\n', text, re.M))[-1]
    inc = ''.join(f'#include "{h}"\n' for h in sorted(hdrs) if f'#include "{h}"' not in text)
    text = text[:m.end()] + inc + text[m.end():]
    body = ('\n#pragma force_active on\n' if FORCE else '\n') + (decl + '\n\n' if decl else '') + '// clang-format off\n' + src + '\n// clang-format on\n' + ('#pragma force_active off\n' if FORCE else '')
    tail = APPEND_TAIL.strip('\n')
    if re.sub(r'(?m)^\s*(//.*|#pragma.*|/\*.*?\*/)?\s*$', '', tail).strip():
        print('WARNING: the source after its last function holds more than comments and pragmas; it is kept at the end '
              'of the file, check it against the data written here', file=sys.stderr)
    sys.stdout.write(text.rstrip('\n') + '\n' + body + ('\n' + tail + '\n' if tail else ''))
else:
    out = pre + ('\n#pragma force_active on\n' if FORCE else '\n') + (decl + '\n\n' if decl else '') + '// clang-format off\n' + src + '\n// clang-format on\n' + ('#pragma force_active off\n' if FORCE else '')
    sys.stdout.write(out)
if HDR_ADD:
    _cands = glob.glob(f'include/**/{os.path.basename(UNIT)}.h', recursive=True)
    _hp = _cands[0] if _cands else f'include/{UNIT}.h'
    _ht = open(_hp).read() if os.path.exists(_hp) else '#pragma once\n\n#include "evt/evt_cmd.h"\n'
    _new = [d for d in HDR_ADD if d not in _ht]
    if _new:
        os.makedirs(os.path.dirname(_hp), exist_ok=True)
        # the types the declarations use must be visible in the header: include the headers that define them
        _incs = []
        for d in _new:
            for tname in set(re.findall(r'\b([A-Z][A-Za-z0-9_]+)\b', re.sub(r'USER_FUNC|EVT_DEF', '', d))):
                if re.search(rf'\b{tname}\b', _ht):
                    continue
                for h, t in hdrtext.items():
                    if h != _hp and re.search(rf'(\}}\s*{tname}\s*;|typedef\s+struct\s+\w*\s+{tname}\s*;|typedef\s+enum\s+\w*\s*\{{[^}}]*\}}\s*{tname}\s*;)', t):
                        _incs.append(h[8:])
                        break
        if any('USER_FUNC' in d or 'EVT_DEF' in d for d in _new):
            _incs.append('evt/evt_cmd.h')
        _incs = [i for i in dict.fromkeys(_incs) if f'#include "{i}"' not in _ht]
        if _incs and '#pragma once' in _ht:
            _ht = _ht.replace('#pragma once\n', '#pragma once\n' + ''.join(f'\n#include "{i}"' for i in _incs) + '\n', 1)
        open(_hp, 'w').write(_ht.rstrip('\n') + '\n' + '\n'.join(_new) + '\n')
        print(f'declarations added to {_hp}: {len(_new)}', file=sys.stderr)
        if _hp[8:] not in [h for h in hdrs]:
            print(f'(include "{_hp[8:]}" in the source)', file=sys.stderr)
print(f'{UNIT}: {len(emit)} objects, {sum(1 for o in emit if isscript(o))} scripts, {len(ext)} externs', file=sys.stderr)
