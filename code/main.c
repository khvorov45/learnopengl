#include "stdint.h"

#include "windows.h"

#include "GL/GL.h"
#include "GL/glext.h"
#include "GL/wglext.h"

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
    LPCWSTR class_name = L"WindowClass";
    WNDCLASSEXW window_class = {
        .cbSize = sizeof(WNDCLASSEXW),
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
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

    HWND window_fake = CreateWindowExW(
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

    PIXELFORMATDESCRIPTOR pixel_format_fake =
    {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 32,
        .cRedBits = 0,
        .cRedShift = 0,
        .cGreenBits = 0,
        .cGreenShift = 0,
        .cBlueBits = 0,
        .cBlueShift = 0,
        .cAlphaBits = 0,
        .cAlphaShift = 0,
        .cAccumBits = 0,
        .cAccumRedBits = 0,
        .cAccumGreenBits = 0,
        .cAccumBlueBits = 0,
        .cAccumAlphaBits = 0,
        .cDepthBits = 24,
        .cStencilBits = 8,
        .cAuxBuffers = 0,
        .iLayerType = PFD_MAIN_PLANE,
        .bReserved = 0,
        .dwLayerMask = 0,
        .dwVisibleMask = 0,
        .dwDamageMask = 0,
    };

    HDC device_context_fake = GetDC(window_fake);
    i32 pixel_format_fake_index = ChoosePixelFormat(device_context_fake, &pixel_format_fake);
    SetPixelFormat(device_context_fake, pixel_format_fake_index, &pixel_format_fake);

    HGLRC opengl_context_fake = wglCreateContext(device_context_fake);
    wglMakeCurrent(device_context_fake, opengl_context_fake);

    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
        (PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB"));
    PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
        (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    HWND window = CreateWindowExW(
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

    HDC device_context = GetDC(window);

    i32 pixel_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
    };

    i32 pixel_format_id;
    u32 n_formats;
    b32 status = wglChoosePixelFormatARB(device_context, pixel_attribs, NULL, 1, &pixel_format_id, &n_formats);

    PIXELFORMATDESCRIPTOR pixel_format;
    DescribePixelFormat(device_context, pixel_format_id, sizeof(pixel_format), &pixel_format);
    SetPixelFormat(device_context, pixel_format_id, &pixel_format);

    i32 major_min = 4;
    i32 minor_min = 5;
    i32 contextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    HGLRC opengl_context = wglCreateContextAttribsARB(device_context, 0, contextAttribs);

    wglMakeCurrent(0, 0);
    wglDeleteContext(opengl_context_fake);
    ReleaseDC(window_fake, device_context_fake);
    DestroyWindow(window_fake);

    b32 make_current_result = wglMakeCurrent(device_context, opengl_context);

    // NOTE(sen) This is to avoid the white flash when the window is shown but
    // haven't yet received the paint background message
    ShowWindow(window, SW_SHOWMINIMIZED);
    ShowWindow(window, SW_SHOWNORMAL);

    for (;;) {
        MSG message;
        while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) {
            DispatchMessageW(&message);
        }
        glClearColor(1.0f, 0.3f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        SwapBuffers(device_context);
    }

    return 0;
}
