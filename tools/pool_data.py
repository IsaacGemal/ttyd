#!/usr/bin/env python3
"""Write the .data/.sbss objects a dol unit owns (vertex tables from tools/dol_gen.py) in place of its extern declarations.

    python3 tools/pool_data.py UNIT

Run after tools/pool_literals.py. u8-declared tables come out as byte arrays; effVtxArray uses get a cast."""
import re,sys,subprocess
unit=sys.argv[1]; src=f"src/{unit}.c"
s=open(src).read()
gen=subprocess.run(["python3","tools/dol_gen.py",unit],capture_output=True,text=True).stdout
# split into definition blocks
blocks=re.findall(r"// TODO: type\n((?:__declspec\([^)]*\) )?(?:static )?u32 (\w+)\[\d+\] = \{.*?\n\};\n)",gen,re.S)
asm=open(f"build/G8MJ01/asm/{unit}.s").read()
sb=set(re.findall(r"\.obj (\w+), \w+\n\t\.skip",asm))
added=[]
for text,name in blocks:
    ext=re.search(r"extern [^;\n]*\b%s\b[^;\n]*;\n"%name,s)
    if ext:
        if 'effVtxArray' in ext.group(0): s=re.sub(r'&%s\b'%name,'(effVtxArray*)%s'%name,s)
        s=s.replace(ext.group(0),"",1); added.append((name,text))
# blocks referenced from other units
import glob
for text,name in blocks:
    if any(n==name for n,t in added): continue
    if re.search(r"\b%s\b"%name,s): continue
    ref=False
    for f in glob.glob('src/**/*.c',recursive=True)+glob.glob('include/**/*.h',recursive=True):
        if f==src: continue
        if re.search(r"\b%s\b"%name,open(f,errors='replace').read()): ref=True; break
    if ref and not text.startswith('static'): added.append((name,text))
# sbss/sbss2 uninit objects
for name in sb:
    m=re.search(r"extern ((?:const )?[A-Za-z_0-9]+ %s(?:\[[^\]]*\])?);\n"%name,s)
    if m: s=s.replace(m.group(0),m.group(1)+";\n",1)
# convert u32 blocks whose name is declared u8 in some header to byte arrays
import glob
def as_u8(text,name):
    m=re.search(r"u32 %s\[(\d+)\] = \{(.*?)\};"%name,text,re.S)
    words=[int(x,16) for x in re.findall(r"0x([0-9A-Fa-f]{8})",m.group(2))]
    bs=[]
    for w in words: bs+= [(w>>24)&255,(w>>16)&255,(w>>8)&255,w&255]
    rows=[]
    for i in range(0,len(bs),16): rows.append("    "+", ".join("0x%02X"%b for b in bs[i:i+16])+",")
    return text[:m.start()].replace("u32 ","u8 ")+"u8 %s[%d] = {\n"%(name,len(bs))+"\n".join(rows)+"\n};\n"
hdrs=" ".join(open(f,errors='replace').read() for f in glob.glob('include/**/*.h',recursive=True))
new=[]
for n,t in added:
    if re.search(r"extern u8 %s\[\]"%n,hdrs): t=as_u8(t,n)
    new.append((n,t))
added=new
if added:
    inc=list(re.finditer(r"^#include [^\n]*\n",s,re.M))
    pos=inc[-1].end()
    txt="\n"+"\n".join(t for n,t in added)
    s=s[:pos]+txt+s[pos:]
open(src,'w').write(s)
print(unit,"data added",[n for n,t in added])
