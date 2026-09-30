#!/usr/bin/env python3
"""Turn a dol unit's extern pool constants into literals so the compiler builds the unit's .rodata/.sdata2 itself.

    python3 tools/pool_literals.py UNIT          e.g. effect/eff_miss_star  (edits src/UNIT.c in place)

float/double externs become literals, named double_to_int_*/vec3_*/str_* definitions go away (zero and constant Vecs become
(Vec){...}), dat_* colours become (GXColor){...}, zero .sbss2 GXColor scratch objects become (GXColor){0,0,0,0}, RO(base, off, sym)
becomes the string. Then check with tools/agent_check.py UNIT and tools/autolink.py UNIT.c (the ledger's Grind 21 chapter has
the method and the ways it can still fail: pool order, .data objects the unit owns)."""
import re,sys,subprocess
unit=sys.argv[1]           # e.g. effect/eff_miss_star
src=f"src/{unit}.c"
asm=open(f"build/G8MJ01/asm/{unit}.s").read()
s=open(src).read()
# float/double constants in asm (sdata2)
vals={}
for m in re.finditer(r"\.obj (\w+), \w+\n\t\.(float|double) ([^\n]+)\n(?=\.endobj)",asm):
    n,kind,v=m.groups()
    if kind=="float":
        try: x=float(v)
        except: continue
        import struct
        x=struct.unpack('f',struct.pack('f',x))[0]
        t=None
        for k in range(1,10):
            c='%.*g'%(k,x)
            if struct.unpack('f',struct.pack('f',float(c)))[0]==x:
                t=c if 'e' not in c else ('%.12f'%float(c)).rstrip('0'); break
        if t is None: t=repr(float(f'{x:.9g}'))
        if '.' not in t and 'e' not in t: t+='.0'
        vals[n]=t+'f'
    else:
        vals[n]=v if ('.' in v or 'e' in v) else v+".0"
strs={}
for m in re.finditer(r"\.obj (str_\w+), \w+\n\t\.string \"((?:[^\"\\]|\\.)*)\"\n",asm):
    n,t=m.groups(); strs[n]=t.split("\\000")[0]
changed=0
DECL_DUMMY=1
DECL=r'(?:__declspec\(section "\.sdata2"\) )?(?:extern )?(?:volatile )?(?:const )?(?:volatile )?f(?:32|64) %s(?: = [^;]*)?;[ \t]*(?://[^\n]*)?\n'
out=s
# remove extern decls of float symbols & defs
for n,lit in vals.items():
    if n.startswith("double_to_int"): 
        out=re.sub(DECL%n,"",out); continue
    out=re.sub(DECL%n,"",out)
    if re.search(r"\b%s\b"%n,out):
        out=re.sub(r"\b%s\b"%n,lit,out); changed+=1
# vec3 defs
zero=set()
for m in re.finditer(r'\.obj (vec3_\w+), \w+\n\t\.float 0\n\t\.float 0\n\t\.float 0\n',asm): zero.add(m.group(1))
for n in zero: out=re.sub(r'= %s;'%n,'= (Vec){ 0.0f, 0.0f, 0.0f };',out)
import struct as _st
def _f(v):
    x=_st.unpack('f',_st.pack('f',float(v)))[0]
    for k in range(1,10):
        c='%.*g'%(k,x)
        if _st.unpack('f',_st.pack('f',float(c)))[0]==x:
            c=c if 'e' not in c else ('%.12f'%float(c)).rstrip('0')
            if '.' not in c: c+='.0'
            return c+'f'
    return repr(x)+'f'
for m in re.finditer(r'\.obj (vec3_\w+), \w+\n\t\.float (\S+)\n\t\.float (\S+)\n\t\.float (\S+)\n(?=\.endobj)',asm):
    n=m.group(1)
    lit='(Vec){ %s, %s, %s }'%(_f(m.group(2)),_f(m.group(3)),_f(m.group(4)))
    out=re.sub(r'(?:extern )?const Vec %s(?: = \{[^}]*\})?;[ \t]*(?://[^\n]*)?\n'%n,'',out)
    out=re.sub(r'(?<![&\w])%s\b'%n,lit,out)
out=re.sub(r"const Vec vec3_\w+ = \{[^}]*\};\n","",out)
# strings
for n,t in strs.items():
    d=re.search(r"const char %s\[[^\]]*\] = \"[^\"]*\";\n"%n,out)
    if d: out=out.replace(d.group(0),"")
    out=re.sub(r"extern (?:const )?(?:char|u32|u8) %s(?:\[[^\]]*\])?;\n"%n,"",out)
    if re.search(r"\b%s\b"%n,out): out=re.sub(r"\b%s\b"%n,'"'+t+'"',out); changed+=1


def _ro(m):
    n=m.group(1)
    return '"'+strs[n]+'"' if n in strs else m.group(0)
out=re.sub(r"RO\(\w+, 0x[0-9A-Fa-f]+, (\w+)\)",_ro,out)
# 4-byte colour constants
for m in re.finditer(r"\.obj (dat_\w+), \w+\n\t\.4byte 0x([0-9A-Fa-f]{8})\n(?=\.endobj)",asm):
    n,h=m.groups()
    lit="(GXColor){ 0x%s, 0x%s, 0x%s, 0x%s }"%(h[0:2],h[2:4],h[4:6],h[6:8])
    out=re.sub(r"extern (?:const )?(?:volatile )?(?:const )?(?:GXColor|u32) %s;[ \t]*(?://[^\n]*)?\n"%n,"",out)
    out=re.sub(r"const (?:volatile )?(?:GXColor|u32) %s = [^;]*;\n"%n,"",out)
    out=re.sub(r"\*\((?:volatile )?(?:const )?GXColor\*\)&%s\b"%n,lit,out)
    if re.search(r"\b%s\b"%n,out):
        out=re.sub(r"\b%s\b"%n,lit,out); changed+=1

# zero-filled sbss2 GXColor scratch objects
for m in re.finditer(r"\.obj (unk_\w+), \w+\n\t\.skip 0x4\n(?=\.endobj)",asm):
    n=m.group(1)
    out=re.sub(r"(?:extern )?GXColor %s;[ \t]*(?://[^\n]*)?\n"%n,"",out)
    out=re.sub(r"extern (?:u32|s32) %s;[ \t]*(?://[^\n]*)?\n"%n,"",out)
    if re.search(r"\b%s\b"%n,out):
        out=re.sub(r"\b%s\b"%n,"(GXColor){ 0, 0, 0, 0 }",out); changed+=1
out=re.sub(r'\(\*\(volatile const f32\*\)&\(([^()]+)\)\)',r'\1',out)
out=re.sub(r'\*\(volatile (?:const )?f32\*\)&\(?(-?[\d.]+f)\)?',r'\1',out)
out=re.sub(r'MATCH_F32\(([^()]+)\)',r'\1',out)
out=re.sub(r'MATCH_F64\(([^()]+)\)',r'\1',out)
open(src,'w').write(out)
print(unit,"replaced",changed)
