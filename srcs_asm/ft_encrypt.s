; ****************************************************************************
;
;                                                         :::      ::::::::
;    ft_atoi_base.s                                     :+:      :+:    :+:
;                                                     +:+ +:+         +:+
;    By: alexafer <alexafer@student.s19.be>         +#+  +:+       +#+
;                                                 +#+#+#+#+#+   +#+
;    Created: 2024/04/19 11:28:12 by alexafer          #+#    #+#
;    Updated: 2024/04/19 11:31:18 by alexafer         ###   ########.fr
;
; ****************************************************************************

section .text
    global _ft_encrypt

; void xor_crypt(unsigned char *data, unsigned char *key, size_t size_data, size_t size_key)
; data = rdi
; key = rsi
; size_data = rdx
; size_key = rcx

_ft_encrypt:
    xor     r8, r8          ; i = 0
    xor     r9, r9          ; j = 0

.loop:
    cmp     r8, rdx
    je      .end

    mov     r10b, byte [rsi + r9]  ; key[j]
    mov     r11b, byte [rdi + r8]  ; data[i]
    xor     r11b, r10b
    mov     byte [rdi + r8], r11b

    inc     r8
    inc     r9
    cmp     r9, rcx
    jne     .loop

    xor     r9, r9  ; reset j = 0
    jmp     .loop

.end:
    ret
