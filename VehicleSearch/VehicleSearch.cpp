#include "framework.h"
#include "VehicleSearch.h"
#include "VehicleSearchAlgorithm.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

VehicleSearchAlgorithm Vehicle(".\\vehicle.json");

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VEHICLESEARCH, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VEHICLESEARCH));

    MSG msg;

    // Main message loop:
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

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VEHICLESEARCH));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable
   
   int windowWidth = 400;   // 设置窗口宽度
   int windowHeight = 550;  // 设置窗口高度
   
   int screenWidth = GetSystemMetrics(SM_CXSCREEN);  // 获取屏幕宽度
   int screenHeight = GetSystemMetrics(SM_CYSCREEN); // 获取屏幕高度

   int windowX = (screenWidth - windowWidth) / 2;   // 计算窗口的水平位置
   int windowY = (screenHeight - windowHeight) / 2; // 计算窗口的垂直位置

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


std::vector<SearchResult> PerformSearch(const std::wstring& searchText)
{
    return Vehicle.runSearch(wstringToString(searchText));
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hSearchBox = nullptr;  // 保存搜索框句柄
    static HWND hListBox = nullptr;    // 保存列表框句柄
    static HWND hLabel = nullptr;
    static int listItemHeight = 200;    // 设置每个列表项的高度，单位为像素
    static std::vector<SearchResult> searchResults;  // 保存搜索结果

    switch (message)
    {
    case WM_CREATE:
    {
        // 创建搜索框
        hSearchBox = CreateWindowEx(
            0,
            L"EDIT",
            L"",     // 初始文本为空
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_CENTER,
            0, 0, 0, 0,  // 位置和大小
            hWnd,
            NULL,
            hInst,
            NULL
        );

        // 创建标签控件
        hLabel = CreateWindowW(
            L"STATIC",       // 控件类名
            L"Search Results: 0",  // 初始文本
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            0, 0, 0, 0,  // 位置和大小
            hWnd, nullptr, hInst, nullptr
        );

        // 创建列表框
        hListBox = CreateWindowEx(
            0,
            L"LISTBOX",
            L"",
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD | LBS_NOTIFY | LBS_OWNERDRAWFIXED,
            0, 0, 0, 0,  // 位置和大小
            hWnd,
            NULL,
            hInst,
            NULL
        );
    }
    break;

    case WM_SIZE:
    {
        // 当窗口尺寸变化时，重新定位控件位置
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int searchBoxWidth = 300;
        int searchBoxHeight = 25;
        int searchBoxX = (clientRect.right - clientRect.left - searchBoxWidth) / 2;
        int searchBoxY = 20;
        int listBoxY = searchBoxY + 50;
        int listBoxHeight = clientRect.bottom - clientRect.top - listBoxY;
        SetWindowPos(hSearchBox, NULL, searchBoxX, searchBoxY, searchBoxWidth, searchBoxHeight, SWP_NOZORDER);
        SetWindowPos(hListBox, NULL, searchBoxX, listBoxY, searchBoxWidth, listBoxHeight, SWP_NOZORDER);
        SetWindowPos(hLabel, NULL, searchBoxX, clientRect.bottom - clientRect.top - 30, 150, 20, SWP_NOZORDER);
    }
    break;

    case WM_MEASUREITEM:
    {
        LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT)lParam;
        if (lpMeasureItem->CtlType == ODT_LISTBOX)
        {
            lpMeasureItem->itemHeight = listItemHeight; // 设置每个列表项的高度
        }
    }
    break;

    case WM_DRAWITEM:
    {
        LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
        if (lpDrawItem->CtlType == ODT_LISTBOX)
        {
            // 获取搜索结果
            SearchResult* result = reinterpret_cast<SearchResult*>(lpDrawItem->itemData);

            // 绘制背景
            COLORREF bgColor = RGB(255, 255, 255);  // 设置背景颜色为白色
            HBRUSH hBrush = CreateSolidBrush(bgColor);
            FillRect(lpDrawItem->hDC, &lpDrawItem->rcItem, hBrush);
            DeleteObject(hBrush);

            // 绘制图片
            HBITMAP image = (HBITMAP)LoadImage(nullptr, result->imagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

            if (image != nullptr)
            {
                HDC hdcMem = CreateCompatibleDC(lpDrawItem->hDC);
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, image);

                BITMAP bm;
                GetObject(image, sizeof(BITMAP), &bm);

                int imageX = lpDrawItem->rcItem.left + (lpDrawItem->rcItem.right - lpDrawItem->rcItem.left - bm.bmWidth) / 2;
                int imageY = lpDrawItem->rcItem.top + 10;

                StretchBlt(lpDrawItem->hDC, imageX, imageY, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
                SelectObject(hdcMem, hOldBitmap);
                DeleteDC(hdcMem);

                // 计算文本位置
                SIZE textSize;
                std::wstring text = std::wstring(L"") + std::to_wstring(result->id) + L" " + result->name;
                GetTextExtentPoint32(lpDrawItem->hDC, text.c_str(), text.length(), &textSize);
                int textX = lpDrawItem->rcItem.left + (lpDrawItem->rcItem.right - lpDrawItem->rcItem.left - textSize.cx) / 2;
                int textY = imageY + 5;

                TextOut(lpDrawItem->hDC, textX, textY, text.c_str(), text.length());
            }
            else
            {
                MessageBox(hWnd, L"ERROR", L"Waring", MB_ICONWARNING | MB_OK);
            }
        }
    }
    break;

    case WM_COMMAND:
        {
            if (LOWORD(wParam) == IDM_EXIT) DestroyWindow(hWnd);

            // 处理搜索框文本变化事件
            if (hSearchBox != nullptr && (HWND)lParam == hSearchBox && HIWORD(wParam) == EN_CHANGE)
            {
                SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
                searchResults.clear();
                wchar_t searchTextBuffer[256];
                GetWindowText(hSearchBox, searchTextBuffer, sizeof(searchTextBuffer) / sizeof(searchTextBuffer[0]));
                searchResults = PerformSearch(searchTextBuffer);

                for (const SearchResult& result : searchResults)
                {
                    int index = SendMessage(hListBox, LB_ADDSTRING, 0, 0);
                    SendMessage(hListBox, LB_SETITEMDATA, index, (LPARAM)&result);
                }

                int itemCount = SendMessage(hListBox, LB_GETCOUNT, 0, 0);
                int scrollPos = SendMessage(hListBox, LB_GETTOPINDEX, 0, 0);
                SendMessage(hListBox, LB_SETTOPINDEX, scrollPos, 0);
                std::wstring itemCountStr = std::to_wstring(itemCount);
                SetWindowText(hLabel, (L"Search Results: " + itemCountStr).c_str());
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
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

