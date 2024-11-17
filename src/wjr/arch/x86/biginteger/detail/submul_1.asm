section .text
global  __wjr_asm_submul_1
__wjr_asm_submul_1:
        mov    r11, r8
        push   rbx
        mov    rax, rcx
        mov    r10, rdx
        shr    r11, 3
        mov    r8d, r8d
        mov    rdx, r9
        mov    rcx, r11
        and    r8d, 7
        stc
        lea    r9,  [rel .Llookup33]
        movsxd r8,  DWORD [r9 + r8 * 4]
        lea    r8,  [r8 + r9]
        jmp    r8

        align   8
        section .data
.Llookup33:
        dd .Ll033-.Llookup33
        dd .Ll133-.Llookup33
        dd .Ll233-.Llookup33
        dd .Ll333-.Llookup33
        dd .Ll433-.Llookup33
        dd .Ll533-.Llookup33
        dd .Ll633-.Llookup33
        dd .Ll733-.Llookup33

        align   16
        section .text
.Ll033:
        mulx rbx, r8, [r10]
        jmp .Lb033
.Ll233:
        mulx rbx, r8, [r10]
        lea r10, [r10 + 16]
        lea rax, [rax - 48]
        jmp .Lb233
.Ll333:
        mulx r9, r11, [r10]
        lea r10, [r10 + 24]
        lea rax, [rax - 40]
        jmp .Lb333
.Ll433:
        mulx rbx, r8, [r10]
        lea r10, [r10 + 32]
        lea rax, [rax - 32]
        jmp .Lb433
.Ll533:
        mulx r9, r11, [r10]
        lea r10, [r10 + 40]
        lea rax, [rax - 24]
        jmp .Lb533
.Ll633:
        mulx rbx, r8, [r10]
        lea r10, [r10 - 16]
        lea rax, [rax - 16]
        jmp .Lb633
.Ll733:
        mulx r9, r11, [r10]
        lea r10, [r10 - 8]
        lea rax, [rax - 8]
        jmp .Lb733
.Ld133:
        adc r11,   [rax]
        sbb r9,    -1
        mov [rax], r11
        jmp .Ldone33
.Ll133:
        mulx r9, r11, [r10]
        not   r11
        jrcxz .Ld133
        lea   r10, [r10 + 8]
        lea   rax, [rax + 8]
.Lloop33:
        mulx rbx, r8, [r10]
        adcx r11,       [rax - 8]
        adox r8,        r9
        mov  [rax - 8], r11
.Lb033:
        not  r8
        mulx r9, r11, [r10 + 8]
        lea  rcx,   [rcx - 1]
        adox r11,   rbx
        adcx r8,    [rax]
        mov  [rax], r8
.Lb733:
        not  r11
        mulx rbx, r8, [r10 + 16]
        adcx r11,       [rax + 8]
        adox r8,        r9
        mov  [rax + 8], r11
.Lb633:
        not  r8
        mulx r9, r11, [r10 + 24]
        lea  r10,        [r10 + 64]
        adox r11,        rbx
        adcx r8,         [rax + 16]
        mov  [rax + 16], r8
.Lb533:
        not  r11
        mulx rbx, r8, [r10 - 32]
        adcx r11,        [rax + 24]
        adox r8,         r9
        mov  [rax + 24], r11
.Lb433:
        mulx r9, r11, [r10 - 24]
        not  r8
        adox r11,        rbx
        adcx r8,         [rax + 32]
        mov  [rax + 32], r8
.Lb333:
        not  r11
        mulx rbx, r8, [r10 - 16]
        adcx r11,        [rax + 40]
        adox r8,         r9
        mov  [rax + 40], r11
.Lb233:
        not   r8
        mulx r9, r11, [r10 - 8]
        adox  r11,        rbx
        adcx  r8,         [rax + 48]
        not   r11
        mov   [rax + 48], r8
        lea   rax,        [rax + 64]
        jrcxz .Lloop_out33
        jmp   .Lloop33
.Lloop_out33:
        adcx r11,       [rax - 8]
        mov  [rax - 8], r11
        adox r9,        rcx
        sbb  r9,        -1
.Ldone33:
        mov rax, r9
        pop rbx
        ret