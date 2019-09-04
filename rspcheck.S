

.globl stack_dec_populate
.globl stack_dec_print


stack_dec_populate:
	movq	%rsp, %rax
	subq	%rdi, %rax
	movq	%rax, 0(%rax)
	retq

stack_dec_print:
	popq	%r15
	subq 	$0x100, %rsp
	movq	%rsp, %rax
	pushq	%r15
	retq	