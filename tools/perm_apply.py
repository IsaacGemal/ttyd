#!/usr/bin/env python3
"""Apply tools/perm_batch.py results (PB_DIR/results.jsonl): rewrite each improved function's declaration block, rebuild the unit,
keep it only if the function improved and no function in the unit dropped, then update the NONMATCHING comments."""
import json,os,re,subprocess,sys
PB=os.environ.get("PB_DIR","/tmp/perm_batch")
DECL=re.compile(r"^\s+[\w\s\*]+(\[[^\]]*\])?(\s*=\s*[^;]+)?(,\s*\**\w+(\[[^\]]*\])?)*;\s*(//.*)?$")
def scores(u):
    r=subprocess.run(["python3","tools/agent_check.py",u],capture_output=True,text=True).stdout
    d={}
    for l in r.splitlines():
        m=re.match(r"\s+([\d.]+)%\s+\d+ B\s+(\S+)",l)
        if m:d[m.group(2)]=float(m.group(1))
    return d,("BUILD FAILED" in r)
seen=set(json.loads(l)["unit"]+"|"+json.loads(l)["func"] for l in open(""+PB+"/applied.jsonl")) if os.path.exists(""+PB+"/applied.jsonl") else set()
recs=[json.loads(l) for l in open(""+PB+"/results.jsonl")]
recs=[r for r in recs if r["order"] and r["best"]>r["base"]+1e-9 and r["unit"]+"|"+r["func"] not in seen]
byunit={}
for r in recs: byunit.setdefault(r["unit"],[]).append(r)
log=open(""+PB+"/applied.jsonl","a")
for u,rs in sorted(byunit.items()):
    p=f"src/{u}.c"; orig=open(p).read()
    before,f=scores(u)
    cur=orig
    for r in rs:
        m=re.search(rf"\b{re.escape(r['func'])}\b[^;{{]*\)\s*{{\n",cur)
        if not m: continue
        lines=cur[m.end():].split("\n"); n=0
        while n<len(lines) and DECL.match(lines[n]): n+=1
        if n!=len(r["order"]): continue
        cur=cur[:m.end()]+"\n".join(r["order"])+"\n"+"\n".join(lines[n:])
    open(p,"w").write(cur)
    after,fail=scores(u)
    ok=(not fail) and all(after.get(k,0)>=v-1e-9 for k,v in before.items())
    gain={k:(before[k],after[k]) for k in before if after.get(k,0)>before[k]+1e-9}
    if ok and gain:
        print("OK ",u,{k:(round(a,2),round(b,2)) for k,(a,b) in gain.items()},flush=True)
        for r in rs: log.write(json.dumps(r)+"\n")
    else:
        open(p,"w").write(orig); print("REV",u,"fail" if fail else "no gain/drop",flush=True)

# NONMATCHING comments of the functions just improved
seen=set()
if os.path.exists(""+PB+"/comments_done.txt"): seen=set(open(PB+"/comments_done.txt").read().split("\n"))
done=open(""+PB+"/comments_done.txt","a")
for l in open(""+PB+"/applied.jsonl"):
    r=json.loads(l); key=r["unit"]+"|"+r["func"]
    if key in seen: continue
    p=f"src/{r['unit']}.c"; s=open(p).read()
    m=re.search(rf"\b{re.escape(r['func'])}\b[^;{{]*\)\s*{{\n",s)
    if not m: continue
    start=s.rfind("\n",0,m.start())+1
    # walk back over contiguous comment lines
    lines=s[:start].split("\n")[:-1]
    i=len(lines)
    while i>0 and lines[i-1].lstrip().startswith("//"): i-=1
    block=lines[i:]
    idx=next((k for k,b in enumerate(block) if "NONMATCHING" in b),None)
    if idx is None:
        done.write(key+"\n"); continue
    if r["best"]>=99.995:
        # drop the NONMATCHING comment (from that line through the comment lines that belong to it)
        new=block[:idx]
        newtext="\n".join(lines[:i]+new)+"\n"+s[start:] if (lines[:i]+new) else s[start:]
    else:
        block[idx]=re.sub(r"NONMATCHING \(\d+(\.\d+)?%\)",f"NONMATCHING ({r['best']:.2f}%)",block[idx])
        newtext="\n".join(lines[:i]+block)+"\n"+s[start:]
    open(p,"w").write(newtext); done.write(key+"\n"); print("fixed",key)
