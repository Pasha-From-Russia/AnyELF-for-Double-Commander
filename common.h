#ifndef _COMMON_H
#define _COMMON_H

#ifdef __GNUC__

#include <stdint.h>

#if defined(__WIN32__) || defined(_WIN32) || defined(_WIN64)
    #define DCPCALL __attribute__((stdcall))
#else
    #define DCPCALL
#endif

#define MAX_PATH 260

typedef int32_t LONG;
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef void *HANDLE;
typedef HANDLE HICON;
typedef HANDLE HBITMAP;
typedef HANDLE HWND;
typedef int BOOL;
typedef char CHAR;
typedef uint16_t WCHAR;
typedef intptr_t LPARAM;
typedef uintptr_t WPARAM;

#else

#if defined(_WIN32) || defined(_WIN64)
    #define DCPCALL __stdcall
#else
    #define DCPCALL __cdecl
#endif

#endif

#endif // _COMMON_H
