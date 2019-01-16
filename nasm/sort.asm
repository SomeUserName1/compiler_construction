%pragma macho64 prefix _
         global   main
         extern   printf

dim	 DD      20
         section  .data
msg:     db       "The value is %d!", 10, 0
         section  .bss
num:     resb     80                        ; reserve 20 * 4 bytes
a:       resb     dim * 4

         section  .text
init:    push     rbp
         mov      rbp, rsp
         mov      r12, 0
         cmp      r12, dim
         jge      endloop_init_1
startloop_init_1:
         lea      r13, [rel num]      ;holt eine adresse in's register 13? 
         mov      r8, [dim]           ; copy constant 20 to register 8
         mov      [r13,r12*4], r8     ; copy the constant 20 from r8 to the appropriate array location
				      ; (cannot be copied directly from dim since assembler doesn't support to copy mem to mem)
         sub      [r13,r12*4], r12    ; subtract i-counter from the 20 in the array
         inc      r12                 ; increment i-counter
         cmp      r12, 20             ; check if counter still below the maximum
         jl       startloop_init_1
endloop_init_1:
         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret

print:
         push     rbp
         mov      rbp, rsp
         mov      r12, 0
         cmp      r12, [dim]
         jge      endloop_print_1
startloop_print_1:
         lea      r13, [rel num]
         inc      r12
         cmp      r12, 20
         jle      startloop_print_1
endloop_print_1:
         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret

; takes two pointers and swaps the elements behind them
swap:
         push     rbp
         mov      rbp, rsp
         mov      r12, [rdi]
         mov      r13, [rsi]
         mov      [rdi], r13
         mov      [rsi], r12
         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret

main:    ;save pointer
         push     rbp
         mov      rbp, rsp
         
         ;call init
         call     init
         xor      rax, rax
        
         ;call print
         call     print
         xor      rax, rax
 
         ;call a final print (still here from arrays.asm)
         mov      rdi, msg
         lea      r13, [rel num]
         mov      rsi, [r13+12]
         call     printf
         xor      rax, rax
         
         ;restore pointer and return
         mov      rsp, rbp
         pop      rbp
         ret

; nasm -fmacho64 arrays.asm
; ld -macosx_version_min 10.11 -lSystem -e _main -o arrays arrays.o
