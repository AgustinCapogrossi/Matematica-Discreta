/*___________________________________________________________________________*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Control del orden. ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*___________________________________________________________________________*/

#include "orden.h"
//Incluir time.h para usarlo de seed de rand()
#include <time.h>

static char srandunavez = 0;

//Sirve para hacer pruebas
//Hace una permutacion de vertices contenidos en Vertice* orden
//Para eso hace un swap de cada vertice con otro random
Vertice* randomizarorden(Vertice* restrict orden, u32 size) {
    srand((u32)time(NULL));
    --size; //porque los indices el array van de 0 a size-1
    for (u32 i = 0; i < size; ++i) {
        Vertice temp = orden[i];
        long long int posicionrand = ((int64_t)rand() % ((int64_t)size - i + 1)) + i;
        orden[i] = orden[posicionrand];
        orden[posicionrand] = temp;
    }
    return (orden);
}

void randomizarordenconseed_directo(Vertice* restrict orden, u32 size, unsigned int seed) {
    //al parecer, estamos haciendo "Fisher–Yates shuffle"
    //Se puede hacer mas rapido:https://lemire.me/blog/2016/06/30/fast-random-shuffling/
    if(srandunavez != 0){
        --size; //porque los indices el array van de 0 a size-1. EDIT: porque sino cuando i == size-1, estariamos sobre el ultimo elemento del array. Cuando intentemos cambiarlo de lugar por otro elemento, ya no quedan elementos para mover.
        for (u32 i = 0; i < size; ++i) {
            Vertice temp = orden[i];
            long long int posicionrand = ((int64_t)rand() % ((int64_t)size - i + 1)) + i;
            orden[i] = orden[posicionrand];
            orden[posicionrand] = temp;
        }

    }
    else{
        srand(seed);
        ++srandunavez;
        --size; //porque los indices el array van de 0 a size-1. EDIT: porque sino cuando i == size-1, estariamos sobre el ultimo elemento del array. Cuando intentemos cambiarlo de lugar por otro elemento, ya no quedan elementos para mover.
        for (u32 i = 0; i < size; ++i) {
            Vertice temp = orden[i];
            long long int posicionrand = ((int64_t)rand() % ((int64_t)size - i + 1)) + i;
            orden[i] = orden[posicionrand];
            orden[posicionrand] = temp;
        }
    }
}


//Sirve para hacer pruebas
//Aleatoriza el orden pero tambien "despinta" los vertices, todo en el mismo loop
Vertice* randomizarordenresetcolor(Vertice* orden, u32 size) {
    srand((u32)time(NULL));
    --size; //porque los indices el array van de 0 a size-1
    for (u32 i = 0; i < size; ++i) {
        Vertice temp = orden[i];
        temp->color = UINT32_MAX;
        long long int posicionrand = ((int64_t)rand() % ((int64_t)size - i + 1)) + i;
        orden[i] = orden[posicionrand];
        orden[posicionrand] = temp;
    }
    return (orden);
}


/*Función swap en caso de ser necesaria*/
void swap(Vertice* orden, u32 x, u32 y){
    Vertice aux;
    aux = orden[x];
    orden[x] = orden[y];
    orden[y] = aux;
}

void swap64(Vertice* orden, long long int x, long long int y) {
    Vertice aux;
    u32 xx = (u32)x;
    u32 yy = (u32)y;
    aux = orden[xx];
    orden[xx] = orden[yy];
    orden[yy] = aux;
}


/* Es la funcion que necesita stdlib.qsort() para comparar los elementos.
En este caso, compara el nombre de los vertices a y b.*/
int cmpfunc(const void* a, const void* b) {
    Vertice* restrict aa = (Vertice*)a;
    
    //int64_t nombre_a = aa[0]->nombre;
    u32 nombre_a = aa[0]->nombre;
    
    Vertice* restrict bb = (Vertice*)b;
   
    //int64_t nombre_b = bb[0]->nombre;
    u32 nombre_b = bb[0]->nombre;
    
    if(nombre_a < nombre_b){
        return (-1);
    }
    if(nombre_a > nombre_b){
        return (1);
    }

    return (0);


}

//deberia funcionar con: qsort((void*)orden[0], size, sizeof(Vertice), cmpfunc);
Vertice* ordenqsort(Vertice* orden, u32 size) {
#ifdef _WIN64
    qsort((void*)orden, size, sizeof(Vertice), cmpfunc);
#else
    //anterior: qsort((void*)orden, size, sizeof(Vertice*), cmpfunc);
    qsort((void*)orden, size, sizeof(Vertice), cmpfunc);
#endif
    return (orden);

}


//Insertion sort
Vertice* isOrdenNatural(Vertice* orden, u32 size) {
    Vertice* copy = orden;

    u32 i;
    u32 key;
    long long int j; // si no usamos un int, si j==0, y hacemos --j, j pasa a tener como valor UINT32_MAX.
    for (i = 1; i < size; i++)
    {
        key = nombreVertice(copy[i]);
        Vertice pkey = copy[i];
        j = (long long int)i - 1;

        while (j >= 0 && copy[j]->nombre > key)
        {
            copy[j + 1] = copy[j];
            j = j - 1;
        }
        copy[j + 1] = pkey;
    }
    return (copy);
}





