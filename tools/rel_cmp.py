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
                r[off]=(n,tsec,val+add,info&0xff)
            rel[names[s[7]]]=r
    return body,rel
A,AR=load(f'build/G8MJ01/{MOD}/obj/{UNIT}.o'); import json
_u=next((u for u in json.load(open('objdiff.json'))['units'] if u['name']==f'{MOD}/{UNIT}'),None)
B,BR=load(_u['base_path'] if _u else f'build/G8MJ01/src/{UNIT}.o')
def target(r,body):
    # the relocation's kind and where it points: a section of this object plus offset (the sections' bytes are
    # compared separately, so equal offsets mean equal targets), or an external symbol plus addend
    n,tsec,off,typ=r
    if tsec in('.data','.bss','.text','.rodata'): return ('local',tsec,off,typ)
    return ('sym',n,off,typ)
bad=[]
for sec in ('.text','.data','.rodata','.bss'):
    a=A.get(sec,(None,0)); b=B.get(sec,(None,0))
    if a[1]!=b[1]: bad.append(f'{sec} size {a[1]:#x} (original) vs {b[1]:#x}')
    if a[0] is not None and b[0] is not None:
        # a relocated word may differ only in the bits the relocation fills in (per type); everything else must match
        MASK={1:0xFFFFFFFF,4:0xFFFF,5:0xFFFF,6:0xFFFF,10:0x03FFFFFC,11:0x0000FFFC,24:0xFFFF,26:0xFFFF}
        def _word_ok(w):
            ra,rb=AR.get(sec,{}).get(w),BR.get(sec,{}).get(w)
            if ra is None and rb is None: return False
            m=MASK.get((ra or rb)[3],0xFFFFFFFF)
            if ra is None or rb is None or ra[3]!=rb[3]: m=0xFFFFFFFF
            wa=int.from_bytes(a[0][w:w+4],'big'); wb=int.from_bytes(b[0][w:w+4],'big')
            return (wa&~m&0xFFFFFFFF)==(wb&~m&0xFFFFFFFF)
        dif=[i for i in range(min(len(a[0]),len(b[0]))) if a[0][i]!=b[0][i] and not _word_ok(i//4*4)]
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
        out[nm]=(bool(struct.unpack('>H',d[e+4:e+6])[0]&8), info>>4==0, val, sz)
    return out
_orig=f'build/G8MJ01/{MOD}/obj/{UNIT}.o'; _ours=_u['base_path'] if _u else f'build/G8MJ01/src/{UNIT}.o'
_fa,_fb=force_active(_orig),force_active(_ours)
# only a static function nothing references gets dead-stripped (globals are kept by the ldscript's FORCEACTIVE list),
# so the flag matters only there; references are relocations to the function's name or to its .text offset
# a relocation inside the function it points to (recursion) doesn't keep that function alive
_fnrange={nm:(v[2],v[2]+v[3]) for nm,v in _fb.items()}
def _inside_target(sec,off,t):
    if sec!='.text': return False
    for nm,(lo,hi) in _fnrange.items():
        if (t[0]==nm or (t[1]=='.text' and t[2]==lo)) and lo<=off<hi: return True
    return False
_refs={t[0] for sec,r in BR.items() for off,t in r.items() if not _inside_target(sec,off,t)}|{('.text',t[2]) for sec,r in BR.items() for off,t in r.items() if t[1]=='.text' and not _inside_target(sec,off,t)}
def _unreferenced_static(nm):
    # static in either object: a function the original has static but ours made global is stripped all the same
    fa,local,addr,_=_fb.get(nm,(True,False,0,0))
    local=local or _fa.get(nm,(False,False,0,0))[1]
    return local and nm not in _refs and ('.text',addr) not in _refs
for nm in sorted(_fa):
    if _fa[nm][0] and nm in _fb and not _fb[nm][0] and _unreferenced_static(nm): bad.append(f'function {nm} lacks the force-active flag (write `#pragma force_active on` and do not end the file with `off`)')
print('OK' if not bad else '\n'.join(bad[:12]))
sys.exit(1 if bad else 0)
