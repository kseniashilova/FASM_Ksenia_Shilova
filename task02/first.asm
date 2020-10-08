format PE console
entry start

include 'win32a.inc'

section '.data' data readable writable

        strVecSize   db 'size of vector? ', 0
        strIncorSize db 'Incorrect size of vector', 10, 0
        strVecElemI  db '[%d]? ', 0
        strScanInt   db '%d', 0
        strSumValue  db 'Summa = %d', 10, 0
        strVecElemOut  db '[%d] = %d', 10, 0

        vec_size     dd 0
        sum          dd 0
        i            dd ?
        tmp          dd ?
        newtmp       dd ?
        elem         dd ?
        vec          rd 100
        newVec       rd 100

section '.code' code readable executable






start:
call vectorInput
call newVector
call putVector

finish:
     call [getch]
     push 0
     call [ExitProcess]
;------------------------------------------------------
vectorInput:
        push strVecSize
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

        mov eax, [vec_size]
        cmp eax, 0
        jg getVector
        ; fail size
        push strIncorSize
        call [printf]
        add esp, 4
        jmp start

        ; input of elements
        getVector:
        xor ecx, ecx ; ecx = 0
        mov ebx, vec ; ebx = &vec

getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endInputVector ; to end of loop

; input element
        mov [i], ecx
        push ecx
        push strVecElemI
        call [printf]
        add esp, 8

        push ebx
        push strScanInt
        call [scanf]
        add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
endInputVector:
        ret
;----------------------------------------------------------------
newVector:
  xor ecx, ecx ; ecx = 0
  xor eax, eax ; eax = 0
  mov ebx, vec
  mov edx, newVec ;
newVecLoop:
        cmp ecx, [vec_size]
        je endNewVector ; to end of loop
        test dword [ebx + ecx*4], 1
        jz Even
        jnz Odd
Even:
        inc ecx
        jmp newVecLoop
Odd:
        push dword [ebx + ecx*4]
        pop dword [edx + eax*4]


        inc eax
        inc ecx
        jmp newVecLoop
endNewVector:
        mov [vec_size], eax
        ret

;-------------------------------------------------------------------
putVector:
        xor ecx, ecx ; ecx = 0
        mov edx, newVec ; ebx = &vec
        putVecLoop:
        mov [newtmp], edx
        cmp ecx, [vec_size]
        je endOutputVector ; to end of loop
        mov [i], ecx

        ; output element
        push dword [edx]
        push ecx
        push strVecElemOut
        call [printf]
        add esp, 12

        mov ecx, [i]
        inc ecx
        mov edx, [newtmp]
        add edx, 4
        jmp putVecLoop
endOutputVector:
        ret
;----------------------------------------------------------




;-------------------------------third act - including HeapApi--------------------------
                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'