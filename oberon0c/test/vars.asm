;=====INIT======
global   main
extern   printf

section  .data
   amsg:     db       "The address is %d!", 10, 0
   vmsg:     db       "The value is %d!", 10, 0
   bmsg:     db       "The [value] is %d!", 10, 0
   prsp:    db        "The rsp is at %d!", 16, 0
   fail_su: db         "Stack unaligned!",10,0
section .text

main:
   push    rbp
   mov     rbp, rsp

  ; mov      rdi, prsp
 ;  mov      rsi, rsp
 ;  call     printf

 ;  mov      rdi, prsp
  ; mov      rsi, rsp
  ; call     printf

 ;  mov      r8, rsp
;   cmp      r8w, 0x0
;   jne      .fail_stack_unaligned

sub rsp,16
   ;mov      rdi, prsp
   ;mov      rsi, rsp
   ;call     printf
   ;reserve space for vars by moving the stack
   ;pointer as many bytes as the maximum offset
   ;in the symbol table

  ; mov      rdi, prsp
   ;mov      rsi, rsp
   ;call     printf
      mov      r8, rsp
      cmp      r8w, 0x0
      jne      .fail_stack_unaligned
; Now there should be 16 byte free to use on the stack aka 2x 64 bit uint
; first of all print whats there already
mov      rdi, amsg
mov      rsi, rbp
call     printf

mov      rdi, bmsg
mov      rsi, [rbp]
call     printf

mov      rdi, amsg
mov      r8, rbp
add      r8, 8
mov      rsi, r8
call     printf

mov      rdi, bmsg
mov      r8, rbp
add      r8, 8
mov      rsi, [r8]
call     printf

;=====PUSH ADDRESS====== of the second 64 bit int and align the stack to 16 bytes
   mov    r8,rbp
   sub    r8,8
   push   r8
   sub    rsp, 8

      mov      r8, rsp
      cmp      r8w, 0x0
      jne      .fail_stack_unaligned

mov      rdi, vmsg
mov      rsi, 42
call     printf
xor      rax,rax
mov      rsp,rbp
pop      rbp
ret

.fail_stack_unaligned:
    mov      rdi, fail_su
    call     printf
    xor      rax,rax
    mov      rsp,rbp
    pop      rbp
    ret

;nasm -felf64 -o out.o out.s
;clang -o out out.o
