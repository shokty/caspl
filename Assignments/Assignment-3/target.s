
section .bss
	global mayDystroy
	global ProperingForMayDestroy
	global DestroyTarget
	global initTarget

section .data
	x1: dd 0
	y1: dd 0
	tmp180:dd 180
	tmpsDistance: dd 0
	swap: dd 0



section .text
	extern beta
	extern random
	extern LFSR
	extern maxInt
	extern tmp100
	extern stackTarget
	extern targetsArray
	
	extern disFlag
	
	extern currDrown 
	extern maxDistance
	extern resume
	extern do_resume
	extern returnAdress
	extern continueEsp
	extern realEsp
	extern currEsp
	
	extern espTarget


initTarget: 
	call random
	fild dword [LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp100]
	fstp dword[targetsArray]

	call random
	fild dword[LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp100]
	mov eax, 4
	fstp dword[targetsArray+eax] 	

	mov dword[realEsp], esp
	mov esp, stackTarget
	add esp, 16000
	push mayDystroy
	pushad
	pushfd
	mov dword[espTarget], esp
	mov esp, dword[realEsp]
	ret

ProperingForMayDestroy:
	pop dword[returnAdress]
	mov edx, dword[espTarget]
	mov dword[continueEsp],edx
	call resume

mayDystroy: 
	;calc squre 
	mov eax, targetsArray 		
	fld dword[eax]
	mov ebx, dword[currDrown]
	add ebx,4
	fsub dword[ebx]		    	;st0 <- x2 - x1
	fmul st0, st0               ;st0^2
	fstp dword[tmpsDistance]
	add eax, 4
	fld dword[eax]           
	add ebx, 4
	fsub dword[ebx] 		    ;st0 <- y2 - y1
	fmul st0, st0
	fadd dword[tmpsDistance] 	;(x2-x1)^2 + (y2-y1)^2
	fsqrt                       ;sqr ( (x2-x1)^2 + (y2-y1)^2 )
	fild dword[maxDistance]
	clc
	fcomi st0 , st1             ;chack tmpsDistance <= maxDistance
	jnc moveOn 
	mov dword[disFlag], 0
	jmp endOfFunc
moveOn:
	mov dword[disFlag], 1
	jmp endOfFunc

endOfFunc:
	mov dword[returnAdress], mayDystroy
	mov ebx, dword[currEsp]
	mov dword[continueEsp], ebx
	call resume

DestroyTarget:
	push ebp
    mov ebp, esp
	pushad
;get new x
	call random
	fild dword [LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp100]
	fstp dword[targetsArray]
;get new y
	call random
	fild dword[LFSR]
	fidiv dword[maxInt]
	fimul dword[tmp100]
	mov eax, 4
	fstp dword[targetsArray+eax] ;in the y cell of the array 

	popad
    mov esp, ebp
    pop ebp
	ret