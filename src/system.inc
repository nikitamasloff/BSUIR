CODESEG

macro terminate@system
    
        mov ax, 4C00h
        int 21h
    
endm
    
