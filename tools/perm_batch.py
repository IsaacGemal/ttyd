#!/usr/bin/env python3
"""Parallel declaration-order search over every partial function.

    PB_WORKERS=4 python3 tools/perm_batch.py MINPCT MAXPCT [LIMIT]      (from the repo root, after a build)

For each function scoring MINPCT..MAXPCT it permutes the leading local declarations (single moves plus random shuffles),
compiles the variants from copies in a scratch dir (PB_DIR, default /tmp/perm_batch), scores just that function with objdiff
and appends the best order to PB_DIR/results.jsonl when it beats the baseline. Never edits src/: apply the results with
tools/perm_apply.py. Skips files listed in scratch/wave*/units.json of a sibling checkout when present."""
import concurrent.futures as cf, json, os, random, re, subprocess, sys, tempfile, itertools
PB=os.environ.get("PB_DIR","/tmp/perm_batch"); os.makedirs(PB,exist_ok=True)
lo, hi = float(sys.argv[1]), float(sys.argv[2])
limit = int(sys.argv[3]) if len(sys.argv) > 3 else 10**9
excl=set()
for w in ("wave26","wave28","wave29","wave30"):
    p=os.path.expanduser(f"~/ttyd/scratch/{w}/units.json")
    if os.path.exists(p):
        for u in json.load(open(p)): excl.add(u['unit'])
mine=set()
report=json.load(open("build/G8MJ01/report.json"))
project=json.load(open("objdiff.json"))
ninja=open("build.ninja").read()
done=set()
if os.path.exists(PB+"/results.jsonl"):
    for l in open(PB+"/results.jsonl"):
        r=json.loads(l); done.add((r["unit"],r["func"]))
cands=[]
for u in report["units"]:
    if not u["name"].startswith("MarioSt/"): continue
    n=u["name"][8:]
    if n in excl or n in mine or not os.path.exists(f"src/{n}.c"): continue
    for f in u.get("functions",[]):
        p=f.get("fuzzy_match_percent",0)
        if lo<=p<hi and (n,f["name"]) not in done: cands.append((p,n,f["name"]))
cands.sort(reverse=True)
if os.environ.get("PB_ONLY"):  # "unit:func,unit:func": search exactly these (baseline measured from the current source)
    cands=[(None,x.split(":")[0],x.split(":")[1]) for x in os.environ["PB_ONLY"].split(",")]
print(len(cands),"candidates",flush=True)
def flags_for(unit):
    m=re.search(rf"^build (build/G8MJ01/src/{re.escape(unit)}\.o): mwcc_sjis\w* \$\n.*?\n  cflags = (.*?)\n  basedir",ninja,re.M|re.S)
    if not m: return None
    f=re.sub(r"\$\n\s*","",m.group(2)); f=re.sub(r"\s+"," ",f).replace("-MMD","").strip()
    return f
DECL=re.compile(r"^\s+[\w\s\*]+(\[[^\]]*\])?(\s*=\s*[^;]+)?(,\s*\**\w+(\[[^\]]*\])?)*;\s*(//.*)?$")
def find_decls(text,func):
    m=re.search(rf"\b{re.escape(func)}\b[^;{{]*\)\s*{{\n",text)
    if not m: return None
    lines=text[m.end():].split("\n"); n=0
    while n<len(lines) and DECL.match(lines[n]) and not re.match(r"^\s*(return|goto|else|break|continue)\b",lines[n]): n+=1
    return m.end(),lines[:n],"\n".join(lines[n:])
def score_variant(unit,func,text,flags,tag):
    d=tempfile.mkdtemp(prefix="pb_",dir=PB)
    srcdir=os.path.dirname(f"src/{unit}.c")
    tmp=os.path.join(d,os.path.basename(unit)+".c")
    open(tmp,"w").write(text)
    cmd=f"build/tools/wibo build/tools/sjiswrap.exe build/compilers/GC/2.6/mwcceppc.exe {flags} -i {srcdir} -c {tmp} -o {d}"
    r=subprocess.run(cmd,shell=True,capture_output=True,text=True)
    obj=os.path.join(d,os.path.basename(unit)+".o")
    if r.returncode or not os.path.exists(obj): return -1
    e=[x for x in project["units"] if x["name"]=="MarioSt/"+unit][0]
    one=dict(project,units=[dict(e,base_path=os.path.abspath(obj),target_path=os.path.abspath(e["target_path"]))])
    json.dump(one,open(os.path.join(d,"objdiff.json"),"w"))
    rr=subprocess.run(["build/tools/objdiff-cli","report","generate","-p",d,"-o","-"],capture_output=True,text=True,env={**os.environ,"RAYON_NUM_THREADS":"1"})
    if rr.returncode: return -1
    for f in json.loads(rr.stdout)["units"][0].get("functions",[]):
        if f["name"]==func:
            subprocess.run(["rm","-rf",d]); return f.get("fuzzy_match_percent",0)
    subprocess.run(["rm","-rf",d]); return -1
pool=cf.ThreadPoolExecutor(max_workers=int(os.environ.get("PB_WORKERS","6")))
out=open(PB+"/results.jsonl","a")
count=0
for base,unit,func in cands:
    if count>=limit: break
    text=open(f"src/{unit}.c").read()
    fd=find_decls(text,func); flags=flags_for(unit)
    if not fd or not flags or len(fd[1])<2 or len(fd[1])>14: continue
    pos,decls,tail=fd; head=text[:pos]
    if base is None:
        base=score_variant(unit,func,text,flags,0)
    orders=[]
    n=len(decls)
    for i in range(n):
        for j in range(n):
            if i!=j:
                o=decls[:]; x=o.pop(i); o.insert(j,x); orders.append(tuple(o))
    rnd=random.Random(hash((unit,func))&0xffff)
    for _ in range(30):
        o=decls[:]; rnd.shuffle(o); orders.append(tuple(o))
    orders=list(dict.fromkeys(o for o in orders if list(o)!=decls))[:70]
    futs={pool.submit(score_variant,unit,func,head+"\n".join(o)+"\n"+tail,flags,i):o for i,o in enumerate(orders)}
    best=(base,None)
    for fu in cf.as_completed(futs):
        s=fu.result()
        if s>best[0]+1e-9: best=(s,futs[fu])
    count+=1
    rec={"unit":unit,"func":func,"base":base,"best":best[0],"order":list(best[1]) if best[1] else None}
    out.write(json.dumps(rec)+"\n"); out.flush()
    print(unit,func,round(base,2),"->",round(best[0],2),flush=True)
