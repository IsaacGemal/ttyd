"""Write a REL unit's source from the original: MSL floats, scripts (ttydasm), other data as raw words.

    python3 gen.py MOD UNIT        (UNIT as in the splits without .c, e.g. gor_12) -> rels/MOD/src/UNIT.c
Functions are not written (for units without any); tables come out as u32 arrays marked TODO: type."""
import re,sys,os,subprocess,collections,glob
MOD,UNIT=sys.argv[1],sys.argv[2]
asm=open(f'build/G8MJ01/{MOD}/asm/{UNIT}.s',errors='replace').read()
MSL=re.compile(r'^(negone_one|neg_one|__local_PI|__local_3PIO2|one_negone|__local_PIO2|one|_three|_half)\$')
# objects in order, per section
objs=[];sec=None
for m in re.finditer(r'^(\.(?:text|data|rodata|bss|ctors|dtors)\b[^\n]*)$|^# \.\w+:0x[0-9A-F]+ \| (0x[0-9A-F]+) \| size: (0x[0-9A-F]+)\n\.(obj|fn) (\S+?),? (\w+)\n(.*?)^\.end(?:obj|fn) ',asm,re.M|re.S):
    if m.group(1): sec=m.group(1).split()[0]; continue
    objs.append(dict(sec=sec,addr=int(m.group(2),16),size=int(m.group(3),16),kind=m.group(4),name=m.group(5),scope=m.group(6),body=m.group(7)))
fns=[o for o in objs if o['kind']=='fn']
strings={}
for o in objs:
    if o['sec']=='.rodata':
        s=re.findall(r'\.string "((?:[^"\\]|\\.)*)"',o['body'])
        if s: strings[o['name']]=s[0]
        elif re.fullmatch(r'\s*\.4byte 0x00000000\s*',o['body']): strings[o['name']]=''
def cstr(s):
    b=s.encode('latin1','backslashreplace').decode('unicode_escape').encode('latin1')
    b=b.split(b'\0')[0]
    t=b.decode('shift_jis')
    return '"'+t.replace('\\','\\\\').replace('"','\\"').replace('\n','\\n')+'"'
# unique extend_data index per unit within the REL: position of the unit in the REL's splits
spl=[l.strip()[:-3] for l in open(f'config/G8MJ01/{MOD}/splits.txt') if l.strip().endswith('.c:')]
base_idx=spl.index(UNIT)*4
def isscript(o):
    m=re.match(r'\s*\.4byte 0x000([0-9A-F])([0-9A-F]{4})',o['body'])  # first command: arg count, opcode 1-118
    return bool(m) and 1<=int(m.group(2),16)<=118 and bool(re.search(r'\.4byte 0x00000001(\s*\.4byte 0x00000000)*\s*$',o['body']))
out=[];decl=[];fd=0;names={o['name'] for o in objs}
defined=set()
used_before=collections.Counter()
def ref(sym):
    if sym in strings: return cstr(strings[sym])
    return sym
def wrap_ptr(line):
    if not re.match(r'\s*[A-Z_0-9]+\(',line): return line
    parts=re.split(r'(STRING\("(?:[^"\\]|\\.)*"\))',line)
    for i in range(0,len(parts),2):
        p=parts[i]
        m=re.match(r'(\s*CALL\()(\w+)(.*)',p) if i==0 else None
        head,rest=(m.group(1)+m.group(2),m.group(3)) if m else (re.match(r'(\s*[A-Z_0-9]+\()?',p).group(0) if i==0 else '', p[len(re.match(r'(\s*[A-Z_0-9]+\()?',p).group(0)):] if i==0 else p)
        rest=re.sub(r'(?<![\w"])([A-Za-z_][A-Za-z0-9_]*)(?![\w(])',lambda mm: mm.group(1) if mm.group(1)=='EVTDAT_POINTER_MAX' else f'PTR({mm.group(1)})',rest)
        parts[i]=head+rest
    s=''.join(parts)
    return re.sub(r'\b(CALL_CHILD|SPAWN_EVENT)\(PTR\((\w+)\)\)',r'\1(\2)',s)
i=0;todo=[];placed=set();bytes_objs=set()
def as_bytes(o):
    """The object's bytes if it has no pointers and needs byte granularity (.string/.byte/.2byte/odd .skip)."""
    import struct
    b=bytearray(); odd=False
    for m in re.finditer(r'^\s*\.(\w+) (.*)$',o['body'],re.M):
        k,v=m.group(1),m.group(2).strip()
        if k=='4byte':
            if not v.startswith('0x'): return None
            b+=int(v,16).to_bytes(4,'big')
        elif k=='float': b+=struct.pack('>f',float(v))
        elif k=='double': b+=struct.pack('>d',float(v))
        elif k=='2byte': b+=int(v,0).to_bytes(2,'big'); odd=True
        elif k=='byte': b+=bytes(int(x,0)&0xFF for x in v.split(',')); odd=True
        elif k=='skip': b+=bytes(int(v,0)); odd=odd or int(v,0)%4!=0
        elif k=='string':
            b+=re.fullmatch(r'"((?:[^"\\]|\\.)*)"',v).group(1).encode('latin1','backslashreplace').decode('unicode_escape').encode('latin1')+b'\0'; odd=True
        else: return None
    return bytes(b) if odd and len(b)==o['size'] else None
def placeholder(f):
    return (f"// TODO: write {f['name']} ({'global' if f['scope']=='global' else 'static'}, {f['size']:#x} bytes) here:\n"
            f"// its asm is in build/G8MJ01/{MOD}/asm/{UNIT}.s; this is where the original has it, just before the first script that uses it")
data=[o for o in objs if o['sec']=='.data']
while i<len(data):
    o=data[i]
    if MSL.match(o['name']):
        grp=[]
        while i<len(data) and (MSL.match(data[i]['name']) or data[i]['name'].startswith('gap_')):
            if not data[i]['name'].startswith('gap_'): grp.append(data[i])
            i+=1
        # split into sets starting at each negone_one
        sets=[];cur=[]
        for g in grp:
            if g['name'].startswith(('negone_one','_three')) and cur: sets.append(cur);cur=[]
            cur.append(g)
        if cur: sets.append(cur)
        for st in sets:
            vals=[]
            for g in st:
                n=g['name'].split('$')[0]
                w=re.findall(r'\.(4byte|float|double) (\S+)',g['body'])
                if n in('negone_one','one_negone'): vals.append(f"     u64, {n}, 0x{w[0][1][2:]}{w[1][1][2:]}")
                elif n=='neg_one': vals.append(f"     u32, {n}, 0xBF800000")
                elif n=='__local_PI': vals.append(f"     f32, {n}, 3.1415926535897932384626433832795f")
                elif n=='__local_3PIO2': vals.append(f"     f32, {n}, 3.0f * 1.57079632679489661923132169163975f")
                elif n=='__local_PIO2': vals.append(f"     f32, {n}, 1.57079632679489661923132169163975f")
                elif n=='one': vals.append(f"     f32, {n}, 1.0f")
                elif n in('_three','_half'): vals.append(f"     f64, {n}, {w[0][1]}")
            out.append(f"extend_data({base_idx+fd},\n"+',\n'.join(vals)+")")
            fd+=1
        continue
    if o['name'].startswith('gap_'):
        i+=1; continue
    st='' if o['scope']=='global' else 'static '
    for f in fns:
        if f['name'] not in placed and re.search(rf'\b{re.escape(f["name"])}\b',o['body']):
            out.append(placeholder(f)); placed.add(f['name'])
    if isscript(o):
        t=subprocess.run(['tools/ttydasm/target/release/ttydasm','extract','-m',MOD,'-s',o['name'],'-a',hex(o['addr']),'-p','.'],capture_output=True,text=True)
        if t.returncode: sys.exit(f"ttydasm failed on {o['name']}: {t.stderr[-300:]}")
        lines=[l for l in t.stdout.rstrip('\n').split('\n') if l.strip()!='END']
        body=[lines[0]]+[wrap_ptr(l) for l in lines[1:]]
        out.append(st+'\n'.join(body))
    elif as_bytes(o) is not None:
        # plain bytes (strings, .byte, odd .skip) with no pointers: a u8 array holding exactly those bytes
        b=as_bytes(o)
        todo.append(o['name']); bytes_objs.add(o['name'])
        table=f"{st}u8 {o['name']}[{len(b)}] = {{\n    "+',\n    '.join(', '.join(f'0x{x:02X}' for x in b[j:j+16]) for j in range(0,len(b),16))+",\n};"
        out.append("// TODO: type\n"+table)
    else:
        words=[]
        for m in re.finditer(r'^\s*\.(4byte|float|string|byte|2byte|double|rel|skip) (.*)$',o['body'],re.M):
            k,v=m.group(1),m.group(2).strip()
            if k=='4byte':
                if v.startswith('0x'): words.append(v)
                elif v in strings: words.append(f'(u32){cstr(strings[v])}')
                else: words.append(f'(u32)&{v}' if v in names else f'(u32){v}')
            elif k=='float':
                import struct; words.append('0x%08X'%struct.unpack('>I',struct.pack('>f',float(v)))[0])
            elif k=='skip' and int(v,0)%4==0:
                words+=['0x00000000']*(int(v,0)//4)
            elif k=='rel':
                sym,lbl=[x.strip() for x in v.split(',')]
                tgt=[x for x in objs if x['name']==sym]
                if not tgt or not lbl.startswith('.L_'): sys.exit(f"{o['name']}: unsupported .rel {v}")
                off=int(lbl[3:],16)-tgt[0]['addr']
                words.append(f'(u32)((u8*)&{sym} + {off:#x})')
            else: sys.exit(f"{o['name']}: unsupported .{k}")
        if len(words)*4!=o['size']: sys.exit(f"{o['name']}: {len(words)*4} bytes vs {o['size']}")
        todo.append(o['name'])
        table=f"{st}u32 {o['name']}[{len(words)}] = {{\n    "+',\n    '.join(', '.join(words[j:j+4]) for j in range(0,len(words),4))+",\n};"
        if '$' in o['name']:
            # an initialized static local (name$NNN): it belongs inside its function; left here commented out
            out.append(f"// TODO: {o['name']} is a static local of a function: move it inside that function as\n"
                       f"// `static` with this data (it's here at its place in .data)\n"+'\n'.join('// '+l for l in table.split('\n')))
        else:
            out.append("// TODO: type\n"+table)
    defined.add(o['name']); i+=1
for f in fns:
    if f['name'] not in placed: out.append(placeholder(f))
bss=[o for o in objs if o['sec']=='.bss']
if bss:
    # a name with $ is a function's static local (name$NNN): it belongs inside that function, not at file scope
    out.insert(0,'// TODO: type these (.bss, in original order)\n'+'\n'.join(
        f"// {o['name']} ({o['size']:#x} bytes): a static local of a function; declare it inside that function"
        if '$' in o['name'] else f"{'' if o['scope']=='global' else 'static '}u8 {o['name']}[{o['size']:#x}];" for o in bss))
    defined.update(o['name'] for o in bss)
# forward declarations for local objects used before their definition
src='\n\n'.join(out)
fwd=[]
seen=set()
for o in data:
    if o['name'] in defined:
        pos=src.find(f"({o['name']})") if isscript(o) else -1
for o in data:
    n=o['name']
    if n not in defined: continue
    dpos=min([p for p in (src.find(f'EVT_BEGIN({n})'),src.find(f' {n}[')) if p>=0] or [0])
    if re.search(rf'[(&]\s*{re.escape(n)}\b',src[:dpos]):
        st='' if o['scope']=='global' else 'static '
        fwd.append(f"{st}s32 {n}[{o['size']//4}];" if isscript(o) else f"{st}u8 {n}[{o['size']}];" if n in bytes_objs else f"{st}u32 {n}[{o['size']//4}];")
# headers declaring the names the scripts use
idents=set(re.findall(r'\b([A-Za-z_]\w*)\b',re.sub(r'"(?:[^"\\]|\\.)*"','',re.sub(r'//[^\n]*','',src))))-defined-{f['name'] for f in fns}-{'negone_one','neg_one','__local_PI','__local_3PIO2','one_negone','__local_PIO2','one','_three','_half'}
hdrs=set()
for h in glob.glob('include/**/*.h',recursive=True):
    t=open(h,errors='replace').read()
    for n in list(idents):
        if re.search(rf'(USER_FUNC|EVT_DEF)\({n}\)|\b{n}\s*\(|\b{n}\[\]',t): hdrs.add(h[8:])
hdrs|={'evt/evt_cmd.h','forcedata.h'}
missing=[n for n in idents if not any(re.search(rf'(USER_FUNC|EVT_DEF)\({n}\)|\b{n}\s*\(|\b{n}\[\]',open('include/'+h,errors='replace').read()) for h in hdrs)
         and not n.isupper() and not re.match(r'^(LW|LF|GW|GF|GSW|GSWF|LSW|LSWF|UW|UF|FLOAT|STRING|PTR|EVT_BEGIN|EVT_END|EVTDAT_POINTER_MAX|extend_data|static|u8|u32|s32|u64|f32|f64|TODO|type)$',n)]
ext=[f'extern s32 {n}[];' for n in sorted(missing)]
# declarations of the unit's own functions, so the file builds before they're written: global ones (and global
# scripts) in the unit's header, static ones here
called={m for m in re.findall(r'\bCALL\((\w+)',src)}
# functions the SDK headers already declare (the REL's own copies of atan2f, GX inlines): include the header instead
sdk={}
for root in ('libs/dolsdk2004/include','libs/dolsdk2004/include/libc'):
    for d,_,fs_ in os.walk(root):
        for fn_ in fs_:
            if fn_.endswith('.h'):
                ht=open(os.path.join(d,fn_),errors='replace').read()
                for f in fns:
                    if f['scope']=='global' and f['name'] not in sdk and re.search(rf'^[^\n#/]*\b{re.escape(f["name"])}\s*\(',ht,re.M):
                        sdk[f['name']]=os.path.relpath(os.path.join(d,fn_),root)
fns_decl=[f for f in fns if f['name'] not in sdk]
hdrs|=set(sdk.values())
def fdecl(f): return f'USER_FUNC({f["name"]});' if f['name'] in called else f'void {f["name"]}(void);  // TODO: real signature'
hdr_lines=[fdecl(f) for f in fns_decl if f['scope']=='global']+[f'EVT_DEF({o["name"]});' for o in data if o['scope']=='global' and isscript(o)]
fwd=[('static '+fdecl(f)) for f in fns if f['scope']!='global']+fwd
if hdr_lines:
    hname=f'rels/{MOD}/include/{os.path.basename(UNIT)}.h'
    os.makedirs(os.path.dirname(hname),exist_ok=True)
    open(hname,'w').write('#pragma once\n\n#include "evt/evt_cmd.h"\n\n'+'\n'.join(hdr_lines)+'\n')
    hdrs.add('__UNIT__')
text=(f'#include "{os.path.basename(UNIT)}.h"\n' if '__UNIT__' in hdrs else '')+''.join(f'#include "{h}"\n' for h in sorted(hdrs-{'__UNIT__'}))+('\n#pragma pool_data off\n' if fns else '')+'\n#pragma force_active on\n'+'\n'.join(ext+fwd)+('\n\n' if ext or fwd else '\n')+'// clang-format off\n'+src+'\n// clang-format on\n'+('' if fns else '#pragma force_active off\n')
os.makedirs(os.path.dirname(f'rels/{MOD}/src/{UNIT}.c'),exist_ok=True); os.makedirs(f'rels/{MOD}/include',exist_ok=True)
open(f'rels/{MOD}/src/{UNIT}.c','w').write(text)
print(f'{MOD}/{UNIT}: {len(data)} data objects, {len(todo)} tables to type, {len(ext)} externs')
