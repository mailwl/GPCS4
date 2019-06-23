.code

; TODO:
; Clang doesn't support using va_start in sysv_abi functions.
; Thus it is very hard to implement a full featured variadic function in our situation.
; Because the calling convention of variadic function is totally different
; between ms_abi and sysv_abi. We can not determin the register used order without
; parsing format string. And even worse,
; we can not parse the format string using C language,
; since we can't use va_arg and va_start stuffs.
;
; Here I only pass the first six interger arguments, assuming the caller
; only use those registers, no float point values involved.

extern sprintf_s:proc
scec_sprintf_s_asm proc

	sub rsp, 30h
	test al, al  ; al stores number of vector registers used.
	jnz L_FOUND_FP
	mov [rsp + 28h], r9
	mov [rsp + 20h], r8
	mov r9, rcx
	mov r8, rdx
	mov rdx, rsi
	mov rcx, rdi
	call sprintf_s
	add rsp, 30h
	ret
L_FOUND_FP:
	xor bl, bl 
	div bl    ; for debugging purpose

scec_sprintf_s_asm endp


end