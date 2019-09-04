#include <linux/linkage.h>
#include <asm/segment.h>
#include <asm/cache.h>
#include <asm/errno.h>
#include <asm/asm-offsets.h>
#include <asm/msr.h>
#include <asm/unistd.h>
#include <asm/thread_info.h>
#include <asm/hw_irq.h>
#include <asm/page_types.h>
#include <asm/irqflags.h>
#include <asm/paravirt.h>
#include <asm/percpu.h>
#include <asm/asm.h>
#include <asm/smap.h>
#include <asm/pgtable_types.h>
#include <asm/export.h>
#include <asm/frame.h>
#include <asm/nospec-branch.h>
#include <linux/err.h>



.globl ukl_stack_switch
.globl ukl_stack_switch_back

ukl_stack_switch:
	movq	$0x0, %rax
	movq	$0x7FFFFFFFFFFFFFFF, %r15
	cmp 	%r15, %rsp
	ja	1f
	
	jmp 	ukl_stack_switch
	popq	%r15
	movq	%rsp, %r14
	movq	PER_CPU_VAR(cpu_current_top_of_stack), %rsp
	pushq	%r14
	movq	%rsp, %rax
	pushq	%r15
1:
	retq

ukl_stack_switch_back:
	popq	%r15
	movq	%rdi, %rsp
	popq	%rsp
	pushq	%r15
	retq
