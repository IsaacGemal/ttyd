"""Collect PuniWork / PuniData field accesses (offset, width, kind) from the mri asm by linear register tracking.
A register holds Punigp (W), a Punidat element (D, via mulli 0x5a8 + add, or += 0x5a8), or an offset into one;
a tracked pointer passed in r3-r5 to a `bl` marks that callee's parameter, iterated until nothing new is learned."""
import collections, glob, json, re, sys

MEM = {'lwz': (4, 'i'), 'lwzu': (4, 'i'), 'stw': (4, 'i'), 'stwu': (4, 'i'), 'lhz': (2, 'u'), 'lha': (2, 's'),
       'sth': (2, 'i'), 'lbz': (1, 'u'), 'stb': (1, 'i'), 'lfs': (4, 'f'), 'stfs': (4, 'f'), 'lfd': (8, 'd'),
       'stfd': (8, 'd'), 'psq_l': (8, 'p'), 'psq_st': (8, 'p')}
NODEST = {'stw', 'stwu', 'sth', 'stb', 'stfs', 'stfd', 'psq_st', 'cmpw', 'cmpwi', 'cmplw', 'cmplwi', 'stmw', 'b', 'bl',
          'beq', 'bne', 'blt', 'bgt', 'ble', 'bge', 'blr', 'mtlr', 'mtctr', 'bctrl', 'bctr', 'fcmpu', 'fcmpo'}
paths = glob.glob('build/G8MJ01/mri/asm/**/*.s', recursive=True)
params = collections.defaultdict(dict)


def scan():
    acc = {'W': collections.defaultdict(collections.Counter), 'D': collections.defaultdict(collections.Counter)}
    where = {'W': collections.defaultdict(set), 'D': collections.defaultdict(set)}
    for path in paths:
        fn, st = None, {}
        for line in open(path, errors='replace'):
            m = re.match(r'\.fn (\w+)', line)
            if m:
                fn = m.group(1); st = dict(params.get(fn, {})); continue
            m = re.search(r'\*/\s+([\w.]+)\s*(.*)$', line)
            if not m:
                continue
            op, args = m.group(1).rstrip('.'), [a.strip() for a in m.group(2).split(',')] if m.group(2) else []
            mm = re.match(r'(-?0x[0-9a-fA-F]+|-?\d+)\((r\d+)\)', args[1]) if len(args) > 1 else None
            if op in MEM and mm and mm.group(2) in st:
                kind, base = st[mm.group(2)]
                off = int(mm.group(1), 0) + base
                if kind == 'D':
                    off %= 0x5A8
                acc[kind][off][MEM[op]] += 1
                where[kind][off].add(fn)
            if op == 'bl':
                for r in ('r3', 'r4', 'r5'):
                    if r in st and st[r][1] == 0:
                        params[args[0]][r] = st[r]
                for r in ['r0'] + [f'r{i}' for i in range(3, 13)]:
                    st.pop(r, None)
                if args[0] == 'puniGetPunidat':
                    st['r3'] = ('D', 0)
                elif args[0] == 'puniGetPunigp':
                    st['r3'] = ('W', 0)
                continue
            if not args or op in NODEST or not re.match(r'r\d+$', args[0]):
                continue
            dst, new = args[0], None
            if op == 'lwz' and 'Punigp@l' in args[1]:
                new = ('W', 0)
            elif op == 'lwz' and 'Punidat@l' in args[1]:
                new = ('D', 0)
            elif op == 'mr' and args[1] in st:
                new = st[args[1]]
            elif op == 'mulli' and args[2].lower() == '0x5a8':
                new = ('M', 0)
            elif op == 'add' and len(args) == 3 and {st.get(args[1], ('',))[0], st.get(args[2], ('',))[0]} == {'D', 'M'}:
                new = ('D', 0)
            elif op == 'addi' and args[1] in st and st[args[1]][0] in 'WD' and re.match(r'-?(0x[0-9a-fA-F]+|\d+)$', args[2]):
                k, b = st[args[1]]
                new = (k, (b + int(args[2], 0)) % 0x5A8 if k == 'D' else b + int(args[2], 0))
            if op.endswith('u') and op in MEM:
                continue  # update form: the base register stays a pointer
            st.pop(dst, None)
            if new:
                st[dst] = new
    return acc, where


for _ in range(8):
    before = sum(len(v) for v in params.values())
    acc, where = scan()
    if sum(len(v) for v in params.values()) == before:
        break
out = {k: {hex(o): {'kinds': {f'{w}{t}': n for (w, t), n in c.items()}, 'fns': sorted(where[k][o])[:4]}
           for o, c in sorted(v.items())} for k, v in acc.items()}
json.dump(out, open(sys.argv[1], 'w'), indent=1)
for k in 'WD':
    print(k, len(out[k]), 'offsets; max', hex(max(acc[k])) if acc[k] else None)
print(len(params), 'functions with a Puni pointer parameter')
