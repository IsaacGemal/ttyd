"""Turn a data-only unit's disassembly into C source.

Usage (from the repo root, after a build): python3 tools/asm2c_data.py <unit> > src/<unit>.c
where <unit> is the split's name without .c (e.g. data/party_data). Reads build/G8MJ01/asm/<unit>.s.

Each `.obj` becomes a definition in the section it sits in: strings as char arrays, uniform words/halves/bytes/floats
as arrays (symbol references as `(u32)(void*)sym`), numeric objects of mixed widths as u8 arrays, other mixed objects
as structs, loose trailing padding as a static u32. Objects too small for their section (.data <= 48 bytes,
.rodata <= 8, .sdata2 strings > 6) get __declspec(section). The output is a starting point for a unit with no
source: check it with the unit's flip to Matching and a full build, and add `#pragma force_active on` around data that
nothing in the DOL references (the linker strips it otherwise). Types are raw, not the game's.
"""
import re,sys
unit=sys.argv[1]   # e.g. data/party_data
src=open('build/G8MJ01/asm/%s.s'%unit).read()
lines=src.split('\n')
sec=None; objs=[]; cur=None; labels={}
for l in lines:
    ms=re.match(r'^\.section (\.\w+)',l)
    if ms: sec=ms.group(1); continue
    m=re.match(r'^\.(text|data|rodata|sdata|sdata2|bss|sbss|sbss2|ctors|dtors|init|extab|extabindex)\b',l)
    if m and not l.startswith('.obj'): sec='.'+m.group(1); continue
    m=re.match(r'^\.obj (\S+), (global|local)',l)
    if m: cur=dict(name=m.group(1),bind=m.group(2),sec=sec,items=[]); continue
    if l.startswith('.endobj'): objs.append(cur); cur=None; continue
    if cur is None:
        t0=l.strip()
        m0=re.match(r'\.(4byte|2byte|byte|skip)\s+(.*)$',t0)
        if m0 and sec and objs is not None:
            if objs and objs[-1].get('pad') and objs[-1]['sec']==sec: objs[-1]['items'].append((m0.group(1),m0.group(2)))
            else: objs.append(dict(name='pad_%s_%d'%(sec[1:],len(objs)),bind='local',sec=sec,items=[(m0.group(1),m0.group(2))],pad=True))
        continue
    t=l.strip()
    if not t or t.startswith('#'): continue
    ml=re.match(r'^(\.L_\w+):$',t)
    if ml:
        off=0
        for k,v in cur['items']:
            off+={'4byte':4,'2byte':2,'byte':1,'float':4,'double':8,'8byte':8}.get(k,0)+ (int(v,0) if k=='skip' else 0)
        labels[ml.group(1)]=(cur['name'],off); continue
    mr=re.match(r'^\.rel (\S+), (\.L_\w+)$',t)
    if mr:
        cur['items'].append(('rel',mr.group(1)+' '+mr.group(2))); continue
    m=re.match(r'\.(4byte|2byte|byte|float|double|string|skip|8byte)\s*(.*)$',t)
    if not m: print('UNHANDLED',cur['name'],t,file=sys.stderr); continue
    cur['items'].append((m.group(1),m.group(2)))
for o in objs:
    o['items']=[(('4byte','%s+%d'%(labels[v.split()[1]][0],labels[v.split()[1]][1])) if k=='rel' else (k,v)) for k,v in o['items']]
    if o['name'].startswith('gap_'): o['bind']='local'
def flatten(o):
    ks={k for k,_ in o['items']}
    if len(ks)>1 and ks<= {'4byte','2byte','byte'} and all(re.match(r'^(0x[0-9a-fA-F]+|-?\d+)$',v.strip()) for k,v in o['items']):
        b=[]
        for k,v in o['items']:
            n={'4byte':4,'2byte':2,'byte':1}[k]; x=int(v.strip(),0)&((1<<(8*n))-1)
            b+=list(x.to_bytes(n,'big'))
        o['items']=[('byte','0x%02X'%x) for x in b]
for o in objs: flatten(o)
defined={o['name'] for o in objs}
structs=set()
for o in objs:
    ks={k for k,_ in o['items']}
    if not (all(k=='string' for k in ks) or (len(ks)==1 and list(ks)[0] in ('4byte','2byte','byte','float','double','8byte'))): structs.add(o['name'])
ext=set()
def cval(kind,v):
    v=v.strip()
    if kind in('4byte','2byte','byte','8byte'):
        if re.match(r'^(0x[0-9a-fA-F]+|-?\d+)$',v): return v,False
        ext.add(re.split(r'[+-]',v)[0].strip()); 
        base=re.split(r'[+-]',v)[0].strip(); off=v[len(base):].strip()
        if base in structs and not off: return '(u32)(void*)&'+base, True
        return ('(u32)(void*)'+base if not off else '(u32)((u8*)%s%s %s)'%('&' if base in structs else '',base,off)),True
    if kind=='float':
        return v if ('.' in v or 'e' in v.lower()) else v+'.0', False
    return v,False
out=[]
sects=[]
for o in objs:
    if o['sec'] not in sects: sects.append(o['sec'])
def strdecode(s):
    # keep escapes as in asm
    return s
body=[]
fwd=[]
order={'.rodata':0,'.sdata2':1,'.sdata':2,'.data':3,'.sbss':4,'.bss':5,'.sbss2':6}
objs=sorted(objs,key=lambda o:order.get(o['sec'],9))  # stable: keeps order within a section
for o in objs:
    n=o['name']; items=o['items']; sec=o['sec']
    const = sec in ('.rodata','.sdata2')
    static = 'static ' if o['bind']=='local' else ''
    q=static+('const ' if const else '')
    if all(k=='string' for k,_ in items):
        txt=''.join(re.match(r'"(.*)"$',v.strip()).group(1) for k,v in items)
        # size: compute from comment line? use decoded length
        def declen(t):
            i=0;c=0
            while i<len(t):
                if t[i]=='\\':
                    if t[i+1] in '01234567': j=i+1; 
                    if t[i+1] in '01234567':
                        j=i+1
                        while j<len(t) and j<i+4 and t[j] in '01234567': j+=1
                        i=j
                    else: i+=2
                else: i+=1
                c+=1
            return c
        size=declen(txt)+1  # .string adds terminator
        # trailing \000 escapes produced by dtk for padding beyond the terminator: exact bytes = decoded len + 1
        qq=q+('__declspec(section ".sdata2") ' if (sec=='.sdata2' and size>6) else '')
        fwd.append('%s%schar %s[%d];'%('static ' if static else 'extern ',('const ' if const else ''),n,size))
        body.append('%schar %s[%d] = "%s";'%(qq,n,size,txt))
        continue
    kinds={k for k,_ in items}
    if len(kinds)==1 and list(kinds)[0] in ('4byte','2byte','byte','float','double','8byte'):
        k=list(kinds)[0]
        ty={'4byte':'u32','2byte':'u16','byte':'u8','float':'f32','double':'f64','8byte':'u64'}[k]
        vals=[cval(k,v)[0] for _,v in items]
        rows=[', '.join(vals[i:i+8]) for i in range(0,len(vals),8)]
        fwd.append('%s%s%s %s[%d];'%('static ' if static else 'extern ',('const ' if const else ''),ty,n,len(vals)))
        wid={'u32':4,'u16':2,'u8':1,'f32':4,'f64':8,'u64':8}[ty]
        q2=q+('__declspec(section ".data") ' if sec=='.data' and len(vals)*wid<=48 else '')+('__declspec(section ".rodata") ' if sec=='.rodata' and len(vals)*wid<=8 else '')
        body.append('%s%s %s[%d] = {\n    %s\n};'%(q2,ty,n,len(vals),',\n    '.join(rows)))
        continue
    # mixed -> struct
    fields=[];inits=[]
    for i,(k,v) in enumerate(items):
        if k=='skip': fields.append('u8 f%d[%s];'%(i,v)); inits.append('{0}'); continue
        if k=='string':
            txt=re.match(r'"(.*)"$',v.strip()).group(1); 
            fields.append('char f%d[%d];'%(i,len(txt.encode().decode('unicode_escape'))+1)); inits.append('"%s"'%txt); continue
        ty={'4byte':'u32','2byte':'u16','byte':'u8','float':'f32','double':'f64','8byte':'u64'}[k]
        fields.append('%s f%d;'%(ty,i)); inits.append(cval(k,v)[0])
    body.append('%sstruct {\n    %s\n} %s = {\n    %s\n};'%(q,'\n    '.join(fields),n,',\n    '.join(inits)))
ext-=defined
hdr=['#include <dolphin/types.h>','']+['extern u8 %s[];'%e for e in sorted(ext)]+['']+fwd+['']
print('\n'.join(hdr[:1])+'\n#pragma force_active on\n'+'\n'.join(hdr[1:])+'\n'+'\n\n'.join(body)+'\n#pragma force_active reset')
