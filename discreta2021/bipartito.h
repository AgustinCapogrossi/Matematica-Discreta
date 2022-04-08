#ifndef BIPARTITO_H
#define BIPARTITO_H

//#include "GrafoSt21.h"
#include "UnleashHell.h"
#include "greedy.h"

struct queue_s{

    Vertice* queuep;
    u32 size;
    u32 slotsocupados;
    u32 head;
    u32 tail;

};

typedef struct queue_s* queue;

struct queue_st{

    u32* queuep;
    u32 size;
    u32 slotsocupados;
    u32 head;
    u32 tail;

};

typedef struct queue_st* queue2;




/// <summary>
/// Devuelve 1 si G es bipartito, 0 si no
/// </summary>
/// <remarks>
/// Ademas, si devuelve 1, colorea G con un coloreo propio de dos colores.
/// Si devuelve 0, debe dejar a G coloreado con algun coloreo propio (no necesariamente el mismo que tenia al principio).
/// </remarks>
/// <returns> 1 si es bipartito. 0 si no es bipartito</returns>
/// TODO: Falta decidir que algoritmo usar
/// *Es este algoritmo pero corregido, porque ese tiene errores: https://www.baeldung.com/cs/graphs-bipartite -> BFS O(n vertices + m lados)
#ifndef __uh_H
char Bipartito_directo(Grafo G);
char Bipartito(Grafo G);
#endif

#endif