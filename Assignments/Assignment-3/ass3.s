%macro Get_From_User 2
    mov ebx,dword[esi+ %1]               ;ebx=*argv[3]
    pushad
    push %2
    push format_decimal
    push ebx
    call sscanf                         ;K
    add esp,12
    popad
%endmacro

%macro Get_From_UserLF 2
    mov ebx,dword[esi+ %1]               ;ebx=*argv[3]
    pushad
    push %2
    push format_decimal
    push ebx
    call sscanf                         ;K
    add esp,12
    mov ebx, dword[%2]
    sub ebx,1
    mov dword[%2], ebx 
    popad
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

section .bss
    global main
	global numberOfDrones
	global StepToElimination
	global stepsToPrint
	global maxDistance
	global drownArray
	global stackDrownArray
	global counterToDrones
	global random
	global LFSR
	global maxInt
	global tmp100
	global stackTarget
	global targetsArray
    global stackScheduler
    global realEsp
    global stackPrinter
    global stackEliminition
	stackTarget: resb 16000 
	stackPrinter: resb 16000
    stackEliminition: resb 16000
	stackScheduler: resb 16000
	tmp1: resb 1
    realEsp:resb 4

section .data
	numberOfDrones: dd 0
	StepToElimination: dd 0
	stepsToPrint: dd 0
	maxDistance: dd 0 ;TODO: change to flot
	LFSR: dd 0
	counterToDrones: dd 0
	maxInt: dd 65535
	tmp100: dd 100

	drownArray:dd 0
	stackDrownArray: dd 0
	targetsArray: dd 0.0, 0.0

	format_decimal: db "%d"
    format_error: db "Not enogh arguments", 10, 0

section .text
    extern initPrinter
	extern initDrones
	extern initTarget
	extern printf
    extern sscanf
    extern malloc
    extern calloc
    extern initPrinter
    extern schesuler

main:
	push ebp
    mov ebp,esp
    sub esp,8
    mov edi, dword[ebp+8]         ;edi=argc
    cmp edi,6   
    jl error                      ;Printing Error if there is not enef args.

    mov esi,dword[ebp+12]         ;esi=argv

    Get_From_User 4,numberOfDrones       ; Put N    , Args[1]
    Get_From_User 8,StepToElimination    ; Put R    , Args[2]
    Get_From_User 12,stepsToPrint        ; Put K    , Args[3]
    Get_From_User 16,maxDistance         ; Put D    , Args[4]
    Get_From_UserLF 20,LFSR                ; Put LFSR , Args[5]


startMain:
	mov eax,dword[numberOfDrones]
	push eax
    push 36
	call calloc          ;TODO: change to calloc
	mov dword[drownArray], eax
	add esp, 8          

;****************Init all*******************
	mov eax,dword[numberOfDrones]
	push eax
    push 16000
	call calloc          ;TODO: change to calloc
	mov dword[stackDrownArray], eax
	add esp, 8

	mov ecx, dword[numberOfDrones]
	mov dword[counterToDrones], 0
	my_loop: 
		pushad
		call initDrones
		inc dword[counterToDrones]
		popad
		loop my_loop, ecx


	call initTarget		     ;Init the target with random x and y
;****************Finish init*******************
    call schesuler           ;here we start the infit loop

	                                        
    RANDOM_BASE EQU   0x2D   

    random_bit: 
        mov al, RANDOM_BASE    
        xor al, [LFSR]         
        jpe result_ok          
        stc                    
    result_ok:
        rcr word [LFSR], 1     
        ret
    random:
        mov ecx, 16
    next_bit:
        call    random_bit
        loop    next_bit, ecx
        ret

error:
    Print format_error
    mov eax, 1
    mov ebx ,0
    int 0x80