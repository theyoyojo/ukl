
.global save_callee_regs
.global restore_callee_regs
.global	addr_print
.extern printaddr

.macro allpush
	pushq	%rax
	pushq	%rbx
	pushq	%rcx
	pushq	%rdx
	pushq	%rsi
	pushq	%rdi
	pushq	%rbp
	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15
.endm

.macro allpop
	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8
	popq	%rbp
	popq	%rdi
	popq	%rsi
	popq	%rdx
	popq	%rcx
	popq	%rbx
	popq	%rax
.endm

addr_print:
	popq	%r14
	pushq	%r14
	allpush
	movq	%r14,	%rdi
	call	printaddr
	allpop
	retq

save_callee_regs:
	movq	%rcx,-8(%rsp)
	popq	%rcx
	pushq   %rax
        subq	$8,%rsp
        pushq   %rdx
        pushq   %rsi
        pushq   %rdi
        pushq   %r8
        pushq   %r9
        pushq   %r10
        pushq   %r11
	pushq	%rcx
	retq

restore_callee_regs:
	movq	72(%rsp),%rax
	popq	%r11
	movq	%r11,64(%rsp)
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8
	popq	%rdi
	popq	%rsi
	popq	%rdx
	popq	%rcx
	retq
