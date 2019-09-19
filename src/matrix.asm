include 'system.inc'
include 'io.inc'

.MODEL small
.STACK 100h

jmp START

        
.DATA

    matrix@rows                 equ         5
    matrix@columns              equ         6
    matrix@item_size            equ         2    
    matrix@matrix               dw          matrix@rows * matrix@columns dup(?)
    matrix@rows_products        dw          matrix@rows dup(?)
    matrix@max_rows_products    db          matrix@rows dup(?)
    matrix@min_value            equ         -32768
    matrix@marker_max           equ         2
    matrix@marker_not_max       equ         1
    matrix@marker_overflow      equ         0
    
    
    ui@init_matrix              equ         'Initialize matrix $'
    ui@print_matrix             equ         'Printing matrix $'
    ui@calc_rows_products       equ         'Calculating rows products... $'
    ui@print_rows_products      equ         'Printing rows products $'
    ui@find_max_rows_products   equ         'Searching for maximum rows products... $'
    ui@print_max_rows_products  equ         'Printing maximum rows products $'
    ui@element                  equ         'Element $'
    ui@row                      equ         'Row $'
    ui@max                      equ         'Maximum $'
    ui@not_max                  equ         'Not maximum $'
    ui@overflow                 equ         'Overflow $'
    ui@separator                equ         ' : $'
    ui@space                    equ         ' '
    ui@tab                      equ         9h
   
   
   
.CODE 

io@define_print_num
io@define_scan_num

matrix@define_init
matrix@define_print
matrix@define_calc_rows_products
matrix@define_print_rows_products
matrix@define_find_max_rows_products
matrix@define_print_max_rows_products


matrix@define_init macro       
    local skip_proc
    local rows_loop, rows_loop_fnlz
    local columns_loop, columns_loop_fnlz
        
    jmp skip_proc 
        
        
    matrix@init proc
        
        io@println_const_str ui@init_matrix
        
        push bx
        push cx
        push si
        
        ; rows_loop: for(cx = 0; cx < matrix@rows; cx++).
        xor cx, cx 
        rows_loop:
        
            ; if(cx >= matrix@rows) goto rows_loop_fnlz.
            cmp cx, matrix@rows
            jge rows_loop_fnlz
            
            ; Remember (rows_loop) current `cx` value in `bx`
            ; and push in the stack.
            mov bx, cx
            push cx
            
            ; columns_loop: for(cx = 0; cx < matrix@columns; cx++).
            xor cx, cx
            columns_loop:
            
                ; For-loop condition.
                ; if(cx >= matrix@columns) goto columns_loop_fnlz.
                cmp cx, matrix@columns
                jge columns_loop_fnlz
                
                ; Remember (columns_loop) current `cx` value in `si`.     
                mov si, cx
                
                    ; Show element hint for user.
                    ui@print_element_at bx, si
                    
                    ; Normalize `bx` and `si` to the matrix item size.
                    util@normalize matrix@item_size
                    ; Can change `bx` and `si` 
                    ; since they are persisted in stack 
                    ; by `util@normalize` macro.
                    util@mul_bx matrix@columns
                
                    ; Scan signed number to `cx` register 
                    ; and insert it in matrix cell.
                    push cx                
                    call io@scan_num
                    io@println          
                    mov matrix@matrix[bx][si], cx
                    pop cx
                    
                    ; Restore raw `bx` and `si`.
                    util@denormalize
                
                ; For-loop increment and looping.
                inc cx
                jmp columns_loop
                
            columns_loop_fnlz:                
            ; Restore (rows_loop) `cx` value from the stack.    
            pop cx
            ; For-loop increment and looping.
            inc cx
            jmp rows_loop
            
        rows_loop_fnlz:
        pop si
        pop cx
        pop bx
        
        io@println
            
    ret
    matrix@init endp 
        
        
    skip_proc:
        
matrix@define_init endm
                  
    
    
matrix@define_print macro
    local skip_proc
    local rows_loop, rows_loop_fnlz
    local columns_loop, columns_loop_fnlz
        
    jmp skip_proc
        
        
    matrix@print proc
        
        io@println_const_str ui@print_matrix    
        
        push bx
        push cx
        push si
        
        ; rows_loop: for(cx = 0; cx < matrix@rows; cx++).
        xor cx, cx 
        rows_loop:
        
            ; if(cx >= matrix@rows) goto rows_loop_fnlz.
            cmp cx, matrix@rows
            jge rows_loop_fnlz
            
            ; Remember (rows_loop) current `cx` value in `bx`
            ; and push in the stack.
            mov bx, cx
            push cx
            
            ; columns_loop: for(cx = 0; cx < matrix@columns; cx++).
            xor cx, cx
            columns_loop:
            
                ; For-loop condition.
                ; if(cx >= matrix@columns) goto columns_loop_fnlz.
                cmp cx, matrix@columns
                jge columns_loop_fnlz
                
                ; Remember (columns_loop) current `cx` value in `si`.     
                mov si, cx
                    
                    ; Normalize `bx` and `si` to the matrix item size.
                    util@normalize matrix@item_size
                    ; Can change `bx` and `si` 
                    ; since they are persisted in stack 
                    ; by `util@normalize` macro.
                    util@mul_bx matrix@columns
                
                    ; Put signed number from matrix cell in `ax` register 
                    ; and print it.
                    push ax   
                    mov ax, matrix@matrix[bx][si]             
                    call io@print_num
                    io@print_char ui@space
                    io@print_char ui@tab
                    pop ax
                    
                    ; Restore raw `bx` and `si`.
                    util@denormalize
                
                ; For-loop increment and looping.
                inc cx
                jmp columns_loop
                
            columns_loop_fnlz: 
            io@println               
            ; Restore (rows_loop) `cx` value from the stack.    
            pop cx
            ; For-loop increment and looping.
            inc cx
            jmp rows_loop
            
        rows_loop_fnlz:
        pop si
        pop cx
        pop bx
        
        io@println
        
    ret
    matrix@print endp
        
        
    skip_proc:
        
matrix@define_print endm



matrix@define_calc_rows_products macro    
    local skip_proc
    local rows_loop, rows_loop_fnlz
    local columns_loop, columns_loop_continue, columns_loop_fnlz
    local check_overflow, overflow, no_overflow
    
    jmp skip_proc
    
    
    matrix@calc_rows_products proc
        
        io@println_const_str ui@calc_rows_products   
        
        push ax
        push bx
        push cx
        push si
        
        ; rows_loop: for(cx = 0; cx < matrix@rows; cx++).
        xor cx, cx 
        rows_loop:
        
            ; if(cx >= matrix@rows) goto rows_loop_fnlz.
            cmp cx, matrix@rows
            jge rows_loop_fnlz
            
            ; Remember (rows_loop) current `cx` value in `bx`
            ; and push in the stack.
            mov bx, cx
            push cx
            
            ; Prepare buffer for product calculation.
            mov ax, 1
            
            ; columns_loop: for(cx = 0; cx < matrix@columns; cx++).
            xor cx, cx
            columns_loop:
            
                ; For-loop condition.
                ; if(cx >= matrix@columns) goto columns_loop_fnlz.
                cmp cx, matrix@columns
                jge columns_loop_fnlz
                
                ; Remember (columns_loop) current `cx` value in `si`.     
                mov si, cx
                    
                    ; Normalize `bx` and `si` to the matrix item size.
                    util@normalize matrix@item_size
                    ; Can change `bx` and `si` 
                    ; since they are persisted in stack 
                    ; by `util@normalize` macro.
                    util@mul_bx matrix@columns                
                    
                    ; Unsigned multiplication.                    
                    imul matrix@matrix[bx][si] ; ax = ax * `matrix@matrix[bx][si]`.
                    
                    ; Restore raw `bx` and `si`.
                    util@denormalize
                    
                    jmp check_overflow
                    
                    check_overflow:
                        jo overflow
                        jmp no_overflow                                        
                   
                    overflow:
                        mov ax, matrix@min_value
                        jmp columns_loop_fnlz
                        
                    no_overflow:
                        jmp columns_loop_continue
                
                ; For-loop increment and looping.
                columns_loop_continue:
                inc cx
                jmp columns_loop
                
            columns_loop_fnlz:               
            ; Restore (rows_loop) `cx` value from the stack.    
            pop cx
            
            ; Write accumulated `ax` value
            ; in `matrix@rows_products` cell,
            ; using normalized `bx` register
            ; for memory addressing.
            mov bx, cx
            util@normalize matrix@item_size            
            mov matrix@rows_products[bx], ax
            util@denormalize
            
            ; For-loop increment and looping.
            inc cx
            jmp rows_loop
            
        rows_loop_fnlz:
        pop si
        pop cx
        pop bx
        pop ax
        
        io@println
        
    ret
    matrix@calc_rows_products endp
    
    
    skip_proc:
    
matrix@define_calc_rows_products endm



matrix@define_print_rows_products macro
    
    local skip_proc
    local rows_products_loop, rows_products_loop_fnlz
    
    jmp skip_proc
    
    
    matrix@print_rows_products proc
       
        io@println_const_str ui@print_rows_products   
        
        push bx
        push cx
        
        ; rows_products_loop: for(cx = 0; cx < matrix@rows; cx++).
        xor cx, cx
        rows_products_loop:
        
            ; For-loop condition.
            cmp cx, matrix@rows
            jge rows_products_loop_fnlz
            
            ; Show row hint for user.
            ui@print_row_at cx
        
            ; Prepare `bx` for memory addressing.
            mov bx, cx
            util@normalize matrix@item_size
            
            ; Put signed number from rows products cell
            ; in `ax` register and print it.
            push ax   
            mov ax, matrix@rows_products[bx]
            call io@println_num
            pop ax            
            
            ; Restore raw `bx`.
            util@denormalize
            
            ; For-loop increment and looping.
            inc cx
            jmp rows_products_loop
        
        rows_products_loop_fnlz:        
        pop cx
        pop bx
        
        io@println
        
    ret
    matrix@print_rows_products endp
    
    
    skip_proc:
    
matrix@define_print_rows_products endm



matrix@define_find_max_rows_products macro
    
    local skip_proc
    local find_max_loop, find_max_loop_continue find_max_loop_fnlz
    local new_max
    local fill_loop, fill_loop_continue, fill_loop_fnlz
    local max, not_max, overflow
    
    jmp skip_proc
    
    
    matrix@find_max_rows_products proc
       
        io@println_const_str ui@find_max_rows_products
           
        push ax
        push bx   
        push cx
        push si
        
        ; Mark first element as maximum.
        mov ax, matrix@rows_products[0]
        
        ; find_max_loop: for(cx = 0; cx < matrix@rows; cx++).
        xor cx, cx
        find_max_loop:
            
            ; For-loop (find_max_loop) condition.
            cmp cx, matrix@rows
            jge find_max_loop_fnlz
            
            ; Prepare `bx` for memory addressing. 
            mov bx, cx            
            util@normalize matrix@item_size
            
            ; if(ax < matrix@rows_products[bx]) goto new_max
            ; else goto find_max_loop_continue.
            cmp ax, matrix@rows_products[bx]
            jl new_max
            jmp find_max_loop_continue
            
            new_max:
                mov ax, matrix@rows_products[bx]
                
            find_max_loop_continue:                            
            util@denormalize
           
            ; For-loop (find_max_loop) increment and looping.
            inc cx
            jmp find_max_loop
            
        find_max_loop_fnlz:
        
        
        ; fill_loop: for(cx = 0; cx < matrix@rows; cx++).
        xor cx, cx
        fill_loop:
        
            ; For-loop (fill_loop) condition.
            cmp cx, matrix@rows
            jge fill_loop_fnlz
            
                ; Prepare `bx` for memory addressing. 
                mov bx, cx            
                util@normalize matrix@item_size
                
                ; if(matrix@rows_products[bx] == matrix@min_value) goto overflow.
                cmp matrix@rows_products[bx], matrix@min_value
                je overflow
                
                ; if(matrix@rows_products[bx] == ax) goto max
                ; else if(matrix@rows_products[bx] < ax) goto not_max                
                cmp matrix@rows_products[bx], ax 
                je max
                jl not_max
                jmp fill_loop_continue
                
                max:
                    mov si, cx
                    mov matrix@max_rows_products[si], matrix@marker_max
                    jmp fill_loop_continue
                    
                not_max:
                    mov si, cx
                    mov matrix@max_rows_products[si], matrix@marker_not_max
                    jmp fill_loop_continue
                
                overflow:
                    mov si, cx
                    mov matrix@max_rows_products[si], matrix@marker_overflow
                    jmp fill_loop_continue
                    
                                               
            fill_loop_continue:
            
            util@denormalize
            inc cx
            jmp fill_loop
        
        fill_loop_fnlz:
        pop si
        pop cx
        pop bx
        pop ax
        
        io@println
        
    ret
    matrix@find_max_rows_products endp
    
    
    skip_proc:
    
matrix@define_find_max_rows_products endm



matrix@define_print_max_rows_products macro
    
    local skip_proc
    
    local max_rows_products_loop
    local max_rows_products_loop_continue
    local max_rows_products_loop_fnlz
    
    local max, not_max, overflow
    
    
    jmp skip_proc
    
    
    matrix@print_max_rows_products proc
        
        io@println_const_str ui@print_max_rows_products
           
        push ax
        push bx
        push cx
        
        ; max_rows_products_loop: for(cx = 0; cx < matrix@rows; cx++).
        xor cx, cx
        max_rows_products_loop:
        
            ; For-loop condition.
            cmp cx, matrix@rows
            jge max_rows_products_loop_fnlz
            
            ; Show row hint for user.
            ui@print_row_at cx
        
            ; Prepare `bx` for memory addressing.
            mov bx, cx
            
            ; Store current item in `ax` register.
            mov al, matrix@max_rows_products[bx]
            
            
            ; if(al == matrix@marker_max) goto max
            cmp al, matrix@marker_max
            je max
            
            ; if(al == matrix@marker_not_max) goto not_max
            cmp al, matrix@marker_not_max
            je not_max
            
            ; if(al == matrix@marker_overflow) goto overflow
            cmp al, matrix@marker_overflow
            je overflow
            
            jmp max_rows_products_loop_continue
            
            max:
                io@println_const_str ui@max
                jmp max_rows_products_loop_continue
                
            not_max:
                io@println_const_str ui@not_max
                jmp max_rows_products_loop_continue
                
            overflow:
                io@println_const_str ui@overflow
                jmp max_rows_products_loop_continue
                         
            
            max_rows_products_loop_continue:
            ; For-loop increment and looping.
            inc cx
            jmp max_rows_products_loop
        
        max_rows_products_loop_fnlz:        
        pop cx
        pop bx
        pop ax
        
        io@println
        
    ret    
    matrix@print_max_rows_products endp
    
    
    skip_proc:
    
matrix@define_print_max_rows_products endm



; CAUTION: always use this macro
; paired with util@denormalize
; for proper work.
; (first - util@normalize, then - util@denormalize)
util@normalize macro param@factor
    
    push bx
    push si
    
    util@mul_bx param@factor
    util@mul_si param@factor    
    
endm



; CAUTION: always use this macro
; paired with util@normalize
; for proper work.
; (first - util@normalize, then - util@denormalize)
util@denormalize macro
    
    pop si
    pop bx
    
endm



; Multiplies `bx` by `param@factor`.
util@mul_bx macro param@factor
    
    push ax
    push cx
    
    mov ax, bx ; ax = bx
    mov cx, param@factor ; cx = param@factor
    mul cx ; ax = ax * cx
    mov bx, ax ; bx = ax
    
    pop cx
    pop ax
    
endm



; Multiplies `si` by `param@factor`.
util@mul_si macro param@factor
    
    push ax
    push cx
    
    mov ax, si ; ax = si
    mov cx, param@factor ; cx = param@factor
    mul cx ; ax = ax * cx
    mov si, ax ; si = ax
    
    pop cx
    pop ax
    
endm



; Prints number surrounded by brackets.
; E.g. ui@print_num_in_brackets 1 -> '[1]'.         
ui@print_num_in_brackets macro param@num
    
    push ax
    
    io@print_char '['
    mov ax, param@num
    call io@print_num
    io@print_char ']'
        
    pop ax
    
endm



; Prints element hint.
; E.g. ui@print_element_at 1,2 -> 'Element[1][2] : '.    
ui@print_element_at macro param@row_num, param@column_num 
           
    io@print_const_str ui@element
    ui@print_num_in_brackets param@row_num
    ui@print_num_in_brackets param@column_num
    io@print_const_str ui@separator
    
endm



; Prints row hint.
; E.g. ui@print_row_at 1 -> 'Row[1] : '.    
ui@print_row_at macro param@num 
           
    io@print_const_str ui@row
    ui@print_num_in_brackets param@num
    io@print_const_str ui@separator
    
endm

                             
         
     
                                  
START:  

    mov ax, @data
    mov ds, ax
    
    call matrix@init
    call matrix@print 
    call matrix@calc_rows_products
    call matrix@print_rows_products
    call matrix@find_max_rows_products
    call matrix@print_max_rows_products
        
    jmp EXIT                          
    
        
        
EXIT:

    system@terminate
    
        
END START
