#ifndef BITARRAY_H
#define BITARRAY_H

#include <stdlib.h>
/*
#ifdef __GNUC__

#include <x86intrin.h>

#else

#include <intrin.h>

#endif
*/
/*
#ifdef __GNUC__
#ifndef __clang__

#include <x86intrin.h>

#endif // !ifndef __clang__

#else
#ifdef __clang__

//#include <intrin.h>

#include <x86intrin.h>

#endif

#endif

#include <stdint.h>
//#include <assert.h>

#ifdef __INTEL_COMPILER
#include <nmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>
#endif

#ifdef _MSC_VER
#include <nmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>
#endif

#ifdef __clang__
#include <nmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>
#endif

#ifdef __GNUC__
#include <nmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>
#endif
*/
#ifdef __GNUC__

#include <x86intrin.h>

#else

//#include <intrin.h>

#endif


#include <stdint.h>
#include <nmmintrin.h>
#include <smmintrin.h>
#include <immintrin.h>

#define WIDTH 64

typedef uint32_t u32;
typedef int32_t i32;
typedef uint_fast64_t u64;
typedef int_fast64_t i64;
typedef u64* bitarray;
typedef i64* bitarraysigned;

struct bitarray_st {
	u32 cantidadbits;
	u32 cantidadu64reservados;
	bitarray arr;
};

typedef struct bitarray_st* pbitarr;

/// <summary>
/// Crea un array de tama�o (size/64) [si size no es multiplo de 64, agrega un elemento m�s ] u64's.
/// Para mas informacion leer: http://www.mathcs.emory.edu/~cheung/Courses/255/Syllabus/1-C-intro/bit-array.html
/// </summary>
/// <param name="size"></param>
/// <returns>Puntero al array de u64's</returns>
bitarray createbitarray(u32 size, u32* uintscreados);
bitarraysigned createbitarraysigned(u32 size, u32* ints64creados);
bitarraysigned reallocbitarraysigned(bitarraysigned bas, u32 size, u32* ints64creados);

void destroybitarray(bitarray restrict br);
void destroybitarraysigned(bitarraysigned restrict br);

void setbitk(bitarray restrict br, u32  pos);
void clearbitk(bitarray restrict br, u32  pos);
void setbitkf(bitarraysigned restrict br, u32  pos);
void clearbitkf(bitarraysigned restrict br, u32  pos);
u32 getbittoint(bitarray restrict br, u32 pos);
u32 findfirst0(u64 const* restrict  br, u32  cantidadbits);

//#ifdef __POPCNT__

u32 findfirst0fast(u64 const* restrict  br, u32  cantidad_u64ints);
u32 findfirst1fast(i64 const* restrict  bitarr, u32  sizearray);
u32 findfirst1fastmod(i64 const* restrict  bitarr, u32 sizearray);

//#endif // !__POPCNT__


#endif // !BITARRAY_H