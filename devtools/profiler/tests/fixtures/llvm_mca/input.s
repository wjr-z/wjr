.text
# LLVM-MCA-BEGIN dependent_add
addq %rax, %rbx
addq %rbx, %rcx
# LLVM-MCA-END
# LLVM-MCA-BEGIN independent_add
addq %rax, %rbx
addq %rax, %rcx
# LLVM-MCA-END
