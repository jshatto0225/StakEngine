#pragma once

#include "Platform.h"

#ifdef SK_WINDOWS

#include <Windows.h>
#undef CreateWindow
#undef DestroyWindow
#undef CreateFile

#include "Types.h"
#include "Event.h"
#include "Window.h"

typedef const char *(WINAPI *WGLGETEXTENSIONSSTRINGEXTPROC)(void);
extern WGLGETEXTENSIONSSTRINGEXTPROC Stak_wglGetExtensionsStringEXT;
#define wglGetExtensionsStringEXT Stak_wglGetExtensionsStringEXT
typedef const char *(WINAPI *WGLGETEXTENSIONSSTRINGARBPROC)(HDC);
extern WGLGETEXTENSIONSSTRINGARBPROC Stak_wglGetExtensionsStringARB;
#define wglGetExtensionsStringARB Stak_wglGetExtensionsStringARB
typedef HGLRC(WINAPI *WGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int *);
extern WGLCREATECONTEXTATTRIBSARBPROC Stak_wglCreateContextAttribsARB;
#define wglCreateContextAttribsARB Stak_wglCreateContextAttribsARB
typedef BOOL(WINAPI *WGLSWAPINTERVALEXTPROC)(int);
extern WGLSWAPINTERVALEXTPROC Stak_wglSwapIntervalEXT;
#define wglSwapIntervalEXT Stak_wglSwapIntervalEXT
typedef BOOL(WINAPI *WGLGETPIXELFORMATATTRIBIVARBPROC)(HDC, int, int, UINT, const int *, int *);
extern WGLGETPIXELFORMATATTRIBIVARBPROC Stak_wglGetPixelFormatAttribivARB;
#define wglGetPixelFormatAttribivARB Stak_wglGetPixelFormatAttribivARB
typedef BOOL(WINAPI *WGLCHOOSEPIXELFORMATARB)(HDC, const int *, const FLOAT *, UINT, int *, UINT *);
extern WGLCHOOSEPIXELFORMATARB Stak_wglChoosePixelFormatARB;
#define wglChoosePixelFormatARB Stak_wglChoosePixelFormatARB
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

typedef BOOL(WINAPI *TRANSLATEMESSAGEPROC)(const MSG *);
extern TRANSLATEMESSAGEPROC Stak_TranslateMessage;
#define TranslateMessage Stak_TranslateMessage
typedef LRESULT(WINAPI *DISPATCHMESSAGEAPROC)(const MSG *);
extern DISPATCHMESSAGEAPROC Stak_DispatchMessageA;
#define DispatchMessageA Stak_DispatchMessageA
typedef BOOL(WINAPI *PEEKMESSAGEAPROC)(LPMSG, HWND, UINT, UINT, UINT);
extern PEEKMESSAGEAPROC Stak_PeekMessageA;
#define PeekMessageA Stak_PeekMessageA
typedef LRESULT(WINAPI *DEFWINDOWPROCAPROC)(HWND, UINT, WPARAM, LPARAM);
extern DEFWINDOWPROCAPROC Stak_DefWindowProcA;
#define DefWindowProcA Stak_DefWindowProcA
typedef BOOL(WINAPI *UNREGISTERCLASSAPROC)(LPCSTR, HINSTANCE);
extern UNREGISTERCLASSAPROC Stak_UnregisterClassA;
#define UnregisterClassA Stak_UnregisterClassA
typedef ATOM(WINAPI *REGISTERCLASSEXAPROC)(WNDCLASSEXA *);
extern REGISTERCLASSEXAPROC Stak_RegisterClassExA;
#define RegisterClassExA Stak_RegisterClassExA
typedef HWND(WINAPI *CREATEWINDOWEXAPROC)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
extern CREATEWINDOWEXAPROC Stak_CreateWindowExA;
#define CreateWindowExA Stak_CreateWindowExA
typedef BOOL(WINAPI *DESTROYWINDOWPROC)(HWND);
extern DESTROYWINDOWPROC Stak_Win32DestroyWindow;
#define Win32DestroyWindow Stak_Win32DestroyWindow
typedef BOOL(WINAPI *SETWINDOWPOSPROC)(HWND, HWND, int, int, int, int, UINT);
extern SETWINDOWPOSPROC Stak_SetWindowPos;
#define Win32SetWindowPos Stak_SetWindowPos
typedef SHORT(WINAPI *GETASYNCKEYSTATEPROC)(int);
extern GETASYNCKEYSTATEPROC Stak_GetAsyncKeyState;
#define GetAsyncKeyState Stak_GetAsyncKeyState
typedef UINT_PTR(WINAPI *SETTIMERPROC)(HWND, UINT_PTR, UINT, TIMERPROC);
extern SETTIMERPROC Stak_SetTimer;
#define SetTimer Stak_SetTimer
typedef BOOL(WINAPI *KILLTIMERPROC)(HWND, UINT_PTR);
extern KILLTIMERPROC Stak_KillTimer;
#define KillTimer Stak_KillTimer
typedef HDC(WINAPI *BEGINPAINTPROC)(HWND, LPPAINTSTRUCT);
extern BEGINPAINTPROC Stak_BeginPaint;
#define BeginPaint Stak_BeginPaint
typedef BOOL(WINAPI *ENDPAINTPROC)(HWND, const PAINTSTRUCT *);
extern ENDPAINTPROC Stak_EndPaint;
#define EndPaint Stak_EndPaint
typedef BOOL(WINAPI *REDRAWWINDOWPROC)(HWND, const RECT *, HRGN, UINT);
extern REDRAWWINDOWPROC Stak_RedrawWindow;
#define RedrawWindow Stak_RedrawWindow
typedef BOOL(WINAPI *GETCLIENTRECTPROC)(HWND, LPRECT);
extern GETCLIENTRECTPROC Stak_GetClientRect;
#define GetClientRect Stak_GetClientRect
typedef LONG_PTR(WINAPI *GETWINDOWLONGPTRAPROC)(HWND, int);
extern GETWINDOWLONGPTRAPROC Stak_GetWindowLongPtrA;
#define GetWindowLongPtrA Stak_GetWindowLongPtrA
typedef LONG_PTR(WINAPI *SETWINDOWLONGPTRAPROC)(HWND, int, LONG_PTR);
extern SETWINDOWLONGPTRAPROC Stak_SetWindowLongPtrA;
#define SetWindowLongPtrA Stak_SetWindowLongPtrA
typedef HCURSOR(WINAPI *LOADCURSORWPROC)(HINSTANCE, LPCWSTR);
extern LOADCURSORWPROC Stak_LoadCursorW;
#define LoadCursorW Stak_LoadCursorW
typedef HICON(WINAPI *LOADICONWPROC)(HINSTANCE, LPCWSTR);
extern LOADICONWPROC Stak_LoadIconW;
#define LoadIconW Stak_LoadIconW
typedef int(WINAPI *CHOOSEPIXELFORMATPROC)(HDC, const PIXELFORMATDESCRIPTOR *);
extern CHOOSEPIXELFORMATPROC Stak_ChoosePixelFormat;
#define ChoosePixelFormat Stak_ChoosePixelFormat
typedef BOOL(WINAPI *GETCURSORPOSPROC)(LPPOINT);
extern GETCURSORPOSPROC Stak_GetCursorPos;
#define GetCursorPos Stak_GetCursorPos

typedef BOOL(WINAPI *SETPIXELFORMATPROC)(HDC, int, const PIXELFORMATDESCRIPTOR *);
extern SETPIXELFORMATPROC Stak_SetPixelFormat;
#define SetPixelFormat Stak_SetPixelFormat
typedef BOOL(WINAPI *SWAPBUFFERSPROC)(HDC);
extern SWAPBUFFERSPROC Stak_SwapBuffers;
#define SwapBuffers Stak_SwapBuffers
typedef HDC(WINAPI *GETDCPROC)(HWND);
extern GETDCPROC Stak_GetDC;
#define GetDC Stak_GetDC

constexpr const char *WIN32_DEFAULT_WNDCLASS_NAME = "SK_DEFAULT_WNDCLASS";

LRESULT CALLBACK Win32MessageCallback(HWND, UINT, WPARAM, LPARAM);

struct platform
{
  HINSTANCE Instance;
  ATOM DefaultWindowClass;
  HWND DummyWindow;
  bool Initialized;
  HINSTANCE RendererApi;
  HINSTANCE User32;
  HINSTANCE Gdi32;
};

struct window
{
  HWND Handle;
  i32 X;
  i32 Y;
  i32 Width;
  i32 Height;
  const char *Title;
  EventFn EventFunc;
  context *Context;
  void *Parent;
};

extern platform Platform;

#endif