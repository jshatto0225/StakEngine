#pragma once

#include <windows.h>

#undef CreateWindow
#undef DestroyWindow

#include "Types.h"
#include "Event.h"

#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_TYPE_RGBA_ARB 0x202b
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x0001

typedef const char *(WINAPI *PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
typedef const char *(WINAPI *PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const i32 *);
typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(i32);
typedef BOOL(WINAPI *PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC, i32, i32, u32, const i32 *, i32 *);
typedef BOOL(WINAPI *PFNWGLCHOOSEPIXELFORMATARB)(HDC, const i32 *, const f32 *, u32, i32 *, u32 *);

extern PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;
extern PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
extern PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB;
extern PFNWGLCHOOSEPIXELFORMATARB wglChoosePixelFormatARB;

typedef HGLRC(WINAPI *WGLCREATECONTEXTPROC)(HDC);
extern WGLCREATECONTEXTPROC Stak_wglCreateContext;
#define wglCreateContext Stak_wglCreateContext
typedef BOOL(WINAPI *WGLDELETECONTEXTPROC)(HGLRC);
extern WGLDELETECONTEXTPROC Stak_wglDeleteContext;
#define wglDeleteContext Stak_wglDeleteContext
typedef HDC(WINAPI *WGLGETCURRENTDCPROC)();
extern WGLGETCURRENTDCPROC Stak_wglGetCurrentDC;
#define wglGetCurrentDC Stak_wglGetCurrentDC
typedef PROC(WINAPI *WGLGETPROCADDRESSPROC)(LPCSTR);
extern WGLGETPROCADDRESSPROC Stak_wglGetProcAddress;
#define wglGetProcAddress Stak_wglGetProcAddress
typedef HGLRC(WINAPI *WGLGETCURRENTCONTEXTPROC)();
extern WGLGETCURRENTCONTEXTPROC Stak_wglGetCurrentContext;
#define wglGetCurrentContext Stak_wglGetCurrentContext
typedef BOOL(WINAPI *WGLMAKECURRENTPROC)(HDC, HGLRC);
extern WGLMAKECURRENTPROC Stak_wglMakeCurrent;
#define wglMakeCurrent Stak_wglMakeCurrent
typedef BOOL(__stdcall *TRANSLATEMESSAGEPROC)(const MSG *);
extern TRANSLATEMESSAGEPROC Stak_TranslateMessage;
#define TranslateMessage Stak_TranslateMessage
typedef LRESULT(__stdcall *DISPATCHMESSAGEAPROC)(const MSG *);
extern DISPATCHMESSAGEAPROC Stak_DispatchMessageA;
#define DispatchMessageA Stak_DispatchMessageA
typedef BOOL(__stdcall *PEEKMESSAGEAPROC)(LPMSG, HWND, UINT, UINT, UINT);
extern PEEKMESSAGEAPROC Stak_PeekMessageA;
#define PeekMessageA Stak_PeekMessageA
typedef LRESULT(__stdcall *DEFWINDOWPROCAPROC)(HWND, UINT, WPARAM, LPARAM);
extern DEFWINDOWPROCAPROC Stak_DefWindowProcA;
#define DefWindowProcA Stak_DefWindowProcA
typedef BOOL(__stdcall *UNREGISTERCLASSAPROC)(LPCSTR, HINSTANCE);
extern UNREGISTERCLASSAPROC Stak_UnregisterClassA;
#define UnregisterClassA Stak_UnregisterClassA
typedef ATOM(__stdcall *REGISTERCLASSEXAPROC)(WNDCLASSEXA *);
extern REGISTERCLASSEXAPROC Stak_RegisterClassExA;
#define RegisterClassExA Stak_RegisterClassExA
typedef HWND(__stdcall *CREATEWINDOWEXAPROC)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
extern CREATEWINDOWEXAPROC Stak_CreateWindowExA;
#define CreateWindowExA Stak_CreateWindowExA
typedef BOOL(__stdcall *DESTROYWINDOWPROC)(HWND);
extern DESTROYWINDOWPROC Stak_Win32DestroyWindow;
#define Win32DestroyWindow Stak_Win32DestroyWindow
typedef BOOL(__stdcall *SETWINDOWPOSPROC)(HWND, HWND, int, int, int, int, UINT);
extern SETWINDOWPOSPROC Stak_SetWindowPos;
#define Win32SetWindowPos Stak_SetWindowPos
typedef SHORT(__stdcall *GETASYNCKEYSTATEPROC)(int);
extern GETASYNCKEYSTATEPROC Stak_GetAsyncKeyState;
#define GetAsyncKeyState Stak_GetAsyncKeyState
typedef UINT_PTR(__stdcall *SETTIMERPROC)(HWND, UINT_PTR, UINT, TIMERPROC);
extern SETTIMERPROC Stak_SetTimer;
#define SetTimer Stak_SetTimer
typedef BOOL(__stdcall *KILLTIMERPROC)(HWND, UINT_PTR);
extern KILLTIMERPROC Stak_KillTimer;
#define KillTimer Stak_KillTimer
typedef HDC(__stdcall *BEGINPAINTPROC)(HWND, LPPAINTSTRUCT);
extern BEGINPAINTPROC Stak_BeginPaint;
#define BeginPaint Stak_BeginPaint
typedef BOOL(__stdcall *ENDPAINTPROC)(HWND, const PAINTSTRUCT *);
extern ENDPAINTPROC Stak_EndPaint;
#define EndPaint Stak_EndPaint
typedef BOOL(__stdcall *REDRAWWINDOWPROC)(HWND, const RECT *, HRGN, UINT);
extern REDRAWWINDOWPROC Stak_RedrawWindow;
#define RedrawWindow Stak_RedrawWindow
typedef BOOL(__stdcall *GETCLIENTRECTPROC)(HWND, LPRECT);
extern GETCLIENTRECTPROC Stak_GetClientRect;
#define GetClientRect Stak_GetClientRect
typedef LONG_PTR(__stdcall *GETWINDOWLONGPTRAPROC)(HWND, int);
extern GETWINDOWLONGPTRAPROC Stak_GetWindowLongPtrA;
#define GetWindowLongPtrA Stak_GetWindowLongPtrA
typedef LONG_PTR(__stdcall *SETWINDOWLONGPTRAPROC)(HWND, int, LONG_PTR);
extern SETWINDOWLONGPTRAPROC Stak_SetWindowLongPtrA;
#define SetWindowLongPtrA Stak_SetWindowLongPtrA
typedef HCURSOR(__stdcall *LOADCURSORWPROC)(HINSTANCE, LPCWSTR);
extern LOADCURSORWPROC Stak_LoadCursorW;
#define LoadCursorW Stak_LoadCursorW
typedef HICON(__stdcall *LOADICONWPROC)(HINSTANCE, LPCWSTR);
extern LOADICONWPROC Stak_LoadIconW;
#define LoadIconW Stak_LoadIconW
typedef int(__stdcall *CHOOSEPIXELFORMATPROC)(HDC, const PIXELFORMATDESCRIPTOR *);
extern CHOOSEPIXELFORMATPROC Stak_ChoosePixelFormat;
#define ChoosePixelFormat Stak_ChoosePixelFormat
typedef BOOL(__stdcall *SETPIXELFORMATPROC)(HDC, int, const PIXELFORMATDESCRIPTOR *);
extern SETPIXELFORMATPROC Stak_SetPixelFormat;
#define SetPixelFormat Stak_SetPixelFormat
typedef BOOL(__stdcall *SWAPBUFFERSPROC)(HDC);
extern SWAPBUFFERSPROC Stak_SwapBuffers;
#define SwapBuffers Stak_SwapBuffers
typedef HDC(__stdcall *GETDCPROC)(HWND);
extern GETDCPROC Stak_GetDC;
#define GetDC Stak_GetDC

struct platform {
  HINSTANCE Instance;
  ATOM DefaultWindowClass;
  HWND DummyWindow;
  bool Initialized;
  HINSTANCE RendererApi;
  HINSTANCE User32;
  HINSTANCE Gdi32;
};

struct window {
  HWND Handle;
  i32 X;
  i32 Y;
  i32 Width;
  i32 Height;
  const char *Title;
  void(*EventFn)(void *, const event *);
  context *Context;
  void *Parent;
};

extern platform Platform;