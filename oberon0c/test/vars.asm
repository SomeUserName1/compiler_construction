;=====INIT======
global   main
extern   printf

section  .data
   amsg:     db       "The address is %x!", 10, 0
   vmsg:     db       "The value is %d!", 10, 0
   bmsg:     db       "The [value] is %u!", 10, 0
   prsp:     db       "The rsp is at %x!", 32, 0
   fail_su:  db       "Stack unaligned!",10,0
section .text

main:
    push    rbp
    mov     rbp, rsp

    sub rsp,16

 mov QWORD  [rsp+0], 0

    mov      rdx,0
    mov      rax, rsp
    mov      r8,16
    div      r8
    cmp      rdx, 0x0
    jne      .fail_stack_unaligned

    mov      rdi, amsg
    mov      r8, rbp
    sub      r8,7
    mov      rsi, rbp
    call     printf

    mov      rdi, bmsg
    mov      rsi, QWORD [rbp-9]
    call     printf



;=====PUSH ADDRESS======
   mov    r8,rbp
; offset is 24
   sub    r8, 9
   push   r8
   sub    rsp, 8

;=====PUSH CONST======
   push   3
   sub    rsp, 8

;=====ASSIGN======
   add    rsp, 8
   pop    r9
   add    rsp, 8
   pop    r8
   mov    [r8], r9

    mov      rdx,0
    mov      rax, rsp
    mov      r8,16
    div      r8
    cmp      rdx, 0x0
    jne      .fail_stack_unaligned

    mov      rdi, amsg
    mov      r8, rbp
    sub      r8,7
    mov      rsi, rbp
    call     printf

    mov      rdi, bmsg
    mov      rsi, QWORD [rbp-9]
    call     printf

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
