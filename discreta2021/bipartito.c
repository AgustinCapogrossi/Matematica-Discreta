#include "bipartito.h"
#include "GrafoSt21.h"
#include "assert.h"
#include "vertice.h"
#include <stdint.h>
#include <stdio.h>
#include "hashtable_b.h"

//static hashtable2_t* tablahash = NULL;

queue2 newqueue2(u32 size){
    queue2 r = (queue2)malloc(sizeof(struct queue_st));
    r->queuep = (u32*)malloc(sizeof(u32) * size);
    r->size = size;
    r->slotsocupados = 0;
    r->head = 0;
    r->tail = 0;

    return (r);

}

void destroyqueue2(queue2 fila){
    free(fila->queuep);
    free(fila);
}

void enqueue2(queue2 fila, u32 elemento){
    
    //*no revisa que la cola tenga lugar. Puede haber overflow si la lista se modifica mediante otros procedimientos
    //*que no sean enqueue y dequeue
    fila->queuep[fila->tail] = elemento;
    ++fila->slotsocupados;
    if(fila->tail == fila->size -1){
        //*es un arreglo "circular" Cuidado porque si esta llena la cola, no avisa.
        //*sabemos el Δ del grafo es el numero maximo de elemenos que vamos a necesitar guardar en esta cola.
        //*hacer el malloc correspondiente
        fila->tail = 1;

    }
    else {
        ++fila->tail;
    }
}

u32 dequeue2(queue2 fila){
    if(fila->slotsocupados != 0){
        --fila->slotsocupados;

        u32 r = fila->queuep[fila->head];
        if(fila->head == fila->size -1){
            fila->head = 1;

        }
        else {
            ++fila->head;

        }
        return (r);
    }
    else {
        //!hay underflow: la lista esta vacia
        return (UINT32_MAX); //Usamos eso porque es poco probable que tengamos tantos vertices en un grafo. No es ideal, es poco transparente para el usuario final
    }

}

/*-------------------------------*/

queue newqueue(u32 size){
    queue r = (queue)malloc(sizeof(struct queue_s));
    r->queuep = (Vertice*)malloc(sizeof(Vertice) * size);
    r->size = size;
    r->slotsocupados = 0;
    r->head = 0;
    r->tail = 0;

    return (r);

}

void destroyqueue(queue fila){
    free(fila->queuep);
    free(fila);
}

void enqueue(queue fila, Vertice elemento){
    
    //*no revisa que la cola tenga lugar. Puede haber overflow si la lista se modifica mediante otros procedimientos
    //*que no sean enqueue y dequeue
    fila->queuep[fila->tail] = elemento;
    ++fila->slotsocupados;
    if(fila->tail == fila->size -1){
        //*es un arreglo "circular" Cuidado porque si esta llena la cola, no avisa.
        //*sabemos el Δ del grafo es el numero maximo de elemenos que vamos a necesitar guardar en esta cola.
        //*hacer el malloc correspondiente
        fila->tail = 1;

    }
    else {
        ++fila->tail;
    }
}

Vertice dequeue(queue fila){
    if(fila->slotsocupados != 0){
        --fila->slotsocupados;

        Vertice r = fila->queuep[fila->head];
        if(fila->head == fila->size -1){
            fila->head = 1;

        }
        else {
            ++fila->head;

        }
        return (r);
    }
    else {
        //!hay underflow: la lista esta vacia
        return (NULL);
    }

}

/**/



//-------------------------------//

#ifndef __uh_H

char Bipartitoarray(Grafo G, Vertice* arrayVert, u32 size, char resrecursion){

    if (resrecursion == 0){
        return ((char)0);
    }
    char resultado = 1;
    u32 pintados = 1;
    u32 despintados = 0;

    queue fila = newqueue(size);

    //Vertice* arrayVert = Miembros(G);
    //Vertice* arrayVert = subset;
    

    //pintar el primero
    assert(arrayVert != NULL);
    if (arrayVert == NULL) {
      printf("\nProblema en %s, linea %u\n", __FILE__, __LINE__);
      return (0);
    }

    arrayVert[0]->color = 0;

    enqueue(fila, arrayVert[0]);
    char endloop = 0;
    while(fila->slotsocupados != 0){
        Vertice vert = dequeue(fila);
        Vertice* arrayVec = VecinosVertice(vert);
        u32 grado = vert->grado;

        for(u32 k = 0; k < grado; ++k){
            Vertice vecino = arrayVec[k];

            if (vecino->color == UINT32_MAX){
                ++despintados;


                if(vert->color == 0){
                    //pintar vecino
                    vecino->color = 1;
                    enqueue(fila, vecino);
                    ++pintados;
                }
                else {
                    //pintar vecino del color contrario
                    vecino->color = 0;
                    enqueue(fila, vecino);
                    ++pintados;

                }

            }
            else {
                    
                    if(vert->color == vecino->color){
                    //no es bipartito, salir
                    endloop = 1;
                    resultado = 0;
                    break;

                }
            }

        }/*!endfor*/

        if(endloop == 1){
            //!Hay que devolver un coloreo propio antes de salir.
            //TODO: testear que devuelva un coloreo propio.
            break;
        }
    }
    
    if((pintados < size) && resultado == 1){
        //hay otra componente mas
        Vertice* subset = (Vertice*)malloc(sizeof(Vertice) * (size-pintados));
        u32 cursor = 0;
        for(u32 l = 0; l < size; ++l){
            if(arrayVert[l] != NULL && arrayVert[l]->color == UINT32_MAX){
                //no esta pintado
                subset[cursor] = arrayVert[l];
                ++cursor;
                
            }
        }

        assert(cursor != 0);

        //recursion
        free(arrayVert);
        destroyqueue(fila);
        return(Bipartitoarray(G, subset, (size - pintados), resultado));

    }
    free(arrayVert);
    destroyqueue(fila);
    return (resultado);
}


char Bipartito_directo(Grafo restrict G){
    u32 coloresfinal = 0;
    char resultado = 1;
    u32 pintados = 1;
    u32 despintados = 0;

    queue fila = newqueue(NumeroDeVertices(G));
    
    Vertice* arrayVert = Orden(G);
    

    //pintar el primero
    
    arrayVert[0]->color = 0;

    enqueue(fila, arrayVert[0]);
    char endloop = 0;
    while(fila->slotsocupados != 0){
        Vertice vert = dequeue(fila);
        Vertice* arrayVec = VecinosVertice(vert);
        u32 grado = vert->grado;

        for(u32 k = 0; k < grado; ++k){
            Vertice vecino = arrayVec[k];

            if (vecino->color == UINT32_MAX){
                ++despintados;


                if(vert->color == 0){
                    //pintar vecino
                    vecino->color = 1;
                    enqueue(fila, vecino);
                    ++pintados;
                }
                else {
                    //pintar vecino del color contrario
                    vecino->color = 0;
                    enqueue(fila, vecino);
                    ++pintados;

                }

            }
            else {
                    //++pintados;
                    if(vert->color == vecino->color){
                    //no es bipartito, salir
                    endloop = 1;
                    resultado = 0;
                    break;

                }
            }

        }/*!endfor*/

        if(endloop == 1){
            //!Hay que devolver un coloreo propio antes de salir.
            //TODO: testear que devuelva un coloreo propio. ==> RESUELTO
            resetColores(G);
            coloresfinal = greedycoloresprimero(G, Orden(G));
            break;
        }
    }
    
    if((pintados < NumeroDeVertices(G)) && resultado == 1){
        //G tiene mas de una componente. Por lo tanto, nuestro coloreo no es propio.
        //TODO: Eliminar recursion pasando el resultado a la siguiente llamada. Asi no llenamos el stack. Tambien llamado "tail recursion"
        //EDIT: Ahora usamos una recursion por cola. Testear que ande siempre bien y que no tenga leaks.


        //hay otra componente mas
        //*Buscamos los vertices sin pintar, y los recorremos con una funcion identica pero que usa un subconjunto de los vertices solamente
        //*Esta funcion identica es recursiva.
        Vertice* subset = (Vertice*)calloc(((NumeroDeVertices(G))-pintados), sizeof(Vertice));
        assert(subset != NULL);
        u32 cursor = 0;
        for(u32 l = 0; l < NumeroDeVertices(G); ++l){
            if(arrayVert[l]->color == UINT32_MAX){
                //no esta pintado
                subset[cursor] = arrayVert[l];
                ++cursor;
                
            }
        }
        assert(cursor != 0);
        //recursion pero con el subset, usando una funcion analoga
        char resultadorecursion = Bipartitoarray(G, subset, ((NumeroDeVertices(G))-pintados), resultado);
        //asumimos que salio todo bien ==> resultadorecursion == 1
        coloresfinal = 2;
        if(resultadorecursion == 0){
            //entonces alguna de las otras componentes del grafo no es bipartita.
            resultado = 0;
            resetColores(G);
            coloresfinal = Greedy(G);

        }
        //free(subset); //Esto no hace falta, porque bipartitoarray en el caso no recursivo lo hace al final


    }

    destroyqueue(fila);
    return (resultado);
}
#endif


char Bipartitoarray2(Grafo G, u32* arrayVert, u32 size, char resrecursion){

    if (resrecursion == 0){
        return ((char)0);
    }
    char resultado = 1;
    u32 pintados = 1;
    u32 despintados = 0;

    queue2 fila = newqueue2(size);

    //pintar el primero
    assert(arrayVert != NULL);
    if (arrayVert == NULL) {
      printf("\nProblema en %s, linea %u\n", __FILE__, __LINE__);
      return (0);
    }


    FijarColor(0, arrayVert[0], G);


    enqueue2(fila, arrayVert[0]);
    char endloop = 0;
    while(fila->slotsocupados != 0){
        u32 posicionVert = dequeue2(fila);
        u32 grado = Grado(posicionVert, G);
        u32 colorVert = Color(posicionVert, G);


        for(u32 k = 0; k < grado; ++k){
            u32 colorVecino = ColorVecino(k, posicionVert, G);
            
            u32 ordenVecino = OrdenVecino(k, posicionVert, G);
            

            if ( colorVecino == UINT32_MAX){
                ++despintados;


                if(colorVert == 0){
                    //pintar vecino
                    //vecino->color = 1; // ==>
                    FijarColor(1, ordenVecino, G); //


                    enqueue2(fila, ordenVecino);
                    ++pintados;
                }
                else {
                    //pintar vecino del color contrario
                    //vecino->color = 0;
                    FijarColor(0, ordenVecino, G);
                    enqueue2(fila, ordenVecino);
                    ++pintados;

                }

            }
            else {
                    //++pintados;
                    //if(vert->color == vecino->color){
                    if(colorVert == colorVecino){
                    //no es bipartito, salir
                    endloop = 1;
                    resultado = 0;
                    break;

                }
            }

        }/*!endfor*/

        if(endloop == 1){
            //!Hay que devolver un coloreo propio antes de salir.
            
            
            break;
        }
    }
    
    if((pintados < size) && resultado == 1){
        //hay otra componente mas
        u32* subset = (u32*)malloc(sizeof(u32) * (size-pintados));
        u32 cursor = 0;
        u32 pintadosvisitados = 0;
        for(u32 l = 0; l < size; ++l){

            if(Color(arrayVert[l] , G) == UINT32_MAX){
                //no esta pintado
                subset[cursor] = arrayVert[l];
                ++cursor;
                
            }
            else{
                ++pintadosvisitados;
            }

            if(pintadosvisitados == pintados){
                for(u32 m = l+1; m < size; ++m){
                    subset[cursor] = arrayVert[m];
                    ++cursor;
                }
                break;
            }

        }

        assert(cursor != 0);

        //recursion
        free(arrayVert);
        destroyqueue2(fila);
        return(Bipartitoarray2(G, subset, (size - pintados), resultado));

    }
    free(arrayVert);
    destroyqueue2(fila);
    
    return (resultado);
}

/*---------------------------------*/

//-----------------------------------//
//Bipartito pero usando fijarOrden.

char Bipartito(Grafo restrict G){

    char resultado = 1;
    u32 pintados = 1;
    u32 despintados = 0;

    
    queue2 fila = newqueue2(NumeroDeVertices(G));

    

    //pintar el primero
    

    FijarColor(0, 0, G);

    enqueue2(fila, 0);
    char endloop = 0;
    while(fila->slotsocupados != 0){
        u32 posicionVert = dequeue2(fila);

        u32 grado = Grado(posicionVert, G);
        u32 colorVert = Color(posicionVert, G);

        for(u32 k = 0; k < grado; ++k){

            u32 colorVecino = ColorVecino(k, posicionVert, G);
            u32 ordenVecino = OrdenVecino(k, posicionVert, G);


            if ( colorVecino == UINT32_MAX){
                ++despintados;

                if(colorVert == 0){
                    //pintar vecino

                    FijarColor(1, ordenVecino, G);

                    enqueue2(fila, ordenVecino);
                    ++pintados;
                }
                else {
                    //pintar vecino del color contrario

                    FijarColor(0, ordenVecino, G);
                    enqueue2(fila, ordenVecino);
                    ++pintados;

                }

            }
            else {

                    if(colorVert == colorVecino){
                    //no es bipartito, salir
                    endloop = 1;
                    resultado = 0;
                    break;

                }
            }

        }/*!endfor*/

        if(endloop == 1){
            //!Hay que devolver un coloreo propio antes de salir.
            //TODO: testear que devuelva un coloreo propio. ==> RESUELTO
            resetColores(G);

            Greedy(G);
            break;
        }
    }
    
    if((pintados < NumeroDeVertices(G)) && resultado == 1){
        //G tiene mas de una componente. Por lo tanto, nuestro coloreo no es propio.
        //EDIT: Ahora usamos una recursion por cola. Testear que ande siempre bien y que no tenga leaks.


        //hay otra componente mas
        //*Buscamos los vertices sin pintar, y los recorremos con una funcion identica pero que usa un subconjunto de los vertices solamente
        //*Esta funcion identica es recursiva.

        u32 numvert = NumeroDeVertices(G);

        u32* subset = (u32*)malloc(sizeof(u32) * ((numvert)-pintados));
        assert(subset != NULL);
        u32 cursor = 0;
        u32 pintadosvisitados = 0;
        
        for(u32 l = 0; l < numvert; ++l){
            if(Color(l, G) == UINT32_MAX){
                //no esta pintado
                subset[cursor] = l;
                ++cursor;
                
            }
            else{
                ++pintadosvisitados;
            }

            if(pintadosvisitados == pintados){
                for(u32 m = l+1; m < numvert; ++m){
                    subset[cursor] = m;
                    ++cursor;
                }
                break;
            }
        }
        assert(cursor != 0);
        //recursion pero con el subset, usando una funcion analoga
        char resultadorecursion = Bipartitoarray2(G, subset, ((numvert)-pintados), resultado);
        //asumimos que salio todo bien ==> resultadorecursion == 1

        if(resultadorecursion == 0){
            //entonces alguna de las otras componentes del grafo no es bipartita.
            resultado = 0;
            resetColores(G);

            Greedy(G);

        }
        //free(subset); //Esto no hace falta, porque bipartitoarray en el caso no recursivo lo hace al final


    }

    destroyqueue2(fila);
    return (resultado);
}

//-------------------------------//

