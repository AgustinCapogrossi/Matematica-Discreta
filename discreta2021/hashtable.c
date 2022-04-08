#include "hashtable.h"
#include <stdlib.h>
#include <assert.h>

//#define hashfunc(key, buckets) ((key)*((key)+3)) %(mod) (buckets)

//profesor recomienda 2 o 3 veces los buckets necesarios para almacenar la informacion de las aristas (en nuestro caso, lo usamos como un diccionario (u32 NombredelVertice) -> (vertice*)

static u32 BUCKETS;

u32 INSERTS;

u32 cuantosinserts() {
	return (INSERTS);
}

plista* createTable(u32 buckets) {

	//printf("Creando tabla hash\n");

	plista* ret;

	BUCKETS = buckets;

	INSERTS = 0;

	size_t tableSize = sizeof(plista);

	//ret = (void*)malloc(tableSize);

	ret = (plista*)calloc((size_t)buckets, tableSize);

	if (ret == NULL) {
		return (NULL);
	}

	//assert(ret != NULL);

	for (u32 i = 0; i < buckets; ++i) {
		ret[i] = crearlista();
	}

	return (ret);

	//if (ret != NULL) {
	//	return (ret);
	//}

	//return (NULL);

	//retorna array: ret[0..buckets-1]
	//donde *(ret + hashfunc(nombreVertice)) devuelte un puntero void* (que hay que castear a (Vertice*)) a un vertice.
}

u32 insertVertice(plista* hashTable, Vertice addrVertice) {

	assert(BUCKETS != 0);

	
	
	//u64 hashp = hashfunc(((u64)addrVertice->nombre), ((u64)BUCKETS));
	//u32 hash = (u32)hashp;
	u32 hash = hashfunc((addrVertice->nombre), (BUCKETS));

	if (hashTable[hash] != NULL) { //Para compararlo con NULL; hay que asegurarse que toda la tablahash este inicializada en 0, porque NULL == (void)0

		//printf("InsertVertice(): insertando nodo en hash:%u\n", hash);
		insertarnodo(hashTable[hash], addrVertice);
		++INSERTS;

		//printf("Nodo insertado\n");

		//printf("Nodo nombre en tabla hash:%u\n", (buscarnodo(hashTable[hash], addrVertice->nombre))->valor->nombre);
	}
		

	//assert(hashTable[hash] == NULL);

	return hash; //solo para diagnostico?
}

Vertice getVertice(plista* hashTable, u32 nombreVertice) {

	//assert(BUCKETS != 0);

	u32 hash = hashfunc(nombreVertice, BUCKETS);

	plista list = hashTable[hash];

	Vertice res = buscarnodo(list, nombreVertice)->valor;

	return (res);

}

void destroyTable(plista* hashT) {
	if (hashT != NULL) {
		for (u32 i = 0; i < BUCKETS; ++i) {
			if (hashT[i] != NULL) {
				if (hashT[i]->cantidad != 0) {
					destruirlistacont(hashT[i]);
				}
				//sino solamente destruimos la cabecera
				free(hashT[i]);
			}
		}

		free(hashT);
	}
	

	//free(hashT);
	hashT = NULL;
	
}

Vertice existeVertice(plista* hashTable, u32 const nombreVertice) {
	//retorna NULL si no existe en la tabla

	Vertice r = NULL;
	pnodo s = NULL;

	//printf("Buscando existencia de %u\n", nombreVertice);

	u32 hash = hashfunc(nombreVertice, BUCKETS);

	/*
	u64 hashp = hashfunc(((u64)nombreVertice), ((u64)BUCKETS));
	u32 hash = (u32)hashp;
	*/

	//printf("Hash existencia:%u\n", hash);

	if (hashTable[hash] != NULL) {
		//printf("Existe a nivel 1\n");
		s = buscarnodo(hashTable[hash], nombreVertice);
		if (s != NULL) {
			r = s->valor;
		}
	}

	return (r);
}
