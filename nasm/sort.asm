         global   main
         extern   printf

         section  .data
msg:     db       "The value is %d!", 10, 0
dim:     equ      20

         section  .bss
num:     resb     80                        ; reserve 20 * 4 bytes
a:       resb     dim * 4

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
         
init2:   ;Alternate initialization of array so quicksort is more interesting
         push      rbp
         mov       rbp, rsp
         lea       r13, [rel a]
         mov       r12, 0
         mov       [r13, r12*4], dword 62
         inc       r12
         mov       [r13, r12*4], dword 87
         inc       r12
         mov       [r13, r12*4], dword 84
         inc       r12
         mov       [r13, r12*4], dword 54
         inc       r12
         mov       [r13, r12*4], dword 96
         inc       r12
         mov       [r13, r12*4], dword 44
         inc       r12
         mov       [r13, r12*4], dword 88
         inc       r12
         mov       [r13, r12*4], dword 30
         inc       r12
         mov       [r13, r12*4], dword 25
         inc       r12
         mov       [r13, r12*4], dword 85
         inc       r12
         mov       [r13, r12*4], dword 35
         inc       r12
         mov       [r13, r12*4], dword 100
         inc       r12
         mov       [r13, r12*4], dword 46
         inc       r12
         mov       [r13, r12*4], dword 79
         inc       r12
         mov       [r13, r12*4], dword 98
         inc       r12
         mov       [r13, r12*4], dword 76
         inc       r12
         mov       [r13, r12*4], dword 93
         inc       r12
         mov       [r13, r12*4], dword 72
         inc       r12
         mov       [r13, r12*4], dword 49
         inc       r12
         mov       [r13, r12*4], dword 14
         xor       rax, rax
         mov       rsp, rbp
         pop       rbp
         ret

print:
         ; Note: Since the funny stuff is commented in the source program we act like a good compiler and did what the programmer obviously intended ;)
         ; Note2: The comparision i <= Dim is wrong. Accesses memory outside of the array (if comments wouldnt save the day)
         ; Note3: i is not initialized with a value. Don't no if this is critical, maybe oberon initializes integers as zero...
         push     rbp
         mov      rbp, rsp
         mov      r12d, 0
         cmp      r12d, 19
         jge      endloop_print_1
startloop_print_1:
         mov      rdi, msg
         lea      r13, [rel a]
         mov      rsi, [r13, r12*4]
         call     printf
         xor      rax, rax
         inc      r12d
         cmp      r12d, 19
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

bubblesort:
        push    rbp
        mov     rbp, rsp

        mov     r8, 0                          ; i := 0

.outer_cond:                      ; While
        cmp     r8, dim                        ; i < Dim
        jge     .outer_end      ; jump outer_end if not

.outer_loop:                     ; Do
        mov     r9, dim-1                      ; j := Dim - 1

.inner_cond:                      ; While
        cmp     r9, r8                        ; j > i
        jle     .inner_end            ; jump inner_end if not

.inner_loop: ; Do TODO correct addr calc below: split or adjust
        lea     r13, [rel a]                    ; access a[]
        mov     rax, r9
        dec     rax
        mov     r10d, [r13,rax*4]            ; access a[j - 1]
        mov     r11d, [r13,r9*4]              ; access a[j]
        cmp     r10, r11                      ; a[j - 1] > a[j]
        jle     .reentry_if           ; Jump over then when cond false
        push    rdi                             
        push    rsi
        push    r8
        push    r9
        push    r10
        push    r11
        push    r13
        
        mov     rdi, r13
        mov     r14, r9
        dec     r14
        mov     rax, 4
        mul     r14
        add     rdi, rax
        
        mov     rsi, r13
        mov     rax, 4
        mul     r9
        add     rsi, rax
        call    swap
        xor     rax, rax
        
        pop     r13
        pop     r11
        pop     r10
        pop     r9
        pop     r8
        pop     rsi
        pop     rdi       

.reentry_if:
        dec     r9d                             ; j := j - 1
        jmp     .inner_cond

.inner_end:
        inc r8d
        jmp .outer_cond

.outer_end:
        
.end:
        xor rax, rax
        mov rsp, rbp
        pop rbp
        ret


quicksort:
         push     rbp
         mov      rbp, rsp
         
         mov      rdi, 0                 ; 0 @ 86
         mov      rsi, 19 ;[dim]         ; Dim - 1 @ 86
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
         add      r12d, edi              ; (r+l) @ 70
         shr      r12d, 1                ; (r+l) / 2 @ 70
         lea      r13, [rel num]         ; access a[] @ 70
         mov      r10d, [r13, r12 * 4]   ; x := a[(r+l) DIV 2] @ 70
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
         
         inc      r8                       ; i := i + 1 @76
         dec      r9                       ; j := j + 1 @77
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
         ;dec      esi
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
         ;inc      edi
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
         call     init2
         xor      rax, rax
        
         ;call quicksort
         ;call     quicksort
         call     bubblesort
         xor      rax, rax
        
         ;call print
         call     print
         xor      rax, rax
 
         ;call a final print (still here from arrays.asm)
         ;mov      rdi, msg
         ;lea      r13, [rel num]
         ;mov      rsi, [r13+12]
         ;call     printf
         ;xor      rax, rax
         
         ;restore pointer and return
         mov      rsp, rbp
         pop      rbp
         ret

; nasm -f elf64 -o sort.o sort.asm && gcc -m64 sort.o -o sort -no-pie
