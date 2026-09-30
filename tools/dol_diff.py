"""Compare build/G8MJ01/MarioSt.dol with orig/G8MJ01/sys/main.dol: the differing words, each with the symbol
(config/G8MJ01/symbols.txt) it falls in. Use it when a unit scores 100% but the checksum fails (pool or data order,
section alignment).

    python3 tools/dol_diff.py [MAX_ROWS]
"""
import re,struct,sys,bisect
a=open('build/G8MJ01/MarioSt.dol','rb').read()
b=open('orig/G8MJ01/sys/main.dol','rb').read()
# dol header: 18 sections
offs=struct.unpack('>18I',b[0:72]); addrs=struct.unpack('>18I',b[72:144]); sizes=struct.unpack('>18I',b[144:216])
secs=[(offs[i],addrs[i],sizes[i]) for i in range(18) if sizes[i]]
syms=[]
for l in open('config/G8MJ01/symbols.txt'):
    m=re.match(r'(\S+) = (\.\w+):(0x[0-9A-Fa-f]+); // (.*)',l)
    if m:
        sz=re.search(r'size:(0x[0-9A-Fa-f]+)',m.group(4))
        syms.append((int(m.group(3),16),m.group(1),int(sz.group(1),16) if sz else 0))
syms.sort()
keys=[s[0] for s in syms]
def sym(addr):
    i=bisect.bisect_right(keys,addr)-1
    return f"{syms[i][1]}+{addr-syms[i][0]:#x}" if i>=0 else '?'
n=0;prev=None;rows=[]
for off,addr,size in secs:
    for i in range(off,off+size):
        if a[i]!=b[i]:
            w=i&~3
            if w!=prev:
                rows.append((addr+(w-off),a[w:w+4].hex(),b[w:w+4].hex())); prev=w
print(len(rows),'words differ')
for r in rows[:int(sys.argv[1]) if len(sys.argv)>1 else 20]:
    print(hex(r[0]),r[1],r[2],sym(r[0]))
