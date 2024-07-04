global  __wjr_asm_basecase_sqr_impl

section .text
__wjr_asm_basecase_sqr_impl:
        push rbp
        mov  rax, rcx
        mov  rcx, rdx
        push rdi
        push rsi
        push rbx
        cmp  r8,  2
        ja   .L2
        mov  rdx, QWORD [rdx]
        je   .L3
        mulx rcx, rdx, rdx

        mov QWORD [rax],   rdx
        mov QWORD [rax+8], rcx
.L5:
        pop rbx
        pop rsi
        pop rdi
        pop rbp
        ret
.L2:
        mov    r9,  rdx
        mov    rdx, r8
        lea    rdi, [rdx - 1]
        mov    r8d, edi
        mov    rsi, rdi
        shr    rsi, 3
        and    r8d, 7
        mov    ebp, r8d
        mov    rcx, rsi
        mov    rdx, [r9]
        lea    r10, [rel .Llookup80]
        movsxd r11, DWORD [r10 + r8 * 4]
        lea    r10, [r10 + r11]
        jmp    r10

        align   8
        section .data
.Llookup80:
        dd .Ll080-.Llookup80
        dd .Ll180-.Llookup80
        dd .Ll280-.Llookup80
        dd .Ll380-.Llookup80
        dd .Ll480-.Llookup80
        dd .Ll580-.Llookup80
        dd .Ll680-.Llookup80
        dd .Ll780-.Llookup80

        align   16
        section .text
.Ll080:
        mulx rbx, r11, [r9 + 8]
        lea r9,  [r9 + 8]
        lea rax, [rax + 8]
        jmp .Lb080
.Ll280:
        mulx rbx, r11, [r9 + 8]
        lea r9,  [r9 - 40]
        lea rax, [rax - 40]
        inc rcx
        jmp .Lb280
.Ll380:
        mulx r10, r8, [r9 + 8]
        lea r9,  [r9 - 32]
        lea rax, [rax - 32]
        inc rcx
        jmp .Lb380
.Ll480:
        mulx rbx, r11, [r9 + 8]
        lea r9,  [r9 - 24]
        lea rax, [rax - 24]
        inc rcx
        jmp .Lb480
.Ll580:
        mulx r10, r8, [r9 + 8]
        lea r9,  [r9 - 16]
        lea rax, [rax - 16]
        inc rcx
        jmp .Lb580
.Ll680:
        mulx rbx, r11, [r9 + 8]
        lea r9,  [r9 - 8]
        lea rax, [rax - 8]
        inc rcx
        jmp .Lb680
.Ll780:
        mulx r10, r8, [r9 + 8]
        inc rcx
        jmp .Lb780
.Ll180:
        mulx r10, r8, [r9 + 8]
        lea r9,  [r9 + 16]
        lea rax, [rax + 16]
.Lloop80:
        mulx rbx, r11, [r9]
        mov [rax - 8], r8
        adc r11,       r10
.Lb080:
        mulx r10, r8, [r9 + 8]
        mov [rax], r11
        adc r8,    rbx
.Lb780:
        mulx rbx, r11, [r9 + 16]
        mov [rax + 8], r8
        adc r11,       r10
.Lb680:
        mulx r10, r8, [r9 + 24]
        mov [rax + 16], r11
        adc r8,         rbx
.Lb580:
        mulx rbx, r11, [r9 + 32]
        mov [rax + 24], r8
        adc r11,        r10
.Lb480:
        mulx r10, r8, [r9 + 40]
        mov [rax + 32], r11
        adc r8,         rbx
.Lb380:
        mulx rbx, r11, [r9 + 48]
        mov [rax + 40], r8
        adc r11,        r10
.Lb280:
        mulx r10, r8, [r9 + 56]
        mov    [rax + 48], r11
        adc    r8,         rbx
        lea    r9,         [r9 + 64]
        lea    rax,        [rax + 64]
        dec    rcx
        jne    .Lloop80
        adc    r10,        rcx
        mov    [rax - 8],  r8
        mov    [rax],      r10
        lea    r10,        [rel .Lslookup80]
        movsxd r11,        DWORD [r10 + rbp * 4]
        lea    r10,        [r10 + r11]
        jmp    r10

        align   8
        section .data
.Lslookup80:
        dd .Ljs080-.Lslookup80
        dd .Ljs180-.Lslookup80
        dd .Ljs280-.Lslookup80
        dd .Ljs380-.Lslookup80
        dd .Ljs480-.Lslookup80
        dd .Ljs580-.Lslookup80
        dd .Ljs680-.Lslookup80
        dd .Ljs780-.Lslookup80

        align   16
        section .text
.Ljs080:
        lea rbp, [rdi - 2]
        not rbp
        jmp .Ls780
.Ljs180:
        lea rbp, [rdi - 3]
        not rbp
        jmp .Ls080
.Ljs280:
        lea rbp, [rdi + 4]
        not rbp
        jmp .Ls180
.Ljs380:
        lea rbp, [rdi + 3]
        not rbp
        jmp .Ls280
.Ljs480:
        lea rbp, [rdi + 2]
        not rbp
        jmp .Ls380
.Ljs580:
        lea rbp, [rdi + 1]
        not rbp
        jmp .Ls480
.Ljs680:
        lea rbp, [rdi]
        not rbp
        jmp .Ls580
.Ljs780:
        lea rbp, [rdi - 1]
        not rbp
        jmp .Ls680
.Lsloop80:
        lea rbp, [rbp + 8]
.Ls080:
        lea r9,  [r9 + rbp * 8 - 8]
        lea rax, [rax + rbp * 8]
        mov rdx, [r9 - 8]
        mov rcx, rsi
        mulx rbx, r11, [r9]
        jmp .Ls0b080
.Ls0loop80:
        mulx rbx, r11, [r9]
        adcx r8,        [rax - 8]
        adox r11,       r10
        mov  [rax - 8], r8
.Ls0b080:
        mulx r10, r8, [r9 + 8]
        lea   rcx,        [rcx - 1]
        adox  r8,         rbx
        adcx  r11,        [rax]
        mov   [rax],      r11
        mulx rbx, r11, [r9 + 16]
        adcx  r8,         [rax + 8]
        adox  r11,        r10
        mov   [rax + 8],  r8
        mulx r10, r8, [r9 + 24]
        lea   r9,         [r9 + 64]
        adox  r8,         rbx
        adcx  r11,        [rax + 16]
        mov   [rax + 16], r11
        mulx rbx, r11, [r9 - 32]
        adcx  r8,         [rax + 24]
        adox  r11,        r10
        mov   [rax + 24], r8
        mulx r10, r8, [r9 - 24]
        adox  r8,         rbx
        adcx  r11,        [rax + 32]
        mov   [rax + 32], r11
        mulx rbx, r11, [r9 - 16]
        adcx  r8,         [rax + 40]
        adox  r11,        r10
        mov   [rax + 40], r8
        mulx r10, r8, [r9 - 8]
        adox  r8,         rbx
        adcx  r11,        [rax + 48]
        mov   [rax + 48], r11
        lea   rax,        [rax + 64]
        jrcxz .Ls0loop_out80
        jmp   .Ls0loop80
.Ls0loop_out80:
        adcx r8,        [rax - 8]
        mov  [rax - 8], r8
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax],     r10
.Ls780:
        lea rcx, [rsi - 1]
        lea rsi, [rsi - 1]
        lea r9,  [r9 + rbp * 8 - 8]
        lea rax, [rax + rbp * 8]
        mov rdx, [r9]
        mulx r10, r8, [r9 + 8]
        jmp .Ls7b780
.Ls7loop80:
        mulx rbx, r11, [r9]
        adcx r8,        [rax - 8]
        adox r11,       r10
        mov  [rax - 8], r8
        mulx r10, r8, [r9 + 8]
        lea  rcx,       [rcx - 1]
        adox r8,        rbx
        adcx r11,       [rax]
        mov  [rax],     r11
.Ls7b780:
        mulx rbx, r11, [r9 + 16]
        adcx  r8,         [rax + 8]
        adox  r11,        r10
        mov   [rax + 8],  r8
        mulx r10, r8, [r9 + 24]
        lea   r9,         [r9 + 64]
        adox  r8,         rbx
        adcx  r11,        [rax + 16]
        mov   [rax + 16], r11
        mulx rbx, r11, [r9 - 32]
        adcx  r8,         [rax + 24]
        adox  r11,        r10
        mov   [rax + 24], r8
        mulx r10, r8, [r9 - 24]
        adox  r8,         rbx
        adcx  r11,        [rax + 32]
        mov   [rax + 32], r11
        mulx rbx, r11, [r9 - 16]
        adcx  r8,         [rax + 40]
        adox  r11,        r10
        mov   [rax + 40], r8
        mulx r10, r8, [r9 - 8]
        adox  r8,         rbx
        adcx  r11,        [rax + 48]
        mov   [rax + 48], r11
        lea   rax,        [rax + 64]
        jrcxz .Ls7loop_out80
        jmp   .Ls7loop80
.Ls7loop_out80:
        adcx r8,        [rax - 8]
        mov  [rax - 8], r8
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax],     r10
.Ls680:
        lea r9,  [r9 + rbp * 8 - 8]
        lea rax, [rax + rbp * 8]
        mov rdx, [r9 + 8]
        mov rcx, rsi
        mulx rbx, r11, [r9 + 16]
        jmp .Ls6b680
.Ls6loop80:
        mulx rbx, r11, [r9]
        adcx r8,        [rax - 8]
        adox r11,       r10
        mov  [rax - 8], r8
        mulx r10, r8, [r9 + 8]
        lea  rcx,       [rcx - 1]
        adox r8,        rbx
        adcx r11,       [rax]
        mov  [rax],     r11
        mulx rbx, r11, [r9 + 16]
        adcx r8,        [rax + 8]
        adox r11,       r10
        mov  [rax + 8], r8
.Ls6b680:
        mulx r10, r8, [r9 + 24]
        lea   r9,         [r9 + 64]
        adox  r8,         rbx
        adcx  r11,        [rax + 16]
        mov   [rax + 16], r11
        mulx rbx, r11, [r9 - 32]
        adcx  r8,         [rax + 24]
        adox  r11,        r10
        mov   [rax + 24], r8
        mulx r10, r8, [r9 - 24]
        adox  r8,         rbx
        adcx  r11,        [rax + 32]
        mov   [rax + 32], r11
        mulx rbx, r11, [r9 - 16]
        adcx  r8,         [rax + 40]
        adox  r11,        r10
        mov   [rax + 40], r8
        mulx r10, r8, [r9 - 8]
        adox  r8,         rbx
        adcx  r11,        [rax + 48]
        mov   [rax + 48], r11
        lea   rax,        [rax + 64]
        jrcxz .Ls6loop_out80
        jmp   .Ls6loop80
.Ls6loop_out80:
        adcx r8,        [rax - 8]
        mov  [rax - 8], r8
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax],     r10
.Ls580:
        lea r9,  [r9 + rbp * 8 + 56]
        lea rax, [rax + rbp * 8]
        mov rdx, [r9 - 48]
        mov rcx, rsi
        mulx r10, r8, [r9 - 40]
        jmp .Ls5b580
.Ls5loop80:
        mulx rbx, r11, [r9]
        adcx r8,         [rax - 8]
        adox r11,        r10
        mov  [rax - 8],  r8
        mulx r10, r8, [r9 + 8]
        lea  rcx,        [rcx - 1]
        adox r8,         rbx
        adcx r11,        [rax]
        mov  [rax],      r11
        mulx rbx, r11, [r9 + 16]
        adcx r8,         [rax + 8]
        adox r11,        r10
        mov  [rax + 8],  r8
        mulx r10, r8, [r9 + 24]
        lea  r9,         [r9 + 64]
        adox r8,         rbx
        adcx r11,        [rax + 16]
        mov  [rax + 16], r11
.Ls5b580:
        mulx rbx, r11, [r9 - 32]
        adcx  r8,         [rax + 24]
        adox  r11,        r10
        mov   [rax + 24], r8
        mulx r10, r8, [r9 - 24]
        adox  r8,         rbx
        adcx  r11,        [rax + 32]
        mov   [rax + 32], r11
        mulx rbx, r11, [r9 - 16]
        adcx  r8,         [rax + 40]
        adox  r11,        r10
        mov   [rax + 40], r8
        mulx r10, r8, [r9 - 8]
        adox  r8,         rbx
        adcx  r11,        [rax + 48]
        mov   [rax + 48], r11
        lea   rax,        [rax + 64]
        jrcxz .Ls5loop_out80
        jmp   .Ls5loop80
.Ls5loop_out80:
        adcx r8,        [rax - 8]
        mov  [rax - 8], r8
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax],     r10
.Ls480:
        lea r9,  [r9 + rbp * 8 + 56]
        lea rax, [rax + rbp * 8]
        mov rdx, [r9 - 40]
        mov rcx, rsi
        mulx rbx, r11, [r9 - 32]
        jmp .Ls4b480
.Ls4loop80:
        mulx rbx, r11, [r9]
        adcx r8,         [rax - 8]
        adox r11,        r10
        mov  [rax - 8],  r8
        mulx r10, r8, [r9 + 8]
        lea  rcx,        [rcx - 1]
        adox r8,         rbx
        adcx r11,        [rax]
        mov  [rax],      r11
        mulx rbx, r11, [r9 + 16]
        adcx r8,         [rax + 8]
        adox r11,        r10
        mov  [rax + 8],  r8
        mulx r10, r8, [r9 + 24]
        lea  r9,         [r9 + 64]
        adox r8,         rbx
        adcx r11,        [rax + 16]
        mov  [rax + 16], r11
        mulx rbx, r11, [r9 - 32]
        adcx r8,         [rax + 24]
        adox r11,        r10
        mov  [rax + 24], r8
.Ls4b480:
        mulx r10, r8, [r9 - 24]
        adox  r8,         rbx
        adcx  r11,        [rax + 32]
        mov   [rax + 32], r11
        mulx rbx, r11, [r9 - 16]
        adcx  r8,         [rax + 40]
        adox  r11,        r10
        mov   [rax + 40], r8
        mulx r10, r8, [r9 - 8]
        adox  r8,         rbx
        adcx  r11,        [rax + 48]
        mov   [rax + 48], r11
        lea   rax,        [rax + 64]
        jrcxz .Ls4loop_out80
        jmp   .Ls4loop80
.Ls4loop_out80:
        adcx r8,        [rax - 8]
        mov  [rax - 8], r8
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax],     r10
.Ls380:
        lea r9,  [r9 + rbp * 8 + 56]
        lea rax, [rax + rbp * 8]
        mov rdx, [r9 - 32]
        mov rcx, rsi
        mulx r10, r8, [r9 - 24]
        jmp .Ls3b380
.Ls3loop80:
        mulx rbx, r11, [r9]
        adcx r8,         [rax - 8]
        adox r11,        r10
        mov  [rax - 8],  r8
        mulx r10, r8, [r9 + 8]
        lea  rcx,        [rcx - 1]
        adox r8,         rbx
        adcx r11,        [rax]
        mov  [rax],      r11
        mulx rbx, r11, [r9 + 16]
        adcx r8,         [rax + 8]
        adox r11,        r10
        mov  [rax + 8],  r8
        mulx r10, r8, [r9 + 24]
        lea  r9,         [r9 + 64]
        adox r8,         rbx
        adcx r11,        [rax + 16]
        mov  [rax + 16], r11
        mulx rbx, r11, [r9 - 32]
        adcx r8,         [rax + 24]
        adox r11,        r10
        mov  [rax + 24], r8
        mulx r10, r8, [r9 - 24]
        adox r8,         rbx
        adcx r11,        [rax + 32]
        mov  [rax + 32], r11
.Ls3b380:
        mulx rbx, r11, [r9 - 16]
        adcx  r8,         [rax + 40]
        adox  r11,        r10
        mov   [rax + 40], r8
        mulx r10, r8, [r9 - 8]
        adox  r8,         rbx
        adcx  r11,        [rax + 48]
        mov   [rax + 48], r11
        lea   rax,        [rax + 64]
        jrcxz .Ls3loop_out80
        jmp   .Ls3loop80
.Ls3loop_out80:
        adcx r8,        [rax - 8]
        mov  [rax - 8], r8
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax],     r10
.Ls280:
        lea r9,  [r9 + rbp * 8 + 56]
        lea rax, [rax + rbp * 8]
        mov rdx, [r9 - 24]
        mov rcx, rsi
        mulx rbx, r11, [r9 - 16]
        jmp .Ls2b280
.Ls2loop80:
        mulx rbx, r11, [r9]
        adcx r8,         [rax - 8]
        adox r11,        r10
        mov  [rax - 8],  r8
        mulx r10, r8, [r9 + 8]
        lea  rcx,        [rcx - 1]
        adox r8,         rbx
        adcx r11,        [rax]
        mov  [rax],      r11
        mulx rbx, r11, [r9 + 16]
        adcx r8,         [rax + 8]
        adox r11,        r10
        mov  [rax + 8],  r8
        mulx r10, r8, [r9 + 24]
        lea  r9,         [r9 + 64]
        adox r8,         rbx
        adcx r11,        [rax + 16]
        mov  [rax + 16], r11
        mulx rbx, r11, [r9 - 32]
        adcx r8,         [rax + 24]
        adox r11,        r10
        mov  [rax + 24], r8
        mulx r10, r8, [r9 - 24]
        adox r8,         rbx
        adcx r11,        [rax + 32]
        mov  [rax + 32], r11
        mulx rbx, r11, [r9 - 16]
        adcx r8,         [rax + 40]
        adox r11,        r10
        mov  [rax + 40], r8
.Ls2b280:
        mulx r10, r8, [r9 - 8]
        adox  r8,         rbx
        adcx  r11,        [rax + 48]
        mov   [rax + 48], r11
        lea   rax,        [rax + 64]
        jrcxz .Ls2loop_out80
        jmp   .Ls2loop80
.Ls2loop_out80:
        adcx r8,        [rax - 8]
        mov  [rax - 8], r8
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax],     r10
.Ls180:
        lea r9,  [r9 + rbp * 8 + 56]
        lea rax, [rax + rbp * 8 + 64]
        mov rdx, [r9 - 16]
        mov rcx, rsi
        jmp .Ls1l180
.Ls1d180:
        add r8,        [rax - 8]
        mov [rax - 8], r8
        adc r10,       rcx
        mov [rax],     r10
        jmp .Ls1done80
.Ls1l180:
        mulx r10, r8, [r9 - 8]
        jrcxz .Ls1d180
.Ls1loop80:
        mulx rbx, r11, [r9]
        adcx  r8,         [rax - 8]
        adox  r11,        r10
        mov   [rax - 8],  r8
        mulx r10, r8, [r9 + 8]
        lea   rcx,        [rcx - 1]
        adox  r8,         rbx
        adcx  r11,        [rax]
        mov   [rax],      r11
        mulx rbx, r11, [r9 + 16]
        adcx  r8,         [rax + 8]
        adox  r11,        r10
        mov   [rax + 8],  r8
        mulx r10, r8, [r9 + 24]
        lea   r9,         [r9 + 64]
        adox  r8,         rbx
        adcx  r11,        [rax + 16]
        mov   [rax + 16], r11
        mulx rbx, r11, [r9 - 32]
        adcx  r8,         [rax + 24]
        adox  r11,        r10
        mov   [rax + 24], r8
        mulx r10, r8, [r9 - 24]
        adox  r8,         rbx
        adcx  r11,        [rax + 32]
        mov   [rax + 32], r11
        mulx rbx, r11, [r9 - 16]
        adcx  r8,         [rax + 40]
        adox  r11,        r10
        mov   [rax + 40], r8
        mulx r10, r8, [r9 - 8]
        adox  r8,         rbx
        adcx  r11,        [rax + 48]
        mov   [rax + 48], r11
        lea   rax,        [rax + 64]
        jrcxz .Ls1loop_out80
        jmp   .Ls1loop80
.Ls1loop_out80:
        adcx r8,        [rax - 8]
        mov  [rax - 8], r8
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax],     r10
.Ls1done80:
        test   rsi,  rsi
        jne    .Lsloop80
        mov    rcx,  rdi
        neg    rdi
        mov    r11d, ecx
        lea    r9,   [r9 + rdi * 8 - 8]
        shl    rdi,  4
        add    rax,  rdi
        shr    rcx,  3
        and    r11d, 7
        mov    rdx,  [r9]
        lea    r10,  [rel .Llslookup80]
        movsxd r11,  DWORD [r10 + r11 * 4]
        lea    r10,  [r10 + r11]
        jmp    r10

        align   8
        section .data
.Llslookup80:
        dd .Llsl080-.Llslookup80
        dd .Llsl180-.Llslookup80
        dd .Llsl280-.Llslookup80
        dd .Llsl380-.Llslookup80
        dd .Llsl480-.Llslookup80
        dd .Llsl580-.Llslookup80
        dd .Llsl680-.Llslookup80
        dd .Llsl780-.Llslookup80

        align   16
        section .text
.Llsl080:
        mulx r10, r11, rdx
        mov  [rax], r11
        mov  r8,    [rax + 8]
        adox r8,    r8
        jmp  .Llsb080
.Llsl280:
        mulx r10, r11, rdx
        mov  [rax], r11
        mov  r8,    [rax + 8]
        adox r8,    r8
        lea  r9,    [r9 - 48]
        lea  rax,   [rax - 96]
        jmp  .Llsb280
.Llsl380:
        mulx rbx, r11, rdx
        mov  [rax], r11
        mov  rdi,   [rax + 8]
        adox rdi,   rdi
        lea  r9,    [r9 - 40]
        lea  rax,   [rax - 80]
        jmp  .Llsb380
.Llsl480:
        mulx r10, r11, rdx
        mov  [rax], r11
        mov  r8,    [rax + 8]
        adox r8,    r8
        lea  r9,    [r9 - 32]
        lea  rax,   [rax - 64]
        jmp  .Llsb480
.Llsl580:
        mulx rbx, r11, rdx
        mov  [rax], r11
        mov  rdi,   [rax + 8]
        adox rdi,   rdi
        lea  r9,    [r9 - 24]
        lea  rax,   [rax - 48]
        jmp  .Llsb580
.Llsl680:
        mulx r10, r11, rdx
        mov  [rax], r11
        mov  r8,    [rax + 8]
        adox r8,    r8
        lea  r9,    [r9 - 16]
        lea  rax,   [rax - 32]
        jmp  .Llsb680
.Llsl780:
        mulx rbx, r11, rdx
        mov  [rax], r11
        mov  rdi,   [rax + 8]
        adox rdi,   rdi
        lea  r9,    [r9 - 8]
        lea  rax,   [rax - 16]
        jmp  .Llsb780
.Llsl180:
        mulx rbx, r11, rdx
        mov  [rax], r11
        mov  rdi,   [rax + 8]
        adox rdi,   rdi
        lea  r9,    [r9 + 8]
        lea  rax,   [rax + 16]
.Llsloop80:
        mov  rdx,       [r9]
        mulx r10, r11, rdx
        mov  rsi,       [rax]
        mov  r8,        [rax + 8]
        adox rsi,       rsi
        adcx rbx,       rdi
        adox r8,        r8
        adcx r11,       rsi
        mov  [rax - 8], rbx
        mov  [rax],     r11
.Llsb080:
        mov  rdx,        [r9 + 8]
        mulx rbx, r11, rdx
        mov  rsi,        [rax + 16]
        mov  rdi,        [rax + 24]
        adox rsi,        rsi
        adcx r10,        r8
        lea  rcx,        [rcx - 1]
        adox rdi,        rdi
        adcx r11,        rsi
        mov  [rax + 8],  r10
        mov  [rax + 16], r11
.Llsb780:
        mov  rdx,        [r9 + 16]
        mulx r10, r11, rdx
        mov  rsi,        [rax + 32]
        mov  r8,         [rax + 40]
        adox rsi,        rsi
        adcx rbx,        rdi
        adox r8,         r8
        adcx r11,        rsi
        mov  [rax + 24], rbx
        mov  [rax + 32], r11
.Llsb680:
        mov  rdx,        [r9 + 24]
        mulx rbx, r11, rdx
        mov  rsi,        [rax + 48]
        mov  rdi,        [rax + 56]
        adox rsi,        rsi
        adcx r10,        r8
        adox rdi,        rdi
        adcx r11,        rsi
        mov  [rax + 40], r10
        mov  [rax + 48], r11
.Llsb580:
        mov  rdx,        [r9 + 32]
        mulx r10, r11, rdx
        mov  rsi,        [rax + 64]
        mov  r8,         [rax + 72]
        adox rsi,        rsi
        adcx rbx,        rdi
        adox r8,         r8
        adcx r11,        rsi
        mov  [rax + 56], rbx
        mov  [rax + 64], r11
.Llsb480:
        mov  rdx,        [r9 + 40]
        mulx rbx, r11, rdx
        mov  rsi,        [rax + 80]
        mov  rdi,        [rax + 88]
        adox rsi,        rsi
        adcx r10,        r8
        adox rdi,        rdi
        adcx r11,        rsi
        mov  [rax + 72], r10
        mov  [rax + 80], r11
.Llsb380:
        mov  rdx,        [r9 + 48]
        mulx r10, r11, rdx
        mov  rsi,        [rax + 96]
        mov  r8,         [rax + 104]
        adox rsi,        rsi
        adcx rbx,        rdi
        adox r8,         r8
        adcx r11,        rsi
        mov  [rax + 88], rbx
        mov  [rax + 96], r11
.Llsb280:
        mov   rdx,         [r9 + 56]
        mulx rbx, r11, rdx
        mov   rsi,         [rax + 112]
        mov   rdi,         [rax + 120]
        adox  rsi,         rsi
        adcx  r10,         r8
        adox  rdi,         rdi
        adcx  r11,         rsi
        mov   [rax + 104], r10
        mov   [rax + 112], r11
        lea   rax,         [rax + 128]
        lea   r9,          [r9 + 64]
        jrcxz .Llsloop_out80
        jmp   .Llsloop80
.Llsloop_out80:
        mov  rdx,       [r9]
        mulx r10, r11, rdx
        mov  rsi,       [rax]
        adox rsi,       rsi
        adcx rbx,       rdi
        adcx r11,       rsi
        adox r10,       rcx
        adc  r10,       rcx
        mov  [rax - 8], rbx
        mov  [rax],     r11
        mov  [rax + 8], r10
        pop  rbx
        pop  rsi
        pop  rdi
        pop  rbp
        ret
.L3:
        mulx r9, r8, rdx
        mov rsi,        [rcx + 8]
        mulx r11, r10, rsi
        mov rdx,        rsi
        mulx rdx, rbx, rdx
        add r10,        r10
        adc r11,        r11
        adc rdx,        0
        add r9,         r10
        adc rbx,        r11
        adc rdx,        0
        mov [rax],      r8
        mov [rax + 8],  r9
        mov [rax + 16], rbx
        mov [rax + 24], rdx

        jmp .L5