#include "greedy.h"
//#include "GrafoSt21.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h> //para usar memset

//#pragma intrinsic(_mm_popcnt_u64)

void resetColores(Grafo G) {

	/// <summary>
	/// Esta funcion "despinta" todos los vertices del Grafo G
	/// </summary>

	u32 cantidadVert = NumeroDeVertices(G);

	for (u32 i = 0; i < cantidadVert; ++i) {

		FijarColor(UINT32_MAX, i, G);

	}
	

}
/*Por cada vertice, buscamos primero los colores de cada vecino. Luego, buscamos el numero de color mas chico que sea posible 
  Devuelve el nombre del color mas grande utilzado (como empezamos por cero, la cantidad utilizada es el resultado devuelto mas 1)
  La suma de 1 no la hacemos dentro de esta funcion*/
#ifndef __uh_H
u32 greedycoloresprimero(Grafo G, const Vertice* restrict orden) {

	u32 sizearrayVecinos = 4;
	bool* forbiddencolors = (bool*)calloc(sizearrayVecinos, sizeof(bool)); //este array de bool tiene tantas posiciones como colores posibles (Δ + 1)
	u32 cantidadVert = NumeroDeVertices(G);

	//Guarda el color mas alto utilizado. No es la cantidad de colores utilizada!
	u32 maxcolor = 0;

	//pintar el primero
	orden[0]->color = 0;

	for (u32 i = 1; i < cantidadVert; ++i) {


		Vertice restrict vert = orden[i];
		Vertice* arrayVec = vert->vecinos->avecinos;
		u32 grado = vert->grado;

		if (i > 1) {

			memset(forbiddencolors, '\0', sizearrayVecinos);

		}
		//Busca cada vecino, para establecer que colores se pueden utilizar y cuales no.
		//Si el color C no se puede utilizar, entonces forbiddencolors[C] = true;

		u32 vecinospintadosyavisitados = 0;
		for (u32 k = 0; k < grado; ++k) {
			
			u32 colorvecino = arrayVec[k]->color;
			if (colorvecino != UINT32_MAX) {
				//entonces el vecino esta pintado
				if(sizearrayVecinos <= colorvecino){
					//hay que agrandar el array
					//esto es peligroso porque como argumento estamos usando el color del vecino, que maliciosamente podria ser muy grande
					bool* tempfcolors = (bool*)realloc(forbiddencolors, (sizeof(bool)*(colorvecino + 1)));
					if(tempfcolors != NULL){
						forbiddencolors = tempfcolors;
						
						memset((forbiddencolors + sizearrayVecinos), '\0', (colorvecino + 1 - sizearrayVecinos));
						sizearrayVecinos = colorvecino + 1;
					}
					else{
						//salio todo mal
						free(forbiddencolors);
						printf("No se pudo hacer el realloc en %s, linea %i\n", __FILE__, __LINE__);
						exit(12);
					}
				}
				++vecinospintadosyavisitados;
				forbiddencolors[colorvecino] = true;

			}
			
			if (vecinospintadosyavisitados == i) {
				break;
			}
		}

		u32 j;
		//Ahora probamos todos los colores. Si no ningun vecino lo utiliza, lo usamos para pintar el vertice.
		//Si ningun color esta disponible, simplemente va a elegir un nuevo color

		for (j = 0; j < sizearrayVecinos; ++j) {
		
			//esto podria usar popcnt para buscar los bits en 1
			if (forbiddencolors[j] == false) {
				maxcolor = maxcolor > (u32)forbiddencolors[j] ? maxcolor : (u32)forbiddencolors[j];
				break;
			}
		}
		//Pintar el vertice del color j
		vert->color = j;
		//Actualizar maxcolor
		maxcolor = maxcolor > j ? maxcolor : j;

		
	}
	free(forbiddencolors);

	return (maxcolor + 1);
}
#endif

/*----------------------------------------------------------------------------------------------*/

/*Por cada vertice, buscamos primero los colores de cada vecino. Luego, buscamos el numero de color mas chico que sea posible 
  Devuelve el nombre del color mas grande utilzado (como empezamos por cero, la cantidad utilizada es el resultado devuelto mas 1)
  La suma de 1 no la hacemos dentro de esta funcion*/

/*Greedy() como fue declarado en la especificacion de la parte 2 del proyecto*/


/*----------------------------------------------------------------------------------------------*/

/*Por cada vertice, buscamos primero los colores de cada vecino. Luego, buscamos el numero de color mas chico que sea posible 
  Devuelve el nombre del color mas grande utilzado (como empezamos por cero, la cantidad utilizada es el resultado devuelto mas 1)
  La suma de 1 la hacemos dentro de esta funcion*/

static u32 cantidadcoloresinicial = 4; //Esto lo usamos para hacer menos realloc's cada vez que se ejecuta Greedy. Se actualiza en cada ejecucion, antes de retornar. No es necesario de usar, se puede borrar

u32 Greedy(Grafo restrict G) {

	//Es copia de greedycoloresprimero()

	//Esta variable cuenta la cantidad de arrays de colores tenemos. Inicialmente se crean 4, luego se va a actualizando segun el valor de la ultima ejecucion de greedy dividido en 2.
	u32 sizearrayVecinos = cantidadcoloresinicial;

	bool* forbiddencolors = (bool*)calloc(sizearrayVecinos, sizeof(bool)); //este array de bool tiene tantas posiciones como colores posibles (Δ + 1)
	u32 cantidadVert = NumeroDeVertices(G);

	//Guarda el color mas alto utilizado. No es la cantidad de colores utilizada!
	u32 maxcolor = 0;

	//pintar el primero

	FijarColor(0,0,G);

	for (u32 i = 1; i < cantidadVert; ++i) {


		u32 grado = Grado(i, G);


		if (i > 1) {
			//Llenamos de ceros al arreglo forbiddencolors.

			memset(forbiddencolors, '\0', sizearrayVecinos);

		}

		//Busca cada vecino, para establecer que colores se pueden utilizar y cuales no.
		//Si el color C no se puede utilizar, entonces forbiddencolors[C] = true;

		u32 vecinospintadosyavisitados = 0;
		for (u32 k = 0; k < grado; ++k) {
			
			u32 colorvecino = ColorVecino(k, i, G);
			if (colorvecino != UINT32_MAX) {
				//entonces el vecino esta pintado

				if(sizearrayVecinos <= colorvecino){
					//hay que agrandar el array
					//esto es peligroso porque como argumento estamos usando el color del vecino, que maliciosamente podria ser muy grande: por ejemplo, se podria pintar un vertice antes de ejecutar Greedy, y despues que Greedy() haga un realloc gigante
					//Le agregamos 1 para intentar evitar hacer tantos realloc's
					bool* tempfcolors = (bool*)realloc(forbiddencolors, (sizeof(bool)*(colorvecino + 1)));
					if(tempfcolors != NULL){
						forbiddencolors = tempfcolors;
						
						memset((forbiddencolors + sizearrayVecinos), '\0', (colorvecino + 1 - sizearrayVecinos));
						sizearrayVecinos = colorvecino + 1;
					}
					else{
						//salio todo mal

						#ifdef NDEBUG
						printf("No se pudo hacer el realloc en %s, linea %i\n", __FILE__, __LINE__);
						#endif
						free(forbiddencolors);
						exit(12);
					}
				}
				++vecinospintadosyavisitados;
				//establecemos que el color que usa el vecino no se puede usar, esta prohibido.
				forbiddencolors[colorvecino] = true;
			}
			
			if (vecinospintadosyavisitados == i) {
				break;
			}
		}

		u32 j;
		//Ahora probamos todos los colores. Si no ningun vecino lo utiliza, lo usamos para pintar el vertice.
		//Si ningun color esta disponible, simplemente va a elegir un nuevo color
		for (j = 0; j < sizearrayVecinos; ++j) {
			if (forbiddencolors[j] == false) {
				//j es un color que podemos usar. Salir del for()
				break;
			}
		}
		//Pintar el vertice del color j
		
		FijarColor(j, i, G);
		//Actualizar maxcolor
		
		if(maxcolor < j){
			maxcolor = j;
		}

		
	}

	free(forbiddencolors);
	//Esto es para que la proxima ejecucion de greedy reserve una cantidad de memoria mas cercana a la que muy posiblemente termine usando. No cambia casi nada el tiempo de ejecucion, se podria eliminar
	cantidadcoloresinicial = (maxcolor/2) + 2;
	return (maxcolor + 1);
}



/*----------------------------------------------------------------------------------------------*/
/*Por cada vertice de G, y por cada color disponible, busca que ningun vertice vecino esté pintado de ese color.
De ser eso cierto, lo pinta del color que está analizando en ese momento. Sino hay ningun color disponible posible, lo pinta de un nuevo color*/
//Este greedy lo usamos para verificar cosas en mains viejos
u32 greedynaive(Grafo G, const Vertice* restrict orden) {

	u32 cantidadVert = NumeroDeVertices(G);

	u32 maxcolor = 0;

	for (u32 i = 0; i < cantidadVert; ++i) {

		u32 color = 0;
		Vertice vert = orden[i];
		Vertice* arrayVec = vert->vecinos->avecinos;
		u32 grado = vert->grado;

		if (i == 0) {
		//pintar el primero ipso facto
			vert->color = 0;
			continue;

		}

		
		for (u32 k = 0; k <= maxcolor; k++) {

			u32 encontrado = 0;
			
			u32 l;

			color = k;

			for (l = 0; l < grado; ++l) {


				if (arrayVec[l]->color == color) {
					encontrado = 1;
					break;

				}
				
			} // end for() vecinos


			if (encontrado == 0) {
				vert->color = color;
				break;
			}
			if (k == maxcolor) {

				++maxcolor;
				vert->color = maxcolor;
				break;

			}

		} //end for() colores


	} //end for() vertices

	return (maxcolor + 1);

}

char escoloreopropio(Grafo restrict G){

	u32 cantvertices = NumeroDeVertices(G);
	Vertice* ord = Orden(G);

	for(u32 i = 0; i < cantvertices; ++i){
		Vertice vert = ord[i];
		Vertice* arrayVec = VecinosVertice(vert);
		u32 grado = gradoVertice(vert);

		if(Color(i, G) == UINT32_MAX){
			printf("\nHay un vertice que no fue pintado!\n");
			return((char)0);
		}
		
		for(u32 j = 0; j < grado; ++j){
			if(arrayVec[j]->color == vert->color){
				return ((char)0);
			}
		}
	}

	return ((char)1);
}
