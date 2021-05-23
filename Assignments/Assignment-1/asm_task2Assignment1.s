section	.rodata			; we define (global) read-only variables in .rodata section
	format_string: db "%s", 10, 0	; format string

section .bss			; we define (global) uninitialized variables in .bss section
	an: resb 12		; enough to store integer in [-2,147,483,648 (-2^31) : 2,147,483,647 (2^31-1)]
	
section .data 
	number: dd 0
section .text
	global convertor
	extern printf

convertor:
	push ebp
	mov ebp, esp	
	pushad
	mov ecx,0
clean_an:
	mov byte[an+ecx],0
	add ecx,1
	cmp	ecx,11
	jne clean_an
	mov dword[number],0

	mov ecx, dword [ebp+8]	; get function argument (pointer to string)
	; your code comes here...


	mov edx,0
	mov byte[an],0
loop:
	cmp word [ecx],10
	je end
	cmp word[ecx],0
	je end                       ;finish cond

	mov bh,[ecx]                 ;we save in bh register (small register) the curr char (start with the biggest -> smalls)
	sub bh, 48                   ;make it real number (-0 in ACII)
	mov eax,10                    
	mul dword[number]            ;Multply our curr number by 10 so we can add the next
	mov dword[number],eax        ;eax is where the Multply sel is store.
	add [number],bh              ;add the last char to the number
	add ecx,1                    ;this the the counter to see what char i am so i add 1 to mover forword
	jmp loop                     ;do it all over agine

end:
	mov eax,dword[number]
next_hex:
	mov bl,al
	and bl, 0FH
	shr eax,4
	cmp bl,10
	jnl alpbit
	add bl,48
	jmp inserting
alpbit:
	add bl,55
inserting:
	mov ecx,0
insert_loop:
	mov bh,byte[an+ecx]
	mov byte[an+ecx],bl
	mov bl,bh
	add ecx,1
	cmp	ecx, 11
	jne insert_loop
	cmp eax,0
	jne next_hex
 	;mov edx, dword[number]        ;save the selotion on edx 
	
	
	push an			; call printf with 2 arguments -  
	push format_string	; pointer to str and pointer to format string
	call printf
	add esp, 8		; clean up stack after call

	popad			
	mov esp, ebp	
	pop ebp
	ret
