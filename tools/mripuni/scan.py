"""Collect PuniWork / PuniData field accesses (offset, width, kind) from the mri asm by register tracking.

A register holds Punigp (W), a Punidat element (D), or a pointer at an offset into one. Sources: `lwz/lwzu r, G@l(r)`
of the globals, `addi r, r, G@l` then `lwz r, 0(r)`, `mulli ..., 0x5a8` + `add` indexing and `+= 0x5a8` steps,
`mr` copies, the puniGetPunidat/puniGetPunigp results, and callee parameters (any offset, iterated until nothing new
is learned). Register state is merged at branch targets from every branch to them. A pointer at an offset passed to a
PSVEC* function marks a Vec there.
Usage (repo root, after a build): python3 tools/mripuni/scan.py tools/mripuni/access.json"""
import collections, glob, json, re, sys

MEM = {'lwz': (4, 'i'), 'lwzu': (4, 'i'), 'stw': (4, 'i'), 'stwu': (4, 'i'), 'lhz': (2, 'u'), 'lha': (2, 's'),
       'sth': (2, 'i'), 'lbz': (1, 'u'), 'stb': (1, 'i'), 'lfs': (4, 'f'), 'lfsu': (4, 'f'), 'stfs': (4, 'f'),
       'stfsu': (4, 'f'), 'lfd': (8, 'd'), 'stfd': (8, 'd'), 'psq_l': (8, 'p'), 'psq_st': (8, 'p')}
STORES = {'stw', 'stwu', 'sth', 'stb', 'stfs', 'stfsu', 'stfd', 'psq_st', 'stmw'}
BRANCH = re.compile(r'b(eq|ne|lt|gt|le|ge|dnz|dz)?(\+|-)?$')
GLOBALS = {'Punigp': 'W', 'Punidat': 'D'}
paths = glob.glob('build/G8MJ01/mri/asm/**/*.s', recursive=True)
params = collections.defaultdict(dict)
vecs = {'W': set(), 'D': set()}


def norm(kind, off):
    return off % 0x5A8 if kind == 'D' else off


def merge(a, b):
    return {r: v for r, v in a.items() if b.get(r) == v}


def scan():
    acc = {'W': collections.defaultdict(collections.Counter), 'D': collections.defaultdict(collections.Counter)}
    where = {'W': collections.defaultdict(set), 'D': collections.defaultdict(set)}
    for path in paths:
        fn, st, pending, live = None, {}, {}, True
        for line in open(path, errors='replace'):
            m = re.match(r'\.fn (\w+)', line)
            if m:
                fn, st, pending, live = m.group(1), dict(params.get(m.group(1), {})), {}, True
                continue
            m = re.match(r'(\.L_\w+):', line)
            if m:
                came = pending.pop(m.group(1), None)
                if came is not None:
                    st = merge(st, came) if live else dict(came)
                live = True
                continue
            m = re.search(r'\*/\s+([\w.+-]+)\s*(.*)$', line)
            if not m:
                continue
            op, args = m.group(1).rstrip('.'), [a.strip() for a in m.group(2).split(',')] if m.group(2) else []
            if BRANCH.match(op) and args and args[-1].startswith('.L_'):
                tgt = args[-1]
                pending[tgt] = merge(pending[tgt], st) if tgt in pending else dict(st)
                if op == 'b':
                    live = False  # unconditional: nothing falls through
                continue
            if op in ('blr', 'bctr'):
                live = False
                continue
            mm = re.match(r'(-?0x[0-9a-fA-F]+|-?\d+)\((r\d+)\)', args[1]) if len(args) > 1 else None
            if op in MEM and mm and mm.group(2) in st and st[mm.group(2)][0] in 'WD':
                kind, base = st[mm.group(2)]
                off = norm(kind, int(mm.group(1), 0) + base)
                acc[kind][off][MEM[op]] += 1
                where[kind][off].add(fn)
            if op == 'bl':
                tgt = args[0]
                for r in ('r3', 'r4', 'r5', 'r6'):
                    if r in st and st[r][0] in 'WD':
                        params[tgt][r] = st[r]
                        if tgt.startswith('PSVEC') and st[r][1] != 0:
                            vecs[st[r][0]].add(st[r][1])
                for r in ['r0'] + [f'r{i}' for i in range(3, 13)]:
                    st.pop(r, None)
                if tgt == 'puniGetPunidat':
                    st['r3'] = ('D', 0)
                elif tgt == 'puniGetPunigp':
                    st['r3'] = ('W', 0)
                continue
            if not args or not re.match(r'r\d+$', args[0]) or op in STORES or op.startswith(('cmp', 'fcmp', 'mt')):
                continue
            dst, new = args[0], None
            g = re.search(r'(\w+)@l', ','.join(args[1:])) if len(args) > 1 else None
            if op in ('lwz', 'lwzu') and g and g.group(1) in GLOBALS:
                new = (GLOBALS[g.group(1)], 0)
            elif op == 'addi' and g and g.group(1) in GLOBALS:
                new = ('A' + GLOBALS[g.group(1)], 0)  # the global's address
            elif op == 'lwz' and mm and mm.group(1) in ('0', '0x0') and st.get(mm.group(2), ('',))[0] in ('AW', 'AD'):
                new = (st[mm.group(2)][0][1], 0)
            elif op == 'mr' and args[1] in st:
                new = st[args[1]]
            elif op == 'mulli' and args[2].lower() == '0x5a8':
                new = ('M', 0)
            elif op == 'add' and len(args) == 3 and {st.get(args[1], ('',))[0], st.get(args[2], ('',))[0]} == {'D', 'M'}:
                new = ('D', 0)
            elif op == 'addi' and args[1] in st and st[args[1]][0] in 'WD' and re.match(r'-?(0x[0-9a-fA-F]+|\d+)$', args[2]):
                k, b = st[args[1]]
                new = (k, norm(k, b + int(args[2], 0)))
            if op in ('lwzu', 'lfsu', 'stwu', 'stfsu') and mm and mm.group(2) != dst and mm.group(2) in st:
                k, b = st[mm.group(2)]
                if k in 'WD':
                    st[mm.group(2)] = (k, norm(k, b + int(mm.group(1), 0)))  # update form moves the base
            st.pop(dst, None)
            if new:
                st[dst] = new
    return acc, where


for _ in range(10):
    before = (sum(len(v) for v in params.values()), sum(len(v) for v in vecs.values()))
    acc, where = scan()
    if (sum(len(v) for v in params.values()), sum(len(v) for v in vecs.values())) == before:
        break
out = {k: {hex(o): {'kinds': {f'{w}{t}': n for (w, t), n in c.items()}, 'fns': sorted(where[k][o])[:4]}
           for o, c in sorted(v.items())} for k, v in acc.items()}
out['vec'] = {k: [hex(o) for o in sorted(v)] for k, v in vecs.items()}
json.dump(out, open(sys.argv[1], 'w'), indent=1)
for k in 'WD':
    print(k, len(out[k]), 'offsets; Vec starts from PSVEC calls:', out['vec'][k])
print(len(params), 'functions with a Puni pointer parameter')
