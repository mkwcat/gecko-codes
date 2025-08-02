#pragma once

// Type definitions
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

typedef float f32;
typedef double f64;

#ifdef CLANGD
typedef unsigned long size_t;
#else
typedef unsigned int size_t;
#endif

typedef int BOOL;
#define TRUE 1
#define FALSE 0
