%macro Next_Link 1               ;transfer you to the next link
    pushad
    mov eax,0
    mov eax,[%1]
    mov eax, dword[eax+1]
    mov dword [%1],eax
    popad
%endmacro


%macro Print_link 1							; args: 1 - pointer to link
	pushad
    mov ecx , 0
    mov eax , [%1]
    mov cl , byte [eax]
    push ecx
    push format_Link
    push dword [stderr]
    call fprintf
	add esp, 12
	popad
%endmacro

%macro Print_list 1							; args: 1 - pointer to first link of the list
	pushad
    mov eax,[%1]
    mov dword [Printlink] ,eax
    Print_link Printlink
    %%start:
    mov eax,[Printlink]
    cmp dword [eax+1] , 0
	je %%end
    Next_Link Printlink
    Print_link Printlink
    jmp %%start
    %%end:
	popad
%endmacro

%macro Create_Link 1
    pushfd
    pushad
	push 1                  ;calloc calling covension
    push 5                  ; the first one is to number and the other 4 is for the pointer
    call calloc           	; TODO: Change it to calloc
    add esp, 8            	
    mov [%1], eax  	
    popad
    popfd
%endmacro

%macro Print_If_Dubugg 1 
    pushfd
    pushad
    mov ecx ,dword [debug_mode]
    cmp ecx,0
    je %%end_dubugg
    Print_list %1 
    %%end_dubugg:
    popad
    popfd
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

%macro Print_Number 1    
    push ebp
    mov ebp, esp
    pushad

    push %1
    push format_int
    call printf
    add esp, 8

    popad
    mov esp, ebp
    pop ebp
%endmacro


%macro getUserInput 0-1 0
    push ebp
    mov ebp, esp
    pushad
    
    push dword [stdin]
    push dword 80                 ;we said no more then 80 char
    push input 
    call fgets
    add esp, 12
	
    popad
    mov esp, ebp
    pop ebp
%endmacro


%macro endOfFunc 0-1 0
    popad
    mov esp, ebp
    pop ebp
%endmacro

%macro ByteToHex 1							; args: 1 - byte to hex string to 
	cmp %1,10
	jnl %%alpbit
	add %1,48
	jmp %%inserting
  %%alpbit:
	add %1,55
  %%inserting:
%endmacro


section	.rodata                   		; we define (global) read-only variables in .rodata section
    format_string: db "%s", 10, 0 		; format string
    format_Link: db "Value: %X", 10, 0
	format_int: db "the number is: %X", 10, 0 			; format int
	format_calc: db "calc: ", 0 	;format of calc: 
    format_chacking_stuff: db "WE GOT HERE YAY!" , 10 , 0             ; Format to chack
	format_overflow: db "Error: Operand Stack Overflow", 10 , 0
    format_underflow: db "Error: Insufficient Number of Arguments on Stack", 10, 0
    numOfOp: db "%X", 10, 0   ;TODO: CHACK WTH IS THAT
    


section .bss            	            ; we define (global) uninitialized variables in .bss section
    input: resb 80                      ; input of user
    output: resb 80  
	myLink: resb 5
    freeLink: resb 5
    Printlink: resb 4
	firstLink: resb 5
	currLink: resb 5 
    TestLink: resb 5
    even: resd 1          
	BeforeCurrLink: resb 5 
	nextLinkInput: resb 4              ; next number to add to link 
	op1Link: resd 5                                         ; link operand 1 (example: plus need 2 operands)
    op2Link: resd 5                                         ; link operand 2 
    tempLink: resd 5
    New_Link: resb 5
    stack_pointer: resb 4


section .data
	;stackSequize dword 5
    Plus_Sum: dd 0
    Howmany1: dd 0
    Howmany2: dd 0
    carry: db 0
    argc: dd 0               
    opSum: dd 0
	curr_vars: dd 0
	input_Counter: dd 0                  ; the letter of the inout that is currently worked on
	Stack_Counter: dd 0                  ;how mand element`s in the stuck (number`s \ oprands)
    Stack_Size: dd 0                     ; size of stack (changes with input)
    int_size: dd 4
	debug_mode: dd 0					;if 1 debug if 0 dont debbug 
	exit_flag: dd 0



section .text
  align 16
  global main
  extern printf
  extern fprintf
  extern fflush
  extern malloc
  extern calloc
  extern free
  extern gets
  extern getchar
  extern fgets
  extern stdin
  extern stderr


main:
    push ebp
    mov ebp, esp
    pushad

	mov    ecx ,dword [ebp + 8]             ; ecx = argc
    mov    esi,esp                ; esi = argv
	mov    dword[argc],ecx       ; put the number of arguments into argc
    mov dword[Stack_Size],5

	cmp ecx,1         ; we can reccive 2 inputs, number of stack, dubeg or not
	je .locate_space
	cmp ecx, 2
	je .dubegOrStack
	mov dword[debug_mode],1    ;dubug mode activted 
    mov eax,dword [ebp + 12]
    mov ebx, dword[eax+4]
	push  ebx ; push the second argumnet (cotains the number of stack space )
	call StackSizeMaker ; changes Stack_Size to the correct size
	add esp, 4
	jmp .locate_space
    .dubegOrStack:
    mov eax,dword [ebp + 12]
    mov ebx, dword[eax+4]
	mov dl, byte[ebx]
	
	cmp dl,45
	je .makeDubug
	push ebx
	call StackSizeMaker
	add esp, 4
	jmp .locate_space
	.makeDubug:
	mov dword[debug_mode],1
	jmp .locate_space

    .locate_space: ; this is after run on all argements
     mov eax , dword[debug_mode]
	 push dword[Stack_Size]
	 push 4
	 call calloc
     mov [stack_pointer],eax
    
     ;mov eax,dword [edx + ebx*4-4]
	 ;mov stack_pointer,eax
	 add esp,8

    call myCalc
    
    push eax
    push numOfOp
    call printf
    add esp, 8

    popad  
    mov esp, ebp
    pop ebp
    ret



StackSizeMaker:
    push ebp
    mov ebp, esp
	pushad
	mov eax,0
    mov ebx,0
	;mov [debug_mode],1
    mov ecx, [ebp+8]
	cmp byte [ecx+1],0 ;checkes if two letters 
	je .oneLetterHex
	mov bl, byte [ecx]
	inc ecx ;adds one for second letter to be in next postion 
	cmp bl, 64
	jge .aphbitLargeLetter
	sub bl, 48
	jmp .beforeEnd
  	.aphbitLargeLetter:
	sub bl,55
    .beforeEnd:	
    shl ebx,4
	add al,bl
	
	.oneLetterHex:
	mov bl, byte [ecx]
	cmp bl, 64
	jge .aphbitSmallLetter
	sub bl, 48
    add al,bl
	jmp .end
  	.aphbitSmallLetter:
	sub bl,55	
	add al,bl
    .end:
	mov dword[Stack_Size],eax
	popad
    mov esp, ebp
    pop ebp
    ret




myCalc:
    push ebp               ; set the base pointer
    mov ebp, esp           ;  ^^  ^^  ^^   ^^

    .loop:        
        Print format_calc         		 ; print "calc: " to the screen
        call getUserInputTry      		 ; save user input from stdin to input varibale
        ;mov eax,dword[Stack_Counter]
		cmp byte [input], 'q' 	 ; chack`s that the input is 'q'
    	je .removeAllInStack           ; if it is q we gonna get back to main
        call ChackOp             ; if we are here we know that it is an op
    jmp .loop

    .removeAllInStack:
    cmp dword[Stack_Counter],0
    je .backtomain
    call popOpAndFree
    jmp .removeAllInStack
    .backtomain:
    push dword[stack_pointer]
    call free
    add esp, 4
    mov eax, [opSum] ;Chack what is opSum
    mov esp, ebp
    pop ebp
    ret


ChackOp: ;TODO: Change it and finish this
    push ebp
    mov ebp, esp
    pushad
    
    .PlusOp:
    cmp byte [input], '+'
    jne .PopAndPrintOp
    add dword [opSum], 1
    call ExecutePlus
    jmp .EndChackOp

    .PopAndPrintOp:
    cmp byte [input], 'p'
    jne .DupOp
    add dword [opSum], 1
    call ExecutePopAndPrint
    jmp .EndChackOp

    .DupOp:
    cmp byte [input], 'd'
    jne .OrOp
    add dword [opSum], 1
    call ExecuteDupOp
    jmp .EndChackOp


    .OrOp:
    cmp byte [input], '|'
    jne .AndOp
    add dword [opSum], 1
    call ExecuteOr
    jmp .EndChackOp

    .AndOp:
    cmp byte [input], '&'
    jne .NumberOfHex
    add dword [opSum], 1
    call ExecuteAnd
    jmp .EndChackOp


    .NumberOfHex:
    cmp byte [input], 'n'
    jne .AddNuberOp
    add dword [opSum], 1
    call ExecuteNumberOfHex
    jmp .EndChackOp

    .AddNuberOp:
    call inputNumber
    .EndChackOp:
    popad
    mov esp, ebp
    pop ebp
    ret



ExecutePlus:
	push ebp
    mov ebp, esp

    cmp dword [Stack_Counter], 2  ;Chack if there is enef op in Our Stack to make this OP (+) happend 
    jge .DoPlusOp                 ;if we have enef we gonna do it!

    Print format_underflow        ;this is if we have 1 opparnd or less so we gonna print an Error to the screen.
    jmp .MybeError


    .DoPlusOp:
    ;poping one
    mov ebx, dword[Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-4]
    mov [op1Link],eax

    ; popping two
    mov ebx, dword[Stack_Counter]
    mov edx,[stack_pointer]
    mov eax, [edx + ebx*4-8]
    mov [op2Link], eax

    .MakeThemEven1:
    mov eax,[op1Link]            ;put in eax the first number link
    cmp dword [eax+1] , 0        ;chack if we have next
	je .FinishEven1                   ;if we do we jump to the end.
    
    mov ecx,[op2Link]            ;put in eax the first number link
    cmp dword [ecx+1] , 0        ;chack if we have next
	jne .next1                    ;if we do we jump to the end.

    Create_Link New_Link
    mov esi, [New_Link]
    mov byte [esi] , 0
    mov dword [ecx+1] , esi

    .next1:
    Next_Link op1Link             ;goto next link
    Next_Link op2Link             ;goto next link
    jmp .MakeThemEven1

    .FinishEven1:
    ;poping one
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-8]
    mov [op1Link],eax

    ; popping two
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax, [edx + ebx*4-4]
    mov [op2Link], eax

    .MakeThemEven2:
    mov eax,[op1Link]            ;put in eax the first number link
    cmp dword [eax+1] , 0        ;chack if we have next
	je .FinishEven2                   ;if we do we jump to the end.
    
    mov ecx,[op2Link]            ;put in eax the first number link
    cmp dword [ecx+1] , 0        ;chack if we have next
	jne .next2                    ;if we do we jump to the end.

    Create_Link New_Link
    mov esi, [New_Link]
    mov byte [esi] , 0
    mov dword [ecx+1] , esi

    .next2:
    Next_Link op1Link             ;goto next link
    Next_Link op2Link             ;goto next link
    jmp .MakeThemEven2
    

    


    .FinishEven2:
    ;poping one
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-4]
    mov [op1Link],eax

    ;popping two
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax, [edx + ebx*4-8]
    mov [op2Link], eax
    Create_Link tempLink


    mov eax , [tempLink]
    mov [TestLink], eax

    .Loopy:
    mov eax,[op1Link]            ;put in eax the first number link
    mov ecx,[op2Link]            ;put in ecx the secound number link 

    mov ebx , 0                  ;clean garbage
    mov bl , byte[eax]           ;bl <--- op1Link   : move the op1Link number value to bl

    mov eax , 0                  ;clean garbage
    mov al , byte[ecx]           ;bl <--- op2Link   : move the op1Link number value to bl


    add ebx , eax
    add bl , byte [carry]
    mov byte [carry] ,0 
    cmp ebx, 0xFF
    jle .continue
    mov byte [carry],1

    .continue:
    mov ecx,[tempLink]            ;put in eax the curry new link on the new link list link
    mov byte [ecx] , bl           ;chack if we have next

    mov eax,[op1Link]            ;put in eax the first number link
    cmp dword [eax+1] , 0        ;chack if we have next
	je .end                      ;if we do we jump to the end.

    Create_Link New_Link
    mov esi, [New_Link]
    mov dword [ecx+1] , esi


    Next_Link tempLink            ;goto next link
    Next_Link op1Link             ;goto next link
    Next_Link op2Link             ;goto next link
    jmp .Loopy 

    .end:
    cmp byte[carry] , 0
    je .EndPlus
    Create_Link New_Link
    mov eax,[New_Link]
    mov byte [eax] , 1
    mov ebx , [tempLink]
    mov dword [ebx+1] , eax
    Next_Link tempLink

    .EndPlus:
    mov eax,[tempLink]            ;put in eax the curry new link on the new link list link
    mov dword [eax+1] , 0
    
    call popOpAndFree
    call popOpAndFree
    mov eax , [TestLink]
    push eax
    call pushOp
    .MybeError:
    Print_If_Dubugg TestLink
    mov esp, ebp
    pop ebp
    ret
    
ExecuteDupOp:   
	push ebp
    mov ebp, esp

    cmp dword [Stack_Counter], 1  ;Chack if there is enef op in Our Stack to make this OP (Dup) happend 
    jge .DoDupOp                 ;if we have enef we gonna do it!

    Print format_underflow        ;this is if we have 1 opparnd or less so we gonna print an Error to the screen.
    jmp .MybeError


    .DoDupOp:
    ;poping one
    mov ebx, dword[Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-4]
    mov [op1Link],eax

    Create_Link tempLink
    mov eax , [tempLink]
    mov [TestLink], eax

    .Loopy:
    mov eax,[op1Link]            ;put in eax the first number link

    mov ebx , 0                  ;clean garbage
    mov bl , byte[eax]           ;bl <--- op1Link   : move the op1Link number value to bl

    mov edx , [tempLink]
    mov byte [edx] , bl

    mov eax,[op1Link]            ;put in eax the first number link
    cmp dword [eax+1] , 0        ;chack if we have next
	je .EndDup                      ;if we do we jump to the end.

    Create_Link New_Link
    mov esi , [New_Link]
    mov dword [edx+1] , esi


    Next_Link tempLink
    Next_Link op1Link
    jmp .Loopy
    .EndDup:

    mov eax , [tempLink]
    mov dword [eax +1],0
    mov eax ,[TestLink]
    push eax
    call pushOp

    .MybeError:
    Print_If_Dubugg TestLink
    mov esp, ebp
    pop ebp
    ret



inputNumber:
    push ebp
    mov ebp, esp
    pushad

    ; create first link
    mov eax ,dword [Stack_Size]
    cmp dword[Stack_Counter],eax
    jne .continue
    push format_overflow
    call printf
    add esp, 4

    jmp .ending

    .continue:
	push 1                  ;calloc calling covension
    push 5                  ; the first one is to number and the other 4 is for the pointer
    call calloc           	; TODO: Change it to calloc
    add esp, 8            	
    mov [currLink], eax  	
    mov [BeforeCurrLink], eax
	mov ebx, [BeforeCurrLink]
    mov dword[ebx+1], 0 ;point to zero / end of number last link
    
    ;call setInputEvenOrOdd
	mov dword[input_Counter], 0
	
	mov ecx,0
	.RemoveStartingZero: ;remove all 0 in beging input 
    mov bl, byte[input+ecx]
	cmp byte[input+ecx], 48
	jne .counterAdjuting
	inc ecx
	jmp .RemoveStartingZero
	
    ; have to be after setInputEvenOrOdd
   ; mov dword [isTheStartZero], 1 
   ; mov [counter], byte 0
    .counterAdjuting:
    mov dword[input_Counter], ecx
    ; if (uneven number of letters)
    call CheckEvenInput
    cmp dword[even], 0
    je .loop
    call getOneLetter
    jmp .afterExtractingNum

    ; getLetter
    

    .loop:
		;mov dword[input_Counter], ecx
        call getTwoLetters ; gets two letters to nextLinkInput and moves conter 
        .afterExtractingNum:
		mov ecx, dword[input_Counter]   ;recives the next letter of input
		cmp byte[input+ecx],10   ; if pointing to \n
		je .endOfFunc
		mov ebx,[currLink]
		mov [BeforeCurrLink], ebx
		push 1                  ;calloc calling covension
    	push 5                  ; the first one is to number and the other 4 is for the pointer
    	call calloc           	; TODO: Change it to calloc
    	add esp, 8
		mov [currLink], eax 
        ;mov edx,
		mov dword[eax+1], ebx   ;make the link point to the link chain 
		jmp .loop

            
    .endOfFunc:
    Print_If_Dubugg currLink
    
    .notDebug:
    mov eax, [currLink]
    push eax
    call pushOp
    add esp, 4
    .ending:
    popad
    mov esp, ebp
    pop ebp
    ret

pushOp:
    push ebp
    mov ebp, esp
    pushad                                          ; DO NOT DELETE THIS LINE!
    
    mov eax ,dword [Stack_Size]

    cmp dword[Stack_Counter],eax
    jne .continue
    
    push format_overflow
    call printf
    add esp, 4
    mov eax, [ebp + 8]
    .loop:
    cmp dword [eax+1],0
    je .endofloop
    mov ebx, dword [eax+1]
    mov [freeLink],ebx
    push eax
    call free
    add esp, 4
    mov eax,[freeLink]
    jmp .loop

    .endofloop:
    push eax
    call free
    add esp, 4
    sub [Stack_Counter], dword 1
    
    jmp .pushEnd
           
    .continue:
        mov eax, [ebp + 8] 
        mov ebx, dword[Stack_Counter]
        mov edx,[stack_pointer]
        mov [edx + ebx*4], eax
        add dword[Stack_Counter],  1
    
    .pushEnd:                                       
        popad                                       ; DO NOT DELETE THIS LINE!
        mov esp, ebp
        pop ebp
        ret




getTwoLetters:
	push ebp
    mov ebp, esp
	pushad
	mov ebx,0
	mov ecx,0
	mov eax,0
	mov word[nextLinkInput],0
    mov ecx, dword[input_Counter]
	mov bh, byte[input+ecx] 
	cmp bh,10    ;checkes if first letter is /n  letters 
	je .end

    mov bl,byte [input+ecx+1]
	;cmp byte [input+ecx+1],10   ; checkes if second letter is /n  letters 
    cmp bl,10
	je .oneLetterHex
	inc ecx            ;adds one for second letter to be in next postion 
	cmp bh, 64
	jge .aphbitLargeLetter
	sub bh, 48
	shl bh, 4
    add al,bh
	jmp .oneLetterHex
  	.aphbitLargeLetter:
	sub bh, 55
	shl bh, 4	
	add al,bh
	jmp .oneLetterHex
	
	.oneLetterHex:
	mov bl, byte [input+ecx]
	inc ecx
	cmp bl, 64
	jge .aphbitSmallLetter
	sub bl, 48
    add al,bl
	jmp .finishedExtracting
  	.aphbitSmallLetter:
	sub bl,55
	add al,bl
    .finishedExtracting:
    mov ebx,[currLink]
	mov byte [ebx],al ; 
	mov dword[input_Counter],ecx

	.end:
	popad
    mov esp, ebp
    pop ebp
    ret
        ;ADDED THIS PART TO MY CODE
getOneLetter:
    push ebp
    mov ebp, esp
	pushad
	mov ebx,0
	mov ecx,0
	mov eax,0
	mov word[nextLinkInput],0
    mov ecx, dword[input_Counter]
	mov bl, byte[input+ecx] 
	cmp bl,10    ;checkes if first letter is /n  letters 
	je .end
	
	.oneLetterHex:
	inc ecx
	cmp bl, 64
	jge .aphbitSmallLetter
	sub bl, 48
	jmp .finishedExtracting
  	.aphbitSmallLetter:
	sub bl,55
    .finishedExtracting:
    mov eax,[currLink]
	mov byte [eax],bl ; 
	mov dword[input_Counter],ecx

	.end:
	popad
    mov esp, ebp
    pop ebp
    ret

CheckEvenInput:
    push ebp
    mov ebp, esp
	pushad

    mov ecx, dword[input_Counter]
    mov ebx,0
    .loop:
    ;mov al, byte[input+ecx]
    cmp byte[input+ecx],10
    je .endOfLoop
    inc ebx
    inc ecx
    cmp byte[input+ecx],10
    je .endOfLoop
    sub ebx,1
    inc ecx
    jmp .loop
    .endOfLoop:
    mov [even],ebx
    popad
    mov esp, ebp
    pop ebp
    ret


	
ExecutePopAndPrint:
    push ebp
    mov ebp, esp
	pushad

    cmp dword[Stack_Counter], 0
    je .Pop

    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-4]
    mov dword [currLink],eax
    mov ebx,0
    mov ecx,80
    mov byte[output+ecx],0
    .loop:
    mov eax, dword [currLink]
    mov ebx,0
    cmp dword[eax+1],0
    je .lastlink       ;checks if last link
    ;sub ecx, 1
    mov bl, byte[eax]
    and bl, 0FH
    ByteToHex bl ;turns bl ro string hex
    mov byte[output+ecx-1],bl
    mov bl, byte[eax]
    shr ebx,4
    ByteToHex bl
    ;sub ecx, 1
    mov byte[output+ecx-2], bl
    mov ebx, dword[eax+1]
    mov [currLink],ebx
    sub ecx, 2
    jmp .loop

    .lastlink:
    mov eax, dword [currLink]
    mov ebx,0
    mov bl, byte[eax]
    and bl, 0FH
    ByteToHex bl ;turns bl ro string hex
    sub ecx,1
    mov byte[output+ecx],bl
    mov bl, byte[eax]
    shr ebx,4
    cmp bl,0
    je .removeUnwantedZero
    ByteToHex bl
    sub ecx,1
    mov byte[output+ecx],bl
    .removeUnwantedZero:
    cmp byte[output+ecx],48
    jne .printingOutput
    cmp ecx,79
    je .printingOutput
    inc ecx
    jmp .removeUnwantedZero
    .printingOutput:
    add ecx,output    ;send pointer of output+ecx s
    push ecx
    push format_string
    call printf
    add esp,8
    
    .Pop:
    call popOpAndFree
    popad
    mov esp, ebp
    pop ebp
    ret
	



popOpAndFree:
    push ebp
    mov ebp, esp
    pushad

    cmp [Stack_Counter], dword 0
    jne .continue
    
    push format_underflow
    call printf
    add esp, 4
    
    jmp .popEnd
    
    .continue:
         mov ebx, [Stack_Counter]
         mov edx,[stack_pointer]
        mov  eax,dword [edx + ebx*4-4]
        .loop:

        cmp dword [eax+1],0
        je .endofloop
        mov ebx, dword [eax+1]
        mov [myLink],ebx
        push eax
        call free
        add esp, 4
        mov eax,[myLink]
        jmp .loop

        .endofloop:
        push eax
        call free
        add esp, 4
        sub [Stack_Counter], dword 1

    .popEnd:
        popad
        mov esp, ebp
        pop ebp
        ret


ExecuteNumberOfHex:
    push ebp
    mov ebp, esp
	pushad
    cmp [Stack_Counter], dword 0
    jne .work
    push format_underflow
    call printf
    add esp, 4
    jmp .endOfFun

    .work:
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-4]
    mov dword [currLink],eax
    push 1                  ;calloc calling covension
    push 5                  ; the first one is to number and the other 4 is for the pointer
    call calloc           	; TODO: Change it to calloc
    add esp, 8            	
    mov [myLink], eax
    mov dword[eax+1],0
    mov ebx,0
    .loop:
    mov eax, dword[currLink]
    cmp dword[eax+1],0
    je .lastLink
    add ebx, 2
    mov ecx, dword[eax+1]
    mov [currLink], ecx
    jmp .loop

    .lastLink:
    add ebx, 1
    mov eax, dword[currLink]
    mov ecx,0
    mov cl, byte[eax]
    shr cl, 4 
    cmp cl, 0
    je .insert
    inc ebx
    .insert:
    mov ecx,[myLink]
    mov byte[ecx],bl
    call popOpAndFree
    Print_If_Dubugg myLink
    push ecx
    call pushOp
    add esp, 4

    .endOfFun:
    popad
    mov esp, ebp
    pop ebp
    ret


ExecuteOr:
    push ebp
    mov ebp, esp
	pushad

    cmp [Stack_Counter], dword 2
    jge .work      ;jump less =jle
    push format_underflow
    call printf
    add esp, 4
    jmp .endOffun

    .work:
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-4]
    mov [op1Link], eax
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-8]
    mov [op2Link], eax
    push 1                  ;calloc calling covension
    push 5                  ; the first one is to number and the other 4 is for the pointer
    call calloc           	; TODO: Change it to calloc
    add esp, 8            	
    mov [firstLink], eax
    mov [currLink], eax
    mov ecx, 0
    mov ebx,0
    .loopTwoLinks:
    mov eax, [op1Link]
    cmp dword[eax+1], 0
    je .lastLinkOp1

    mov eax, [op2Link]
    cmp dword[eax+1], 0
    je .lastLinkOp2

    mov cl, byte[eax]
    mov eax, [op1Link] 
    mov ch, byte[eax]
    or cl,ch
    mov ebx, [currLink]
    mov byte[ebx],cl
    push 1                  ;calloc calling covension
    push 5                  ; the first one is to number and the other 4 is for the pointer
    call calloc           	; TODO: Change it to calloc
    add esp, 8    
    mov dword[ebx+1],eax      
    mov [currLink],eax      ;currlink=currlink->next
    mov eax, [op1Link] 
    mov ebx,dword[eax+1]
    mov [op1Link],ebx   ;op1Link=op1link->next
    mov eax, [op2Link] 
    mov ebx,dword[eax+1]
    mov [op2Link],ebx   ;op2Link=op2link->next
    jmp .loopTwoLinks


    .lastLinkOp1:
    mov eax, [op1Link]
    mov ebx, [op2Link]
    mov [op1Link],ebx
    mov [op2Link],eax
    .lastLinkOp2:
    mov eax,[op2Link]
    mov ecx,0
    mov cl, byte[eax]
    mov eax,[op1Link]
    mov ch, byte[eax]
    or cl,ch
    mov ebx, [currLink]
    mov byte[ebx],cl
    mov eax,[op1Link]
    cmp dword[eax+1],0
    je .finishRun
    .DeepCopy:
    mov ecx,0
    mov eax, [op1Link] 
    mov ebx,dword[eax+1]
    mov [op1Link],ebx   ;op1Link=op1link->next
    cmp ebx,0
    je .finishRun
    push 1                  ;calloc calling covension
    push 5                  ; the first one is to number and the other 4 is for the pointer
    call calloc           	; TODO: Change it to calloc
    add esp, 8  
    mov ebx,[op1Link]
    mov cl,byte[ebx]
    mov ebx, [currLink]  
    mov dword[ebx+1],eax      
    mov [currLink],eax
    mov byte[eax],cl
    jmp .DeepCopy



    .OnlyOplink1:
    push 1                  ;calloc calling covension
    push 5                  ; the first one is to number and the other 4 is for the pointer
    call calloc           	; TODO: Change it to calloc
    add esp, 8  
    mov ebx, [currLink]  
    mov dword[ebx+1],eax      
    mov [currLink],eax
    mov eax, [op1Link] 
    mov ebx,dword[eax+1]
    mov [op1Link],ebx   ;op1Link=op1link->next
    mov cl, byte[ebx]
    mov eax, [currLink]
    mov byte[eax],cl
    mov eax,[op1Link]
    cmp dword [op1Link],0
    je .finishRun
    jmp .OnlyOplink1

    .finishRun:
    mov ebx, [currLink]
    mov dword[ebx+1],0
    call popOpAndFree
    call popOpAndFree
    mov ebx,[firstLink]
    Print_If_Dubugg firstLink
    push ebx
    call pushOp
    add esp, 4

    .endOffun:
    popad
    mov esp, ebp
    pop ebp
    ret

    ;poping one
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-4]
    mov [op1Link],eax


ExecuteAnd:
    push ebp
    mov ebp, esp

    cmp dword [Stack_Counter], 2  ;Chack if there is enef op in Our Stack to make this OP (+) happend 
    jge .DoAndOp                 ;if we have enef we gonna do it!

    Print format_underflow        ;this is if we have 1 opparnd or less so we gonna print an Error to the screen.
    jmp .MybeError


    .DoAndOp:
    ;poping one
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax,dword [edx + ebx*4-4]
    mov [op1Link],eax

    ;popping two
    mov ebx, [Stack_Counter]
    mov edx,[stack_pointer]
    mov eax, [edx + ebx*4-8]
    mov [op2Link], eax

    Create_Link tempLink
    mov eax , [tempLink]
    mov [TestLink], eax

    .Loopy:
    mov eax,[op1Link]            ;put in eax the first number link
    mov ecx,[op2Link]            ;put in ecx the secound number link 

    mov ebx , 0                  ;clean garbage
    mov bl , byte[eax]           ;bl <--- op1Link   : move the op1Link number value to bl

    mov eax , 0                  ;clean garbage
    mov al , byte[ecx]           ;bl <--- op2Link   : move the op1Link number value to bl

    and bl , al

    Create_Link New_Link
    mov esi,[New_Link]
    mov edx, [tempLink]
    mov byte [edx] , bl
    mov dword[edx+1],esi

    mov eax,[op1Link]            ;put in eax the first number link
    mov ecx,[op2Link]            ;put in ecx the secound number link 
    cmp dword [eax+1] ,0
    je .end
    cmp byte [ecx+1] ,0
    je .end
    Next_Link tempLink            ;goto next link
    Next_Link op1Link             ;goto next link
    Next_Link op2Link             ;goto next link
    jmp .Loopy
    

    .end:
    call popOpAndFree
    call popOpAndFree
    mov eax , [TestLink]
    push eax
    call pushOp

    .MybeError:
    mov esp, ebp
    pop ebp
    ret


getUserInputTry:
    push ebp
    mov ebp, esp
    pushad
    
    push dword [stdin]
    push dword 80
    push input
    call fgets
    add esp, 12

    .endOfFunc:
    popad
    mov esp, ebp
    pop ebp
    ret