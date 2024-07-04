global __wjr_asm_addmul_1

section .text
__wjr_asm_addmul_1:
        mov     r11, r8
        push    rbx
        mov     rax, rcx
        mov     r10, rdx
        shr     r11, 3
        mov     rdx, r9
        mov     rcx, r11
        and r8d, 7
        lea r9, [rel .Llookup36]
        movsxd r8, DWORD [r9 + r8 * 4]
        lea r8, [r8 + r9]
        jmp r8

        align 8
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

        align 16
        section .text
.Ll036:
        mulx rbx, r8, [r10]
        jmp .Lb036
.Ll236:
        mulx rbx, r8, [r10]
        lea r10, [r10 + 16]
        lea rax, [rax - 48]
        jmp .Lb236
.Ll336:
        mulx r9, r11, [r10]
        lea r10, [r10 + 24]
        lea rax, [rax - 40]
        jmp .Lb336
.Ll436:
        mulx rbx, r8, [r10]
        lea r10, [r10 + 32]
        lea rax, [rax - 32]
        jmp .Lb436
.Ll536:
        mulx r9, r11, [r10]
        lea r10, [r10 + 40]
        lea rax, [rax - 24]
        jmp .Lb536
.Ll636:
        mulx rbx, r8, [r10]
        lea r10, [r10 - 16]
        lea rax, [rax - 16]
        jmp .Lb636
.Ll736:
        mulx r9, r11, [r10]
        lea r10, [r10 - 8]
        lea rax, [rax - 8]
        jmp .Lb736
.Ld136:
        add r11, [rax - 8]
        mov [rax - 8], r11
        adc r9, rcx
        jmp .Ldone36
.Ll136:
        mulx r9, r11, [r10]
        lea r10, [r10 + 8]
        lea rax, [rax + 8]
        jrcxz .Ld136
.Lloop36:
        mulx rbx, r8, [r10]
        adcx r11, [rax - 8]
        adox r8, r9
        mov [rax - 8], r11
.Lb036:
        mulx r9, r11, [r10 + 8]
        lea rcx, [rcx - 1]
        adox r11, rbx
        adcx r8, [rax]
        mov [rax], r8
.Lb736:
        mulx rbx, r8, [r10 + 16]
        adcx r11, [rax + 8]
        adox r8, r9
        mov [rax + 8], r11
.Lb636:
        mulx r9, r11, [r10 + 24]
        lea r10, [r10 + 64]
        adox r11, rbx
        adcx r8, [rax + 16]
        mov [rax + 16], r8
.Lb536:
        mulx rbx, r8, [r10 - 32]
        adcx r11, [rax + 24]
        adox r8, r9
        mov [rax + 24], r11
.Lb436:
        mulx r9, r11, [r10 - 24]
        adox r11, rbx
        adcx r8, [rax + 32]
        mov [rax + 32], r8
.Lb336:
        mulx rbx, r8, [r10 - 16]
        adcx r11, [rax + 40]
        adox r8, r9
        mov [rax + 40], r11
.Lb236:
        mulx r9, r11, [r10 - 8]
        adox r11, rbx
        adcx r8, [rax + 48]
        mov [rax + 48], r8
        lea rax, [rax + 64]
        jrcxz .Lloop_out36
        jmp .Lloop36
.Lloop_out36:
        adcx r11, [rax - 8]
        mov [rax - 8], r11
        adox r9, rcx
        adc r9, rcx
.Ldone36:
        mov     rax, r9
        pop     rbx
        ret