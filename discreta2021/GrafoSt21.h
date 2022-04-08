#ifndef GRAFOST21_H
#define GRAFOST21_H

#include <math.h>
#include "u32.h"
#include "vertice.h"
#include "orden.h"


#ifndef __rv_H
//No esta definido RomaVictor.h, entonces tenemos que declarar las funciones de RomaVictor.
//Esto lo hemos dejado asi para poder usar main's viejos

typedef struct GrafoSt* Grafo;

Grafo ConstruccionDelGrafo();

void DestruccionDelGrafo(Grafo G);


/*Copia el grafo*/
#if defined(__OPTIMIZE__)
#if defined(__clang__) && defined(_WIN64)
Grafo CopiarGrafo(Grafo G);
Grafo CopiarGrafomodhs(Grafo G);
#elif defined(__clang__)
Grafo CopiarGrafo(Grafo G) __attribute__ ((nonnull (1), hot));
Grafo CopiarGrafomodhs(Grafo G) __attribute__ ((nonnull (1), hot));
#elif defined(__GNUC__)
//Grafo CopiarGrafo(Grafo G) __attribute__ ((hot, nonnull (1), access (read_only, 1)));
//Grafo CopiarGrafomodhs(Grafo G) __attribute__ ((hot, nonnull (1), access (read_only, 1)));
Grafo CopiarGrafo(Grafo G) __attribute__ ((hot, nonnull (1)));
Grafo CopiarGrafomodhs(Grafo G) __attribute__ ((hot, nonnull (1)));
#endif
#else
Grafo CopiarGrafo(Grafo G);
Grafo CopiarGrafomodhs(Grafo G);
#endif



//funciones para extraer datos del grafo. u32 esta definida en el .h de arriba

u32 NumeroDeVertices(Grafo G);
u32 NumeroDeLados(Grafo G);
u32 Delta(Grafo G);


//funciones para extraer informacion de los vertices 
//valor de retorno (2 a la 32)-1 para reportar errores.
//salvo para las de "Nombre" que no tienen forma de reportar errores.
//las otras no hay problema pues es imposible que (2 a la 32)-1 sea un color o un grado.
u32 Nombre(u32 i,Grafo G);
u32 Color(u32 i,Grafo G);
#if defined(__clang__) && defined(_WIN64)
extern u32 Grado(u32 i,Grafo G);
#else
u32 Grado(u32 i,Grafo G);
#endif

//para extraer informacion de los vecinos
#if defined(__clang__) && defined(_WIN64)
extern u32 ColorVecino(u32 j,u32 i,Grafo G);
#else
u32 ColorVecino(u32 j,u32 i,Grafo G);
#endif
u32 NombreVecino(u32 j,u32 i,Grafo G);
u32 OrdenVecino(u32 j,u32 i,Grafo G);
u32 PesoLadoConVecino(u32 j,u32 i,Grafo G);


//Funciones para modificar datos de los v'ertices, char es para retornar error
//si se le pide algo fuera de rango o un alloc error.

//asigna color x al v'ertice i del orden interno
#if defined(__clang__) && defined(_WIN64)
extern char FijarColor(u32 x,u32 i,Grafo G);
#else
char FijarColor(u32 x,u32 i,Grafo G);
#endif

//asigna en el lugar i del orden el v'ertice N-esimo del orden natural.
char FijarOrden(u32 i,Grafo G,u32 N);

//ese u32 de valor de retorno deberia ser un void pero lo dejo asi porque 
//asi estaba en el pdf de las especificaciones aunque no se especifica nada sobre ese valor de retorno.
//u32 FijarPesoLadoConVecino(u32 j,u32 i,u32 p,Grafo G);

/* Aleatoriza el orden de vertices con u32 R como argumento para srand()*/
#if defined(__OPTIMIZE__)

#if defined(__clang__) && defined(_WIN64)
char AleatorizarVertices_directo(Grafo G,u32 R) __attribute__ ((hot, nonnull (1)));
char AleatorizarVertices(Grafo G,u32 R) __attribute__ ((hot, nonnull (1)));
#elif defined(__clang__)
char AleatorizarVertices_directo(Grafo G,u32 R) __attribute__ ((hot, nonnull (1)));
char AleatorizarVertices(Grafo G,u32 R) __attribute__ ((hot, nonnull (1)));
#elif defined(__GNUC__)
//char AleatorizarVertices(Grafo G,u32 R) __attribute__ ((hot, nonnull (1), access (read_only, 1)));
char AleatorizarVertices_directo(Grafo G,u32 R) __attribute__ ((hot, nonnull (1)));
char AleatorizarVertices(Grafo G,u32 R) __attribute__ ((hot, nonnull (1)));
#else
char AleatorizarVertices_directo(Grafo G,u32 R);
char AleatorizarVertices(Grafo G,u32 R);
#endif

#else //entonces estamos en compilacion debug
char AleatorizarVertices_directo(Grafo G,u32 R);
char AleatorizarVertices(Grafo G,u32 R);


#endif//!endif __optimize__

#endif//!endif __rv_H

typedef struct GrafoSt* Grafo;


/*Funciones de Construccion/Destruccion/Copia  del Grafo*/

/*Inicializa el orden de los vértices*/
Vertice* ArrayOrdenVertices (u32 num_vertices);

/*Construir el grafo*/
//Grafo ConstruccionDelGrafo();

/*Inicializa el grafo*/
Grafo InicializacionDelGrafo(u32 num_vertices, u32 m_lados);

/*Destruye el grafo*/
//void DestruccionDelGrafo(Grafo G);

void DestruccionDelGrafomod(Grafo G);

Grafo CopiarGrafomod(Grafo G);

void copiaarray(Vertice* arrayorigen, Vertice* arraydestino, u32 longitud);





/*Funciones para extraer informacion de datos del Grafo*/

/*Devuelve el número de vértices del grafo G*/
//u32 NumeroDeVertices(Grafo G);

/*Devuelve el número de lados del grafo G*/
//u32 NumeroDeLados(Grafo G);

/*Devuelve el orden del grafo G*/
Vertice* Orden(Grafo G);

//Devuelve el arreglo de vertices que son parte del Grafo.
Vertice* Miembros(Grafo G);

//Agrega un nuevo miembro (vertice) al Grafo, en la posicion pos

void Addvertice(Grafo G, Vertice vert, u32 pos);

/*Devuelve el grado mas alto del grafo G*/
//u32 Delta(Grafo G);

/*Establece el grado mas alto del grafo G*/
void Setdelta(Grafo G, u32 delt);

/*Devuelve el grado mas alto del grafo G*/
u32 GetDelta(Grafo G);

/*Copia el array de vertices de G en la memoria reservada para el orden de G*/
void copiavertaorden(Grafo G);

/*Copia el array de vertices de G en la memoria reservada para el ordenNatural de G*/
void copiavertaordenNatural(Grafo G);

/*Establece el array ordenNatural en el ordenr dado*/
void setOrdenNatural(Grafo G, Vertice* ordenr);

/*Devuelve el array ordenNatural*/
Vertice* OrdenNatural(Grafo G);


/*Funciones para extraer información de los Vertices*/

/*Devuelve el nombre del vértice*/
//u32 Nombre(u32 i,Grafo G);

/*Devuelve el color del vértice*/
#if defined(__clang__) && defined(_WIN64)
#else
//u32 Color(u32 i,Grafo G);
#endif

/*Devuelve el grado del vértice*/
#if defined(__OPTIMIZE__)
#if defined(__clang__) && defined(_WIN64)
//extern u32 Grado(u32 i,Grafo G) __attribute__((always_inline));
#else
//u32 Grado(u32 i,Grafo G) __attribute__((always_inline, nonnull (2)));
//u32 Grado(u32 i,Grafo G) __attribute__((nonnull (2))); //last
#endif
#else
//u32 Grado(u32 i,Grafo G);


#endif


/* Funciones para extraer información de los vecinos de un vértice */

/*Devuelve el color del vecino*/
#if defined(__OPTIMIZE__)
#if defined(__clang__) && defined(_WIN64)
//extern u32 ColorVecino(u32 j,u32 i,Grafo G) __attribute__((always_inline, nonnull (3)));
#elif defined(__clang__)
//u32 ColorVecino(u32 j,u32 i,Grafo G) __attribute__((always_inline, nonnull (3)));
#elif defined(__GNUC__)
//u32 ColorVecino(u32 j,u32 i,Grafo G) __attribute__((always_inline, nonnull (3)));
//u32 ColorVecino(u32 j,u32 i,Grafo G) __attribute__((nonnull (3)));
#endif
#else
//u32 ColorVecino(u32 j,u32 i,Grafo G);
#endif

/*Devuelve el nombre del vecino*/
//u32 NombreVecino(u32 j,u32 i,Grafo G);

/*Devuelve el orden global del vecino*/
//u32 OrdenVecino(u32 j,u32 i,Grafo G);

/*Devuelve el peso de la arista conformada entre j e i*/
//u32 PesoLadoConVecino(u32 j,u32 i,Grafo G);

/* Funciones para modificar datos de los vértices */


#if defined(__OPTIMIZE__)
#if defined(__clang__) && defined(_WIN64)
/*Le asigna el color x al vertice i en el orden de G*/
//extern char FijarColor(u32 x,u32 i,Grafo G)  __attribute__((always_inline));
#elif defined(__clang__)
char FijarColor(u32 x,u32 i,Grafo G)  __attribute__((always_inline));
#elif defined(__GNUC__)
//char FijarColor(u32 x,u32 i,Grafo G)__attribute__ ((always_inline ,access (read_only, 3)));
//char FijarColor(u32 x,u32 i,Grafo G)__attribute__ ((access (read_only, 3)));
//char FijarColor(u32 x,u32 i,Grafo G);
#endif
#else
//char FijarColor(u32 x,u32 i,Grafo G);
#endif

/*Determina que el vertice i en el orden de G sera el elemento N del orden natural*/
//char FijarOrden(u32 i,Grafo G,u32 N);


void randomizarordenconseed(u32* arrayAux, u32 size, unsigned int seed);
void randomizarpos(u32* restrict orden, u32 size);

#endif //!fin .h

