#common irq code
irq_common_stub:
    pusha
    mov %ds, %eax # Lower 16-bits of eax = ds.
	push %eax # save the data segment descriptor
	mov %ax, 0x10  # kernel data segment descriptor
    mov %ds, %ax
	mov %es, %ax
	mov %fs, %ax
	mov %gs, %ax

    call irq_handler

    pop %ebx 
	mov %ds, %ax
	mov %es, %ax
	mov %fs, %ax
	mov %gs, %ax
	popa
	add %esp, 4 # Cleans up the pushed error code and pushed ISR number
	iret # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

.text
.global irq0
irq0:
    #cli
    push $0
    push $32
    jmp irq_common_stub

.text
.global irq1
irq1:
    #cli
    push $0
    push $33
    jmp irq_common_stub

.text
.global irq2
irq2:
    #cli
    push $0
    push $34
    jmp irq_common_stub

.text
.global irq3
irq3:
    #cli
    push $0
    push $35
    jmp irq_common_stub

.text
.global irq4
irq4:
    #cli
    push $0
    push $36
    jmp irq_common_stub

.text
.global irq5
irq5:
    #cli
    push $0
    push $37
    jmp irq_common_stub

.text
.global irq6
irq6:
    #cli
    push $0
    push $38
    jmp irq_common_stub

.text
.global irq7
irq7:
    #cli
    push $0
    push $39
    jmp irq_common_stub

.text
.global irq8
irq8:
    #cli
    push $0
    push $40
    jmp irq_common_stub

.text
.global irq9
irq9:
    #cli
    push $0
    push $41
    jmp irq_common_stub

.text
.global irq10
irq10:
    #cli
    push $0
    push $42
    jmp irq_common_stub

.text
.global irq11
irq11:
    #cli
    push $0
    push $43
    jmp irq_common_stub

.text
.global irq12
irq12:
    #cli
    push $0
    push $44
    jmp irq_common_stub

.text
.global irq13
irq13:
    #cli
    push $0
    push $45
    jmp irq_common_stub

.text
.global irq14
irq14:
    #cli
    push $0
    push $46
    jmp irq_common_stub

.text
.global irq15
irq15:
    #cli
    push $0
    push $47
    jmp irq_common_stub
