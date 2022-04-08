#ifndef __uh_H
#define __uh_H
/* funciones de las partes 2 y 3 del 2021 */

#include <stdio.h>
#include <stdlib.h>

#include "RomaVictor.h"


//pseudo-aleatoriza vertices usando semilla R.
char AleatorizarVertices(Grafo G,u32 R);

//coloreo
char Bipartito(Grafo G);
u32 Greedy(Grafo G);
//hasta arriba, parte 2.

//abajo, parte 3

/*Ordenar los vertices por colores, poniendo los vertices 
de color perm[0] primero, luego los de color perm[1], etc....*/

char OrdenPorBloqueDeColores(Grafo G,u32* perm);

#endif
