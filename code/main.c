#include "stdint.h"

#include "windows.h"

#define true 1
#define false 0

typedef uint32_t u32;
typedef int32_t i32;
typedef size_t usize;
typedef intptr_t isize;
typedef int32_t b32;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
) {
    // NOTE(sen) Create window
    HWND window = 0;
    {
        LPCWSTR class_name = L"WiredeckClass";
        WNDCLASSEXW window_class = {
            .cbSize = sizeof(WNDCLASSEXW),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = DefWindowProcW,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = hInstance,
            .hIcon = 0,
            .hCursor = 0,
            .hbrBackground = GetSysColorBrush(COLOR_BACKGROUND),
            .lpszMenuName = 0,
            .lpszClassName = class_name,
            .hIconSm = 0,
        };
        RegisterClassExW(&window_class);
        window = CreateWindowExW(
            0,
            class_name,
            L"LearnOpenGL",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            1280,
            720,
            0,
            0,
            hInstance,
            0
        );
        // NOTE(sen) This is to avoid the white flash when the window is shown but
        // haven't yet received the paint background message
        ShowWindow(window, SW_SHOWMINIMIZED);
        ShowWindow(window, SW_SHOWNORMAL);
    }

    return 0;
}
