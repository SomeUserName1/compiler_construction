%pragma macho64 prefix _
         global   main
         extern   printf

dim	 DD      20
         section  .data
msg:     db       "The value is %d!", 10, 0
         section  .bss
num:     resb     80                        ; reserve 20 * 4 bytes
;a:       resb     dim * 4

         section  .text
init:    push     rbp
         mov      rbp, rsp
         mov      r12d, 0
         cmp      r12d, 20
         jge      endloop_init_1
startloop_init_1:
         lea      r13, [rel num]       ; holt eine adresse in's register 13? 
         mov      r8d, 20              ; copy constant 20 to register 8
         mov      [r13,r12*4], r8d     ; copy the constant 20 from r8 to the appropriate array location
                                       ; (cannot be copied directly from dim since assembler doesn't support to copy mem to mem)
         sub      [r13,r12*4], r12d    ; subtract i-counter from the 20 in the array
         inc      r12d                 ; increment i-counter
         cmp      r12d, 20             ; check if counter still below the maximum
         jl       startloop_init_1
endloop_init_1:
         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret

print:
         push     rbp
         mov      rbp, rsp
         mov      r12d, 0
         cmp      r12d, 20
         jge      endloop_print_1
startloop_print_1:
         lea      r13, [rel num]
         inc      r12d
         cmp      r12d, 20
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
         mov      r14d, [rdi]
         mov      r15d, [rsi]
         mov      [rdi], r15d
         mov      [rsi], r14d
         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret

quicksort:
         push     rbp
         mov      rbp, rsp
         
         mov      rdi, 0                 ; 0 @ 86
         mov      rsi, 19 ;[dim]             ; Dim @ 86
         dec      rsi                    ; Dim - 1 @ 86
         call     qsort                  ; QSort(0, Dim - 1) @ 86
         xor      rax, rax
         
         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret

; gets two variables as variables
; legend l = rdi, r = rsi
;        i=r8, j=r9, x=r10
;        num_base_addr=r13
qsort:
         push     rbp,
         mov      rbp, rsp
         
         ; definitions of i, j, x
         mov      r8d, edi               ; i := l @ 68
         mov      r9d, esi               ; j := r @ 69
         mov      r12d, esi              ; r @ 70
         add      r12d, 1                ; (r+1) @ 70
         shr      r12d, 1                ; (r+1) / 2 @ 70
         lea      r13, [rel num]         ; access a[] @ 70
         mov      r10d, [r13, r12 * 4]   ; x := a[(r+1) DIV 2] @ 70
conditionloop_qsort1:
         cmp      r8d, r9d               ; i <= j @ 71
         jg       endloop_qsort_1
startloop_qsort_1:
         mov      r12d, [r13, r8*4]      ;access a[i] @ 72
         cmp      r12d, r10d             ;a[i] < x @ 72
         jl       startloop_qsort_1_1    ; start the first inner while, else: viewing loop 1.1 as ended
endloop_qsort_1_1:
         mov      r12d, [r13, r9*4]      ;access a[j] @ 73 
         cmp      r10d, r12d             ;x < a[j] @ 73
         jl       startloop_qsort_1_2
endloop_qsort_1_2:
         cmp      r8d, r9d               ; i <= j @ 74
         jle      if_qsort_1_3
         jmp      conditionloop_qsort1
endloop_qsort_1:
         cmp      edi, r9d               ; l < j @ 80
         jl       if_qsort_2             ; jump to body of second last if
         cmp      r8d, esi               ; i < r @ 81
         jl       if_qsort_3             ; jump to body of last if
         jmp      endqsort               ; both ifs not taken, jump right to end of procedure
startloop_qsort_1_1:
         inc      r8d                    ; i = i + 1 @ 72
         mov      r12d, [r13, r8*4]      ; access a[i] @ 72
         cmp      r12d, r10d             ; a[i] < x
         jge      endloop_qsort_1_1
         jmp      startloop_qsort_1_1
startloop_qsort_1_2:
         dec      r9d                    ;j := j - 1 @ 73
         mov      r12d, [r13, r9*4]      ;access a[j] @ 73
         cmp      r10d, r12d             ; x < a[j] @ 73
         jge      endloop_qsort_1_2
         jmp      startloop_qsort_1_2
if_qsort_1_3:
        ; save registers as they would be overwritten for qsort, r14, r15 are also overwritten by swap!
         push     rdi
         push     rsi
         push     r8
         push     r9
         push     r10
         push     r13
         
         ; calc memory offset r13 + r8 * 4 and store it as first param
         mov      rdi, r13
         mov      rax, 4
         mul      r8
         add      rdi, rax
         ; calc memory offset r13 + r9 * 4 and store it as second param
         mov      rsi, r13
         mov      rax, 4
         mul      r9
         add      rsi, rax
         ; call swap with adress of r13 + r8 * 4 und r13 + r9 * 4
         call     swap
         xor      rax, rax
         
         ; restore param registers
         pop      r13
         pop      r10
         pop      r9
         pop      r8
         pop      rsi
         pop      rdi
         jmp      conditionloop_qsort1
if_qsort_2:
         ; save registers as they would be overwritten for qsort, r14, r15 are also overwritten by swap!
         push     rdi
         push     rsi
         push     r8
         push     r9
         push     r10
         push     r13
         
         ; prepare params and call qsort subinstance (left)
         mov      esi, r9d
         call     qsort
         xor      rax, rax
         
         ; restore param registers
         pop      r13
         pop      r10
         pop      r9
         pop      r8
         pop      rsi
         pop      rdi
         
         ; check fore right side recursion
         cmp      r8d, esi                 ; i < r @ 81
         jge      endqsort                ; Jumps over next if body (negated comparision)
if_qsort_3:
         ; save registers as they would be overwritten for qsort, r14, r15 are also overwritten by swap!
         push     rdi
         push     rsi
         push     r8
         push     r9
         push     r10
         push     r13
         
         ; prepare params and call qsort subinstance (right)
         mov      edi, r8d
         call     qsort
         xor      rax, rax
         
         ; restore registers
         pop      r13
         pop      r10
         pop      r9
         pop      r8
         pop      rsi
         pop      rdi
endqsort:
         xor      rax, rax
         mov      rsp, rbp
         pop      rbp
         ret
         

main:
    mov rbp, rsp; for correct debugging    ;save pointer
         push     rbp
         mov      rbp, rsp
         
         ;call init
         call     init
         xor      rax, rax
        
         ;call quicksort
         call     quicksort
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
