#ifndef ORDEN_H
#define ORDEN_H

#include "vertice.h"
#include <stdio.h>

/*Intercambia la posicion de dos elementos en un arreglo*/
void swap(Vertice* orden, u32 x, u32 y);

/*Ordena los elementos del arreglo en el orden natural ascendente mediante un insertion sort*/
Vertice *isOrdenNatural(Vertice* orden, u32 size);

//Vertice* qsOrdenNatural(Vertice* orden, long long int izq, long long int der);

Vertice* ordenqsort(Vertice* orden, u32 size);

Vertice* randomizarorden(Vertice* restrict orden, u32 size)__attribute__ ((hot, nonnull (1)));

Vertice* randomizarordenresetcolor(Vertice* orden, u32 size);

void randomizarordenconseed_directo(Vertice* restrict orden, u32 size, unsigned int seed);




#endif