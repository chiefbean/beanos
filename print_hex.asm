print_hex:
  pusha
  mov si, HEX_OUT + 2
  mov cx, 0

print_hex_next:
  inc cx
  mov bx, dx
  and bx, 0xf000
  shr bx, 4
  add bh, 0x30
  cmp bh, 0x39
  jg print_hex_add_7

print_hex_add:
  mov [si], bh
  inc si
  shl dx, 4
  cmp cx, 4
  jne print_hex_next
  jmp print_hex_done

print_hex_done:
  mov bx, HEX_OUT
  call print_string
  popa
  ret

print_hex_add_7:
  add bh, 0x7
  jmp print_hex_add

; variables
HEX_OUT: db "0x0000", 0
