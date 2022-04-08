#ifndef REORDEN_H
#define REORDEN_H
#include "UnleashHell.h"
//#include "GrafoSt21.h"
//#include "u32.h"


/*
Verifica que perm sea una permutación de {0, 1, ..., r-1}, donde r es el numero de colores que tiene G. Retorna 0 si no lo es.
Si es una permutacion, ordena los vertices G segun sus colores, de acuerdo al orden que indica perm y retorna 1.
*/

//Estas son ayudas para que el compilador tenga mas informacion de cada funcion, para que optimice mejor

#ifndef __uh_H

#if defined(__GNUC__) && !defined(__clang__)
char OrdenPorBloqueDeColores(Grafo G,u32* perm);
char OrdenPorBloqueDeColores_arraysaccesodirecto(Grafo restrict G, u32* restrict perm);
//char OrdenPorBloqueDeColoresarr(Grafo G,u32 const* perm) __attribute__((hot, nonnull (1,2)));
#elif defined(__clang__)
char OrdenPorBloqueDeColores_arraysaccesodirecto(Grafo restrict G, u32* restrict perm);
char OrdenPorBloqueDeColores(Grafo G,u32* perm);
//char OrdenPorBloqueDeColoresarr(Grafo G,u32 const* perm) __attribute__((hot, nonnull (1,2)));
#else
char OrdenPorBloqueDeColores(Grafo G,u32* perm);
char OrdenPorBloqueDeColores_arraysaccesodirecto(Grafo restrict G, u32* restrict perm);
//char OrdenPorBloqueDeColoresarr(Grafo G,u32 const* perm);
#endif

#endif

//char OrdenPorBloqueDeColoresarr(Grafo G,u32 const* perm) __attribute__((hot, nonnull (1,2)));

#endif