import re,sys
# usage: ins.py FILE FUNC BODYFILE  -- replace rel_gen's TODO hint for FUNC with BODYFILE's text, fix forward decl
p,fn,bf=sys.argv[1:4]
s=open(p).read(); body=open(bf).read()
s,n=re.subn(r'// TODO: write '+re.escape(fn)+r' [^\n]*\n(?://[^\n]*\n)*',lambda m: body.rstrip('\n')+'\n\n',s)
assert n==1,'hint not found'
s=re.sub(r'static void '+re.escape(fn)+r'\(void\);[^\n]*','static USER_FUNC('+fn+');',s)
s=s.replace('\n#pragma force_active off\n','\n')
inc=[l for l in re.findall(r'^#include [^\n]*',body,re.M)]
body_no=''
open(p,'w').write(s)
