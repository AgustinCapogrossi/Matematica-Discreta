#ifndef GREEDY_H
#define GREEDY_H

//#include "GrafoSt21.h"
#include "UnleashHell.h"
//#include "bitarray.h"


//Si no esta declarado UnleashHell, declaramos estas funciones como teniamos antes de corregir todo
#ifndef __uh_H

#if defined(__GNUC__) && !defined(__clang__)
//u32 Greedy(Grafo G)__attribute__ ((access (read_only, 1), hot, nonnull (1)));
u32 Greedy(Grafo G)__attribute__ ((hot, nonnull (1)));
#elif defined(__clang__)
u32 Greedy(Grafo G)__attribute__ ((hot, nonnull (1)));
#else
u32 Greedy(Grafo G);
#endif

#if defined(__GNUC__) && !defined(__clang__)
void resetColores(Grafo G)__attribute__ ((hot));
//u32 greedycoloresprimero(Grafo G, const Vertice* restrict orden) __attribute__ ((nonnull (1,2), access (read_only, 1)));
u32 greedycoloresprimero(Grafo G, const Vertice* restrict orden) __attribute__ ((nonnull (1,2)));
#elif defined(__clang__)
u32 greedycoloresprimero(Grafo G, const Vertice* restrict orden);
void resetColores(Grafo G)__attribute__ ((hot));
#else
void resetColores(Grafo G);
u32 greedycoloresprimero(Grafo G, const Vertice* restrict orden);
#endif

char escoloreopropio(Grafo G);

#endif

//Este greedy es otra manera de hacer lo mismo, pero es mas ineficiente en general. A veces es mas rapido. Lo usamos para comprobar que Greedy() funcione bien
u32 greedynaive(Grafo G, const Vertice* restrict orden);
void resetColores(Grafo G);
char escoloreopropio(Grafo G);

///Retorna 1 si el grafo tiene un coloreo propio. Retorna 0 si el coloreo no es propio.


#endif // !GREEDY_H

