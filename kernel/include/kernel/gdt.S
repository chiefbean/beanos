.global gdt_flush       # Allows the C code to call gdt_flush().
gdt_flush:
   #movl %eax, 8(%esp)   # Get the pointer to the GDT, passed as a parameter.
   #lgdt (%eax)          # Load the new GDT pointer
   mov %ax, 0x10
   mov %ds, %ax
   mov %es, %ax
   mov %fs, %ax
   mov %gs, %ax
   mov %ss, %ax
   push 0x08
   push flush           # 0x08 is the offset to our code segment: Far jump!
   retf
flush:
   ret

.global idt_flush       # Allows the C code to call idt_flush().
idt_flush:
   movl %eax, 4(%esp)   # Get the pointer to the IDT, passed as a parameter.
   lidt (%eax)          # Load the IDT pointer.
   ret