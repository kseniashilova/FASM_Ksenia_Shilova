format PE GUI 4.0

include 'win32wx.inc'

.data

_class TCHAR 'FASMWIN32',0
_title TCHAR 'Win32 program template',0
_error TCHAR 'Startup failed.',0

wc WNDCLASS 0,WindowProc,0,0,NULL,NULL,NULL,COLOR_BACKGROUND,NULL,_class

msg MSG

hdc dd ?
paintstruct PAINTSTRUCT

.code

start:
    invoke  GetModuleHandle,0
    mov [wc.hInstance],eax
    invoke  LoadIcon,0,IDI_APPLICATION
    mov [wc.hIcon],eax
    invoke  LoadCursor,0,IDC_ARROW
    mov [wc.hCursor],eax
    invoke CreateSolidBrush, 0xFFFFFF  ; белый фон
    mov [wc.hbrBackground], eax
    invoke  RegisterClass,wc
    test    eax,eax
    jz  error

    invoke  CreateWindowEx,0,_class,_title,WS_VISIBLE+WS_OVERLAPPEDWINDOW,128,128,256,192,NULL,NULL,[wc.hInstance],NULL
    test    eax,eax
    jz  error
    ; invoke ShowWindow,eax,SW_SHOWNORMAL
msg_loop:
    invoke  GetMessage,msg,NULL,0,0
    cmp eax,1
    jb  end_loop
    jne msg_loop
    invoke  TranslateMessage,msg
    invoke  DispatchMessage,msg
    jmp msg_loop

error:
    invoke  MessageBox,NULL,_error,NULL,MB_ICONERROR+MB_OK

end_loop:
    invoke  ExitProcess,[msg.wParam]

proc WindowProc uses ebx esi edi, hwnd,wmsg,wparam,lparam
    cmp [wmsg], WM_DESTROY
    je  .wmdestroy
    cmp [wmsg], WM_PAINT
    je .wmpaint

.defwndproc:
    invoke  DefWindowProc,[hwnd],[wmsg],[wparam],[lparam]
    jmp .finish

.wmdestroy:
    invoke  PostQuitMessage,0
    xor eax,eax
    jmp .finish

.wmpaint:
    invoke BeginPaint, [hwnd], paintstruct ; получаем контекст окна
    mov [hdc], eax  ; сохраняем контекст окна в переменную
    invoke MoveToEx, [hdc], 10, 15, 0  ; 10, 15 - начальные координаты
    invoke LineTo, [hdc], 200, 100  ; 200, 100 - конечные координаты
    invoke EndPaint, [hwnd], paintstruct  ; освобождаем контекст окна

.finish:
    ret
endp

.end start