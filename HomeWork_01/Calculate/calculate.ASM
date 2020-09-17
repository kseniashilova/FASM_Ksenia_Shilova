format PE console

entry start

include 'win32a.inc'

section '.data' data readable writable

        strA db 'Enter A: ' , 0
        strB db 'Enter B: ' ,0
        strOp db 'Enter operation' ,0

        resStr db 'Result: %d ' ,0
        resMod db '/%d',0

        spaceStr db ' %d', 0
        emptyStr db '%d', 0

        infinity db 'infinity',0
        point db ',',0

        A dd ?
        B dd ?
        C dd ?

        NULL = 0 ;const

section '.code' code readable executable
        start:
                push strA
                call [printf]

                push A
                push spaceStr
                call [scanf]

                push strB
                call [printf]

                push B
                push spaceStr
                call [scanf]

                push strOp
                call [printf]

                call [getch]

                ;сравниваем номеры символов со знаками
                cmp eax, 43    ;сумма
                ;jump not equals
                jne notAdd
                    mov ecx, [A]
                    add ecx, [B]
                    push ecx
                    push resStr
                    call [printf]

                    jmp finish ;jump to finish

                notAdd:

                cmp eax, 45 ;разность
                jne notSub
                    mov ecx, [A]
                    sub ecx, [B]
                    push ecx
                    push resStr
                    call [printf]
                    jmp finish

                notSub:

                cmp eax, 42; умножение
                jne notMul
                    mov ecx, [A]
                    imul ecx, [B]
                    push ecx
                    push resStr
                    call [printf]
                    jmp finish

                notMul:

                cmp eax, 37 ;%
                jne notMod
                    mov eax, [A]
                    mov ecx, [B]
                    mov edx, 0

                    cmp [B], 0
                    jne notNullDiv
                        push infinity
                        call [printf]
                        jmp finish
                    notNullDiv:
                    div ecx
                    mov [C], edx
                    push eax; целая часть
                    push resStr
                    call [printf]

                    push [C] ;остаток
                    push spaceStr
                    call [printf]

                    push [B];делитель
                    push resMod
                    call [printf]

                    jmp finish

                notMod:
                cmp eax, 47
                jne notDiv
                    mov eax, [A]
                    mov ecx, [B]
                    mov edx, 0

                    cmp [B], 0
                    jne notNullDiv1
                        push infinity
                        call [printf]
                        jmp finish
                    notNullDiv1:
                    div ecx
                    mov [C], edx
                    push eax; целая часть
                    push resStr
                    call [printf]

                    push point;запятая
                    call [printf]
                    mov ebx, 0 ;счетчик по циклу
                    ;loop
                    lp:

                        mov eax, [C]
                        mov ecx, [B]
                        imul eax, 10

                        mov edx, 0
                        div ecx
                        mov [C], edx

                        push eax
                        push emptyStr
                        call [printf]

                        add ebx,1
                        cmp ebx, 3
                    jne lp
                    ;3 знака после запятой
                    jmp finish

                notDiv:


                finish:
                call [getch]
                push NULL
                call [ExitProcess]



section '.idata' import data readable
        library kernel, 'kernel32.dll',\
                msvcrt, 'msvcrt.dll'

        import kernel,\
               ExitProcess, 'ExitProcess'
        import msvcrt,\
               printf, 'printf',\
               scanf, 'scanf',\
               getch, '_getch'








