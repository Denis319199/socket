.global SwitchContext
.global SetTrampoline
.global SmallTrampoline
.global SwitchContextFinally

SmallTrampoline:
    movq %r13, %rcx
    call *%r12

SetTrampoline:
    movq %rsp, %rax # saves the current stack pointer
    movq (%rcx), %rsp # sets the new stack pointer

    pushq %rdx # saves the function pointer

    # saves all nonvolatile registers
    pushq $0 # RBX
    pushq $0 # RBP
    pushq $0 # RDI
    pushq $0 # RSI
    pushq %r8 # R12
    pushq %r9 # R13
    pushq $0 # R14
    pushq $0 # R15

    movq %rsp, (%rcx) # saves the moved stack pointer
    movq %rax, %rsp # returns the initial stack pointer

    ret

SwitchContext:
    # saves all nonvolatile registers to the caller stack
    pushq %rbx
    pushq %rbp
    pushq %rdi
    pushq %rsi
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq %rsp, (%rdx) # saves the caller stack pointer

    pushq %rcx # saves the callee stack pointer before calling
               # the schedule function
    subq $8, %rsp # aligns a stack to 16 bits

    movq %r9, %rcx # passes the fiber pointer as a schedule function parameter
    call *%r8 # calls schedule fucntion

    addq $8, %rsp # removes the alignment set before
    popq %rcx # returns back the callee stack pointer

SwitchContextFinally:
    movq (%rcx), %rsp # sets the callee stack pointer

    # takes out the callee registers
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rsi
    popq %rdi
    popq %rbp
    popq %rbx

	ret