BITS 64
default rel

section .data
    message db 'Hello, world', 10  ; 10 = '\n'
    msg_len equ $ - message

section .text
    global _start

_start:
    ; write(1, message, msg_len)
    mov     rax, 1          ; syscall: write
    mov     rdi, 1          ; file descriptor: stdout
    lea     rsi, [rel message]    ; pointer to message
    mov     rdx, 13    ; message length
    syscall

    ; exit(0)
    mov     rax, 60         ; syscall: exit
    xor     rdi, rdi        ; return code 0
    syscall

