.global gdt_flush       # Allows the C code to call gdt_flush().
gdt_flush:
   mov $0x10, %ax
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs
   mov %ax, %ss
   ljmp $0x08, $flush   # Far jump to reload CS
flush:
   ret

.global idt_flush       # Allows the C code to call idt_flush().
idt_flush:
   movl 4(%esp), %eax   # Get the pointer to the IDT, passed as a parameter.
   lidt (%eax)          # Load the IDT pointer.
   ret