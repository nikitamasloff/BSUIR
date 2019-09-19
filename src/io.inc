CONST

newLine@io              equ     0Ah ; '\n'
carriageReturn@io       equ     0Dh ; '\r'

CODESEG

; Print null-terminated string to STDIO.
;
; Param:
; DS:SI - data buffer address.
proc print@io    
    
        push ax
        push si

        nextChar@print:		
		mov	al, [si]
		cmp	al, 0
		jz printed@print
		inc	si
		mov	ah, 0eh ; Teletype function.
		int	10h
		jmp	nextChar@print
		
		
        printed@print:

        pop si
        pop	ax

ret
endp



; Print null-terminated string and new line to STDIO.
;
; Param:
; DS:SI - data buffer address.
proc println@io    
    
            call print@io
            
            mov ah, 0Eh
            mov al, newLine@io    
            int 10h
            mov al, carriageReturn@io
            int 10h  

ret
endp



; Scan null-terminated string from STDIO.
;
; Param:
; DS:DI - data buffer address;
; DX - data buffer size.
proc scan@io
    
        push ax
        push cx
        push di
        push dx

        xor cx, cx ; Char counter.

        cmp dx, 1 ; Buffer too small?
        jbe emptyBuffer@scan            

        dec dx ; Reserve space for last zero.
    

        ;============================
        ; Loop to get and processes key presses:

        waitForKey@scan:

        mov ah, 0 ; Get pressed key.
        int 16h

        cmp al, 13 ; 'Return' pressed?
        jz exit@scan

        cmp al, 8 ; 'Backspace' pressed?
        jne addToBuffer@scan
        jcxz waitForKey@scan ; Nothing to remove!
        dec cx
        dec di
    
        ; Print backspace.
        push ax
        mov al, 8
        mov ah, 0eh
        int 10h     
        pop ax
    
        ; Print ' ' (clear position).
        push ax
        mov al, ' '
        mov ah, 0eh
        int 10h     
        pop ax
    
        ; Print backspace again.
        push ax
        mov al, 8
        mov ah, 0eh
        int 10h     
        pop ax
    
        jmp waitForKey@scan

        addToBuffer@scan:

                cmp cx, dx ; Buffer is full?
                jae waitForKey@scan ; If so wait for 'backspace' or 'return'...

                mov [di], al
                inc di
                inc cx
        
                ; Print the key:
                mov ah, 0eh
                int 10h

        jmp waitForKey@scan
        ;============================


        exit@scan:
    
        mov [byte di], 0 ; Terminate by null:

        emptyBuffer@scan:
    
                pop dx
                pop di
                pop cx
                pop ax
    
ret
endp
