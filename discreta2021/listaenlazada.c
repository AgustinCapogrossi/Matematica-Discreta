#include "listaenlazada.h"
#include <stdlib.h>

plista crearlista() {

	plista newlista = (plista)malloc(sizeof(struct lista_s));
	
	if (newlista != NULL) {

		newlista->head = NULL;
		newlista->cantidad = 0;

	}

	return (newlista);
}

void destruirlista(plista list) {
	
	free(list);

}

void insertarnodo(plista list, Vertice vertr) {

	pnodo nodonuevo = (pnodo)malloc(sizeof(tnodo));

	if (nodonuevo != NULL) {

		nodonuevo->valor = (Vertice)vertr;
		nodonuevo->sig = list->head;
		list->head = nodonuevo;
		++list->cantidad;

		//printf("Nodo nombre:%u\n", vert->nombre);

	}


}


pnodo buscarnodo(plista list, u32 vert) {
	pnodo res = NULL;
	pnodo cursorlista = list->head;
	for (u32 i = 0; i < list->cantidad; ++i) {
		
		if (cursorlista->valor->nombre == vert) {
			res = cursorlista;
			break;

		}
		cursorlista = cursorlista->sig;
	
	}

	return (res);
}

void destruirlistacont(plista list) {
	pnodo temp = NULL;
	pnodo cursor = list->head;
	
	for (u32 i = 0; i < list->cantidad; ++i) {
			if (cursor != NULL) { //superfluo
				temp = cursor->sig;
				free(cursor);
				cursor = temp;
			}
	}

}