format PE console
entry start

include 'win32a.inc'


section '.code' code readable executable

start:

FINIT  ;инициализируем сопроцессор
call inputNumber
call iterations

displayResult:
    invoke printf, resultNumb, dword[currentNumber], dword[currentNumber+4]

call compareToFuncSQRT



finish:
    invoke getch
    invoke ExitProcess, 0 ; выход из программы с кодом возврата 0



;------------------------------------------------------------------------------------------------------
inputNumber:
    mov [tmp], esp; запоминаем указатель на стек

    invoke printf, mesN    ;запрос ввода числа
    invoke scanf,  formatF, N    ;считывание N
    FLD [zero]         ;загружаем ноль, чтобы с ним сравнивать
    FLD [N] ;помещаем число в стек ;st(0) = N
    FST [prevNumber]  ;начальное значение (x_1).

    ;если число отрицательное, то не берем корень
    FCOMPP   ;сравнение с выталкиванием из стека
    fstsw   ax
    sahf
    jb  start  ;если число отрицательное, то повторный ввод


 checkZero:  ;проверяем число на равенство нулю
 ;(точнее, на то, меньше ли число 0.000 000 000 000 1)
 ;так как если оно меньше, то программа выводит в результат ноль и нет смысла проводить все итерации
     FLD [zero]  ;загрузили ноль
     FST [currentNumber] ;пусть текущее будет ноль (если его понадобится вывести)

     FLD [nearZero] ;загружаем число, близкое к нулю
     FSUB [N]  ;вычли N
     FCOMPP   ;сравниваем N и почти 0
     fstsw   ax
     sahf
     ja displayResult ; печатаем ноль, если N = 0

    mov esp, [tmp]  ;восстанавливаем указатель на стек
ret
;-------------------------------------------------------------------------------------------------------

iterations:
    mov [tmp], esp ; запоминаем указатель на стек

    FLD [N] ;загружаем в вершину N
  oneIteration:
    ;на вершине стека N
    FDIV [prevNumber] ;делим N на x_n
    FADD [prevNumber] ;добавляем к этому x_n => st(0) = (x_n + N/x_n)
    FMUL [half]; умножаем на 1/2
    ;теперь на вершине стека x_n+1
    FST [currentNumber] ;записываем в переменную текущее значение


     ;проверка достижения точности
     ;сейчас на вершине стека текущее число
     FSUB [prevNumber] ;вычитаем из текущего предыдущее число
     FABS ;берем модуль разности
     FDIV [prevNumber] ;делим на предыдущее число -
     ;получаем НА сколько процентов (в долях) изменилось число при новой итерации
     FSUB [precision]   ;вычитаем точность

     FLD [zero]  ;загружаем ноль, чтобы сравнить точность и реальное изменение

     FCOMPP ;сравнение с выталкиванием из стека
     fstsw   ax
     sahf
     jae  displayResult  ;если точность оказалась больше, чем реальное изменение


    ;продолжаем приближение
    FLD [currentNumber]
    FSTP [prevNumber]    ;меняем итерационное значение
    FLD [N]   ;загружаем наше число на вершину

    ;выводим очередное число последовательности
     invoke printf, currentNumb, dword[currentNumber], dword[currentNumber+4]
    jmp oneIteration

    mov esp, [tmp] ;восстанавливаем указатель на стек
ret


;----------------------------------------------------------------------------------------------------------------
compareToFuncSQRT:
    mov [tmp], esp   ;запоминаем указатель на стек

    FLD [N]   ;загружаем наше число на вершину стека
    FSQRT     ;вызываем функцию корня квадратного
    FSTP [realNumber]   ;запоминаем значение
    invoke printf, realNumb, dword[realNumber], dword[realNumber+4]

    mov esp, [tmp]    ;восстанавливаем указатель на стек
ret
;----------------------------------------------------------------------------------------------------------------




section '.data' data readable writable

    mesN  db  "Enter a non-negative number: ",10, 0
    half dq 0.5   ;коэффициент в формуле
    zero dq 0  ;ноль
    nearZero dq 0.0000000000001 ;число, при вводе которого или меньше, считаем результат равным нулю
    precision dq 0.0005 ;Точность в долях
    N   dq   ?  ;число, которое введут
    prevNumber dq ?
    currentNumber dq ?
    realNumber dq ?
    formatF   db   '%lf', 0
    currentNumb   db   'Current number is  %lf',10, 0
    resultNumb   db  10, "RESULT number is  %lf", 10, 0
    realNumb   db  10, "Command FSQRT outcome is  %lf", 10, 0
    pointerSt dw ?
    tmp dd      ?




section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll'

    import kernel,\
           ExitProcess, 'ExitProcess'
    import msvcrt,\
           printf, 'printf',\
           sprintf, 'sprintf',\
           scanf, 'scanf',\
           getch, '_getch'