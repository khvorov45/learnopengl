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
typedef float f32;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
) {
    //
    // SECTION Window with OpenGL context
    //

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

    wglMakeCurrent(device_context, opengl_context);

    // NOTE(sen) This is to avoid the white flash when the window is shown but
    // haven't yet received the paint background message
    ShowWindow(window, SW_SHOWMINIMIZED);
    ShowWindow(window, SW_SHOWNORMAL);

    //
    // SECTION Load OpenGL
    //

    PFNGLCREATESHADERPROC glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    PFNGLGENBUFFERSPROC glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    PFNGLBINDBUFFERPROC glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    PFNGLBUFFERDATAPROC glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    PFNGLSHADERSOURCEPROC glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    PFNGLCOMPILESHADERPROC glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    PFNGLGETSHADERIVPROC glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    PFNGLCREATEPROGRAMPROC glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    PFNGLATTACHSHADERPROC glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    PFNGLLINKPROGRAMPROC glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    PFNGLUSEPROGRAMPROC glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    PFNGLGETPROGRAMIVPROC glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    PFNGLDRAWARRAYSEXTPROC glDrawArrays = (PFNGLDRAWARRAYSEXTPROC)wglGetProcAddress("glDrawArrays");
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    PFNGLUNIFORM4FPROC glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
    PFNGLGENERATEMIPMAPPROC glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");

    //
    // SECTION Shaders
    //

    const char* vertex_shader_source = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aColor;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"
        "out vec3 ourColor;"
        "out vec2 TexCoord;"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos, 1.0);\n"
        "   ourColor = aColor;\n"
        "   TexCoord = aTexCoord;\n"
        "}\0";

    u32 vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    b32 vertex_success = false;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_success);

    const char* fragment_shader_source = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "in vec2 TexCoord;\n"
        "uniform sampler2D ourTexture;\n"
        "void main()\n"
        "{\n"
        "   FragColor = texture(ourTexture, TexCoord);\n"
        "}\0";

    u32 fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    b32 fragment_success = false;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_success);

    u32 shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    b32 program_success = false;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &program_success);

    //
    // SECTION Rectangle
    //

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    u32 vertex_array;
    glGenVertexArrays(1, &vertex_array);

    u32 vertex_buffer;
    glGenBuffers(1, &vertex_buffer);

    u32 element_buffer;
    glGenBuffers(1, &element_buffer);

    glBindVertexArray(vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(f32)));
    glEnableVertexAttribArray(2);

    u32 texture_width = 2;
    u32 texture_height = 2;
    u32 texture_bytes_per_row = texture_width * 4;
    u32* texture_data = VirtualAlloc(0, texture_height * texture_bytes_per_row, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    for (u32 texel_y = 0; texel_y < texture_height; ++texel_y) {
        for (u32 texel_x = 0; texel_x < texture_width; ++texel_x) {
            texture_data[texel_y * texture_width + texel_x] = 0x000000FF;
        }
    }
    texture_data[0] = 0x000000FF;
    texture_data[1] = 0x0000FF00;
    texture_data[2] = 0x00FF0000;
    texture_data[3] = 0xFF000000;

    u32 texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
    //glGenerateMipmap(GL_TEXTURE_2D);

    for (;;) {
        MSG message;
        while (PeekMessageW(&message, window, 0, 0, PM_REMOVE)) {
            DispatchMessageW(&message);
        }
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);

        glUseProgram(shader_program);

        glBindVertexArray(vertex_array);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SwapBuffers(device_context);
    }

    return 0;
}
