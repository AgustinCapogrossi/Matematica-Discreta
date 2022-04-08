#ifndef HASHTABLE_B_H
#define HASHTABLE_B_H

#include "u32.h"
#include <stdint.h>
#include <stdlib.h>
#include "vertice.h"
#define hash2(key, buckets) (((key)*((key)+3)) % (buckets))

typedef unsigned short int u16;
typedef uint64_t u64;

struct hashtable{
    Vertice* arr;
    u32 currsize;
};
typedef struct hashtable hashtable_t;

struct vertOrd{
    u32 nombre;
    u32 posOrden;
};

typedef struct vertOrd vertOrd_t; // no usamos un puntero porque preferimos tener todo en el mismo bloque de memoria.

struct hashtable2{
    vertOrd_t* arr;
    u32 currsize;
};
typedef struct hashtable2 hashtable2_t;

// struct hopnode{
//     Vertice vert_p;
//     u16 hops;
// };

struct hopnode{
    Vertice vert_p;
    u32 hops;
};

typedef struct hopnode hopnode_t;


u64 hashfuncfunc(u32 key, u32 buckets);

#if defined(__GNUC__) && !defined(__clang__)
//todo esto es para ayudar a gcc, aunque en las pruebas no hizo diferencia con -Ofast -march=native
__attribute__ ((malloc)) hashtable_t* inicializartabla(u32 sizeTable);
__attribute__ ((malloc)) hashtable2_t* inicializartabla2(u32 sizeTable);
__attribute__((hot, nonnull (1, 2))) char insertarelemento(hashtable_t* tabla, Vertice vert);
__attribute__((hot, nonnull (1))) char insertarelemento2(hashtable2_t* tabla, u32 nombre , u32 posOrden);
//__attribute__((hot, nonnull (1), access (read_only, 1, 2))) Vertice buscarelemento(hashtable_t* tabla, u32 nombrevertice);
__attribute__((hot, nonnull (1))) Vertice buscarelemento(hashtable_t* tabla, u32 nombrevertice);
__attribute__((hot, nonnull (1))) u32 buscarelemento2(hashtable2_t* tabla, u32 nombrevertice);
__attribute__ ((pure))u32 hash1(u32 key);
void destruirtablahash(hashtable_t* tabla);
void destruirtablahash2(hashtable2_t* tabla);
#elif defined(__clang__)
__attribute__ ((malloc)) hashtable_t* inicializartabla(u32 sizeTable);
__attribute__ ((malloc)) hashtable2_t* inicializartabla2(u32 sizeTable);
__attribute__((hot, nonnull (1, 2))) char insertarelemento(hashtable_t* tabla, Vertice vert);
__attribute__((hot, nonnull (1))) char insertarelemento2(hashtable2_t* tabla, u32 nombre , u32 posOrden);
__attribute__((hot, nonnull (1))) Vertice buscarelemento(hashtable_t* tabla, u32 nombrevertice);
__attribute__((hot, nonnull (1))) u32 buscarelemento2(hashtable2_t* tabla, u32 nombrevertice);
__attribute__ ((pure))u32 hash1(u32 key);
void destruirtablahash(hashtable_t* tabla);
void destruirtablahash2(hashtable2_t* tabla);

#else
hashtable_t* inicializartabla(u32 sizeTable);
char insertarelemento(hashtable_t* tabla, Vertice vert);
Vertice buscarelemento(hashtable_t* tabla, u32 nombrevertice);
void destruirtablahash(hashtable_t* tabla);
u32 hash1(u32 key);

hashtable2_t* inicializartabla2(u32 sizeTable);
char insertarelemento2(hashtable2_t* tabla, u32 nombre , u32 posOrden);
u32 buscarelemento2(hashtable2_t* tabla, u32 nombrevertice);
void destruirtablahash2(hashtable2_t* tabla);

#endif

void setbit(u16* br, u32 pos);

#if defined(__GNUC__) && !defined(__clang__)
__attribute__ ((pure))u32 hash1_hs(u32 key);
__attribute__ ((malloc)) hopnode_t* inicializarhs(u32 size);
__attribute__((hot, nonnull (1, 2))) char insertarelementohs(hopnode_t* tabla, Vertice vert);
//__attribute__((hot, nonnull (1), access (read_only, 1)))Vertice buscarelementohs(hopnode_t* tabla, u32 nombrevert);
__attribute__((hot, nonnull (1)))Vertice buscarelementohs(hopnode_t* tabla, u32 nombrevert);

#elif defined(__clang__)

__attribute__ ((pure))u32 hash1_hs(u32 key);
__attribute__ ((malloc)) hopnode_t* inicializarhs(u32 size);
__attribute__((hot, nonnull (1, 2))) char insertarelementohs(hopnode_t* tabla, Vertice vert);
__attribute__((hot, nonnull (1)))Vertice buscarelementohs(hopnode_t* tabla, u32 nombrevert);


#else

u32 hash1_hs(u32 key);
hopnode_t* inicializarhs(u32 size);
char insertarelementohs(hopnode_t* tabla, Vertice vert);
Vertice buscarelementohs(hopnode_t* tabla, u32 nombrevert);


#endif

void destruirtablahashhs(hopnode_t* tabla);

__attribute__((hot, nonnull (1))) u32 findxbit1en32(u32* solo32bits, u16 iesimo);
char getbitinpositionx32(u32 bits, u32 pos);
void setbit32(u32* br, u32 pos);

u64 int64hash(u64 u);
u32 int32hash(u64 u);
u32 hash3_hs(u32 key);

#endif