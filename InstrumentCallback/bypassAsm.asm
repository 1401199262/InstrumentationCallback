

EXTERN NtUserGetKeySyscallIndex:dd
EXTERN SyscallTrampoline:dq
EXTERN NtUserGetAsyncKeyState:dq
EXTERN JmpRdi:dq

.code

AsmGetKeySkilled PROC
	mov r10, rcx 
	mov eax, dword ptr [NtUserGetKeySyscallIndex]
	syscall
	ret
	int 3
AsmGetKeySkilled ENDP

AsmGetKeyExpert PROC
	mov r10, rcx 
	mov eax, dword ptr [NtUserGetKeySyscallIndex]
	jmp [SyscallTrampoline]
	int 3
AsmGetKeyExpert ENDP


AsmGetKeyEvilIntern PROC
	
	mov rdi,[rsp]
	mov rax,[JmpRdi]
	mov [rsp],rax

	mov r10, rcx 
	mov eax, dword ptr [NtUserGetKeySyscallIndex]
	jmp [SyscallTrampoline]

	int 3
AsmGetKeyEvilIntern ENDP

AsmGetKeyEvil PROC
	;mov rsp, [FakeStack]
	push rdi
	call AsmGetKeyEvilIntern
	pop rdi
	ret
AsmGetKeyEvil ENDP


END
