from pathlib import Path
import subprocess, sys, os
root = Path.cwd()
out = root / 'json-minify-results'
out.mkdir(exist_ok=True)
old = subprocess.check_output(['git', 'show', '0aaf8124:src/arch/x86/json/lexer.cpp']).decode()
start = old.index('#if WJR_HAS_BUILTIN(JSON_LEXER_READER_READ_BUF)\n')
end = old.index('#if WJR_HAS_BUILTIN(JSON_MINIFY_BUF)\n', start)
old = old[:start] + old[end:]
(out/'reference.cpp').write_text(old.replace('char *minify(', 'char *reference_minify('))
for compiler in ['g++-14', 'clang++-18']:
    for target, flags in [('sse42',['-msse4.2','-mpclmul']),('avx2',['-march=x86-64-v3','-mpclmul'])]:
        name = compiler + '-' + target
        common = [compiler,'-std=c++20','-O3','-DNDEBUG','-DWJR_DEBUG_LEVEL=0','-march=x86-64',*flags,'-Iinclude']
        sources = ['src/arch/x86/json/lexer.cpp',str(out/'reference.cpp'),'.github/research/json-minify.cpp']
        cmd = [*common,*sources,'examples/demo/src/simdjson.cpp','-Iexamples/demo/src','-DWITH_SIMDJSON','-o',str(out/name)]
        (out/(name+'-command.txt')).write_text(' '.join(cmd))
        subprocess.run(cmd,check=True)
        subprocess.run([str(out/name)],check=True)
        for repetition in range(5):
            args = [str(out/name),'bench'] + (['reverse'] if repetition%2 else [])
            with (out/(name+'-run'+str(repetition)+'.csv')).open('w') as file:
                subprocess.run(args,stdout=file,check=True)
        for source_file, label in [('src/arch/x86/json/lexer.cpp','candidate'),(str(out/'reference.cpp'),'reference')]:
            subprocess.run([*common,'-S','-masm=intel',source_file,'-o',str(out/(name+'-'+label+'.s'))],check=True)
        with (out/(name+'-symbols.txt')).open('w') as file:
            subprocess.run(['nm','-S','--size-sort',str(out/name)],stdout=file,check=True)
        subprocess.run([*common,'-O1','-fsanitize=address,undefined','-fno-omit-frame-pointer',*sources,'-o',str(out/(name+'-asan'))],check=True)
        subprocess.run([str(out/(name+'-asan'))],check=True)
