#include "DVD.h"
#include <windowsx.h>
#include <Windows.h>
#include <CommCtrl.h>

#define MAX_LOADSTRING 100
#define CMD_BUTTON_START 1001
#define CMD_BUTTON_ELLIPSE 1002
#define TIMER_MOVE 1003

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
BOOL isLeftHold;
BOOL isRightHold;
HDC dc;
HPEN drawPen, erasePen, pen;
COLORREF bgColor;
HBRUSH brush, brush_eraser;
int x, y, width = 100, height = 100;
bool xBorder, yBorder;
RECT rect;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    bgColor = RGB(GetRValue(GetSysColor(5)), GetGValue(GetSysColor(5)), GetBValue(GetSysColor(5)));
    brush = CreateSolidBrush(RGB(100, 100, 250));
    brush_eraser = CreateSolidBrush(RGB(250, 250, 0));
    erasePen = CreatePen(PS_SOLID, 2, bgColor);
    drawPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 250));

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DVD, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DVD));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_DVD));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DVD);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

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

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        isLeftHold = isRightHold = FALSE;
        GetClientRect(hWnd, &rect);
        x = y = 0;
        xBorder = yBorder = FALSE;

        dc = GetDC(hWnd);
        pen = CreatePen(PS_SOLID, 5, RGB(100, 200, 100));
        CreateWindowW(L"Button", L"Start", WS_CHILD | WS_VISIBLE,
            10, 10, 75, 23, hWnd, (HMENU)CMD_BUTTON_START, hInst, 0);

        break;
    }
    case WM_TIMER:
        if (wParam == TIMER_MOVE) {

            SelectObject(dc, erasePen);
            Ellipse(dc, x - width / 2, y - height / 2, x + width / 2, y + width / 2);

            if (x > rect.right - width / 2) xBorder = TRUE;
            if (x < rect.left + width / 2) xBorder = FALSE;
            if (y > rect.bottom - height / 2) yBorder = TRUE;
            if (y < rect.top + height / 2) yBorder = FALSE;

            if (xBorder) x -= 7;
            else x += 7;

            if (yBorder) y -= 7;
            else y += 7;

            SelectObject(dc, drawPen);
            Ellipse(dc, x - width / 2, y - height / 2, x + width / 2, y + width / 2);
            break;
        }
        break;
    case WM_MOUSEMOVE: {
        if (isLeftHold == TRUE) {

            HPEN savedPen = (HPEN)SelectObject(dc, pen);

            LineTo(dc, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

            SelectObject(dc, savedPen);
        }
        break;
    }
    case WM_LBUTTONDOWN: {
        isLeftHold = TRUE;
        MoveToEx(dc, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), NULL);
        SetCapture(hWnd);
        break;
    }
    case WM_LBUTTONUP:
        isLeftHold = FALSE;
        ReleaseCapture();
        break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {

        case CMD_BUTTON_START:
            SetTimer(hWnd, TIMER_MOVE, 0, NULL);
            break;
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
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        KillTimer(hWnd, TIMER_MOVE);
        DeleteObject(pen);
        DeleteObject(drawPen);
        DeleteObject(erasePen);
        DeleteObject(brush);
        DeleteObject(brush_eraser);
        ReleaseDC(hWnd, dc);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
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