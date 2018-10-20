.386
.model	flat, c
option	casemap: none

includelib	masm32\lib\kernel32.lib
includelib	masm32\lib\msvcrt.lib
include	masm32\include\msvcrt.inc
include	masm32\include\kernel32.inc

.data
format_printf_char	db "%c", 0
format_printf_int	db "%d", 0
format_printf_string	db "%s", 0
format_scanf_char	db " %c", 0
format_scanf_int	db " %d", 0
string_1	db "proc1 start...", 0
string_10	db "to: ", 0
string_11	db "func1 start...", 0
string_12	db "argument is num: integer", 0
string_13	db "Hanoi Tower with levels: ", 0
string_14	db "The total number of steps is: ", 0
string_15	db "func1 end with: ", 0
string_16	db "func2 start...", 0
string_17	db "Please input ch1, ch2, int1...", 0
string_18	db "<=", 0
string_19	db "<", 0
string_2	db "int1: ", 0
string_20	db "<>", 0
string_21	db "=", 0
string_22	db "<>", 0
string_23	db ">", 0
string_24	db ">=", 0
string_25	db "func2 end with: ", 0
string_26	db "proc2 start...", 0
string_27	db "arguments are int1, int2: integer; var int3: integer", 0
string_28	db "int3 now is :", 0
string_29	db "proc2 end.", 0
string_3	db "ch1: ", 0
string_4	db "ch2: ", 0
string_5	db "chArray:", 0
string_6	db "intArray:", 0
string_7	db "proc1 end.", 0
string_8	db "step ", 0
string_9	db "from: ", 0

.code

p_proc1_005A5D38	proc
	push	ebp
	mov	ebp, esp
	sub	esp, 48
	push	offset string_1
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, 10
	add	eax, 9
	mov	ebx, eax
	imul	ebx, 8
	mov	ecx, ebx
	neg	ecx
	mov	edi, ecx
	add	edi, 1024
	mov	edx, [ebp+8]
	mov	[edx-4], edi
	mov	edx, [ebp+8]
	mov	esi, 65
	mov	[edx-12], esi
	mov	edx, [ebp+8]
	mov	esi, 48
	mov	[edx-16], esi
	push	offset string_2
	push	offset format_printf_string
	mov	[ebp-4], eax
	mov	[ebp-8], ebx
	mov	[ebp-12], ecx
	mov	[ebp-16], edi
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+8]
	push	[eax-4]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_3
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+8]
	push	[eax-12]
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_4
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+8]
	push	[eax-16]
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_5
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+8]
	mov	ebx, 0
	mov	[eax-8], ebx
loop_for_start_label_1: 
	mov	eax, [ebp+8]
	mov	eax, [eax-8]
	cmp	eax, 0
	jl	loop_for_end_label_2
	mov	eax, [ebp+8]
	sub	eax, 32
	mov	ecx, [ebp+8]
	mov	ebx, [ecx-8]
	shl	ebx, 2
	sub	eax, ebx
	mov	ebx, 48
	mov	[eax], ebx
	mov	ebx, [ebp+8]
	sub	ebx, 32
	mov	edi, [ebp+8]
	mov	ecx, [edi-8]
	shl	ecx, 2
	sub	ebx, ecx
	mov	edx, [ebx]
	push	edx
	push	offset format_printf_char
	mov	[ebp-20], eax
	mov	[ebp-24], ebx
	mov	[ebp-28], edx
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	ebx, [ebp+8]
	mov	eax, [ebx-8]
	add	eax, -1
	mov	ebx, [ebp+8]
	mov	[ebx-8], eax
	mov	[ebp-32], eax
	jmp	loop_for_start_label_1
loop_for_end_label_2: 
	push	offset string_6
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+8]
	mov	ebx, 0
	mov	[eax-8], ebx
loop_for_start_label_3: 
	mov	eax, [ebp+8]
	mov	eax, [eax-8]
	cmp	eax, 2
	jg	loop_for_end_label_4
	mov	eax, [ebp+8]
	sub	eax, 20
	mov	ecx, [ebp+8]
	mov	ebx, [ecx-8]
	shl	ebx, 2
	sub	eax, ebx
	mov	ebx, [ebp+8]
	mov	ebx, [ebx-8]
	mov	[eax], ebx
	mov	ecx, [ebp+8]
	sub	ecx, 20
	mov	edi, ebx
	shl	edi, 2
	sub	ecx, edi
	mov	edx, [ecx]
	push	edx
	push	offset format_printf_int
	mov	[ebp-36], eax
	mov	eax, [ebp+8]
	mov	[eax-8], ebx
	mov	[ebp-40], ecx
	mov	[ebp-44], edx
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	ebx, [ebp+8]
	mov	eax, [ebx-8]
	add	eax, 1
	mov	ebx, [ebp+8]
	mov	[ebx-8], eax
	mov	[ebp-48], eax
	jmp	loop_for_start_label_3
loop_for_end_label_4: 
	push	offset string_7
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	leave
	ret
p_proc1_005A5D38	endp

p_mov_005AB338	proc
	push	ebp
	mov	ebp, esp
	sub	esp, 20
	mov	eax, [ebp+20]
	cmp	eax, 0
	jle	condition_else_label_5
	mov	eax, [ebp+20]
	sub	eax, 1
	push	[ebp+32]
	push	[ebp+28]
	push	[ebp+24]
	push	eax
	push	[ebp+12]
	push	[ebp+16]
	push	[ebp+8]
	mov	[ebp-4], eax
	call	p_mov_005AB338
	add	esp, 28
	mov	eax, [ebp+24]
	mov	ebx, [eax]
	mov	ecx, ebx
	add	ecx, 1
	mov	[eax], ecx
	push	offset string_8
	push	offset format_printf_string
	mov	[ebp+24], eax
	mov	[ebp-8], ebx
	mov	[ebp-12], ecx
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+24]
	mov	ebx, [eax]
	push	ebx
	push	offset format_printf_int
	mov	[ebp+24], eax
	mov	[ebp-16], ebx
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_9
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	[ebp+8]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_10
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	[ebp+12]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+20]
	sub	eax, 1
	push	[ebp+32]
	push	[ebp+28]
	push	[ebp+24]
	push	eax
	push	[ebp+8]
	push	[ebp+12]
	push	[ebp+16]
	mov	[ebp-20], eax
	call	p_mov_005AB338
	add	esp, 28
	jmp	condition_end_label_6
condition_else_label_5: 
condition_end_label_6: 
	leave
	ret
p_mov_005AB338	endp

f_func1_005AB200	proc
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	sub	esp, 4
	sub	esp, 4
	push	offset string_11
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_12
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	[ebp+8]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_13
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	[ebp+8]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	ebx, 0
	mov	[ebp-8], ebx
	mov	eax, ebp
	sub	eax, 8
	push	[ebp+12]
	push	ebp
	push	eax
	push	[ebp+8]
	push	2
	push	3
	push	1
	mov	[ebp-12], eax
	call	p_mov_005AB338
	add	esp, 28
	push	offset string_14
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	[ebp-8]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_15
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	[ebp-8]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp-8]
	mov	[ebp-4], eax
	mov	eax, [ebp-4]
	leave
	ret
f_func1_005AB200	endp

f_func2_005AB5A8	proc
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	sub	esp, 0
	push	offset string_16
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_17
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+8]
	sub	eax, 12
	push	eax
	push	offset format_scanf_char
	call	crt_scanf
	add	esp, 8
	mov	eax, [ebp+8]
	sub	eax, 16
	push	eax
	push	offset format_scanf_char
	call	crt_scanf
	add	esp, 8
	mov	eax, [ebp+8]
	sub	eax, 4
	push	eax
	push	offset format_scanf_int
	call	crt_scanf
	add	esp, 8
	mov	eax, [ebp+8]
	mov	eax, [eax-12]
	mov	ebx, [ebp+8]
	cmp	eax, [ebx-16]
	jg	condition_else_label_7
	mov	eax, [ebp+8]
	mov	eax, [eax-12]
	mov	ebx, [ebp+8]
	cmp	eax, [ebx-16]
	jge	condition_else_label_9
	mov	eax, 0
	mov	ebx, [ebp+8]
	cmp	eax, [ebx-4]
	jne	condition_else_label_11
	push	offset string_18
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	jmp	condition_end_label_12
condition_else_label_11: 
	push	offset string_19
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
condition_end_label_12: 
	jmp	condition_end_label_10
condition_else_label_9: 
	mov	eax, [ebp+8]
	mov	eax, [eax-4]
	cmp	eax, 0
	jl	condition_else_label_13
	mov	eax, [ebp+8]
	mov	eax, [eax-4]
	cmp	eax, 0
	jle	condition_else_label_15
	push	offset string_20
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	jmp	condition_end_label_16
condition_else_label_15: 
	push	offset string_21
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
condition_end_label_16: 
	jmp	condition_end_label_14
condition_else_label_13: 
	push	offset string_22
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
condition_end_label_14: 
condition_end_label_10: 
	jmp	condition_end_label_8
condition_else_label_7: 
	mov	eax, 0
	mov	ebx, [ebp+8]
	cmp	eax, [ebx-4]
	je	condition_else_label_17
	push	offset string_23
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	jmp	condition_end_label_18
condition_else_label_17: 
	push	offset string_24
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
condition_end_label_18: 
condition_end_label_8: 
	push	offset string_25
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	48
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, 48
	mov	[ebp-4], eax
	mov	eax, [ebp-4]
	leave
	ret
f_func2_005AB5A8	endp

p_proc2_005AB610	proc
	push	ebp
	mov	ebp, esp
	sub	esp, 20
	push	offset string_26
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_27
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	[ebp+8]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	[ebp+12]
	push	offset format_printf_int
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+16]
	mov	ebx, [eax]
	push	ebx
	push	offset format_printf_int
	mov	[ebp+16], eax
	mov	[ebp-4], ebx
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	mov	eax, 0
	mov	ebx, [ebp+16]
	mov	[ebx], eax
	mov	[ebp+16], ebx
loop_do_while_label_19: 
	mov	eax, [ebp+16]
	mov	ebx, [eax]
	mov	ecx, ebx
	add	ecx, [ebp+12]
	mov	[eax], ecx
	mov	edi, [ebp+8]
	add	edi, 1
	mov	[ebp+8], edi
	mov	[ebp+16], eax
	mov	[ebp-8], ebx
	mov	[ebp-12], ecx
	mov	[ebp-16], edi
	mov	eax, [ebp+8]
	cmp	eax, [ebp+12]
	jl	loop_do_while_label_19
	push	offset string_28
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	mov	eax, [ebp+16]
	mov	ebx, [eax]
	push	ebx
	push	offset format_printf_int
	mov	[ebp+16], eax
	mov	[ebp-20], ebx
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	push	offset string_29
	push	offset format_printf_string
	call	crt_printf
	add	esp, 8
	push	10
	push	offset format_printf_char
	call	crt_printf
	add	esp, 8
	leave
	ret
p_proc2_005AB610	endp

_main: 
	push	ebp
	mov	ebp, esp
	sub	esp, 4
	sub	esp, 4
	sub	esp, 4
	sub	esp, 4
	sub	esp, 4
	sub	esp, 12
	sub	esp, 28
	push	ebp
	call	p_proc1_005A5D38
	add	esp, 4
	mov	eax, 3
	imul	eax, 3
	mov	ebx, 2
	imul	ebx, 5
	mov	ecx, eax
	sub	ecx, ebx
	mov	edi, ecx
	add	edi, 1
	mov	edx, ebp
	sub	edx, 20
	mov	esi, edi
	shl	esi, 2
	sub	edx, esi
	push	ebp
	push	edx
	push	[ebp-8]
	push	[ebp-4]
	mov	[ebp-36], eax
	mov	[ebp-40], ebx
	mov	[ebp-44], ecx
	mov	[ebp-48], edi
	mov	[ebp-52], edx
	call	p_proc2_005AB610
	add	esp, 16
	push	ebp
	push	4
	call	f_func1_005AB200
	add	esp, 8
	mov	[ebp-56], eax
	mov	ebx, [ebp-56]
	mov	[ebp-4], ebx
	push	ebp
	mov	[ebp-56], ebx
	call	f_func2_005AB5A8
	add	esp, 4
	mov	[ebp-60], eax
	mov	ebx, [ebp-60]
	mov	[ebp-12], ebx
	push	0
	call	ExitProcess
	leave
	ret
end	_main
end
