#ifndef GREEDYRAPIDO_H
#define GREEDYRAPIDO_H


/*
#ifdef __GNUC__
#ifndef __clang__

#include <x86intrin.h>

#endif

#else
#ifdef __clang__

#include <intrin.h>

#endif

#endif
*/

#include <stdint.h>
#include "GrafoSt21.h"
#include "vertice.h"
#include "bitarray.h"

#include "string.h" // para poder usar memset()

//#define __POPCNT__ 1


#ifdef __POPCNT__

u32 greedycoloresprimerobits(Grafo G, const Vertice* restrict orden);

#else

#ifdef __POPCNTINTRIN_H

u32 greedycoloresprimerobits(Grafo G, const Vertice* restrict orden);

#endif // !__POPCNTINTRIN_H


#endif // !__POPCNT__

#ifdef __SSE4_1__
//usa solo sse4.1 pero no todos los compiladores definen esa constante

u32 primeropintado(Vertice const* restrict  arrVecinos, u32  size);

#else

#ifdef __AVX__

u32 primeropintado(Vertice const* restrict const arrVecinos, u32 const size);

#endif //avx

#endif //sse4.1

//SOLO PARA DEBUGGEAR

//#define __SSE2__

//!SOLO PARA DEBUGGEAR

#ifdef __SSE2__

u32 primeropintadosse4ints(Vertice const* restrict  arrVecinos, u32  size);

i64 primeropintadosse4intsmod(Vertice const* restrict  arrVecinos, u32  size, u32  posicioninicial);

u32 greedycoloresprimerobitsallowed(Grafo G, const Vertice* restrict  orden);

#endif

#ifdef __AVX2__

u32 primeropintadoavx2(Vertice const* restrict arrVecinos, u32 size);
i64 primeropintadoavx2mod(Vertice const* restrict arrVecinos, u32 size, u32 posicioninicial);
u32 greedycoloresprimerobitsallowedavx2(Grafo G, const Vertice* restrict orden);
void primeropintadoavx2arr(Vertice const* restrict arrVecinos, u32 size, u32* arrayrespuesta, u32* sizerespuesta, u32 cantidadmaximaposible);
u32 greedycoloresprimerobitsallowedavx2arr(Grafo G, const Vertice* restrict orden);

#endif

#endif // !GREEDYRAPIDO_H