#ifndef VERTICE_H
#define VERTICE_H

//#include "LArray.h"

#include <stdlib.h>
#include "u32.h"

#ifndef LISTANENLAZADA_H
struct lista_s;
typedef struct lista_s* plista;
struct lnodo;
typedef struct lnodo* pnodo;
#else

typedef struct lnodo* pnodo;
typedef struct lista_s* plista;
typedef union implementacionvecinos* impvecinos;
#endif
#include "listaenlazada.h"
struct VerticeSt {
    impvecinos vecinos;
    u32 nombre;    
    u32 grado;     
    u32 color;
    u32 ordenRelativo;

};

typedef struct VerticeSt* Vertice;

union implementacionvecinos {
    Vertice* avecinos;
    plista lvecinos;
};


/*Creación y destrucción de vértices*/

/*Crea un vértice*/
Vertice CrearVertice (u32 nombre, u32 color);

Vertice CrearVerticeb(u32 nombre, u32 color, Vertice buffer, u32* pos);

#ifdef __GNUC__
Vertice Reservarmemoria(u32 cantidadvertices);
Vertice* Reservarmemoriavecindades(u32 cantidadfinal);
#else
Vertice Reservarmemoria(u32 cantidadvertices);
Vertice* Reservarmemoriavecindades(u32 cantidadfinal);
#endif

void DestVerticesmod(Vertice* vertices, u32 num_vertices);
void DestVerticeUnitariomod(Vertice vertice);

/*Arreglo de punteros a los vecinos de los vértices*/
Vertice *ArrayVertices (u32 num_vertices);

/*Destruye un vértice*/
Vertice DestVerticeUnitario(Vertice vertice);

/*Destruye todos los vértices*/
Vertice *DestVertices(Vertice *vertices, u32 num_vertices);


/*Información sobre vértices*/

/*Grado de un vértice*/
u32 gradoVertice (Vertice vertice);

/*Nombre de un vértice*/
u32 nombreVertice (Vertice vertice);

/*Color de un vértice*/
//u32 colorVertice (Vertice vertice);

/*Modificación de vértices*/

/*Pinta un vértice del color dado*/
#if defined(__OPTIMIZE__) && !defined(__NO_INLINE__) && defined(NDEBUG)
#if defined(__clang__) && defined(_WIN64)
void pintarVertice(Vertice vertice, u32 color)  __attribute__((always_inline, hot));

/*Color de un vértice*/
u32 colorVertice (Vertice vertice) __attribute__((always_inline, hot));

#else //clang linux o cualquier otro
void pintarVertice(Vertice vertice, u32 color) __attribute__((hot));

/*Color de un vértice*/
u32 colorVertice (Vertice vertice) __attribute__((hot));

#endif
#else //debug mode
void pintarVertice(Vertice vertice, u32 color);

/*Color de un vértice*/
u32 colorVertice (Vertice vertice);

#endif


/*Vecinos del vértice*/
/*Arreglo con los vecinos del vértice*/
Vertice* VecinosVertice(Vertice vertice);

/*Agrega un nuevo vecino al vértice*/
//void NuevoVecino (Vertice vertice, u32 vecino);

/*Vecino "i" del vértice*/
Vertice iesimoVecino(Vertice vertice, u32 i);

void nuevaVecindad(Vertice vert, u32 cantidadvecinos);


void nuevaVecindadmod(Vertice vert, u32 cantidadvecinos, Vertice* buffer, u32* pos);
void listatoarrayvecinosmod(Vertice vert, Vertice *buffer, u32* pos);

void listatoarrayvecinos(Vertice vert);

u32 getordenRelativo(Vertice vert);
void setordenRelativo(Vertice vert, u32 ord);
#endif
