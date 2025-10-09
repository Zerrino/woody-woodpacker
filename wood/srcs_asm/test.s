section .text
global _start


    mov rcx, qword [rsp]
    cmp rcx, 2
    jl exit

    lea rsi, [rsp+8]
    mov rbx, qword [rsi]
    mov rax, 2
    mov rdi, rbx
    xor rsi, rsi
    syscall

    mov r12, rax

    xor rdi, rdi
    mov rsi, 343
    mov rdx, 1
    mov r10, 2
    mov r8, r12
    mov r9, 4192
    mov rax, 9
    syscall

    mov r13, rax

    xor rdi, rdi
    mov rsi, 343
    mov rdx, 7
    mov r10, 0x22
    xor r8, r8
    dec r8
    xor r9, r9
    mov rax, 9
    syscall

    mov r14, rax

    mov rcx, 343
    mov rsi, r13
    mov rdi, r14

memcpy_loop:
    cmp rcx, 0
    je memcpy_finished

    mov al, [rsi]
    mov [rdi], al

    inc rsi
    inc rdi
    dec rcx
    jmp memcpy_loop

memcpy_finished:
    mov rdi, r14
    mov rsi, qword [rsp+0x10]
    mov rdx, 343
    mov rcx, 16
    xor r8, r8
    xor r9, r9

decrypt_loop:
    cmp r8, rdx
    je decrypt_end

    mov r10b, [rsi+r9]
    mov r11b, [rdi+r8]
    xor r11b, r10b
    mov [rdi+r8], r11b

    inc r8
    inc r9

    cmp r9, rcx
    jne decrypt_loop

    xor r9, r9
    jmp decrypt_loop

decrypt_end:
    mov rdi, r12
    mov rax, 3
    syscall

    mov rcx, qword [rsp]
    lea rbx, [rsp+8]

shift_argv:
    mov rax, qword [rbx+0x10]
    mov [rbx+8], rax
    add rbx, 8
    dec rcx
    cmp rcx, 1
    jg shift_argv

    mov qword ptr [rbx+8], 0

    jmp r14

exit:
    mov rax, 60
    xor rdi, rdi
    syscall
