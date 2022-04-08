//#pragma once
#ifndef HASHTABLE
#define HASHTABLE
#include "u32.h"
#include <stdbool.h>
#include "listaenlazada.h"
#include "vertice.h"

typedef uint64_t u64;

//Knuth Variant on Division h(k) = k(k+3) mod m. k == key; m == number of buckets.
#define hashfunc(key, buckets) (((key)*((key)+3)) % (buckets))
//#define hashfunc(key, buckets) (key) % (buckets)


//struct VerticeSt {
//	u32 nombre;
//	u32 grado;
//	u32 color;
//	u32 peso;
//	LArray vecinos;
//};

//profesor recomienda 2 o 3 veces los buckets necesarios para almacenar la informacion de las aristas (en nuestro caso, lo usamos como un diccionario (u32 NombredelVertice) -> (vertice*)
plista* createTable(u32 buckets);

u32 insertVertice(plista* hashTable, Vertice addrVertice);

Vertice getVertice(plista* hashTable, u32 nombreVertice);

Vertice existeVertice(plista* hashTable, u32 nombreVertice);

void destroyTable(plista* hashTable);

u32 cuantosinserts();



#endif // !HASHTABLE

