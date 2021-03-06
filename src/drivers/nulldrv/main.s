section .bss
mymessage:
	resq 1024

extern main
extern exit

section .text
global getpid
getpid:
	push rbp
	mov rbp, rsp

	; address of out var should already be in rdi
	mov rax, 0x13  ; getpid syscall
	syscall        ; result is now already in rax, so just return

	pop rbp
	ret

global debug
debug:
	push rbp
	mov rbp, rsp

	mov rax, 0x00  ; debug syscall
	syscall

	pop rbp
	ret

global sleep
sleep:
	push rbp
	mov rbp, rsp

	mov rax, 0x16  ; sleep syscall
	syscall

	pop rbp
	ret

global fork
fork:
	push rbp
	mov rbp, rsp

	; address of out var should already be in rdi
	mov rax, 0x12
	syscall    ; result left in rax

	pop rbp
	ret
	

global message
message:
	push rbp
	mov rbp, rsp

	; message should already be in rdi
	mov rax, 0x14
	syscall

	pop rbp
	ret


global _start
_start:
	xor rbp, rbp	; Sentinel rbp
	push rbp
	push rbp
	mov rbp, rsp

	mov rdi, 0
	mov rsi, 0
	call main

	mov rdi, rax
	call exit
