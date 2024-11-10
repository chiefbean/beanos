# Defined in isr.c
#[extern isr_handler]

# Common ISR code
isr_common_stub:
    # 1. Save CPU state
	pusha # Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov %ds, %eax # Lower 16-bits of eax = ds.
	push %eax # save the data segment descriptor
	mov %ax, 0x10  # kernel data segment descriptor
    mov %ds, %ax
	mov %es, %ax
	mov %fs, %ax
	mov %gs, %ax
	
    # 2. Call C handler
	call isr_handler
	
    # 3. Restore state
	pop %eax 
	mov %ds, %ax
	mov %es, %ax
	mov %fs, %ax
	mov %gs, %ax
	popa
	add %esp, 4 # Cleans up the pushed error code and pushed ISR number
	iret # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
	
/*
   We don't get information about which interrupt was caller
   when the handler is run, so we will need to have a different handler
   for every interrupt.
   Furthermore, some interrupts push an error code onto the stack but others
   don't, so we will push a dummy error code for those which don't, so that
   we have a consistent stack for all of them.
*/

# 0: Divide By Zero Exception
.text
.global isr0
isr0:
    #cli
    push $0
    push $0
    jmp isr_common_stub

# 1: Debug Exception
.text
.global isr1
isr1:
    #cli
    push $0
    push $1
    jmp isr_common_stub

# 2: Non Maskable Interrupt Exception
.text
.global isr2
isr2:
    #cli
    push $0
    push $2
    jmp isr_common_stub

# 3: Int 3 Exception
.text
.global isr3
isr3:
    #cli
    push $0
    push $3
    jmp isr_common_stub

# 4: INTO Exception
.text
.global isr4
isr4:
    #cli
    push $0
    push $4
    jmp isr_common_stub

# 5: Out of Bounds Exception
.text
.global isr5
isr5:
    #cli
    push $0
    push $5
    jmp isr_common_stub

# 6: Invalid Opcode Exception
.text
.global isr6
isr6:
    #cli
    push $0
    push $6
    jmp isr_common_stub

# 7: Coprocessor Not Available Exception
.text
.global isr7
isr7:
    #cli
    push $0
    push $7
    jmp isr_common_stub

# 8: Double Fault Exception (With Error Code!)
.text
.global isr8
isr8:
    #cli
    push $8
    jmp isr_common_stub

# 9: Coprocessor Segment Overrun Exception
.text
.global isr9
isr9:
    #cli
    push $0
    push $9
    jmp isr_common_stub

# 10: Bad TSS Exception (With Error Code!)
.text
.global isr10
isr10:
    #cli
    push $10
    jmp isr_common_stub

# 11: Segment Not Present Exception (With Error Code!)
.text
.global isr11
isr11:
    #cli
    push $11
    jmp isr_common_stub

# 12: Stack Fault Exception (With Error Code!)
.text
.global isr12
isr12:
    #cli
    push $12
    jmp isr_common_stub

# 13: General Protection Fault Exception (With Error Code!)
.text
.global isr13
isr13:
    #cli
    push $13
    jmp isr_common_stub

# 14: Page Fault Exception (With Error Code!)
.text
.global isr14
isr14:
    #cli
    push $14
    jmp isr_common_stub

# 15: Reserved Exception
.text
.global isr15
isr15:
    #cli
    push $0
    push $15
    jmp isr_common_stub

# 16: Floating Point Exception
.text
.global isr16
isr16:
    #cli
    push $0
    push $16
    jmp isr_common_stub

# 17: Alignment Check Exception
.text
.global isr17
isr17:
    #cli
    push $17
    jmp isr_common_stub

# 18: Machine Check Exception
.text
.global isr18
isr18:
    #cli
    push $0
    push $18
    jmp isr_common_stub

# 19: Reserved
.text
.global isr19
isr19:
    #cli
    push $0
    push $19
    jmp isr_common_stub

# 20: Reserved
.text
.global isr20
isr20:
    #cli
    push $0
    push $20
    jmp isr_common_stub

# 21: Reserved
.text
.global isr21
isr21:
    #cli
    push $21
    jmp isr_common_stub

# 22: Reserved
.text
.global isr22
isr22:
    #cli
    push $0
    push $22
    jmp isr_common_stub

# 23: Reserved
.text
.global isr23
isr23:
    #cli
    push $0
    push $23
    jmp isr_common_stub

# 24: Reserved
.text
.global isr24
isr24:
    #cli
    push $0
    push $24
    jmp isr_common_stub

# 25: Reserved
.text
.global isr25
isr25:
    #cli
    push $0
    push $25
    jmp isr_common_stub

# 26: Reserved
.text
.global isr26
isr26:
    #cli
    push $0
    push $26
    jmp isr_common_stub

# 27: Reserved
.text
.global isr27
isr27:
    #cli
    push $0
    push $27
    jmp isr_common_stub

# 28: Reserved
.text
.global isr28
isr28:
    #cli
    push $0
    push $28
    jmp isr_common_stub

# 29: Reserved
.text
.global isr29
isr29:
    #cli
    push $0
    push $29
    jmp isr_common_stub

# 30: Reserved
.text
.global isr30
isr30:
    #cli
    push $0
    push $30
    jmp isr_common_stub

# 31: Reserved
.text
.global isr31
isr31:
    #cli
    push $0
    push $31
    jmp isr_common_stub