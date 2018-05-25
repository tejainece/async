.intel_syntax noprefix

.file	"change_context.asm"
.text
.p2align 4,,15
.globl	changeContext
changeContext:

	movq %rax, %rcx
	movq [%rax+0x00], %r12
	movq [%rax+0x08], %r13
	movq [%rax+0x10], %r14
	movq [%rax+0x18], %r15
	movq [%rax+0x20], %rdi
	movq [%rax+0x28], %rsi
	movq [%rax+0x30], %rbx
	movq [%rax+0x38], %rsp
	movq [%rax+0x40], %rbp
	movq [%rax+0x48], 0

	// TODO save XMM

	// TODO restore XMM

	movq %rax, %rdx
	movq %r12, [%rax+0x00]
	movq %r13, [%rax+0x08]
	movq %r14, [%rax+0x10]
	movq %r15, [%rax+0x18]
	movq %rdi, [%rax+0x20]
	movq %rsi, [%rax+0x28]
	movq %rbx, [%rax+0x30]
	movq %rsp, [%rax+0x38]
	movq %rbp, [%rax+0x40]
	movq %r10, [%rax+0x48]
	test %r10, %r10
	je   skip
	jmp  %r10
skip:
	retq
