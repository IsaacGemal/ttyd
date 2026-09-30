"""Put mri_puni.c's functions (TODO placeholders and written bodies) in the asm's order, which mri needs (built
without deferred inlining). Keeps the longest in-order run in place and moves each other function to just before
the next in-order one in asm order (or after the last)."""
import bisect, re
p = 'rels/mri/src/mri_puni.c'
s = open(p).read()
asm = re.findall(r'^\.fn (\w+),', open('build/G8MJ01/mri/asm/mri_puni.s').read(), re.M)

def block(n, s):
    m = re.search(r'^// TODO: write ' + re.escape(n) + r' \([^\n]*\n(?://[^\n]*\n)*', s, re.M)
    if not m:
        m = re.search(r'(?:^//[^\n]*\n)*^(?:[^\n/]*\b' + re.escape(n) + r'\([^;{]*\)\s*\{|(?:static )?USER_FUNC\(' + re.escape(n)
                      + r'\) \{)\n.*?^\}\n', s, re.M | re.S)
    return m

pos = {n: block(n, s).start() for n in asm}
order = sorted(asm, key=lambda n: pos[n])
seq = [asm.index(n) for n in order]
# longest increasing subsequence, with reconstruction
tails, tidx, prev = [], [], [None] * len(seq)
for i, x in enumerate(seq):
    j = bisect.bisect_left(tails, x)
    if j == len(tails):
        tails.append(x); tidx.append(i)
    else:
        tails[j] = x; tidx[j] = i
    prev[i] = tidx[j - 1] if j else None
keep, i = set(), tidx[-1]
while i is not None:
    keep.add(order[i]); i = prev[i]
moves = [n for n in asm if n not in keep]
for n in moves:
    m = block(n, s)
    text = m.group(0)
    s = s[:m.start()] + s[m.end():]
    nxt = next((x for x in asm[asm.index(n) + 1:] if x in keep), None)
    if nxt:
        t = block(nxt, s)
        s = s[:t.start()] + text + '\n' + s[t.start():]
    else:
        last = max((block(x, s) for x in keep), key=lambda b: b.end())
        s = s[:last.end()] + '\n' + text + s[last.end():]
open(p, 'w').write(s)
print(len(moves), 'moved:', moves)
