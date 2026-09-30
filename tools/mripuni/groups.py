#!/usr/bin/env python3
"""Split mri_puni's open functions into contiguous asm-order groups of ~4 KB (at most 14 functions) for a wave,
one agent per group. Run from the repo root after a build; writes tools/mripuni/groups.json."""
import json, re

u = [x for x in json.load(open('build/G8MJ01/report.json'))['units'] if x['name'] == 'mri/mri_puni'][0]
src = open('rels/mri/src/mri_puni.c').read()
asm = re.findall(r'^\.fn (\w+),', open('build/G8MJ01/mri/asm/mri_puni.s').read(), re.M)
sz = {f['name']: int(f.get('size', 0)) for f in u['functions']}
todo = [n for n in asm if f'// TODO: write {n} (' in src]
target = sum(sz[n] for n in todo) / 24
groups, cur, b = [], [], 0
for n in todo:
    if cur and (b + sz[n] > target * 1.3 or len(cur) >= 14):
        groups.append(cur); cur, b = [], 0
    cur.append(n); b += sz[n]
groups.append(cur)
json.dump([dict(group=i + 1, funcs=[(n, sz[n]) for n in g], bytes=sum(sz[n] for n in g)) for i, g in enumerate(groups)],
          open('tools/mripuni/groups.json', 'w'), indent=1)
print(len(todo), 'open functions,', len(groups), 'groups')
