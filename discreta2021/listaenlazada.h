#ifndef LISTANENLAZADA_H
#define LISTANENLAZADA_H

#include "u32.h"


#ifndef VERTICE_H
struct VerticeSt;
union implementacionvecinos;
typedef struct VerticeSt* Vertice;
typedef union implementacionvecinos* impvecinos;
#else
typedef struct VerticeSt* Vertice;
typedef union implementacionvecinos* impvecinos;
#endif
#include "vertice.h"
typedef struct lnodo { //snodo es el nombre de la estructura
    Vertice valor;
    struct lnodo* sig; //El puntero siguiente para recorrer la lista enlazada

}tnodo;

typedef struct lnodo* pnodo;

typedef struct lista_s {
    pnodo head;
    u32 cantidad;
}lista;

typedef struct lista_s* plista;


plista crearlista();

void destruirlista(plista list);


pnodo buscarnodo(plista list, u32 vert);

void destruirlistacont(plista list);

void insertarnodo(plista list, Vertice vertr);

#endif // !LISTANENLAZADA_H

