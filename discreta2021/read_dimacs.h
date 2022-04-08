#ifndef READ_DIMACS_H
#define READ_DIMACS_H

#include <stdio.h>
#include "u32.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "vertice.h"
//#include "listaenlazada.h"
#include "RomaVictor.h"
#include "GrafoSt21.h"
#include "orden.h"
#include "hashtable_b.h"

#define DIMACS_MAXLINE_SIZE 1024
#define LONGITUDCIFRA 120


//void testHash();
Grafo read_dimacs();

#endif // !READ_DIMACS_H