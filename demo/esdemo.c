/*
          Easy Splitter Demo Program v2.0
*/
#define WIN32_DEFAULT_LIBS
#include <windows.h>
#include "easysplit.h"

HINSTANCE gInst;
HWND MainWind;
HWND VSplitter;
HWND HSplitter;
HWND Child1;
HWND Child2;
HWND Child3;

// WM_SIZE handler
// ver 1.x users note changes in how this done!
LRESULT ResizeWindows(void)
{
    RECT wr, esv, esh;
    INT pos;
    // Get parent size information   
    GetClientRect(MainWind, &wr);

    // relocate the vertical splitter
    pos = SendMessageW(VSplitter, ESM_GETPOS, 0, 0);
    MoveWindow(VSplitter, pos - 4, 0, 8, wr.bottom, 0);
    GetChildRect(VSplitter, &esv);

    // relocate the horizontal splitter
    pos = SendMessageW(HSplitter, ESM_GETPOS, 0, 0);
    MoveWindow(HSplitter, esv.right, pos - 4, (wr.right - esv.right), 8, 0);
    GetChildRect(HSplitter, &esh);

    // fit left window beside vertical splitter
    MoveWindow(Child1, 0, 0, esv.left, wr.bottom, 0);

    // fit top window above and to the right of the splitters
    MoveWindow(Child2, esv.right, 0,
        (wr.right - esv.right), esh.top, 0);

    // fit the bottom window below and to the right of the splitters
    MoveWindow(Child3, esv.right, esh.bottom,
        (wr.right - esv.right), (wr.bottom - esh.bottom), 0);

    // force a repaint of the entire window
    InvalidateRect(MainWind, &wr, 0);
    return 1;
}


// cause an undo operation
LRESULT UndoMoves(void)
{
    SendMessageW(VSplitter, ESM_UNDO, 0, 0);
    SendMessageW(HSplitter, ESM_UNDO, 0, 0);
    return 1;
}


// handle minimum window sizes
LRESULT HandleMinMax(HWND win, LPARAM lp)
{
    if (win != MainWind)
        return 0;
    ((PMINMAXINFO)lp)->ptMinTrackSize.x = 200;
    ((PMINMAXINFO)lp)->ptMinTrackSize.y = 200;
    return 0;
}


// whiten up the static control windows
LRESULT HandleColors(WPARAM wp, LPARAM lp)
{
    SetBkColor((HDC)wp, GetSysColor(COLOR_WINDOW));
    SetBkMode((HDC)wp, TRANSPARENT);
    SetTextColor((HDC)wp, GetSysColor(COLOR_WINDOWTEXT));
    return (LRESULT)GetSysColorBrush(COLOR_WINDOW);
}


// message loop 
LRESULT CALLBACK MsgLoop(HWND win, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_SIZE:              // this is the important one
        return ResizeWindows();
    case WM_GETMINMAXINFO:
        return HandleMinMax(win, lp);
    case WM_CTLCOLORSTATIC:
        return HandleColors(wp, lp);
    case WM_RBUTTONUP:
        return SendMessage(HSplitter, ESM_UNDO, 0, 0);
    case WM_DESTROY:
        PostQuitMessage(0);
        return 1;
    default:
        return DefWindowProc(win, msg, wp, lp);
    }
}


// win entry and UI creation
int WINAPI wWinMain(HINSTANCE inst, HINSTANCE pinst, LPWSTR cmdline, INT show)
{
    WNDCLASSW wc;
    RECT wr;
    MSG msg;

    gInst = inst;
    // register a window
    memset(&wc, 0, sizeof(wc));
    wc.lpszClassName = L"EASYSPLIT_DEMO";
    wc.hInstance = inst;
    wc.hbrBackground = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(inst, L"MAINICON");
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = &MsgLoop;
    RegisterClassW(&wc);

    // register the easy splitter class
    RegisterEasySplit(inst);

    // make our windows
    MainWind = CreateWindowExW(0L, L"EASYSPLIT_DEMO", L"EasySplit Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 250, 250,
        NULL, 0, inst, NULL);
    Child1 = CreateWindowExW(WS_EX_CLIENTEDGE, L"STATIC", L"Left Window",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 0, 0, 0, MainWind, 0, inst, NULL);
    Child2 = CreateWindowExW(WS_EX_CLIENTEDGE, L"STATIC", L"Top Window",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 0, 0, 0, MainWind, 0, inst, NULL);
    Child3 = CreateWindowExW(WS_EX_CLIENTEDGE, L"STATIC", L"Bottom Window",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        0, 0, 0, 0, MainWind, 0, inst, NULL);

    // Used to set starting position of splitters
    GetClientRect(MainWind, &wr);

    // create the splitter windows
    VSplitter = CreateWindowW(L"EASYSPLIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP |
        ES_VERT | ES_LINE | ES_TRACK | ES_DOCK,
        100, 0, 12, wr.bottom, MainWind, 0, inst, NULL);

    HSplitter = CreateWindowW(L"EASYSPLIT", NULL,
        WS_CHILD | WS_VISIBLE | WS_TABSTOP |
        ES_HORZ | ES_LINE | ES_TRACK | ES_DOCK,
        100, 100, wr.right - 110, 12, MainWind, 0, inst, NULL);

    // set splitter colors
    SendMessage(VSplitter, ESM_SETCOLORS,
        (WPARAM)GetSysColor(COLOR_3DDKSHADOW), // Line Color
        (LPARAM)GetSysColor(COLOR_BTNFACE));   // Background Color

    SendMessage(HSplitter, ESM_SETCOLORS,
        (WPARAM)GetSysColor(COLOR_3DSHADOW),
        (LPARAM)GetSysColor(COLOR_BTNFACE));

    // set splitter cursors
    SendMessage(VSplitter, ESM_SETCURSOR, 0,
        (LPARAM)LoadCursor(NULL, IDC_SIZEWE));
    SendMessage(HSplitter, ESM_SETCURSOR, 0,
        (LPARAM)LoadCursor(NULL, IDC_SIZENS));

    // set borders
    SendMessage(HSplitter, ESM_SETBORDER, 30, ESB_TOP);
    SendMessage(HSplitter, ESM_SETBORDER, 30, ESB_BOTTOM);
    SendMessage(VSplitter, ESM_SETBORDER, 70, ESB_RIGHT);

    // set docking to 15 pixels
    SendMessage(HSplitter, ESM_SETDOCKING, 15, TRUE);
    SendMessage(VSplitter, ESM_SETDOCKING, 15, TRUE);

    ResizeWindows();
    ShowWindow(MainWind, SW_SHOWNORMAL);

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(GetForegroundWindow(), &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}
