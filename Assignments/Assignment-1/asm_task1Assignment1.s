section .data			; we define (global) uninitialized variables in .bss section
	number: db "%d" , 10, 0			; enough to store integer in [-2,147,483,648 (-2^31) : 2,147,483,647 (2^31-1)]

section .text
	global assFunc
	extern printf
	extern c_checkValidity

assFunc:
	
	push ebp                       ;save Base pointer orginal value
	mov ebp, esp                   ;ebs = start esp = current.
	pushad                         ;			

	push dword [ebp+12]            ;push the secound int
	push dword [ebp+8]             ;push first arg

    call c_checkValidity

	mov ebx,[ebp+8]                ;ebx is the first arg
	cmp eax,1
	je sub

	add ebx,[ebp+12]
	jmp end
	
sub:
    sub ebx,[ebp+12]

end:
	push ebx			          ; call printf with 2 arguments -  
	push number
	call printf
	add esp, 16		              ; clean up stack after call
	popad			
	mov esp, ebp	
	pop ebp
	ret

