format PE console
entry start

include 'win32a.inc'


section '.code' code readable executable

start:

FINIT  ;�������������� �����������
call inputNumber
call iterations

displayResult:
    invoke printf, resultNumb, dword[currentNumber], dword[currentNumber+4]

call compareToFuncSQRT



finish:
    invoke getch
    invoke ExitProcess, 0 ; ����� �� ��������� � ����� �������� 0



;------------------------------------------------------------------------------------------------------
inputNumber:
    mov [tmp], esp; ���������� ��������� �� ����

    invoke printf, mesN    ;������ ����� �����
    invoke scanf,  formatF, N    ;���������� N
    FLD [zero]         ;��������� ����, ����� � ��� ����������
    FLD [N] ;�������� ����� � ���� ;st(0) = N
    FST [prevNumber]  ;��������� �������� (x_1).

    ;���� ����� �������������, �� �� ����� ������
    FCOMPP   ;��������� � ������������� �� �����
    fstsw   ax
    sahf
    jb  start  ;���� ����� �������������, �� ��������� ����


 checkZero:  ;��������� ����� �� ��������� ����
 ;(������, �� ��, ������ �� ����� 0.000 000 000 000 1)
 ;��� ��� ���� ��� ������, �� ��������� ������� � ��������� ���� � ��� ������ ��������� ��� ��������
     FLD [zero]  ;��������� ����
     FST [currentNumber] ;����� ������� ����� ���� (���� ��� ����������� �������)

     FLD [nearZero] ;��������� �����, ������� � ����
     FSUB [N]  ;����� N
     FCOMPP   ;���������� N � ����� 0
     fstsw   ax
     sahf
     ja displayResult ; �������� ����, ���� N = 0

    mov esp, [tmp]  ;��������������� ��������� �� ����
ret
;-------------------------------------------------------------------------------------------------------

iterations:
    mov [tmp], esp ; ���������� ��������� �� ����

    FLD [N] ;��������� � ������� N
  oneIteration:
    ;�� ������� ����� N
    FDIV [prevNumber] ;����� N �� x_n
    FADD [prevNumber] ;��������� � ����� x_n => st(0) = (x_n + N/x_n)
    FMUL [half]; �������� �� 1/2
    ;������ �� ������� ����� x_n+1
    FST [currentNumber] ;���������� � ���������� ������� ��������


     ;�������� ���������� ��������
     ;������ �� ������� ����� ������� �����
     FSUB [prevNumber] ;�������� �� �������� ���������� �����
     FABS ;����� ������ ��������
     FDIV [prevNumber] ;����� �� ���������� ����� -
     ;�������� �� ������� ��������� (� �����) ���������� ����� ��� ����� ��������
     FSUB [precision]   ;�������� ��������

     FLD [zero]  ;��������� ����, ����� �������� �������� � �������� ���������

     FCOMPP ;��������� � ������������� �� �����
     fstsw   ax
     sahf
     jae  displayResult  ;���� �������� ��������� ������, ��� �������� ���������


    ;���������� �����������
    FLD [currentNumber]
    FSTP [prevNumber]    ;������ ������������ ��������
    FLD [N]   ;��������� ���� ����� �� �������

    ;������� ��������� ����� ������������������
     invoke printf, currentNumb, dword[currentNumber], dword[currentNumber+4]
    jmp oneIteration

    mov esp, [tmp] ;��������������� ��������� �� ����
ret


;----------------------------------------------------------------------------------------------------------------
compareToFuncSQRT:
    mov [tmp], esp   ;���������� ��������� �� ����

    FLD [N]   ;��������� ���� ����� �� ������� �����
    FSQRT     ;�������� ������� ����� �����������
    FSTP [realNumber]   ;���������� ��������
    invoke printf, realNumb, dword[realNumber], dword[realNumber+4]

    mov esp, [tmp]    ;��������������� ��������� �� ����
ret
;----------------------------------------------------------------------------------------------------------------




section '.data' data readable writable

    mesN  db  "Enter a non-negative number: ",10, 0
    half dq 0.5   ;����������� � �������
    zero dq 0  ;����
    nearZero dq 0.0000000000001 ;�����, ��� ����� �������� ��� ������, ������� ��������� ������ ����
    precision dq 0.0005 ;�������� � �����
    N   dq   ?  ;�����, ������� ������
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