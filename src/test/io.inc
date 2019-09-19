; Param: DS:SI - 0-terminated string address.
proc printStr@io       
        push ax
        push si
        
        nextChar@printStr:
                    mov al, [ds:si]
                    cmp al, 0
                    je printed@printStr                   
                    
                    mov ah, 0eh
                    int 10h
                    
                    inc si
                    jmp nextChar@printStr        
        
        printed@printStr:
        
        pop si
        pop ax        
        ret
endp


; Param: DS:SI - 0-terminated string address.
proc printlnStr@io
        call printStr@io
        
        push ax
    
        mov al, 0Ah
        mov ah, 0Eh
        int 10h
        
        mov al, 0Dh
        mov ah, 0Eh
        int 10h
        
        pop ax
        ret        
endp