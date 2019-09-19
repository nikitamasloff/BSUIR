IDEAL
MODEL small
STACK 100h

include 'args.inc'
include 'io.inc'

DATASEG

        argsAmount dw ?
        arg1 db 80h dup (0)
        arg2 db 80h dup (0)
        arg3 db 80h dup (0) 

CODESEG



START:

        mov ax, @data
        mov ds, ax
        
        mov di, offset arg1
        mov cx, 1
        call getByOrdinal@args
        
        mov di, offset arg2
        mov cx, 2
        call getByOrdinal@args
        
        mov di, offset arg3
        mov cx, 3
        call getByOrdinal@args
        
        mov si, offset arg1
        call printStr@io
        
        mov si, offset arg2
        call printStr@io
        
        mov si, offset arg3
        call printStr@io
        
        mov ax, 4C00h
        int 21h


END START
