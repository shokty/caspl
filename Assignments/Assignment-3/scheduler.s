%macro Print 1    
    push ebp
    mov ebp, esp
    pushad

    push %1
    call printf
    add esp, 4

    popad
    mov esp, ebp
    pop ebp
%endmacro


section .bss
	global disFlag
	global espTarget
	global espPrinter
	global schesuler
	global returnAdress
	global currEsp
	global continueEsp
	global resume
	global do_resume

	disFlag: resb 1
	espScheduler:resb 4

section .data
	format_elleminate: db "drown %d was elmnated!! ", 10, 0
	format_winner: db "Drone id %d: I am a winner", 10, 0
	returnAdress:dd 0
	currEsp: dd 0
	continueEsp: dd 0
	counterToPrint: dd 0
	counterToEliminat: dd 0
	espTarget: dd 0
	espPrinter: dd 0
	espEliminition: dd 0
	idOfLessKillDrown: dd 0
	foundFirstActiveDrwon: dd 0
	NumberOfsmallestKills: dd 0
	LastOneStendingId: dd 0
	numberOfElimnaitionRunned: dd 1


section .text 
	extern printf
	extern stackScheduler
	extern realEsp
	extern counterToDrones
	extern stepsToPrint
	extern StepToElimination
	extern ProperingForDrownRun
	extern numberOfDrones
	extern PrinterPrint
	extern drownArray
	extern free
	extern initTarget
	extern stackDrownArray

schesuler: 
	mov dword[realEsp] , esp
	mov esp, stackScheduler
	add esp, 16000
	mov dword[counterToDrones], 0
	mov dword[counterToPrint], 0

schedulerMainLoop:

	call ProperingForDrownRun
	inc dword[counterToDrones]  	;Witch drone i working on right now
	inc dword[counterToPrint]		;chack if enef round as passed for me to print
	inc dword[counterToEliminat]	;chack if enef round as passed for me to do elimination

	mov ebx,dword[numberOfDrones]
	cmp dword[counterToDrones], ebx
	jne .chackPrint
	mov dword[counterToDrones], 0 			;make it round robin like %

	.chackPrint:
		mov edx, dword[stepsToPrint] 
		cmp dword[counterToPrint], edx
		jne .chackElimnate
		pushad
		call PrinterPrint
		popad
		mov dword[counterToPrint], 0

	.chackElimnate:
		mov edx, dword[StepToElimination] 
		cmp dword[counterToEliminat], edx
		jne .continue2
		pushad
		mov eax,dword[numberOfElimnaitionRunned]
		inc eax
		mov dword[numberOfElimnaitionRunned],eax
		call Elimination
		popad
		mov dword[counterToEliminat], 0

	.continue2:
		mov edx, dword[numberOfDrones]
		cmp dword[numberOfElimnaitionRunned],edx	
		jne schedulerMainLoop
		call PrintWinnerAndQuit

PrintWinnerAndQuit: 
	push ebp
    mov ebp, esp
	pushad
	mov eax, dword[drownArray]
	mov ecx, 0
	.FindLast:
	add eax,28
	cmp dword[eax],1
	je .PrintLastAndFinish
	add eax,8 ;to be in next starting point of drown
	mov ecx, dword[LastOneStendingId]
	inc ecx
	mov dword[LastOneStendingId],ecx
	jmp .FindLast

	.PrintLastAndFinish:
	sub eax,20 
	push dword [LastOneStendingId]
	push format_winner
	call printf
	add esp, 8
	mov esp, dword[realEsp]
	push dword[drownArray]
	call free
	push dword[stackDrownArray]
	call free
	mov eax, 1
	mov ebx, 0
	int 0x80



Elimination:
    push ebp
    mov ebp, esp
    pushad
	mov eax, dword[drownArray]
	mov ecx, 0
	mov dword[idOfLessKillDrown],0
	mov dword[foundFirstActiveDrwon],0
	mov dword[NumberOfsmallestKills],0

	.my_loop: 
	cmp dword[eax+28],0
	je .nextDrown
	cmp dword[foundFirstActiveDrwon],0
	jne .chackSmallest
	mov dword[foundFirstActiveDrwon],1
	mov dword[idOfLessKillDrown],ecx
	mov ebx,dword[eax+16]
	mov dword[NumberOfsmallestKills],ebx
	jmp .nextDrown
	.chackSmallest:
	;sub eax, 12
	mov ebx,dword[eax+16]
	cmp dword[NumberOfsmallestKills],ebx
	jng .nextDrown   ;jemp greater or equal
	mov dword[idOfLessKillDrown],ecx
	mov dword[NumberOfsmallestKills],ebx
	.nextDrown:
	add eax, 36
	inc ecx
	cmp dword[numberOfDrones],ecx
	jne .my_loop


	.ElimnateOne:
	mov eax, dword[idOfLessKillDrown]
	mov ebx,36
	mul ebx
	add eax,dword[drownArray]
	add eax, 28 
	mov dword[eax],0
	mov eax, dword[idOfLessKillDrown]
	push eax
	push format_elleminate
	call printf
	add esp, 8

	popad
    mov esp, ebp
    pop ebp
    ret


resume: 
	add esp, 4
	push dword[returnAdress]
	pushad 
	pushfd
	mov dword[currEsp], esp

do_resume: 
	mov esp, dword[continueEsp]
	popfd
	popad
	ret

