BITS 64

section .bss
    buffer: resb 10

section .data
    name:   db "poulet", 0
    file:   db "./woody", 0
    exec:   db "/proc/self/fd/", 0
	spac:	db "              "
	key:    db  "JV2cJGHuQabIPaZg"    ; ta clé (16 octets)

section .text
	global _start
	extern _ft_encrypt

_start:
	mov		r15, [rsp + 16]
	lea		rdi, name
	xor		rsi, rsi
	mov		rax, 319
	syscall
	test	rax, rax
	js		.error

	mov		r8, rax ; sys_memfd_create

	mov		rdi, file
	mov		rax, 2
	syscall
	test	rax, rax
	js		.error

	mov		r9, rax

.read_loop:
	mov		rsi, buffer
	xor		rax, rax
	mov		rdi, r9
	mov		rdx, 10
	syscall
	test	rax, rax
	jz		.done_read
	js		.error

	mov		rsi, buffer
	mov		rdx, rax
	mov		rdi, r8
	mov		rax, 1
	syscall
	jmp		.read_loop
.done_read:

	mov rax, [rsp]
	cmp	rax, 1
	jle	.no_shift

	mov	rcx, rax
	sub	rcx, 2
	shl	rax, 3
	mov		r10, rax
	lea rsi, [rsp + r10]
	mov	r11, [rsp + r10]
	mov		rdi, 0
	mov		[rsi], rdi
.looping:
	sub		r10, 8
	lea rsi, [rsp + r10]
	mov	rax, r11
	mov	r11, [rsp + r10]
	mov	[rsi], rax

	loop 	.looping

.no_shift:

	lea rsi, [rsp + 8]
	mov		rax, r8
	xor		rcx, rcx

.convert_fd:
	xor		rdx, rdx
	mov		r11, 10
	div		r11
	add		dl, '0'
	push	rdx
	inc		rcx
	test	rax, rax
	jnz		.convert_fd

	; HERE

	mov		r11, exec

	mov		rdi, r11

	add		rdi, 14
.looping_convert:
	pop		rdx
	mov		[rdi], dl
	inc		rdi
	loop	.looping_convert
	xor		r10, r10
	mov		[rdi], r10b



	push	r8
	push	rsi
	push	r11

    mov     rax, 0x4010        ; valeur little-endian sur 8 octets
    push    rax
    lea     rsi, [rsp]

    mov     rdx, 8
    mov     rdi, r8
    mov     r10, 0x18
    xor     r8, r8

    mov     rax, 18
   	syscall



	pop     rax
	pop		r11
	pop		rsi
	pop		r8




	mov		r12, r8
	mov		r13, r11

	push	rsi

	sub     rsp, 438

    mov     rdi, r8
    lea     rsi, [rsp]
    mov     rdx, 438
    mov     r10, 4224
    xor     r8, r8
    mov     rax, 17
    syscall



    lea     rdi, [rsp]
    mov     rsi, r15
    mov     rdx, 438


    xor     r8, r8
    xor     r9, r9

.loopa:
    cmp     r8, rdx
    je      .fi

    mov     r10b, byte [rsi + r9]
    mov     r11b, byte [rdi + r8]
    xor     r11b, r10b
    mov     byte [rdi + r8], r11b

    inc     r8
    inc     r9
    cmp     r9, 16
    jne     .loopa

    xor     r9, r9
    jmp     .loopa

.fi:



    mov     rdi, r12
    lea     rsi, [rsp]
    mov     rdx, 438
    mov     r10, 4224
    xor     r8, r8
    mov     rax, 18
	syscall


	add     rsp, 438
	pop		rsi

	mov		r11, r13


	mov rax, [rsp]
	mov rcx, rax
	add rcx, 2
	shl rcx, 3
	lea rdx, [rsp+rcx]
	mov		rdi, r11
	mov		rax, 59
	syscall
	mov		r10, rax





	mov		rax, 3
	mov		rdi, r8
	syscall
	mov		rax, 3
	mov		rdi, r9
	syscall

	mov		rdi, r10
	mov		rax, 60
	syscall


.error:
	mov		rdi, 1
	mov		rax, 60
	syscall
