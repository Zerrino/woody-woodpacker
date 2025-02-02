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

section .data

section	.text
	global	_ft_rotate

_ft_rotate:
	ror	rdi, 16
	mov	rax, rdi
		ret
