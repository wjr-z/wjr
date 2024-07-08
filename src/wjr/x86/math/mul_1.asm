section .text
global  __wjr_asm_mul_1
__wjr_asm_mul_1:
        mov    r11, r8
        push   rbx
        mov    rax, rcx
        mov    r10, rdx
        shr    r11, 3
        mov    rdx, r9
        mov    rcx, r11
        and    r8d, 7
        lea    r9,  [rel .Llookup36]
        movsxd r8,  DWORD [r9 + r8 * 4]
        lea    r8,  [r8 + r9]
        jmp    r8

        align   8
        section .data
.Llookup36:
        dd .Ll036-.Llookup36
        dd .Ll136-.Llookup36
        dd .Ll236-.Llookup36
        dd .Ll336-.Llookup36
        dd .Ll436-.Llookup36
        dd .Ll536-.Llookup36
        dd .Ll636-.Llookup36
        dd .Ll736-.Llookup36

        align   16
        section .text
.Ll036:
        mulx rbx, r8, [r10]
        jmp .Lb036
.Ll236:
        mulx rbx, r8, [r10]
        lea r10, [r10 - 48]
        lea rax, [rax - 48]
        inc rcx
        jmp .Lb236
.Ll336:
        mulx r9, r11, [r10]
        lea r10, [r10 - 40]
        lea rax, [rax - 40]
        inc rcx
        jmp .Lb336
.Ll436:
        mulx rbx, r8, [r10]
        lea r10, [r10 - 32]
        lea rax, [rax - 32]
        inc rcx
        jmp .Lb436
.Ll536:
        mulx r9, r11, [r10]
        lea r10, [r10 - 24]
        lea rax, [rax - 24]
        inc rcx
        jmp .Lb536
.Ll636:
        mulx rbx, r8, [r10]
        lea r10, [r10 - 16]
        lea rax, [rax - 16]
        inc rcx
        jmp .Lb636
.Ll736:
        mulx r9, r11, [r10]
        lea r10, [r10 - 8]
        lea rax, [rax - 8]
        inc rcx
        jmp .Lb736
.Ld136:
        jmp .Ldone36
.Ll136:
        mulx r9, r11, [r10]
        lea   r10, [r10 + 8]
        lea   rax, [rax + 8]
        jrcxz .Ld136
.Lloop36:
        mulx rbx, r8, [r10]
        mov [rax - 8], r11
        adc r8,        r9
.Lb036:
        mulx r9, r11, [r10 + 8]
        mov [rax], r8
        adc r11,   rbx
.Lb736:
        mulx rbx, r8, [r10 + 16]
        mov [rax + 8], r11
        adc r8,        r9
.Lb636:
        mulx r9, r11, [r10 + 24]
        mov [rax + 16], r8
        adc r11,        rbx
.Lb536:
        mulx rbx, r8, [r10 + 32]
        mov [rax + 24], r11
        adc r8,         r9
.Lb436:
        mulx r9, r11, [r10 + 40]
        mov [rax + 32], r8
        adc r11,        rbx
.Lb336:
        mulx rbx, r8, [r10 + 48]
        mov [rax + 40], r11
        adc r8,         r9
.Lb236:
        mulx r9, r11, [r10 + 56]
        mov [rax + 48], r8
        adc r11,        rbx
        lea r10,        [r10 + 64]
        lea rax,        [rax + 64]
        dec rcx
        jne .Lloop36
        adc r9,         rcx
.Ldone36:
        mov [rax - 8], r11

        mov rax, r9
        pop rbx
        ret