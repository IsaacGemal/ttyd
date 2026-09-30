#!/usr/bin/env python3
"""Compare a unit's pool entries (target asm vs our object) in order, floats normalised to hex.

    python3 tools/pool_compare.py UNIT [.sdata2|.rodata|...]

Lines marked !! differ; a swapped adjacent pair means the constants are first used in the other order."""
import re,sys,subprocess
unit=sys.argv[1]; sec=sys.argv[2] if len(sys.argv)>2 else ".sdata2"
subprocess.run(["build/tools/dtk","elf","disasm",f"build/G8MJ01/src/{unit}.o","/tmp/t/ours.s"],capture_output=True)
def vals(text):
    a=text.index(f".section {sec}") if f".section {sec}" in text else -1
    if a<0: return []
    b=text.find(".section",a+10); seg=text[a:b if b>0 else None]
    out=[]
    for m in re.finditer(r"\.obj \S+, \w+\n((?:\t[^\n]+\n)+)",seg):
        body=m.group(1)
        parts=[]
        for x in body.strip().split("\n"):
            x=x.strip()
            mm=re.match(r"\.float (\S+)",x)
            if mm:
                import struct
                try: x=".4byte 0x%08X"%struct.unpack('>I',struct.pack('>f',float(mm.group(1))))[0]
                except: pass
            parts.append(x)
        out.append(" ".join(parts))
    return out
t=vals(open(f"build/G8MJ01/asm/{unit}.s").read()); o=vals(open("/tmp/t/ours.s").read())
for i in range(max(len(t),len(o))):
    a=t[i] if i<len(t) else "-"; b=o[i] if i<len(o) else "-"
    print(("  " if a==b else "!!"),i,a,"|",b)
