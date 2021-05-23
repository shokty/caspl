%macro push_var_flot 1
	sub esp, 8			
	fld dword[eax+%1]
	fstp qword[esp]
%endmacro


section .data
		global PrinterPrint
		format_decimal: db "%d"
		format_float: db "targetX: %.2f           targetY: %.2f", 10, 0
		format_printer: db "id:%d, x: %.2f, y: %.2f, alfa: %.2f ,speed: %.2f ,points: %d", 10, 0

section .text
	extern printf
	extern drownArray
	extern targetsArray
	extern stackPrinter
	extern resume
	extern do_resume
	extern realEsp
	extern returnAdress
	extern continueEsp
	extern numberOfDrones
	extern currEsp

	extern espPrinter
	

PrinterPrint: 
    push ebp
    mov ebp, esp
    pushad

	pushad
	finit 
	sub esp, 8
	mov eax, targetsArray
	add eax, 4
	fld dword[eax]
	fstp qword[esp]
	sub esp, 8
	sub eax, 4
	fld dword[eax]
	fstp qword[esp]
	push format_float
	call printf
	add esp, 20
	popad

mov eax, dword[drownArray]
mov ecx, dword[numberOfDrones]
my_loop: 
	pushad
	finit
	cmp dword[eax+28],0
	je .skip

	add eax, 16 		;points to num of killed targets 
	push dword[eax]
	sub eax ,16

	push_var_flot 32 	; pushing speed
	push_var_flot 12    ; pushing alpha
	push_var_flot 8		;pishing y
	push_var_flot 4		;pishing x
	push dword[eax]		;pushing id
	push format_printer
	call printf
	add esp, 44
	.skip:
	popad
	add eax, 36
	loop my_loop, ecx

 end: 
	popad
    mov esp, ebp
    pop ebp
    ret