from pathlib import Path
import subprocess,json,time,csv,statistics,hashlib
import os
r=Path('insertion-results')
r.mkdir(exist_ok=True)
os.sched_setaffinity(0,{min(os.sched_getaffinity(0))})
start=time.time()
rows=[]
for round in range(3):
    for mode in (['main','candidate'] if round%2==0 else ['candidate','main']):
        for operation in ['insert']:
            output=subprocess.check_output([str(Path('build-'+('baseline' if mode=='main' else mode))/'modifiers'),operation],text=True)
            (r/f'linux-{round}-{mode}-{operation}.csv').write_text(output)
            for row in csv.DictReader(output.splitlines()):
                rows.append(dict(row,round=round,source=mode,operation=operation))
summary=[]
for operation in ['insert']:
    for size in [128,1024,4096]:
        for count in [1,16,size//2]:
            group=[x for x in rows if x['operation']==operation and int(x['size'])==size and int(x['count'])==count]
            med={mode:statistics.median(float(x['ns_per_call']) for x in group if x['source']==mode) for mode in ['main','candidate']}
            summary.append(dict(operation=operation,size=size,count=count,**med,ratio=med['candidate']/med['main'],speedup=med['main']/med['candidate']))
manifest=dict(start_epoch=start,end_epoch=time.time(),cpu_affinity=sorted(os.sched_getaffinity(0)),rounds=3,repetitions=11,batch=32,setup_and_destroy='excluded',validation='every output compared to expected after timing',summary=summary)
(r/'summary.json').write_text(json.dumps(manifest,indent=2)+'\n')
for row in summary:print(row)
