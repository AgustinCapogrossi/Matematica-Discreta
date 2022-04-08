#include "vertice.h"
#include <assert.h>


/*Creación y destrucción de vértices*/

//Reserva memoria para guardar todos los vertices en un unico bloque de memoria
Vertice Reservarmemoria(u32 cantidadvertices){
	Vertice r = NULL;
	size_t tamaniost = sizeof(struct VerticeSt);
	r = (Vertice)malloc(tamaniost * cantidadvertices);
	return(r);
}
/*--------------------------------------------*/

//Reserva u32 cantidadfinal * sizeof(Vertice) bytes para guardar ahi todas las vecindades de forma contigua
//De esa manera, ahorramos malloc()'s y es mas rapido para hacer free()
Vertice* Reservarmemoriavecindades(u32 cantidadfinal){
	Vertice* r = NULL;
	size_t tamaniopuntero = sizeof(Vertice);
	
	r = (Vertice*)malloc(cantidadfinal * tamaniopuntero);

	return(r);
}
/*--------------------------------------------*/


Vertice CrearVerticeb(u32 nombre, u32 color, Vertice buffer, u32* pos){
	Vertice vertex;
	vertex = &(buffer[(*pos)]);
	
		vertex->nombre = nombre;
		vertex->grado = 0;
		vertex->color = color;
		vertex->ordenRelativo = (*pos);

		//podriamos hacer un cast to union a alguna zona de memoria reservada para array de vecinos, pero no esta estandarizado, entonces el compilador puede quejarse. Solo andaria en GCC y es una extension
		//vertex->vecinos = (impvecinos)calloc(1, sizeof(union implementacionvecinos));
		vertex->vecinos = (impvecinos)malloc(sizeof(union implementacionvecinos));
		//actualizamos el indice
		++(*pos);
	
	
	
	return (vertex);

}
/*--------------------------------------------*/

/*Crea un vértice*/
Vertice CrearVertice (u32 nombre, u32 color){
	Vertice vertex;
	vertex = (Vertice)malloc(sizeof(struct VerticeSt));
	if (vertex != NULL) {
		vertex->nombre = nombre;
		vertex->grado = 0;
		vertex->color = color;
		//vertex->vecinos = NULL;
		vertex->vecinos = (impvecinos)calloc(1, sizeof(union implementacionvecinos));
	}
	
	
	return (vertex);

}

/*Arreglo de punteros a los vecinos del vértice*/
Vertice *ArrayVertices (u32 num_vertices) {

	Vertice* array = NULL;
	array = (Vertice *)calloc(num_vertices,sizeof(Vertice));

	return (array);
}

/*Destruye un vértice*/
//Solo sirve si ya los vecinos ya estan guardados como un array.
Vertice DestVerticeUnitario(Vertice vertice){
	if (vertice != NULL){
		if (vertice->vecinos != NULL) {
			free(vertice->vecinos->avecinos);
			free(vertice->vecinos);

		}

		free(vertice);
		vertice = NULL;
	}
	else {
		vertice = NULL;
	}
	
	return vertice;
}

/*Destruye el arreglo de vértices*/
Vertice *DestVertices(Vertice* vertices, u32 num_vertices){
	for(u32 i = 0; i<num_vertices; i++){
		vertices[i] = DestVerticeUnitario(vertices[i]);
	}
	free(vertices);
	//vertices = NULL;
	return NULL;
}

/*Obtención de información de vértices*/


/*Devuelve el grado de un vértice*/
u32 gradoVertice (Vertice vertice){
	return (vertice->grado);
} 
/*Devuelve el nombre de un vértice*/
u32 nombreVertice (Vertice vertice){
	return vertice->nombre;
}

/*Devuelve el color de un vértice*/
u32 colorVertice (Vertice vertice){
	return vertice->color;
}

/*Modificar un vértice*/

/*Le asigna el color "color" al vértice*/
void pintarVertice (Vertice vertice, u32 color){
	vertice->color = color;
}

/*Vecinos de un vértice*/


/*Crea el arreglo de vecinos definitivo
Solo se usa una vez, cuando se carga el vertice*/
void nuevaVecindad(Vertice vert, u32 cantidadvecinos) {
	
	vert->vecinos->avecinos = (Vertice*)malloc(cantidadvecinos * sizeof(Vertice));

}

//-----------------------------------------------------------//

/*Crea el arreglo de vecinos definitivo
Solo se usa una vez, cuando se carga el vertice*/
void nuevaVecindadmod(Vertice vert, u32 cantidadvecinos, Vertice* buffer, u32* pos) {
	
	vert->vecinos->avecinos = &buffer[(*pos)];
	//ahora adelantamos la posicion tantos lugares como vecinos tenga este vertice, asi el proximo vertice guarda sus vecinos y nos deja suficiente espacio.
	(*pos) = (*pos) + cantidadvecinos;

}

//-----------------------------------------------------------//

Vertice iesimoVecino(Vertice vert, u32 i) {

	Vertice r = NULL;

	r = vert->vecinos->avecinos[i];

	return (r);

}
//Devuelve un puntero al arreglo de vecinos de vertice.
Vertice* VecinosVertice(Vertice vertice) {

	return (vertice->vecinos->avecinos);
}

//--------------------------------------------------------//

//Transforma una lista de vecinos en un arreglo de vecinos. Solamente se usa cuando se carga el grafo.
void listatoarrayvecinos(Vertice vert) {
	plista destruirfinal = vert->vecinos->lvecinos;
	if (destruirfinal != NULL) {
		u32 qu = vert->vecinos->lvecinos->cantidad;
		plista temp = vert->vecinos->lvecinos;
		pnodo headlista = temp->head;
		nuevaVecindad(vert, qu);
		for (u32 i = 0; i < qu; ++i) {
			if(headlista != NULL){
				vert->vecinos->avecinos[i] = headlista->valor;
				pnodo temp2 = headlista;
				headlista = headlista->sig;
				free(temp2); //elimina el nodo de la lista
			}
		}
		vert->grado = qu;
		free(destruirfinal); //elimina la cabecera de la lista
	}
	
}

//--------------------------------------------------------//

//--------------------------------------------------------//

//Transforma una lista de vecinos en un arreglo de vecinos. Solamente se usa cuando se carga el grafo.
void listatoarrayvecinosmod(Vertice vert, Vertice *buffer, u32* pos) {
	plista destruirfinal = vert->vecinos->lvecinos;
	if (destruirfinal != NULL) {
		u32 qu = vert->vecinos->lvecinos->cantidad;
		plista temp = vert->vecinos->lvecinos;
		pnodo headlista = temp->head;
		nuevaVecindadmod(vert, qu, buffer, pos);
		for (u32 i = 0; i < qu; ++i) {
			if(headlista != NULL){
				vert->vecinos->avecinos[i] = headlista->valor;
				pnodo temp2 = headlista;
				headlista = headlista->sig;
				free(temp2); //elimina el nodo de la lista
			}
		}
		vert->grado = qu;
		free(destruirfinal); //elimina la cabecera de la lista
	}
	
}

//--------------------------------------------------------//

/*Destruye un vértice*/
void DestVerticeUnitariomod(Vertice vertice){

	assert(vertice->vecinos != NULL);
	free(vertice->vecinos);
	
}

/*Destruye el arreglo de vértices*/
//Esto libera toda la memoria ocupada por el arreglo de vecinos y por el arreglo de vecinos de cada vertice, con dos llamadas a free()
void DestVerticesmod(Vertice* vertices, u32 num_vertices){
	Vertice alfa = vertices[0];
	Vertice* buffervecindades = alfa->vecinos->avecinos;
	for(u32 i = 0; i<num_vertices; i++){

		DestVerticeUnitariomod(vertices[i]);
	}
	free(buffervecindades);//destruye todas las vecindades juntas
	free(alfa);//destruye todos los vertices juntos
}
