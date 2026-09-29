"""Compare a unit's compiled object with the original: section sizes, bytes, and relocation targets.
    python3 cmp.py MOD UNIT  -> prints OK or the first differences"""
import struct,sys,re
MOD,UNIT=sys.argv[1],sys.argv[2]
def load(path):
    d=open(path,'rb').read()
    shoff,=struct.unpack('>I',d[0x20:0x24]); shentsize,shnum,shstrndx=struct.unpack('>HHH',d[0x2E:0x34])
    secs=[struct.unpack('>IIIIIIIIII',d[shoff+i*shentsize:shoff+(i+1)*shentsize]) for i in range(shnum)]
    so=secs[shstrndx][4]; nm=lambda x: d[so+x:d.index(b'\0',so+x)].decode()
    names=[nm(s[0]) for s in secs]
    symsec=[s for s in secs if s[1]==2][0]; st=secs[symsec[6]][4]
    syms=[]
    for j in range(symsec[5]//16):
        n,val,sz,info,oth,shn=struct.unpack('>IIIBBH',d[symsec[4]+j*16:symsec[4]+j*16+16])
        syms.append((d[st+n:d.index(b'\0',st+n)].decode('latin1'),val,shn))
    body={names[i]:(d[s[4]:s[4]+s[5]] if s[1]!=8 else None,s[5]) for i,s in enumerate(secs) if names[i] in('.text','.data','.rodata','.bss')}
    rel={}
    for s in secs:
        if s[1]==4 and names[s[7]] in body:
            r={}
            for j in range(s[5]//12):
                off,info,add=struct.unpack('>IIi',d[s[4]+j*12:s[4]+j*12+12]); n,val,shn=syms[info>>8]
                tsec=names[shn] if shn<len(names) else '?'
                r[off]=(n,tsec,val+add)
            rel[names[s[7]]]=r
    return body,rel
A,AR=load(f'build/G8MJ01/{MOD}/obj/{UNIT}.o'); import json
_u=next((u for u in json.load(open('objdiff.json'))['units'] if u['name']==f'{MOD}/{UNIT}'),None)
B,BR=load(_u['base_path'] if _u else f'build/G8MJ01/src/{UNIT}.o')
def target(r,body):
    n,tsec,off=r
    if tsec=='.rodata' and body.get('.rodata') and body['.rodata'][0] is not None:
        b=body['.rodata'][0][off:]; return ('str',b[:b.index(b'\0')] if b'\0' in b else b)
    if tsec in('.data','.bss','.text'): return ('local',tsec,off)
    return ('sym',n)
bad=[]
for sec in ('.text','.data','.rodata','.bss'):
    a=A.get(sec,(None,0)); b=B.get(sec,(None,0))
    if a[1]!=b[1]: bad.append(f'{sec} size {a[1]:#x} (original) vs {b[1]:#x}')
    if a[0] is not None and b[0] is not None:
        dif=[i for i in range(min(len(a[0]),len(b[0]))) if a[0][i]!=b[0][i] and not (i//4*4 in AR.get(sec,{}) or i//4*4 in BR.get(sec,{}))]
        if dif: bad.append(f'{sec} bytes differ from {dif[0]:#x} ({len(dif)} bytes)')
    ra=AR.get(sec,{}); rb=BR.get(sec,{})
    for off in sorted(set(ra)|set(rb)):
        ta=target(ra[off],A) if off in ra else None; tb=target(rb[off],B) if off in rb else None
        if ta!=tb: bad.append(f'{sec}+{off:#x}: original -> {ta}, ours -> {tb}'); 
        if len(bad)>12: break
def force_active(path):
    # per-symbol flags of the object's .comment section (8 bytes each after a 0x2C header): 0x0008 keeps the symbol
    # through the linker's dead-stripping. An unreferenced static function without it vanishes from the REL while the
    # object still compares equal.
    d=open(path,'rb').read()
    shoff,=struct.unpack('>I',d[0x20:0x24]); shentsize,shnum,shstrndx=struct.unpack('>HHH',d[0x2E:0x34])
    secs=[struct.unpack('>IIIIIIIIII',d[shoff+i*shentsize:shoff+(i+1)*shentsize]) for i in range(shnum)]
    so=secs[shstrndx][4]
    names=[d[so+s[0]:d.index(b'\0',so+s[0])].decode() for s in secs]
    if '.comment' not in names or '.symtab' not in names: return {}
    cm=secs[names.index('.comment')]; st=secs[names.index('.symtab')]; ss=secs[st[6]]
    out={}
    for j in range(st[5]//16):
        n,val,sz,info,oth,shn=struct.unpack('>IIIBBH',d[st[4]+j*16:st[4]+j*16+16])
        if info&0xf!=2: continue   # functions only
        e=cm[4]+0x2c+8*j
        nm=d[ss[4]+n:d.index(b'\0',ss[4]+n)].decode('latin1')
        out[nm]=bool(struct.unpack('>H',d[e+4:e+6])[0]&8)
    return out
_orig=f'build/G8MJ01/{MOD}/obj/{UNIT}.o'; _ours=_u['base_path'] if _u else f'build/G8MJ01/src/{UNIT}.o'
_fa,_fb=force_active(_orig),force_active(_ours)
for nm in sorted(_fa):
    if _fa[nm] and not _fb.get(nm,True): bad.append(f'function {nm} lacks the force-active flag (write `#pragma force_active on` and do not end the file with `off`)')
print('OK' if not bad else '\n'.join(bad[:12]))
sys.exit(1 if bad else 0)
