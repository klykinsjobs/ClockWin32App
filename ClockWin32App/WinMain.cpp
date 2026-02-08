#include <windows.h>
#include <string>

// Forward declaration
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Settings
enum TimeFormat { TWELVE_HOUR_FORMAT, TWENTY_FOUR_HOUR_FORMAT };
TimeFormat currentTimeFormat = TWELVE_HOUR_FORMAT;
enum Theme { THEME_LIGHT, THEME_DARK };
Theme currentTheme = THEME_LIGHT;

// Get formatted time string
static std::wstring GetCurrentTimeString()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    wchar_t buffer[20];

    if (currentTimeFormat == TWELVE_HOUR_FORMAT)
    {
        swprintf_s(buffer, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
    }
    else
    {
        swprintf_s(buffer, L"%02d:%02d:%02d %s", (st.wHour % 12 == 0 ? 12 : st.wHour % 12), st.wMinute, st.wSecond, (st.wHour < 12 ? L"AM" : L"PM"));
    }

    return buffer;
}

// Main entry point
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // Register the window class
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = L"ClockWin32App";

    if (!RegisterClass(&wc))
    {
        MessageBox(0, L"RegisterClass failed", L"Error", MB_ICONERROR);
        return false;
    }

    // Create the window
    HWND hWnd = CreateWindow(L"ClockWin32App", L"ClockWin32App", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, 0, 0, hInstance, 0);

    if (!hWnd)
    {
        MessageBox(0, L"CreateWindow failed", L"Error", MB_ICONERROR);
        return false;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        if (!SetTimer(hWnd, 1, 1000, NULL))
        {
            MessageBox(0, L"SetTimer failed", L"Error", MB_ICONERROR);
            PostQuitMessage(1);
        }
        return 0;

    case WM_TIMER:
        InvalidateRect(hWnd, NULL, true);
        return 0;

    case WM_LBUTTONDOWN:
        currentTimeFormat = (currentTimeFormat == TWELVE_HOUR_FORMAT) ? TWENTY_FOUR_HOUR_FORMAT : TWELVE_HOUR_FORMAT;
        InvalidateRect(hWnd, NULL, true);
        return 0;

    case WM_RBUTTONDOWN:
        currentTheme = (currentTheme == THEME_LIGHT) ? THEME_DARK : THEME_LIGHT;
        InvalidateRect(hWnd, NULL, true);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Get dimensions of client area
        RECT r;
        GetClientRect(hWnd, &r);

        // Create a custom font
        HFONT hFont = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));

        // Select the custom font and save the original font
        HFONT hFontOriginal = (HFONT)SelectObject(hdc, hFont);

        // Set background and text color based on current theme
        if (currentTheme == THEME_LIGHT)
        {
            SetBkColor(hdc, RGB(255, 255, 255));
            SetTextColor(hdc, RGB(0, 0, 0));
        }
        else
        {
            SetBkColor(hdc, RGB(25, 25, 25));
            SetTextColor(hdc, RGB(0, 255, 0));
        }

        // Fill background
        SetBkMode(hdc, OPAQUE);
        FillRect(hdc, &r, CreateSolidBrush(GetBkColor(hdc)));

        // Draw the current time
        std::wstring time = GetCurrentTimeString();
        DrawText(hdc, time.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        // Restore the original font and clean up
        SelectObject(hdc, hFontOriginal);
        DeleteObject(hFont);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}