; 0Ah == '\n'
; 0Dh == '\r'



include 'emu8086.inc'



; Prints new line in stdout.
io@println macro
    
    push ax
    
    mov al, 0Ah
    mov ah, 0Eh
    int 10h
    
    mov al, 0Dh
    mov ah, 0Eh
    int 10h
    
    pop ax
    
endm



; Prints char in stdout and advances the current cursor position.
io@print_char macro param@char
    
    push ax
    
    mov al, param@char
    mov ah, 0Eh
    int 10h
    
    pop ax
    
endm



; Prints char in stdout and adds new line.
io@println_char macro param@char
    
    push ax
    
    mov al, param@char
    mov ah, 0Eh
    int 10h
    
    mov al, 0Ah
    mov ah, 0Eh
    int 10h
    
    mov al, 0Dh
    mov ah, 0Eh
    int 10h
    
    pop ax
    
endm
 


; Prints passed const string in stdout.
io@print_const_str macro param@str
    
    local skip_dclr 
    local printed
    local next_char
    local dclr_str
    
    push ax
    push si 
    
    jmp skip_dclr
    
        dclr_str db param@str, '$'
      
        
    skip_dclr:
        lea si, dclr_str
     
        
    next_char:
        mov al, cs:[si]
        cmp al, '$'
        je printed        
        inc si
        mov ah, 0eh
        int 10h
        jmp next_char
        
        
    printed:
        pop si
        pop ax
         
endm



; Prints passed const string and new line in stdout.
io@println_const_str macro param@str
    
    local skip_dclr 
    local printed
    local next_char
    local dclr_str
    
    push ax
    push si 
    
    jmp skip_dclr
    
        dclr_str db param@str, '$'
      
        
    skip_dclr:
        lea si, dclr_str
     
        
    next_char:
        mov al, cs:[si]
        cmp al, '$'
        je printed        
        inc si
        mov ah, 0eh
        int 10h
        jmp next_char
        
        
    printed:
        mov al, 0Ah
        mov ah, 0Eh
        int 10h
    
        mov al, 0Dh
        mov ah, 0Eh
        int 10h
    
        pop si
        pop ax
         
endm  



; Allow usage of 
; io@print_str, 
; io@println_str.
io@define_print_str macro
    
    local next_char
    local printed
    local skip_proc
    
    jmp skip_proc
    
    
    
    ; Prints null terminated string at address DS:SI in stdout. 
    io@print_str proc  
    
        push ax
        push si
        
        next_char:
            mov al, ds:[si]
            cmp al, '$'
            je printed        
            inc si
            mov ah, 0eh
            int 10h
            jmp next_char
        
        
        printed:
            pop si
            pop ax
         
    ret     
    io@print_str endp
    
    
    ; Prints null terminated string at address DS:SI and new line in stdout. 
    io@println_str proc
    
        call io@print_str
        
        push ax
        
        mov al, 0Ah
        mov ah, 0Eh
        int 10h
    
        mov al, 0Dh
        mov ah, 0Eh
        int 10h
        
        pop ax
    
    ret     
    io@println_str endp
    
    
    
    skip_proc:
    
io@define_print_str endm



; Allow usage of 
; io@print_num,
; io@println_num,
; io@print_num_uns,
; io@println_num_uns.
io@define_print_num macro
    
    local skip_proc
    
    DEFINE_PRINT_NUM
    DEFINE_PRINT_NUM_UNS
    
    jmp skip_proc
    
    
    
    ; Procedure that prints a signed
    ; number in AX (not just a single digit) in stdout.
    io@print_num proc
        
        call PRINT_NUM
        
    ret
    io@print_num endp
    
    
    ; Procedure that prints a signed
    ; number in AX (not just a single digit) and new line in stdout.
    io@println_num proc
        
        call io@print_num
        
        push ax
        
        mov al, 0Ah
        mov ah, 0Eh
        int 10h
    
        mov al, 0Dh
        mov ah, 0Eh
        int 10h
        
        pop ax
        
    ret
    io@println_num endp
    
    
    ; Procedure that prints an unsigned
    ; number in AX (not just a single digit) in stdout.
    io@print_num_uns proc
        
        call PRINT_NUM_UNS
        
    ret
    io@print_num_uns endp
    
    
    ; Procedure that prints an unsigned
    ; number in AX (not just a single digit) and new line in stdout.
    io@println_num_uns proc
        
        call io@print_num_uns
        
        push ax
        
        mov al, 0Ah
        mov ah, 0Eh
        int 10h
    
        mov al, 0Dh
        mov ah, 0Eh
        int 10h
        
        pop ax
        
    ret
    io@println_num_uns endp
    
    
    
    skip_proc:
 
io@define_print_num endm  



io@define_scan_num macro
    
    local skip_proc
    
    DEFINE_SCAN_NUM
    
    jmp skip_proc
    
    
    ; Procedure that gets the multi-digit SIGNED number from the keyboard,
    ; and stores the result in CX register:
    io@scan_num proc
        
        call SCAN_NUM
        
    ret
    io@scan_num endp
    
    
    skip_proc:
    
io@define_scan_num endm
