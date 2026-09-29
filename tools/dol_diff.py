import struct,sys
def load(p):
    d=open(p,'rb').read()
    offs=struct.unpack('>18I',d[0:72]); addrs=struct.unpack('>18I',d[72:144]); sizes=struct.unpack('>18I',d[144:216])
    return d,[(offs[i],addrs[i],sizes[i]) for i in range(18) if sizes[i]]
a,sa=load('orig/G8MJ01/sys/main.dol'); b,sb=load('build/G8MJ01/MarioSt.dol')
print('sections orig',[(hex(x[1]),hex(x[2])) for x in sa]); print('sections ours',[(hex(x[1]),hex(x[2])) for x in sb])
def mem(d,secs):
    m={}
    for off,ad,sz in secs:
        for i in range(sz): m[ad+i]=d[off+i]
    return m
ma=mem(a,sa); mb=mem(b,sb)
diff=sorted(k for k in ma if ma[k]!=mb.get(k))
print(len(diff),'bytes differ; first', [hex(x) for x in diff[:10]], 'last',hex(diff[-1]) if diff else None)
ro=[k for k in diff if 0x802c0f20<=k<0x80302c20]
print('first rodata diff',hex(ro[0]) if ro else None)
