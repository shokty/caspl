%macro get_var 1
	mov eax, dword[counterToDrones]
	mov ebx, 36 ;TODO
	mul ebx
	add eax, dword[drownArray] ;the location of the drown in the array 
	mov dword[currDrown], eax
	add eax, %1 ;the location ACTIVE (eax=32)
%endmacro


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


section .data
	global currDrown
	global initDrones
	global ProperingForDrownRun
	x1: dd 0
	y1: dd 0
	tmpAlfa: dd 0
	alfa: dd 0
	tmp360: dd 360
	tmpMinus360: dd -360
	espDrown: dd 0
	tmp0:  dd 10
	tmp10: dd 10
	tmp20: dd 20
	tmp60: dd 60
	tmp180: dd 180
	speed: dd 0
	currDrown: dd 0
	currSchedulerEsp: dd 0
	format_chack: db "GOT HERE!", 10, 0


section .text
	extern StepToElimination
	extern maxDistance
	extern drownArray
	extern stackDrownArray
	extern counterToDrones
	extern random
	extern LFSR
	extern maxInt
	extern tmp100
	extern disFlag

	extern realEsp
	
	extern returnAdress
	extern currEsp
	extern continueEsp
	extern resume
	extern do_resume
	extern realEsp
	extern ProperingForMayDestroy
	extern DestroyTarget

	extern printf
	extern free



initDrones: 
	call random
	fild dword [LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp100]
	fstp dword[x1]

	call random
	fild dword[LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp100]
	fstp dword[y1]

	call random
	fild dword[LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp100]
	fstp dword[speed]

	call random
	fild dword[LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp360]
	fstp dword[alfa]

;Saving ID 	
	mov ecx, dword[drownArray] 
	mov eax, dword[counterToDrones]
	mov ebx, 36 
	mul ebx
	add ecx, eax 				     
	mov edx, dword[counterToDrones]  
	mov dword[ecx], edx			;saving ID in [0,ID]

;Saving X
	add ecx, 4 					;Getting to the right loction of thet filed to save 
	mov edx, dword[x1] 			;edx <- drone X
	mov dword[ecx], edx 		;saving X in [4,x]

;Saving Y
	add ecx, 4 					;Getting to the right loction of thet filed to save
	mov edx, dword[y1] 			;edx <- drone Y
	mov dword[ecx], edx  		;saving Y in [8,Y]

;Saving alfa
	add ecx, 4 					;Getting to the right loction of thet filed to save
	mov edx, dword[alfa] 		;edx <- drone alfa
	mov dword[ecx], edx 		;saving alfa in [12,alfa]

;Saving KilledToArray
	add ecx, 4  				;Getting to the right loction of thet filed to save
	mov dword[ecx], 0  			;saving points in [16,points] (start with 0)

;Saving func ref
	add ecx, 8  			     ;Getting to the right loction of thet filed to save
	mov dword[ecx], droneRun 	 ;saving points in [24,droneRun] (pointer to droneRun so we can resume)

;Saving ActiveStatus
	add ecx, 4  			     ;Getting to the right loction of thet filed to save
	mov dword[ecx], 1  		     ;saving points in [28,drowActiveStatusnRun] (start with 1 as active in init)

;Saving speed
	add ecx, 4  			     ;Getting to the right loction of thet filed to save
	mov edx, dword[speed]		 ;edx <- drone Y
	mov dword[ecx],edx  	     ;saving points in [32,drowActiveStatusnRun] (start with a random number 1-100 in init)

;initEsp
	mov eax, dword[counterToDrones]
	mov ebx, 16000
	mul ebx 
	add eax, dword[stackDrownArray]
	add eax, 16000 		
	mov dword[realEsp], esp
	mov esp, eax
	push droneRun
	pushad
	pushfd
	sub ecx, 12      ;to write in 20
	mov dword[ecx], esp  ;put it in the array
	mov esp, dword[realEsp]
	ret

ProperingForDrownRun:
	pop dword[returnAdress]
	mov eax,dword[counterToDrones]
	mov ebx,36
	mul ebx
	add eax,dword[drownArray]
	add eax,20
	mov edx, dword[eax]
	mov dword[continueEsp],edx
	call resume

droneRun: 
	mov ecx, dword[currEsp]
	mov dword[currSchedulerEsp], ecx

	get_var 28
	cmp dword[eax],0
	je backToScheduler
	
;calcSpeed
	call random
	finit
	fild dword [LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp20]
	fisub dword[tmp10]		;chose number in range of (0,20) to get number in range of (-10,10)
	get_var 32
	fadd dword[eax]
	fst dword[speed]

	fild dword[tmp100]
	fcomip
	jc .morethen100
	fldz
	fcomip
	jc .speedcontinue 
	fild dword[tmp0]
	jmp .speedcontinue

	.morethen100:
	fstp st0
	fst dword[tmp100] 
	jmp .speedcontinue


	.speedcontinue:
	fst dword[speed]
	fst dword[eax]


	call random
	fild word[LFSR] 	;takes an integer and uploads it as a float 
	fidiv dword[maxInt]
	fimul dword[tmp60]
	fst dword[alfa]

;calcAlfa
	get_var 12
	fadd dword[eax]
	fst dword[tmpAlfa]
	fild dword[tmp360]
	fcomip
	jc sub360
	fldz
	fcomip
	jc continue 
	fiadd dword[tmp360]
	jmp continue


sub360:
	fisub dword[tmp360] 
	jmp continue


continue:
	fst dword[alfa]
	fst dword[eax]

calcDistance: 
;calc new x
	fldpi
	fmul st1
	fidiv dword[tmp180]
	fcos 		;save the result in st0
	fmul dword[speed]
	sub eax, 8 		;eax from 12 to 4 to point to x1
	fadd  dword[eax]
	fild dword[tmp100]
	fcomip 
	jc .sub100
	fldz
	fcomip
	jc calcY
	fiadd dword[tmp100]
	jmp calcY
.sub100: 
	fisub dword[tmp100]

calcY:
;calc new y1
	fst dword[x1]
	fstp dword[eax]
	finit
	fld dword[alfa]
	fldpi
	fmul st1
	fidiv dword[tmp180]
	fsin
	fmul dword[speed]
	add eax, 4    ;eax from 4 to 8 to point to y1
	fadd dword[eax]
	fild dword[tmp100]
	fcomip
	jc .sub100
	fldz
	fcomip
	jc .continue
	fiadd dword[tmp100]
	jmp .continue

.sub100: 
	fisub dword[tmp100]
.continue:
	fst dword[y1]
	fstp dword[eax]
	finit
	call ProperingForMayDestroy 	;calls mayDestroy (chack flags if destroy and stuff)
	cmp byte[disFlag], 0
	je backToScheduler
	mov ebx, 16					;getting to number of target destroy by this drone
	mov eax,dword[currDrown]
	inc dword[eax+ebx]			;the adding is happening here.
	call DestroyTarget

backToScheduler: 
	mov ecx, dword[currSchedulerEsp]
	mov dword[continueEsp], ecx
	mov dword[returnAdress], droneRun
	call resume






