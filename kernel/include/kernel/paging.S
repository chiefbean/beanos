.text
.global loadPageDirectory
loadPageDirectory:
    push %ebp
    mov %ebp, %esp
    mov %eax, 8(%esp)
    mov %cr3, %eax
    mov %esp, %ebp
    pop %ebp
    ret

.text
.global enablePaging
enablePaging:
    push %ebp
    mov %ebp, %esp
    mov %eax, %cr0
    or $0x80000000, %eax
    mov %cr0, %eax
    mov %esp, %ebp
    pop %ebp
    ret
