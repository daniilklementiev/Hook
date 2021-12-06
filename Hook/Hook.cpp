// Hook.cpp : 
#include "framework.h"
#include "Hook.h"
#include <fstream>

#define MAX_LOADSTRING          100
#define CMD_KB_HOOK_START       1001
#define CMD_KB_HOOK_LL_START    1002
#define CMD_KB_HOOK_LL_STOP     1003
#define CMD_KB_HOOK_STOP        1004
#define CMD_KB_HOOK_RESET       1005


HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            
HWND list; // console
HWND start; // console
HWND stop; // console
HHOOK kbHook; // hook handle for keyboard
HHOOK kbLL; // hook handle for LL keyboard
WCHAR txt[MAX_LOADSTRING];
std::ofstream file("C:\\Users\\Klem_em31\\source\\repos\\Hook\\Hook\\key.txt", std::fstream::out | std::fstream::app);

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


DWORD   CALLBACK    StartKbHook(LPVOID);
DWORD   CALLBACK    StopKbHook(LPVOID);
LRESULT CALLBACK    KbHookProc(int, WPARAM, LPARAM);

DWORD   CALLBACK    StartKbHookLL(LPVOID);
DWORD   CALLBACK    StopKbHookLL(LPVOID);
LRESULT CALLBACK    KbHookProcLL(int, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.


    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HOOK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HOOK));

    MSG msg;


    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HOOK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+2);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HOOK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        start = CreateWindowW(L"Button", L"KB Start", WS_CHILD | WS_VISIBLE, 10, 10, 75, 23, hWnd, (HMENU)CMD_KB_HOOK_START, hInst, NULL);
        stop = CreateWindowW(L"Button", L"KB Stop", WS_CHILD | WS_VISIBLE, 10, 40, 75, 23, hWnd, (HMENU)CMD_KB_HOOK_STOP, hInst, NULL);

        CreateWindowW(L"Button", L"LL Start", WS_CHILD | WS_VISIBLE, 10, 100, 75, 23, hWnd, (HMENU)CMD_KB_HOOK_LL_START, hInst, NULL);
        CreateWindowW(L"Button", L"LL Stop", WS_CHILD | WS_VISIBLE, 10, 130, 75, 23, hWnd, (HMENU)CMD_KB_HOOK_LL_STOP, hInst, NULL);

        CreateWindowW(L"Button", L"ResetC", WS_CHILD | WS_VISIBLE, 10, 70, 75, 23, hWnd, (HMENU)CMD_KB_HOOK_RESET, hInst, NULL);

        list = CreateWindowW(L"Listbox", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL, 100, 10, 400, 300, hWnd, 0, hInst, NULL);
        kbLL = kbHook = 0;
        break;
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case CMD_KB_HOOK_RESET:     SendMessageW(list, LB_RESETCONTENT, 0, 0); break;

            case CMD_KB_HOOK_START:     StartKbHook(NULL);   break;
            case CMD_KB_HOOK_STOP:      StopKbHook(NULL);    break;
            case CMD_KB_HOOK_LL_START:  StartKbHookLL(NULL); break;
            case CMD_KB_HOOK_LL_STOP:   StopKbHookLL(NULL);  break;
            
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

DWORD CALLBACK StartKbHook(LPVOID params) {
    if (kbHook != 0) {
        _snwprintf_s(txt, MAX_LOADSTRING, L"KB hook works");
    }
    else {

    
    kbHook = SetWindowsHookExW(
        WH_KEYBOARD,
        KbHookProc,
        (HINSTANCE)NULL,
        GetCurrentThreadId()
    );
    if (kbHook != 0) _snwprintf_s(txt, MAX_LOADSTRING, L"KB hook activated");
       
    else _snwprintf_s(txt, MAX_LOADSTRING, L"KB hook activation fail");
    }
    SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)txt);
    return 0;
}

DWORD CALLBACK StopKbHook(LPVOID params) {
    if (kbHook != 0)
    {
        UnhookWindowsHookEx(kbHook);
        kbHook = 0;
        _snwprintf_s(txt, MAX_LOADSTRING, L"KB hook released");
    }
    else {
        _snwprintf_s(txt, MAX_LOADSTRING, L"KB hook is not active");
    }
    SendMessageW(list, LB_ADDSTRING, MAX_LOADSTRING, (LPARAM)txt);
    return 0;
}

LRESULT CALLBACK KbHookProc(int nCode, WPARAM wParam, LPARAM lParam) {

    _snwprintf_s(txt, MAX_LOADSTRING, L"wParam is %d", wParam);
    SendMessageW(list, LB_ADDSTRING, MAX_LOADSTRING, (LPARAM)txt);
    return CallNextHookEx(kbHook, nCode, wParam, lParam);
}


DWORD CALLBACK StartKbHookLL(LPVOID) {
    if (kbLL != 0) {
        _snwprintf_s(txt, MAX_LOADSTRING, L"KB LL works");
    } 
    else {
    kbLL = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        KbHookProcLL,
        GetModuleHandle(NULL),
        0);
    if (kbLL != 0) _snwprintf_s(txt, MAX_LOADSTRING, L"KB LL hook activated");

    else _snwprintf_s(txt, MAX_LOADSTRING, L"KB LL hook activation fail");

    
    }
    SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)txt);
    return 0;
}

DWORD CALLBACK StopKbHookLL(LPVOID) {
    file.close();
    if (kbLL != 0)
    {
        UnhookWindowsHookEx(kbLL);
        kbLL = 0;
        _snwprintf_s(txt, MAX_LOADSTRING, L"KB LL  hook released");
    }
    else {
        _snwprintf_s(txt, MAX_LOADSTRING, L"KB LL hook is not active");
    }
    SendMessageW(list, LB_ADDSTRING, MAX_LOADSTRING, (LPARAM)txt);
    return 0;
}
LRESULT CALLBACK KbHookProcLL(int nCode, WPARAM wParam, LPARAM lParam) {
    
    if (!file.is_open())
    {
        SendMessageW(list, LB_ADDSTRING, 0, (LPARAM)L"Error");
    }
    
    if (nCode == HC_ACTION)
    {
        if (wParam == WM_KEYDOWN 
         || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT keyInfo = *((KBDLLHOOKSTRUCT*)lParam);
            UINT sym = MapVirtualKeyExW(keyInfo.vkCode, MAPVK_VK_TO_CHAR,
                GetKeyboardLayout(0));

            _snwprintf_s(txt, MAX_LOADSTRING,
                L"wParam is %d (%d) %c",
                keyInfo.vkCode,
                keyInfo.scanCode,
                sym
            );
            if (sym == 0)
            {
                file << keyInfo.vkCode;
            }
            else {
                file << (char)sym;
            }
            SendMessageW(list, LB_ADDSTRING, MAX_LOADSTRING, (LPARAM)txt);
            
        }
    }

    return CallNextHookEx(kbLL, nCode, wParam, lParam);
}