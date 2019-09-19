CONST

; File mode.
r@mode = 0h
w@mode = 1h
rw@mode = 2h

; Pointer seek origin.
beg@origin = 0h
cur@origin = 1h
end@origin = 2h


CODESEG

; Create or open file.
;
; Param:
; (required) DS:DX - 0-terminated file name.
;
; Return:
; AX - file id.
;
; Error:
; CF = 1
; AX - error code: | 03h - filepath not found;
;                  | 04h - too many opened files;
;                  | 05h - access denied.
;
; Note:
; - if file already exists, it will be opened
; and its content will be cleared.
proc create@file
    
        push cx
        
        mov ah, 3Ch
        mov cx, 0
        int 21h
        
        pop cx

ret    
endp



; Open existing file.
;
; Param:
; (required) AL - file mode: r@mode, w@mode, rw@mode;
; (required) DS:DX - 0-terminated file name.
;
; Return:
; AX - file id.
;
; Error:
; CF = 1
; AX - error code: | 03h - filepath not found;
;                  | 04h - too many opened files;
;                  | 05h - access denied.
;
; Note:
; - file pointer is set to start of file;
; - file must exist.
proc open@file   
    
        mov ah, 3Dh
        int 21h
    
ret
endp



; Close file.
;
; Param:
; (required) BX - file id.
;
; Error:
; CF = 1
; AX - error code: 06h - wrong file id.
proc close@file
    
        push ax
        
        mov ah, 3Eh
        int 21h
        
        pop ax
    
ret 
endp



; Set pointer position in file.
;
; Param:
; (required) BX - file id;
; (required) AL - origin of move: beg@origin, cur@origin, end@origin;
; (required) CX:DX - offset from origin.
;
; Return:
; DX:AX - new file position in bytes from start of file.
;
; Error:
; CF = 1
; AX - error code: 06h - wrong file id.
proc seekPos@file
    
        mov ah, 42h
        int 21h
   
ret
endp



; Increment pointer position in file.
;
; Param:
; (required) BX - file id;
proc incPos@file
    
        push ax
        push cx
        push dx
        
        mov al, cur@origin
        xor cx, cx
        mov dx, 1
        call seekPos@file
        
        pop dx
        pop cx
        pop ax
    
ret
endp



; Store current pointer position in memory.
;
; Param:
; (required) BX - file id;
; (required) DS:DI - double word data buffer address.
proc storePos@file
    
        push ax
        push cx
        push dx
        
        mov al, cur@origin
        xor cx, cx
        xor dx, dx
        call seekPos@file
        mov [ds:di + 2], dx
        mov [ds:di + 0], ax
        
        pop dx
        pop cx
        pop ax 
    
ret
endp



; Restore current pointer position from memory.
;
; Param:
; (required) BX - file id;
; (required) DS:SI - double word data buffer address.
proc restorePos@file
    
        push ax
        push cx
        push dx
        
        mov al, beg@origin
        mov cx, [ds:si + 2]
        mov dx, [ds:si + 0]
        call seekPos@file
        
        pop dx
        pop cx
        pop ax
    
ret
endp



; Read from file.
;
; Param:
; (required) BX - file id;
; (required) DS:DX - data buffer address;
; (required) CX - number of bytes to read.
;
; Return:
; AX - number of bytes actually read.
;
; Error:
; CF = 1
; AX - error code: | 05h - access denied;
;                  | 06h - wrong file id.
;
; Note:
; - number of bytes actually read can be < than requested,
; that means that pointer have reached EOF.
proc read@file
    
        mov ah, 3Fh
        int 21h
   
ret
endp



; Write to file.
;
; Param:
; (required) BX - file id;
; (required) DS:DX - data buffer address;
; (required) CX - number of bytes to write.
;
; Return:
; AX - number of bytes actually written.
;
; Error:
; CF = 1
; AX - error code: 06h - wrong file id.
proc write@file
    
        mov ah, 40h
        int 21h
   
ret
endp



; Append to the end of file.
;
; Param:
; (required) BX - file id;
; (required) DS:DX - data buffer address;
; (required) CX - number of bytes to write.
;
; Return:
; AX - number of bytes actually written.
;
; Error:
; CF = 1
; AX - error code: 06h - wrong file id.
proc append@file
    
        push cx
        push dx
        
        mov al, end@origin
        xor cx, cx
        xor dx, dx
        call seekPos@file
        
        pop dx
        pop cx
        
        call write@file
    
ret
endp



; Find next matching string after current pointer position.
;
; Param:
; (required) BX - file id;
; (required) DS:DX - query address;
; (required) CX - query size;
; (required) DI - double word buffer address to store matching position.
proc findNext@file
        
        push ax
        push bx
        push cx
        push dx
        push si
        push di
        
        jmp init@findNext
        
        initPos@findNext        dd      ?
        curPos@findNext         dd      ?
        
        fileId@findNext         dw      ?
        
        buffer@findNext         db      80h dup (0)
        bufferSize@findNext     dw      ?
        
        query@findNext          db      80h dup (0)
        querySize@findNext      dw      ?
        
        returnAddr@findNext     dw      ?
         
         
        init@findNext: 
                
        mov [fileId@findNext], bx
        mov [returnAddr@findNext], di
        mov [querySize@findNext], cx
    
        mov si, dx
        mov di, offset buffer@findNext                               
        initQueryLoop@findNext:                
                cmp [byte si], 0
                je initQueryLoopEnd@findNext
                    
                mov al, [si]
                mov [di], al
                    
                inc si
                inc di
                jmp initQueryLoop@findNext  
                                   
        initQueryLoopEnd@findNext:
                mov [byte di], 0
        
        mov bx, [fileId@findNext]        
        mov di, offset initPos@findNext
        call storePos@file        
        
        loop@findNext:        
                mov bx, [fileId@findNext]
                mov di, offset curPos@findNext
                call storePos@file
                
                mov bx, [fileId@findNext]
                mov dx, offset buffer@findNext
                mov cx, [querySize@findNext]
                call read@file                
                mov [bufferSize@findNext], ax
                
                mov bx, [fileId@findNext]
                mov si, offset curPos@findNext
                call restorePos@file
                
                mov ax, [bufferSize@findNext]
                cmp ax, [querySize@findNext]
                jl notFound@findNext
                
                mov si, offset query@findNext
                mov di, offset buffer@findNext
                mov cx, [querySize@findNext]
                cmpBufferLoop@findNext:
                        cmp cx, 0
                        je found@findNext
                        
                        mov al, [si]
                        cmp al, [di]
                        jne cmpBufferLoopEnd@findNext
                        
                        inc si
                        inc di
                        dec cx
                        jmp cmpBufferLoop@findNext                       
                        
                cmpBufferLoopEnd@findNext:
                
                mov bx, [fileId@findNext]
                mov al, cur@origin
                xor cx, cx
                mov dx, 1
                call seekPos@file
                
                jmp loop@findNext
                
                
        notFound@findNext:
                mov di, [returnAddr@findNext]
                mov [word di + 2], 0FFh
                mov [word di + 0], -1
                jmp exit@findNext
                
        found@findNext:
                mov di, [word returnAddr@findNext]
                mov dx, [word curPos@findNext + 2] 
                mov ax, [word curPos@findNext + 0]
                mov [di + 0], ax
                mov [di + 2], dx
                jmp exit@findNext    
                
        exit@findNext:
        
        mov bx, [fileId@findNext]        
        mov si, offset initPos@findNext
        call restorePos@file
        
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop ax
ret
endp



; Copy bounded data from one file to another.
;
; (required) AX - source file id.
; (required) BX - destination file id.
; (required) DS:SI - double word start pos address.
; (required) DS:DI - double word end pos address.
;
; Note:
; - delta between start and end positions
; must be less than word for proper work.
proc copyBounded@file
        push ax
        push bx
        push cx
        push dx
        push si
        push di
        
        jmp init@copyBounded
       
        buffer@copyBounded              db      0Fh dup (0)
        bufferSize@copyBounded          dw      0Fh       
        
        srcFileId@copyBounded           dw      ?
        destFileId@copyBounded          dw      ?
        
        startPos@copyBounded            dd      ?
        endPos@copyBounded              dd      ?
        
        delta@copyBounded               dw      ?
        
        init@copyBounded:
        
        mov [srcFileId@copyBounded], ax
        mov [destFileId@copyBounded], bx
        
        mov ax, [si + 2]
        mov [word startPos@copyBounded + 2], ax
        mov ax, [si + 0]
        mov [word startPos@copyBounded + 0], ax
        
        mov ax, [di + 2]
        mov [word endPos@copyBounded + 2], ax
        mov ax, [di + 0]
        mov [word endPos@copyBounded + 0], ax
                
        mov bx, [srcFileId@copyBounded]
        mov si, offset startPos@copyBounded
        mov cx, [si + 2]
        mov dx, [si + 0]
        mov al, beg@origin
        call seekPos@file
        
        loop@copyBounded:
                mov bx, [srcFileId@copyBounded]
                mov al, cur@origin
                xor cx, cx
                xor dx, dx
                call seekPos@file
                
                mov bx, [word endPos@copyBounded + 0]
                sub bx, ax
                mov [delta@copyBounded], bx
               
                cmp bx, 0
                jle exit@copyBounded
                              
                cmp bx, [bufferSize@copyBounded]
                jl lastCopy@copyBounded
                
                mov bx, [srcFileId@copyBounded]
                mov cx, [bufferSize@copyBounded]
                mov dx, offset buffer@copyBounded
                call read@file
                
                mov bx, [destFileId@copyBounded]
                call append@file  
                
                jmp loop@copyBounded    
                
        lastCopy@copyBounded:
                mov ax, [delta@copyBounded]
                mov [bufferSize@copyBounded], ax
                
                mov bx, [srcFileId@copyBounded]
                mov cx, [bufferSize@copyBounded]
                mov dx, offset buffer@copyBounded
                call read@file
                
                mov bx, [destFileId@copyBounded]
                call append@file        
        
        exit@copyBounded:
        
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop ax
ret
endp




















; Find next matching string after current pointer position.
;
; Param:
; (required) BX - file id;
; (required) DS:DX - data buffer address;
; (required) CX - data buffer size;
; (required) SI - double word buffer address to store start position;
; (required) DI - double word buffer address to store end position.
;proc findNext@file
;        
;        jmp init@findNext
;        
;        fileId@findNext         dw      ?
;        query@findNext          db      80h dup (0)
;        querySize@findNext      dw      ?
;        startPosAddr@findNext   dw      ?
;        endPosAddr@findNext     dw      ?
;        buffer@findNext         db      80h dup (0)
;        bufferSize@findNext     dw      ?
;        initPos@findNext        dd      ?
;        curPos@findNext         dd      ?
;        
;        init@findNext:
;                push ax
;                push bx
;                push cx
;                push dx
;                push si
;                push di
;                
;                call initFile@findNext
;                call initQuery@findNext
;                call initAddr@findNext
;                call initBuffer@findNext
;                call storeInitPos@findNext
;                
;        loop@findNext:
;                call storeCurPos@findNext
;                call readToBuffer@findNext
;                call restoreCurPos@findNext
;                
;                call cmpBufferSizeWithQuery@findNext
;                jl notFound@findNext
;                
;                call cmpBufferContentWithQuery@findNext  
;                je found@findNext
;                
;                call incFilePos@findNext
;                jmp loop@findNext             
;        
;        notFound@findNext:
;                call loadEmptyToAddr@findNext
;                jmp exit@findNext
;                
;        found@findNext:
;                call loadCurPosToAddr@findNext
;                jmp exit@findNext                        
;        
;        exit@findNext:
;                call restoreInitPos@findNext
;                
;                pop di
;                pop si
;                pop dx
;                pop cx
;                pop bx
;                pop ax
;                ret      
;               
;        
;        initFile@findNext:
;                mov [fileId@findNext], bx
;        ret
;        
;        initQuery@findNext:
;                mov [querySize@findNext], cx
;        
;                push ax
;                push si
;                push di
;            
;                mov si, dx
;                mov di, offset buffer@findNext
;                                       
;                loop@initQuery:                
;                        cmp [byte si], 0
;                        je loopEnd@initQuery
;                            
;                        mov al, [si]
;                        mov [di], al
;                            
;                        inc si
;                        inc di
;                        jmp loop@initQuery             
;                
;                loopEnd@initQuery:
;                        mov [byte di], 0
;                
;                pop di
;                pop si
;                pop ax
;        ret
;        
;        initAddr@findNext:
;                mov [startPosAddr@findNext], si
;                mov [endPosAddr@findNext], di 
;        ret
;        
;        initBuffer@findNext:
;                mov [bufferSize@findNext], cx
;        ret
;        
;        storeCurPos@findNext:
;                push bx
;                push di
;                
;                mov di, offset curPos@findNext
;                call storePos@file
;                
;                pop di
;                pop bx
;        ret    
;    
;        restoreCurPos@findNext:
;                push bx
;                push si
;                
;                mov si, offset curPos@findNext
;                call restorePos@file
;                
;                pop si
;                pop bx
;        ret
;      
;        storeInitPos@findNext:    
;                push bx
;                push di
;                
;                mov di, offset initPos@findNext
;                call storePos@file
;                
;                pop di
;                pop bx    
;        ret
;      
;        restoreInitPos@findNext: 
;                push bx
;                push si
;                
;                mov si, offset initPos@findNext
;                call restorePos@file
;                
;                pop si
;                pop bx
;        ret
;        
;        readToBuffer@findNext:
;                push ax
;                push bx
;                push cx
;                push dx
;                
;                mov bx, [fileId@findNext]
;                mov cx, [querySize@findNext]
;                mov dx, offset buffer@findNext
;                call read@file
;                mov [bufferSize@findNext], ax
;                
;                mov bx, [word buffer@findNext]
;                add bx, [bufferSize@findNext]
;                mov [byte bx], 0
;                
;                pop dx
;                pop cx
;                pop bx
;                pop ax
;        ret
;        
;        cmpBufferSizeWithQuery@findNext:
;                push ax
;                
;                mov ax, [bufferSize@findNext]
;                cmp ax, [querySize@findNext]
;                
;                pop ax
;        ret
;                
;        cmpBufferContentWithQuery@findNext:
;                push ax
;                push si
;                push di
;                
;                mov si, offset buffer@findNext
;                mov di, offset query@findNext
;                
;                loop@cmpBufferWithQuery:
;                        mov al, [si]
;                        cmp al, [di]
;                        jne notEqual@cmpBufferContentWithQuery
;                
;                        mov al, [si]
;                        cmp al, 0
;                        je equal@cmpBufferContentWithQuery
;                        
;                        inc si
;                        inc di
;                        jmp loop@cmpBufferWithQuery
;                
;                equal@cmpBufferContentWithQuery:
;                        xor ax, ax
;                        cmp ax, 0
;                        jmp finish@cmpBufferContentWithQuery
;                
;                notEqual@cmpBufferContentWithQuery:
;                        xor ax, ax
;                        cmp ax, 1
;                        jmp finish@cmpBufferContentWithQuery
;                
;                finish@cmpBufferContentWithQuery:
;                
;                pop di
;                pop si  
;                pop ax
;        ret        
;       
;        loadEmptyToAddr@findNext:
;                push si
;                push di
;                
;                mov si, [startPosAddr@findNext]
;                mov di, [endPosAddr@findNext]
;                
;                mov [word si + 2], 0FFh
;                mov [word si + 0], -1 
;                mov [word di + 2], 0FFh
;                mov [word di + 0], -1
;                
;                pop di
;                pop si
;        ret
;        
;        loadCurPosToAddr@findNext:
;                push ax
;                push bx
;                push si
;                push di
;                
;                mov bx, [word curPos@findNext]
;                mov si, [startPosAddr@findNext]
;                mov di, [endPosAddr@findNext]
;                
;                mov ax, [word bx + 2]
;                mov [word si + 2], ax
;                mov ax, [word bx + 0]
;                mov [word si + 0], ax
;                
;                mov ax, [word bx + 2]
;                mov [word di + 2], ax
;                mov ax, [word bx + 0]
;                add ax, [querySize@findNext]
;                mov [word di + 0], ax
;                
;                pop di
;                pop si
;                pop bx
;                pop ax
;        ret
;        
;        incFilePos@findNext:
;                push bx
;                
;                mov bx, [fileId@findNext]
;                call incPos@file
;                
;                pop bx
;        ret
;
;endp
