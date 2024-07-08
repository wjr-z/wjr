%ifndef WJR_ASM_SUB
%define ADCSBB    adc
%define FUNC_NAME __wjr_asm_addlsh_n
%else
%define ADCSBB    sbb
%define FUNC_NAME __wjr_asm_rsblsh_n
%endif

section .text
global  FUNC_NAME
FUNC_NAME:
        push   rdi
        mov    r10,  r9
        mov    r11d, 64
        mov    rax,  rcx
        push   rsi
        shr    r10,  3
        push   rbx
        mov    rbx,  QWORD [rsp+64]
        mov    rcx,  r10
        sub    r11,  rbx
        and    r9d,  7
        lea    rsi,  [rel .Llookup40]
        movsxd r9,   DWORD [rsi + r9 * 4]
        lea    r9,   [r9 + rsi]
        jmp    r9

        align   8
        section .data
.Llookup40:
        dd .Ll040-.Llookup40
        dd .Ll140-.Llookup40
        dd .Ll240-.Llookup40
        dd .Ll340-.Llookup40
        dd .Ll440-.Llookup40
        dd .Ll540-.Llookup40
        dd .Ll640-.Llookup40
        dd .Ll740-.Llookup40

        align   16
        section .text
.Ll040:
        mov    rsi,   [r8]
        shlx r10, rsi, rbx
        ADCSBB r10,   [rdx]
        shrx rsi, rsi, r11
        mov    r9,    [r8 + 8]
        shlx rdi, r9, rbx
        mov    [rax], r10
        jmp    .Lb040
.Ld140:
        mov    [rax], rdi
        shrx r10, r10, r11
        ADCSBB r10,   rcx
        jmp    .Ldone40
.Ll140:
        mov    r10,   [r8]
        shlx rdi, r10, rbx
        ADCSBB rdi,   [rdx]
        jrcxz  .Ld140
        shrx r10, r10, r11
        mov    rsi,   [r8 + 8]
        shlx r9, rsi, rbx
        mov    [rax], rdi
        lea    rdx,   [rdx + 8]
        lea    r8,    [r8 + 8]
        lea    rax,   [rax + 8]
        jmp    .Lb140
.Ll340:
        mov    r9,    [r8]
        shlx rsi, r9, rbx
        ADCSBB rsi,   [rdx]
        shrx r9, r9, r11
        mov    rdi,   [r8 + 8]
        shlx r10, rdi, rbx
        mov    [rax], rsi
        lea    rdx,   [rdx - 40]
        lea    r8,    [r8 - 40]
        lea    rax,   [rax - 40]
        jmp    .Lb340
.Ll440:
        mov    rsi,   [r8]
        shlx r10, rsi, rbx
        ADCSBB r10,   [rdx]
        shrx rsi, rsi, r11
        mov    r9,    [r8 + 8]
        shlx rdi, r9, rbx
        mov    [rax], r10
        lea    rdx,   [rdx - 32]
        lea    r8,    [r8 - 32]
        lea    rax,   [rax - 32]
        jmp    .Lb440
.Ll540:
        mov    r10,   [r8]
        shlx rdi, r10, rbx
        ADCSBB rdi,   [rdx]
        shrx r10, r10, r11
        mov    rsi,   [r8 + 8]
        shlx r9, rsi, rbx
        mov    [rax], rdi
        lea    rdx,   [rdx - 24]
        lea    r8,    [r8 - 24]
        lea    rax,   [rax - 24]
        jmp    .Lb540
.Ll640:
        mov    rdi,   [r8]
        shlx r9, rdi, rbx
        ADCSBB r9,    [rdx]
        shrx rdi, rdi, r11
        mov    r10,   [r8 + 8]
        shlx rsi, r10, rbx
        mov    [rax], r9
        lea    rdx,   [rdx - 16]
        lea    r8,    [r8 - 16]
        lea    rax,   [rax - 16]
        jmp    .Lb640
.Ll740:
        mov    r9,    [r8]
        shlx rsi, r9, rbx
        ADCSBB rsi,   [rdx]
        shrx r9, r9, r11
        mov    rdi,   [r8 + 8]
        shlx r10, rdi, rbx
        mov    [rax], rsi
        lea    rdx,   [rdx - 8]
        lea    r8,    [r8 - 8]
        lea    rax,   [rax - 8]
        jmp    .Lb740
.Ld240:
        lea    rdi,       [rdi + rsi]
        ADCSBB rdi,       [rdx + 8]
        shrx r10, r10, r11
        ADCSBB r10,       rcx
        mov    [rax + 8], rdi
        jmp    .Ldone40
.Ll240:
        mov    rdi,   [r8]
        shlx r9, rdi, rbx
        ADCSBB r9,    [rdx]
        shrx rdi, rdi, r11
        mov    r10,   [r8 + 8]
        shlx rsi, r10, rbx
        mov    [rax], r9
        jrcxz  .Ld240
        lea    rdx,   [rdx + 16]
        lea    r8,    [r8 + 16]
        lea    rax,   [rax + 16]
.Lloop40:
        lea    rdi,       [rdi + rsi]
        ADCSBB rdi,       [rdx - 8]
        shrx r10, r10, r11
        mov    rsi,       [r8]
        shlx r9, rsi, rbx
        mov    [rax - 8], rdi
.Lb140:
        lea    r10,   [r10 + r9]
        ADCSBB r10,   [rdx]
        shrx rsi, rsi, r11
        mov    r9,    [r8 + 8]
        shlx rdi, r9, rbx
        mov    [rax], r10
.Lb040:
        lea    rsi,       [rsi + rdi]
        ADCSBB rsi,       [rdx + 8]
        shrx r9, r9, r11
        mov    rdi,       [r8 + 16]
        shlx r10, rdi, rbx
        mov    [rax + 8], rsi
        lea    rcx,       [rcx - 1]
.Lb740:
        lea    r9,         [r9 + r10]
        ADCSBB r9,         [rdx + 16]
        shrx rdi, rdi, r11
        mov    r10,        [r8 + 24]
        shlx rsi, r10, rbx
        mov    [rax + 16], r9
.Lb640:
        lea    rdi,        [rdi + rsi]
        ADCSBB rdi,        [rdx + 24]
        shrx r10, r10, r11
        mov    rsi,        [r8 + 32]
        shlx r9, rsi, rbx
        mov    [rax + 24], rdi
.Lb540:
        lea    r10,        [r10 + r9]
        ADCSBB r10,        [rdx + 32]
        shrx rsi, rsi, r11
        mov    r9,         [r8 + 40]
        shlx rdi, r9, rbx
        mov    [rax + 32], r10
.Lb440:
        lea    rsi,        [rsi + rdi]
        ADCSBB rsi,        [rdx + 40]
        shrx r9, r9, r11
        mov    rdi,        [r8 + 48]
        shlx r10, rdi, rbx
        mov    [rax + 40], rsi
.Lb340:
        lea    r9,         [r9 + r10]
        ADCSBB r9,         [rdx + 48]
        shrx rdi, rdi, r11
        mov    r10,        [r8 + 56]
        shlx rsi, r10, rbx
        mov    [rax + 48], r9
        lea    rdx,        [rdx + 64]
        lea    r8,         [r8 + 64]
        lea    rax,        [rax + 64]
        jrcxz  .Lloop_out40
        jmp    .Lloop40
.Lloop_out40:
        lea    rdi,       [rdi + rsi]
        ADCSBB rdi,       [rdx - 8]
        shrx r10, r10, r11
        ADCSBB r10,       rcx
        mov    [rax - 8], rdi
.Ldone40:
        pop rbx
        mov rax, r10
        pop rsi
        pop rdi
        ret