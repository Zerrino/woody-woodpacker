BITS 64

section .bss
    buffer: resb 10

section .data
    name:   db "poulet", 0
    file:   db "/proc/self/exe", 0
    exec:   db "/proc/self/fd/", 0

section .text
	global _start

_start:

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
	xor		rax, rax
	mov		rdi, r9
	mov		rsi, buffer
	mov		rdx, 10
	syscall
	test	rax, rax
	jz		.done_read
	js		.error

	mov		rdx, rax
	mov		rdi, r8
	mov		rsi, buffer
	mov		rax, 1
	syscall



	jmp		.read_loop
.done_read:




	mov rax, [rsp]         ; rax = argc
	cmp	rax, 2
	jle	.no_shift

	mov	rcx, rax
	sub	rcx, 2
	shl	rax, 3
	mov		r10, rax
	lea rsi, [rsp + r10]
	mov	r11, [rsp + r10]
	mov	qword	[rsi], 0
.looping:
	sub		r10, 8
	lea rsi, [rsp + r10]
	mov	rax, r11
	mov	r11, [rsp + r10]
	mov	[rsi], rax

	loop 	.looping

.no_shift:

	lea rsi, [rsp + 8]


	;r8
	mov		rax, r8
	xor		rcx, rcx
	; divise rax par 10: quotient dans rax, reste dans rdx

.convert_fd:
	xor		rdx, rdx
	mov		r11, 10
	div		r11
	add		dl, '0'
	push	rdx
	inc		rcx
	test	rax, rax
	jnz		.convert_fd
	mov		rdi, exec
	add		rdi, 14
.looping_convert:
	pop		rdx
	mov		[rdi], dl
	inc		rdi
	loop	.looping_convert
	mov		byte [rdi], 0

	mov rax, [rsp]         ; rax = argc
	mov rcx, rax
	add rcx, 2             ; rcx = argc + 2
	shl rcx, 3             ; rcx = 8 * (argc + 2)
	lea rdx, [rsp+rcx]     ; r13 pointe sur envp
	mov		rax, 59
	mov		rdi, exec
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
