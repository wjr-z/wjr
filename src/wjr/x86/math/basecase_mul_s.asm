global __wjr_asm_basecase_mul_s_impl

section .text
__wjr_asm_basecase_mul_s_impl:
        push    r13
        mov     r10, rcx
        mov     rax, rdx
        push    r12
        push    rbp
        push    rdi
        push    rsi
        push    rbx
        cmp     r8, 2

        ja      .L2
        mov     rdx, QWORD [r9]
        je      .L3
        mulx rax, rdx, [rax]

        mov     QWORD [rcx], rdx
        mov     QWORD [rcx+8], rax
.L6:
        pop     rbx
        pop     rsi
        pop     rdi
        pop     rbp
        pop     r12
        pop     r13
        ret
.L2:
        mov     r10, QWORD [rsp+88]
        mov     r11, rcx
        mov     rdx, r8
        lea r12, [rdx + 2]
        mov r13, rdx
        mov ebp, edx
        and r12, 0xfffffffffffffff8
        shr r13, 3
        neg r12
        and ebp, 7
        mov rcx, r13
        lea rbx, [rel .Llookup95]
        mov rdx, [r9]
        movsxd rsi, DWORD [rbx + rbp * 4]
        lea rbx, [rbx + rsi]
        jmp rbx

        align 8
        section .data
.Llookup95:
        dd .Ll095-.Llookup95
        dd .Ll195-.Llookup95
        dd .Ll295-.Llookup95
        dd .Ll395-.Llookup95
        dd .Ll495-.Llookup95
        dd .Ll595-.Llookup95
        dd .Ll695-.Llookup95
        dd .Ll795-.Llookup95
        
        align 16
        section .text
.Ll095:
        mulx rdi, rsi, [rax]
        jmp .Lb095
.Ll295:
        mulx rdi, rsi, [rax]
        lea rax, [rax - 48]
        lea r11, [r11 - 48]
        inc rcx
        jmp .Lb295
.Ll395:
        mulx rbx, r8, [rax]
        lea rax, [rax - 40]
        lea r11, [r11 - 40]
        inc rcx
        jmp .Lb395
.Ll495:
        mulx rdi, rsi, [rax]
        lea rax, [rax - 32]
        lea r11, [r11 - 32]
        inc rcx
        jmp .Lb495
.Ll595:
        mulx rbx, r8, [rax]
        lea rax, [rax - 24]
        lea r11, [r11 - 24]
        inc rcx
        jmp .Lb595
.Ll695:
        mulx rdi, rsi, [rax]
        lea rax, [rax - 16]
        lea r11, [r11 - 16]
        inc rcx
        jmp .Lb695
.Ll795:
        mulx rbx, r8, [rax]
        lea rax, [rax - 8]
        lea r11, [r11 - 8]
        inc rcx
        jmp .Lb795
.Ll195:
        mulx rbx, r8, [rax]
        lea rax, [rax + 8]
        lea r11, [r11 + 8]
.Lloop95:
        mulx rdi, rsi, [rax]
        mov [r11 - 8], r8
        adc rsi, rbx
.Lb095:
        mulx rbx, r8, [rax + 8]
        mov [r11], rsi
        adc r8, rdi
.Lb795:
        mulx rdi, rsi, [rax + 16]
        mov [r11 + 8], r8
        adc rsi, rbx
.Lb695:
        mulx rbx, r8, [rax + 24]
        mov [r11 + 16], rsi
        adc r8, rdi
.Lb595:
        mulx rdi, rsi, [rax + 32]
        mov [r11 + 24], r8
        adc rsi, rbx
.Lb495:
        mulx rbx, r8, [rax + 40]
        mov [r11 + 32], rsi
        adc r8, rdi
.Lb395:
        mulx rdi, rsi, [rax + 48]
        mov [r11 + 40], r8
        adc rsi, rbx
.Lb295:
        mulx rbx, r8, [rax + 56]
        mov [r11 + 48], rsi
        adc r8, rdi
        lea rax, [rax + 64]
        lea r11, [r11 + 64]
        dec rcx
        jne .Lloop95
        adc rbx, rcx
        mov [r11 - 8], r8
        dec r10
        mov [r11], rbx
        jz .LDone95
        lea rax, [rax + r12 * 8]
        mov rcx, r13
        mov rdx, [r9 + 8]
        lea r9, [r9 + 8]
        lea rbx, [rel .LLookup95]
        movsxd rbp, DWORD [rbx + rbp * 4]
        lea rbp, [rbx + rbp]
        jmp rbp

        align 8
        section .data
.LLookup95:
        dd .LL095-.LLookup95
        dd .LL195-.LLookup95
        dd .LL295-.LLookup95
        dd .LL395-.LLookup95
        dd .LL495-.LLookup95
        dd .LL595-.LLookup95
        dd .LL695-.LLookup95
        dd .LL795-.LLookup95

        align 16
        section .text
.LL095:
        mulx rdi, rsi, [rax]
        lea r11, [r11 + r12 * 8 + 8]
        jmp .LB095
.LL295:
        mulx rdi, rsi, [rax - 16]
        lea r11, [r11 + r12 * 8 - 56]
        jmp .LB295
.LL395:
        mulx rbx, r8, [rax - 24]
        lea r11, [r11 + r12 * 8 - 56]
        jmp .LB395
.LL495:
        mulx rdi, rsi, [rax - 32]
        lea r11, [r11 + r12 * 8 - 56]
        jmp .LB495
.LL595:
        mulx rbx, r8, [rax - 40]
        lea r11, [r11 + r12 * 8 - 56]
        jmp .LB595
.LL695:
        mulx rdi, rsi, [rax + 16]
        lea r11, [r11 + r12 * 8 + 8]
        jmp .LB695
.LL795:
        mulx rbx, r8, [rax + 8]
        lea r11, [r11 + r12 * 8 + 8]
        jmp .LB795
.LL195:
        mulx rbx, r8, [rax - 8]
        lea r11, [r11 + r12 * 8 + 8]
.LLoop95:
        mulx rdi, rsi, [rax]
        adcx r8, [r11 - 8]
        adox rsi, rbx
        mov [r11 - 8], r8
.LB095:
        mulx rbx, r8, [rax + 8]
        lea rcx, [rcx - 1]
        adox r8, rdi
        adcx rsi, [r11]
        mov [r11], rsi
.LB795:
        mulx rdi, rsi, [rax + 16]
        adcx r8, [r11 + 8]
        adox rsi, rbx
        mov [r11 + 8], r8
.LB695:
        mulx rbx, r8, [rax + 24]
        lea rax, [rax + 64]
        adox r8, rdi
        adcx rsi, [r11 + 16]
        mov [r11 + 16], rsi
.LB595:
        mulx rdi, rsi, [rax - 32]
        adcx r8, [r11 + 24]
        adox rsi, rbx
        mov [r11 + 24], r8
.LB495:
        mulx rbx, r8, [rax - 24]
        adox r8, rdi
        adcx rsi, [r11 + 32]
        mov [r11 + 32], rsi
.LB395:
        mulx rdi, rsi, [rax - 16]
        adcx r8, [r11 + 40]
        adox rsi, rbx
        mov [r11 + 40], r8
.LB295:
        mulx rbx, r8, [rax - 8]
        adox r8, rdi
        adcx rsi, [r11 + 48]
        mov [r11 + 48], rsi
        lea r11, [r11 + 64]
        jrcxz .LLoop_out95
        jmp .LLoop95
.LLoop_out95:
        adcx r8, [r11 - 8]
        mov [r11 - 8], r8
        adox rbx, rcx
        adc rbx, rcx
        dec r10
        mov [r11], rbx
        jz .LDone95
        lea rax, [rax + r12 * 8]
        mov rcx, r13
        mov rdx, [r9 + 8]
        lea r9, [r9 + 8]
        jmp rbp
.LDone95:
        pop     rbx
        pop     rsi
        pop     rdi
        pop     rbp
        pop     r12
        pop     r13
        ret
.L3:
        mulx rbx, r11, [rax]
        mulx r8, rdx, [rax + 8]

        cmp     QWORD [rsp+88], 1
        jne     .L5
        mov     rcx, r8
        add rdx, rbx
        adc rcx, 0
        mov [r10 ], r11
        mov [r10 + 8], rdx
        mov [r10 + 16], rcx

        jmp     .L6
.L5:
        mov     rcx, rdx
        add rcx, rbx
        adc r8, 0
        mov rdx, [r9 + 8]
        mov [r10 ], r11
        mulx rbx, r11, [rax]
        mulx rdx, rsi, [rax + 8]
        add rsi, rbx
        adc rdx, 0
        add r11, rcx
        adc rsi, r8
        adc rdx, 0
        mov [r10 + 8], r11
        mov [r10 + 16], rsi
        mov [r10 + 24], rdx

        jmp     .L6