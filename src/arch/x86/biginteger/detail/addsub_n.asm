%ifndef WJR_ASM_SUB
%define ADCSBB    adc
%define FUNC_NAME __wjr_asm_addc_n_impl
%else
%define ADCSBB    sbb
%define FUNC_NAME __wjr_asm_subc_n_impl
%endif

section .text
global  FUNC_NAME
FUNC_NAME:
        mov    rax,  r9
        push   rbx
        mov    r10,  rcx
        mov    r11d, DWORD [rsp+48]
        shr    rax,  3
        and    r9d,  7
        mov    rcx,  rax
        add    r11b, 255
        lea    rax,  [rel .Llookup29]
        movsxd r9,   DWORD [rax + r9 * 4]
        lea    r9,   [rax + r9]
        jmp    r9
.Llookup29:
        dd .Ll029-.Llookup29
        dd .Ll129-.Llookup29
        dd .Ll229-.Llookup29
        dd .Ll329-.Llookup29
        dd .Ll429-.Llookup29
        dd .Ll529-.Llookup29
        dd .Ll629-.Llookup29
        dd .Ll729-.Llookup29
.Ll029:
        mov    rax, [rdx]
        mov    rbx, [rdx + 8]
        ADCSBB rax, [r8]
        jmp    .Lb029
.Ld129:
        ADCSBB r9,    [r8]
        mov    [r10], r9
        jmp    .Ldone29
.Ll129:
        mov    r9,  [rdx]
        jrcxz  .Ld129
        mov    rax, [rdx + 8]
        ADCSBB r9,  [r8]
        lea    rdx, [rdx + 8]
        lea    r8,  [r8 + 8]
        lea    r10, [r10 + 8]
        jmp    .Lb129
.Ll329:
        mov    rbx, [rdx]
        mov    r11, [rdx + 8]
        ADCSBB rbx, [r8]
        lea    rdx, [rdx - 40]
        lea    r8,  [r8 - 40]
        lea    r10, [r10 - 40]
        inc    rcx
        jmp    .Lb329
.Ll429:
        mov    rax, [rdx]
        mov    rbx, [rdx + 8]
        ADCSBB rax, [r8]
        lea    rdx, [rdx - 32]
        lea    r8,  [r8 - 32]
        lea    r10, [r10 - 32]
        inc    rcx
        jmp    .Lb429
.Ll529:
        mov    r9,  [rdx]
        mov    rax, [rdx + 8]
        ADCSBB r9,  [r8]
        lea    rdx, [rdx - 24]
        lea    r8,  [r8 - 24]
        lea    r10, [r10 - 24]
        inc    rcx
        jmp    .Lb529
.Ll629:
        mov    r11, [rdx]
        mov    r9,  [rdx + 8]
        ADCSBB r11, [r8]
        lea    rdx, [rdx - 16]
        lea    r8,  [r8 - 16]
        lea    r10, [r10 - 16]
        inc    rcx
        jmp    .Lb629
.Ll729:
        mov    rbx, [rdx]
        mov    r11, [rdx + 8]
        ADCSBB rbx, [r8]
        lea    rdx, [rdx - 8]
        lea    r8,  [r8 - 8]
        lea    r10, [r10 - 8]
        inc    rcx
        jmp    .Lb729
.Ld229:
        ADCSBB r9,        [r8 + 8]
        mov    [r10],     r11
        mov    [r10 + 8], r9
        jmp    .Ldone29
.Ll229:
        mov    r11, [rdx]
        mov    r9,  [rdx + 8]
        ADCSBB r11, [r8]
        jrcxz  .Ld229
        lea    rdx, [rdx + 16]
        lea    r8,  [r8 + 16]
        lea    r10, [r10 + 16]
.Lloop29:
        mov    rax,        [rdx]
        ADCSBB r9,         [r8 - 8]
        mov    [r10 - 16], r11
.Lb129:
        mov    rbx,       [rdx + 8]
        ADCSBB rax,       [r8]
        mov    [r10 - 8], r9
.Lb029:
        mov    r11,   [rdx + 16]
        ADCSBB rbx,   [r8 + 8]
        mov    [r10], rax
.Lb729:
        mov    r9,        [rdx + 24]
        ADCSBB r11,       [r8 + 16]
        mov    [r10 + 8], rbx
.Lb629:
        mov    rax,        [rdx + 32]
        ADCSBB r9,         [r8 + 24]
        mov    [r10 + 16], r11
.Lb529:
        mov    rbx,        [rdx + 40]
        ADCSBB rax,        [r8 + 32]
        mov    [r10 + 24], r9
.Lb429:
        mov    r11,        [rdx + 48]
        ADCSBB rbx,        [r8 + 40]
        mov    [r10 + 32], rax
.Lb329:
        mov    r9,         [rdx + 56]
        ADCSBB r11,        [r8 + 48]
        mov    [r10 + 40], rbx
        lea    rdx,        [rdx + 64]
        lea    r8,         [r8 + 64]
        lea    r10,        [r10 + 64]
        dec    rcx
        jne    .Lloop29
        ADCSBB r9,         [r8 - 8]
        mov    [r10 - 16], r11
        mov    [r10 - 8],  r9
.Ldone29:
        mov eax, ecx
        adc eax, ecx
        pop rbx
        ret