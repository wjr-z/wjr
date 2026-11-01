import json,re,sys
from pathlib import Path
p=Path(sys.argv[1]); rows={}
for engine in ('prior','candidate'):
    text=(p/f'gcc-{engine}-objdump.txt').read_text()
    parts=re.split(r'(?=^[0-9a-f]+ <)',text,flags=re.M)
    body=next(s for s in parts if '_reuse_copy_inner(' in s.splitlines()[0])
    rows[engine]={'instructions':len(re.findall(r'^\s+[0-9a-f]+:\s+\w',body,re.M)),'canary':'__stack_chk_fail' in body,'function':body}
go=not rows['candidate']['canary'] and rows['candidate']['instructions']<rows['prior']['instructions']
(p/'gcc-gate.txt').write_text('GO' if go else 'NO-GO')
(p/'gcc-gate.json').write_text(json.dumps(rows,indent=2)+'\n')
print({k:{a:b for a,b in v.items() if a!='function'} for k,v in rows.items()},'GO' if go else 'NO-GO')
