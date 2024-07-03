%ifndef WJR_ASM_SUB
%define ADCSBB adc
%define FUNC_NAME __wjr_asm_addc_n_impl
%else
%define ADCSBB sbb
%define FUNC_NAME __wjr_asm_subc_n_impl
%endif

global FUNC_NAME

section .text
FUNC_NAME:
        mov     r10, r9
        push    rbx
        mov     rax, rcx
        and     r9d, 7
        shr     r10, 3
        mov     rcx, r10
        mov     r10, QWORD [rsp+48]
        add r10b, 255
        lea r11, [rel .Llookup32]
        movsxd r9, DWORD [r11 + r9 * 4]
        lea r9, [r11 + r9]
        jmp r9
.Llookup32:
        dd .Ll032-.Llookup32
        dd .Ll132-.Llookup32
        dd .Ll232-.Llookup32
        dd .Ll332-.Llookup32
        dd .Ll432-.Llookup32
        dd .Ll532-.Llookup32
        dd .Ll632-.Llookup32
        dd .Ll732-.Llookup32
.Ll032:
        mov r11, [rdx]
        mov rbx, [rdx + 8]
        ADCSBB r11, [r8]
        jmp .Lb032
.Ld132:
        ADCSBB r9, [r8]
        mov [rax], r9
        jmp .Ldone32
.Ll132:
        mov r9, [rdx]
        jrcxz .Ld132
        mov r11, [rdx + 8]
        ADCSBB r9, [r8]
        lea rdx, [rdx + 8]
        lea r8, [r8 + 8]
        lea rax, [rax + 8]
        jmp .Lb132
.Ll332:
        mov rbx, [rdx]
        mov r10, [rdx + 8]
        ADCSBB rbx, [r8]
        lea rdx, [rdx - 40]
        lea r8, [r8 - 40]
        lea rax, [rax - 40]
        inc rcx
        jmp .Lb332
.Ll432:
        mov r11, [rdx]
        mov rbx, [rdx + 8]
        ADCSBB r11, [r8]
        lea rdx, [rdx - 32]
        lea r8, [r8 - 32]
        lea rax, [rax - 32]
        inc rcx
        jmp .Lb432
.Ll532:
        mov r9, [rdx]
        mov r11, [rdx + 8]
        ADCSBB r9, [r8]
        lea rdx, [rdx - 24]
        lea r8, [r8 - 24]
        lea rax, [rax - 24]
        inc rcx
        jmp .Lb532
.Ll632:
        mov r10, [rdx]
        mov r9, [rdx + 8]
        ADCSBB r10, [r8]
        lea rdx, [rdx - 16]
        lea r8, [r8 - 16]
        lea rax, [rax - 16]
        inc rcx
        jmp .Lb632
.Ll732:
        mov rbx, [rdx]
        mov r10, [rdx + 8]
        ADCSBB rbx, [r8]
        lea rdx, [rdx - 8]
        lea r8, [r8 - 8]
        lea rax, [rax - 8]
        inc rcx
        jmp .Lb732
.Ld232:
        ADCSBB r9, [r8 + 8]
        mov [rax], r10
        mov [rax + 8], r9
        jmp .Ldone32
.Ll232:
        mov r10, [rdx]
        mov r9, [rdx + 8]
        ADCSBB r10, [r8]
        jrcxz .Ld232
        lea rdx, [rdx + 16]
        lea r8, [r8 + 16]
        lea rax, [rax + 16]
.Lloop32:
        mov r11, [rdx]
        ADCSBB r9, [r8 - 8]
        mov [rax - 16], r10
.Lb132:
        mov rbx, [rdx + 8]
        ADCSBB r11, [r8]
        mov [rax - 8], r9
.Lb032:
        mov r10, [rdx + 16]
        ADCSBB rbx, [r8 + 8]
        mov [rax], r11
.Lb732:
        mov r9, [rdx + 24]
        ADCSBB r10, [r8 + 16]
        mov [rax + 8], rbx
.Lb632:
        mov r11, [rdx + 32]
        ADCSBB r9, [r8 + 24]
        mov [rax + 16], r10
.Lb532:
        mov rbx, [rdx + 40]
        ADCSBB r11, [r8 + 32]
        mov [rax + 24], r9
.Lb432:
        mov r10, [rdx + 48]
        ADCSBB rbx, [r8 + 40]
        mov [rax + 32], r11
.Lb332:
        mov r9, [rdx + 56]
        ADCSBB r10, [r8 + 48]
        mov [rax + 40], rbx
        lea rdx, [rdx + 64]
        lea r8, [r8 + 64]
        lea rax, [rax + 64]
        dec rcx
        jne .Lloop32
        ADCSBB r9, [r8 - 8]
        mov [rax - 16], r10
        mov [rax - 8], r9
.Ldone32:
        mov r11d, ecx
        adc r11d, ecx
        mov     rax, r11
        pop     rbx
        ret