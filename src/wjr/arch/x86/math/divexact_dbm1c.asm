section .text
global  __wjr_asm_divexact_dbm1c
__wjr_asm_divexact_dbm1c:
        mov rax, rdx
        mov rdx, r9
        lea r9,  [0+r8*8]
        mov r10, r8
        mov r11, QWORD [rsp+40]
        mov r8d, r8d
        neg r10
        add rcx, r9
        add rax, r9
        and r8d, 3
        je  .Lb042
        lea r10, [r10 + r8 * 1 - 4]
        cmp r8d, 2
        jb  .Lb142
        je  .Lb242
        jmp .Lb342
.Lloop42:
.Lb042:
        mulx r8, r9, [rax + r10 * 8]
        sub r11,             r9
        mov [rcx + r10 * 8], r11
        sbb r11,             r8
.Lb342:
        mulx r8, r9, [rax + r10 * 8 + 8]
        sub r11,                 r9
        mov [rcx + r10 * 8 + 8], r11
        sbb r11,                 r8
.Lb242:
        mulx r8, r9, [rax + r10 * 8 + 16]
        sub r11,                  r9
        mov [rcx + r10 * 8 + 16], r11
        sbb r11,                  r8
.Lb142:
        mulx r8, r9, [rax + r10 * 8 + 24]
        sub r11,                  r9
        mov [rcx + r10 * 8 + 24], r11
        sbb r11,                  r8
        add r10,                  4
        jne .Lloop42

        mov rax, r11
        ret