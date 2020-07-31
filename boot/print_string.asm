print_string:
  pusha
  mov ah, 0x0e

print_string_loop:
  mov al, [bx]
  add bx, 1
  cmp al, 0
  je push_string_done
  int 0x10
  jmp print_string_loop

push_string_done:
  mov al, 0x0a
  int 0x10
  popa
  ret
