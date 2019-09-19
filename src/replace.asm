IDEAL
MODEL small
STACK 100h

include 'system.inc'
include 'file.inc'
include 'io.inc'

DATASEG

    inputFileName@replace               db      80h dup (0)
    outputFileName@replace              db      80h dup (0)
    defaultOutputFileName@replace       db      'output', 0
    
    inputFileId@replace                 dw      ?
    outputFileId@replace                dw      ?    
    
    original@replace                    db      80h dup (0)
    replacement@replace                 db      80h dup (0)
    
    endl@ui                             db      0Ah, 0Dh, 0
    
    original@hint                       db      'Input original: ', 0
    replacement@hint                    db      'Input replacement: ', 0
    
    requiredArg@error                   db      'Existing source file param required! ', 0
    openFile@error                      db      'Error while opening file! ', 0
    closeFile@error                     db      'Error while closing file! ', 0
    
    filePathNotFound@errorCode          db      'File path not found. ', 0
    tooManyOpenedFiles@errorCode        db      'Too many opened files. ', 0
    accessDenied@errorCode              db      'Access denied. ', 0
  
    
CODESEG


proc parseArgs@replace
    
        ; Get the segment (paragraph) address of the PSP in BX.
        mov ah, 62h
        int 21h
    
        mov es, bx ; ES:0000 -> PSP of EXE file.
        mov bx, 81h ; The actual command line starts at offset BX:81h. 
    
        mov di, offset inputFileName@replace
        call getArg@parseArgs
    
        mov di, offset outputFileName@replace
        call getArg@parseArgs    
        
        ret     
    
        ; Param: DX:DI - data buffer address.
        getArg@parseArgs:
                
                skipSpace@getArg:
                        cmp [byte es:bx], ' '
                        jne loop@getArg
            
                        inc bx
                        jmp skipSpace@getArg
    
                loop@getArg:
                        cmp [byte es:bx], ' '
                        je loopEnd@getArg
                        cmp [byte es:bx], 0Ah
                        je loopEnd@getArg
                        cmp [byte es:bx], 0Dh
                        je loopEnd@getArg
                    
                        mov al, [es:bx]
                        mov [ds:di], al
                    
                        inc bx
                        inc di
                        jmp loop@getArg
        
                loopEnd@getArg:
                        mov [byte di], 0
        ret      
endp


proc validateArgs@replace
    
        cmp [byte inputFileName@replace], 0
        je callInvalidArgs@validateArgs
        jne skipInvalidArgs@validateArgs
    
        callInvalidArgs@validateArgs: 
                call invalidArgs@validateArgs        
        skipInvalidArgs@validateArgs:
    
    
        cmp [byte outputFileName@replace], 0
        je callSetDefaultOutput@validateArgs
        jne skipSetDefaultOutput@validateArgs
    
        callSetDefaultOutput@validateArgs:
                call setDefaultOutput@validateArgs        
        skipSetDefaultOutput@validateArgs:        
    
        ret
    
        setDefaultOutput@validateArgs:
                mov si, offset defaultOutputFileName@replace
                mov di, offset outputFileName@replace
                loop@setDefaultOutput:
                        cmp [byte si], 0
                        je finish@setDefaultOutput
            
                        mov al, [si]
                        mov [di], al
            
                        inc si
                        inc di
                        jmp loop@setDefaultOutput           
            
                finish@setDefaultOutput:
                        mov [byte di], 0 
        ret
    
        invalidArgs@validateArgs:
                mov si, offset requiredArg@error
                call println@io
                jmp EXIT    
        ret     
   
endp


proc openFiles@replace   
        mov al, r@mode
        mov dx, offset inputFileName@replace
        clc
        call open@file    
        jc error@openFiles        
        mov [inputFileId@replace], ax    
    
        mov dx, offset outputFileName@replace
        clc
        call create@file
        jc error@openFiles 
        mov [outputFileId@replace], ax
    
        ret    
    
        error@openFiles:
                mov si, offset openFile@error
                call println@io
        
                mov ah, 3Eh
                int 10h
                
                cmp ax, 03h
                je filePathNotFound@openFiles       
                cmp ax, 04h
                je tooManyOpenedFiles@openFiles       
                cmp ax, 05h
                je accessDenied@openFiles
                
                jmp finishError@openFiles
        
                filePathNotFound@openFiles:
                        mov si, offset filePathNotFound@errorCode
                        call println@io
                        jmp finishError@openFiles
        
                tooManyOpenedFiles@openFiles:
                        mov si, offset tooManyOpenedFiles@errorCode
                        call println@io
                        jmp finishError@openFiles
        
                accessDenied@openFiles:
                        mov si, offset accessDenied@errorCode
                        call println@io
                        jmp finishError@openFiles
        
                finishError@openFiles:
                        jmp EXIT
endp


proc closeFiles@replace
        mov bx, [inputFileId@replace]
        clc
        call close@file
        jc error@closeFiles
    
        mov bx, [outputFileId@replace]
        clc
        call close@file
        jc error@closeFiles
    
        ret
    
        error@closeFiles:
                mov si, offset closeFile@error
                call println@io 
                jmp EXIT   
endp


proc inputOriginal@replace
    
        mov si, offset original@hint
        call print@io
    
        mov di, offset original@replace
        mov dx, 80h
        call scan@io
    
        mov si, offset endl@ui
        call print@io

ret    
endp


proc inputReplacement@replace
    
        mov si, offset replacement@hint
        call print@io
    
        mov di, offset replacement@replace
        mov dx, 80h
        call scan@io
    
        mov si, offset endl@ui
        call print@io

ret    
endp


proc findNext@replace
        
        start@temp dd 4
        end@temp dd 30
        
        mov ax, [inputFileId@replace]
        mov bx, [outputFileId@replace]
        mov si, offset start@temp
        mov di, offset end@temp
        call copyBounded@file
        
ret
endp


proc replace@replace
    
    ;TODO
    
ret
endp




START:

        mov ax, @data
        mov ds, ax
        
        call parseArgs@replace
        call validateArgs@replace
    
        call openFiles@replace
    
        call inputOriginal@replace
        call inputReplacement@replace
    
        call findNext@replace
        call replace@replace
    
        call closeFiles@replace

        jmp EXIT

EXIT:
        terminate@system
        ; TODO - call subprogram.
    
END START
