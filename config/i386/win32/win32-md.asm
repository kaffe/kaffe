	TITLE	WIN32_ASM
	.486P
.model FLAT

_TEXT SEGMENT



PUBLIC _i386_do_fixup_trampoline
EXTRN _soft_fixup_trampoline:NEAR

_i386_do_fixup_trampoline PROC NEAR
	call	_soft_fixup_trampoline
	pop	ecx
	jmp	eax
_i386_do_fixup_trampoline ENDP


PUBLIC __currentFrame
__currentFrame PROC NEAR
	mov eax,ebp
	ret
__currentFrame ENDP


PUBLIC __callException
__callException PROC NEAR
	pop eax		; Throw away return value
	pop ebp		; Get new frame
	pop edx		; Get jump address
	pop eax		; Get return argument
	jmp edx		; Jump
__callException ENDP



PUBLIC __sysdepCallMethod
__sysdepCallMethod PROC NEAR
	push edi
	push ebp
	mov ebp,esp
	mov edi,dword ptr [ebp+12]
	mov eax,dword ptr [edi+12]	; nrargs
	mov edx,dword ptr [edi+4]	; args
	lea ecx,dword ptr [edi+22]	; callsize
l1:
	cmp eax,0
	je l3
	dec eax
	cmp byte ptr[ecx+eax],0
	je l1
	cmp byte ptr[ecx+eax],1
	je l2
	push dword ptr [edx+eax*8+4]
l2:
	push dword ptr [edx+eax*8]
	jmp l1
l3:
	call dword ptr [edi]		; function
	mov edi, dword ptr [ebp+12]
	mov edx, dword ptr [edi+8]	; ret
	mov cl,[edi+21]			; rettype
	cmp cl,46h
	jne l4
	fstp dword ptr [edx]
	jmp l6
l4:
	cmp cl,44h
	jne l5
	fstp qword ptr [edx]
	jmp l6
l5:
	mov dword ptr [edx],eax
	cmp cl,4Ah
	jne l6
	mov dword ptr [4+edx],edx
l6:
	leave
	pop edi
	ret
__sysdepCallMethod ENDP

PUBLIC __compareAndExchange
__compareAndExchange PROC NEAR
	mov ecx,dword ptr [esp+4]	; addr
	mov eax,dword ptr [esp+8]	; old
	mov edx,dword ptr [esp+12]	; new
	lock cmpxchg dword ptr [ecx],edx
	je l7
	mov eax,0
	ret
l7:
	mov eax,1
	ret
__compareAndExchange ENDP


_TEXT ENDS
END
