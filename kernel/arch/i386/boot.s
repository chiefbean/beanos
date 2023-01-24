/* Code in this file is adapted from wiki.osdev.org/Bare_Bones */

# multiboot constants
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */
 
 # header as multiboot standard
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
 
 # reserve stack
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# kernel entry
.section .text
.global _start
.type _start, @function
_start:
	movl $stack_top, %esp
 
	# call global constructor
	call _init

	# transfer to kernel
	call kernel_main
 
	# hang
	cli
1:	hlt
	jmp 1b

.text
.global loadPageDirectory
loadPageDirectory:
	push %ebp
	mov %esp, %ebp
	mov 8(%esp), %eax
	mov %eax, %cr3
	mov %ebp, %esp
	pop %ebp
	ret

.text
.global enablePaging
enablePaging:
	push %ebp
 	mov %esp, %ebp
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	mov %ebp, %esp
	pop %ebp
	ret

.size _start, . - _start
