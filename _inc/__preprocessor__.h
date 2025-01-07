#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// #define var(x) cout << #x << " = " << x << '\n';
// #define varr(x) cout << #x << " = " << x << ' ';
// #define line(x) cout << x << '\n';
// #define linee(x) cout << x << ' ';
// #define nline cout << '\n';

#ifdef __cplusplus
extern "C" {
#endif

int my_sum(int a, int b);

#ifdef __cplusplus
}
#endif

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define null nullptr
#define pow2(x) ((x) * (x))
#define base_0(x) (x - 1)

#define FATAL_ERROR(x) { printf("FATAL ERROR: %s\n", x); exit(EXIT_FAILURE); }

#define ASSERT_ER_IF_TRUE(x) if(x) FATAL_ERROR(#x)
#define ASSERT_ER_IF_NULL(x) if(x == null) FATAL_ERROR(#x)

#define SAFETY_CHECK(x) x;
