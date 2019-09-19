CONST

offset@args = 81h

DATASEG

isInitialized@args      db      0

CODESEG

proc init@args        
        push ax
        push bx
        
        ; Get the segment (paragraph) address of the PSP in BX.
        mov ah, 62h                     
        int 21h
        
        ; ES:0000 -> PSP of EXE file.
        mov es, bx  
                            
        mov [isInitialized@args], 1     
        
        pop bx
        pop ax
ret         
endp


proc initIfRequired@args
        cmp [byte isInitialized@args], 0
        jne exit@initIfRequired
        
        call init@args
        
        exit@initIfRequired:
        ret
endp


; Param: BX - current position of pointer in argument line.
; Return: BX - position of pointer after skipping spaces.
proc skipSpace@args
        call initIfRequired@args
        
        loop@skipSpace:
                cmp [byte es:bx], ' '
                jne exit@skipSpace
    
                inc bx
                jmp loop@skipSpace 
            
        exit@skipSpace:  
ret
endp


; Param: BX - current position of pointer in argument line.
; Return: BX - position of pointer after skipping word.
proc skipWord@args
        call initIfRequired@args
        
        loop@skipWord:
                cmp [byte ptr es:bx], ' '
                je exit@skipWord
                cmp [byte ptr es:bx], 0Ah
                je exit@skipWord
                cmp [byte ptr es:bx], 0Dh
                je exit@skipWord
                
                inc bx
                jmp loop@skipWord
                
        exit@skipWord:
ret
endp


; Return: CX - amount of arguments
proc getAmount@args
        call initIfRequired@args
        
        push bx
        
        mov bx, offset@args 
        xor cx, cx 
        
        call skipSpace@args         
             
        loop@getAmount:
                cmp [byte es:bx], 0
                je exit@getAmount
                cmp [byte es:bx], 0Ah
                je exit@getAmount
                cmp [byte es:bx], 0Dh
                je exit@getAmount
        
                call skipWord@args
                call skipSpace@args
                
                inc cx
                jmp loop@getAmount
                
        exit@getAmount:
        
        pop bx             
        ret                
endp


; Param: CX - ordinal of argument;
;        DS:DI - data buffer address.
; Error: CF = 1 - no argument by such number.
proc getByOrdinal@args
        call initIfRequired@args
        
        push bx
        push cx
        push di
                
        clc        
        mov bx, offset@args        
        call skipSpace@args
        
        skipLoop@getByOrdinal:
                cmp cx, 0
                je skipLoopEnd@getByOrdinal
                
                call skipWord@args
                call skipSpace@args
                
                dec cx
                jmp skipLoop@getbyOrdinal
                
        skipLoopEnd@getByOrdinal:
        
        cmp [byte es:bx], ' '
        je noArg@getByOrdinal
        cmp [byte es:bx], 0
        je noArg@getByOrdinal
        cmp [byte es:bx], 0Ah
        je noArg@getByOrdinal
        cmp [byte es:bx], 0Dh
        je noArg@getByOrdinal
        
        noArg@getByOrdinal:
                stc
                jmp exit@getByOrdinal
        
        parseLoop@getByOrdinal:
                cmp [byte es:bx], ' '
                je parseLoopEnd@getByOrdinal
                cmp [byte es:bx], 0
                je parseLoopEnd@getByOrdinal
                cmp [byte es:bx], 0Ah
                je parseLoopEnd@getByOrdinal
                cmp [byte es:bx], 0Dh
                je parseLoopEnd@getByOrdinal
        
                mov al, [byte es:bx]
                mov [byte ds:di], al
                
                inc bx
                inc di
                jmp parseLoop@getByOrdinal
        
        parseLoopEnd@getByOrdinal:
                mov [byte ds:di], 0
                
        exit@getByOrdinal:
        
        pop di
        pop cx
        pop bx
        ret     
endp
