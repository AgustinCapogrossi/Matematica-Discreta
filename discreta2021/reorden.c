#include "reorden.h"
//#include "GrafoSt21.h"
//#include "listaenlazada.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef NDEBUG
#include <stdio.h>
#endif
#define CANTIDADINICIALDELISTAS 1
const u32 CANTIDADINICIALDEARRAYS = 1;
const u32 CAPACIDADINICIALDEARRAY = 10;


struct rsoa{
    Vertice** arrayscolores;
    u32* cantidaddeverticesporcolor;
    u32* tamanioarrporcolor;
    //u32 cantidadarrays;
};
typedef struct rsoa* arrays_p;

struct rsoa2{
    u32** arrayscolores;
    u32* cantidaddeverticesporcolor;
    u32* tamanioarrporcolor;
    //u32 cantidadarrays;
};
typedef struct rsoa2* arrays2_p;


//-----------------------------------------------------------------//

//OrdenPorBloqueDeColores ahora usa fijarOrden. Es hasta 3 veces mas lento que antes, pero sigue siendo rapido
char OrdenPorBloqueDeColores(Grafo G, u32* perm){
    //TODO:verificar O(n) en el peor caso
    //Recorrer G.orden una vez. Ir tirando cada vertice a su respectivo array segun su color
    //Si el color mas grande visto hasta el momento es mayor al mayor establecido en perm, devolver 0.
    //Al final, poner los contenidos de los arrays en G.orden in-place(no copia nueva) segun el orden que establece perm.


    //check rango de los valores de la permutacion son continuos, no importa el numero final, ni el orden de la permutacion.
    //TODO: El segundo for() de esta seccion se puede eliminar revisando que hayamos validado positivo tantos elementos de la permutacion como la cantidad de colores utilizados
    u32 cantidadcoloresutilizados = 0;
    u32 mm = NumeroDeVertices(G);
    u32 cmax = 0;
    for(u32 i = 0; i < mm; ++i){
        //esto no es ideal, pero no hay otra alternativa
        u32 cccolor = Color(i, G);
        if(cccolor > cmax){
            cmax = cccolor;
        }
    }
    cantidadcoloresutilizados = cmax + 1; //encuentra el color mas alto, pero los colores empiezan desde 0

    char* checkcolorespermutacion = (char*)calloc(cantidadcoloresutilizados, sizeof(char));
    for (u32 r = 0; r < cantidadcoloresutilizados; ++r){
        if (perm[r] > (cantidadcoloresutilizados - 1)){
            #ifndef NDEBUG
            printf("Reorden.c: Error 1:Color mayor al posible\n");
            printf("r es %u. perm[r] es %u. cantidaddeutilizados es %u.\n", r, perm[r], cantidadcoloresutilizados);
            #endif
            //la permutacion tiene un color que es mas grande que cualquier color que hayamos utilizado para pintar
            free(checkcolorespermutacion);
            return(0);
        }
        if (checkcolorespermutacion[perm[r]] == 1){
            //el color esta repetido
            #ifndef NDEBUG
            printf("Reorden.c: Error 2:Color repetido dentro de la permutacion\n");
            #endif
            free(checkcolorespermutacion);
            return(0);
        }
        checkcolorespermutacion[perm[r]] = 1;

    }

    for(u32 r = 0; r < cantidadcoloresutilizados; ++r){
        if(checkcolorespermutacion[r] == 0){
            //hay un color que no fue asignado por la permutacion
            //ergo la permutacion es invalida
            #ifndef NDEBUG
            printf("Reorden.c: Error 3:Color faltante en la permutacion\n");
            #endif
            free(checkcolorespermutacion);
            return(0);
        }
    }
    free(checkcolorespermutacion);

    u32 cantidaddearr = cantidadcoloresutilizados;
    u32 almacenamientoinicial = CAPACIDADINICIALDEARRAY;

    u32 cantidadVert = NumeroDeVertices(G);

    arrays2_p organizacion = (arrays2_p)calloc(1, sizeof(struct rsoa2));
    organizacion->arrayscolores = (u32**)malloc(sizeof(u32*) * cantidaddearr);
    organizacion->tamanioarrporcolor = (u32*)malloc(sizeof(u32) * cantidaddearr);
    for(u32 i = 0; i < cantidaddearr; ++i){
        organizacion->arrayscolores[i] = (u32*)malloc(sizeof(u32) * almacenamientoinicial);
        organizacion->tamanioarrporcolor[i] = almacenamientoinicial;
    }
    organizacion->cantidaddeverticesporcolor = (u32*)calloc(cantidaddearr, sizeof(u32));
    

    /*Fin inicializacion*/

    /*Inicio loop de carga*/
    
    for(u32 i = 0; i < cantidadVert; ++i){
        FijarOrden(i, G, i); //esto es para que G.Orden sea igual a G.OrdenNatural.
    }

    char exitloop = 0;
    for (u32 iesimo = 0; iesimo < cantidadVert; ++iesimo){
        u32 colorvert = Color(iesimo, G);


        if(colorvert == UINT32_MAX){
            //*El vertice deberia estar pintado, y no lo está.
            //*Acá hay que salir
            #ifdef NDEBUG
            printf("Se encontro un vertice despintado cuando habia que reordenarlo por color. Esto esta mal\n");
            #endif
            exitloop = 1;
            break;
        }

        if(colorvert < cantidaddearr){
            u32* arraydestino = organizacion->arrayscolores[colorvert];
            u32 capacidad = organizacion->tamanioarrporcolor[colorvert];
            u32* lugaresocupados = &organizacion->cantidaddeverticesporcolor[colorvert];
            if((*lugaresocupados) < capacidad){//menor o igual porque lugaresocupados apunta la posicion desocupada inmediata.EDIT:pero capacidad es capacidad-1
                arraydestino[(*lugaresocupados)] = iesimo;
                ++(*lugaresocupados);

            }
            else{
                //hay que agrandar el arraydestino.
                u32* temp = realloc(organizacion->arrayscolores[colorvert], ((capacidad + 1) * sizeof(u32)));
                if(temp != NULL){
                    organizacion->arrayscolores[colorvert] = temp;
                    ++organizacion->tamanioarrporcolor[colorvert];
                    organizacion->arrayscolores[colorvert][(*lugaresocupados)] = iesimo;
                    ++(*lugaresocupados);
                    
                }else{
                    #ifndef NDEBUG
                    printf("Error en realloc, archivo %s, linea %i\n", __FILE__, __LINE__);
                    #endif
                    //free a todo
                    for(u32 i = 0; i < cantidaddearr; ++i){
                        if(organizacion->arrayscolores[i] != NULL){
                            free(organizacion->arrayscolores[i]);
                        }                       
                    }
                    free(organizacion->arrayscolores);
                    free(organizacion);

                    //exit(12);
                    return((char)0);
                }


            }


        }
        else {
            //hay que agrandar organizacion->arrayscolores
            u32** tempp = (u32**)realloc(organizacion->arrayscolores,(sizeof(u32*) * (colorvert + 1)) );

            if(tempp != NULL){
                organizacion->arrayscolores = tempp;
                u32 tempcantidad = cantidaddearr;
                cantidaddearr = colorvert + 1;
                u32* arrcantidad = organizacion->cantidaddeverticesporcolor;
                u32* arrtamanio = organizacion->tamanioarrporcolor;
                u32* nuevoarrcantidad = (u32*)realloc(arrcantidad, sizeof(u32) * cantidaddearr);
                u32* nuevoarrtamanio = (u32*)realloc(arrtamanio, sizeof(u32) * cantidaddearr);
                if(nuevoarrcantidad != NULL && nuevoarrtamanio != NULL){
                    organizacion->tamanioarrporcolor = nuevoarrtamanio;
                    organizacion->cantidaddeverticesporcolor = nuevoarrcantidad;

                    for(u32 i = tempcantidad; i < cantidaddearr; ++i){
                        organizacion->cantidaddeverticesporcolor[i] = 0;
                        organizacion->arrayscolores[i] = (u32*)malloc(sizeof(u32) * almacenamientoinicial);
                        assert(organizacion->arrayscolores[i] != NULL);
                        organizacion->tamanioarrporcolor[i] = almacenamientoinicial;

                        
                    }


                }
                else{
                    //liberar memoria y salir porque no pudimos agrandar los arreglos que usamos como "listas" (porque las listas son lentas en grafos grandes)
                    if(nuevoarrcantidad != NULL){
                        free(nuevoarrcantidad);
                    }
                    if(nuevoarrtamanio != NULL){
                        free(nuevoarrtamanio);
                    }
                    for(u32 i = 0; i < cantidaddearr; ++i){
                        if(organizacion->arrayscolores[i] != NULL){
                            free(organizacion->arrayscolores[i]);
                        }                       
                    }
                    free(organizacion->arrayscolores);
                    free(organizacion);
                    return((char)0);
                    //exit(12);
                }
                

            }
            else{
                //salio todo mal
                for(u32 i = 0; i < cantidaddearr; ++i){
                    if(organizacion->arrayscolores[i] != NULL){
                        free(organizacion->arrayscolores[i]);
                    }                       
                }
                free(organizacion->arrayscolores);
                free(organizacion);
                return((char)0);
                //exit(12);
            }

            //ahora podemos asegurar que hay espacio, entonces lo agregamos donde corresponda
            u32* arraydestino = organizacion->arrayscolores[colorvert];
            u32* lugaresocupados = &organizacion->cantidaddeverticesporcolor[colorvert];
            arraydestino[(*lugaresocupados)] = iesimo;
            ++(*lugaresocupados);

        }

    }
    //*Establecer el nuevo orden de G, segun lo indica perm.

    if(exitloop == 0){

        //--------------------------//
        //Vertice* neworder = Orden(G);
        u32 offset = 0;
        u32 cantidadanterior = 0;
        for(u32 j = 0; j < cantidaddearr; ++j){
            u32* iesimoarr = organizacion->arrayscolores[perm[j]];
            u32 cantidaddeverticesEnEsteColor = organizacion->cantidaddeverticesporcolor[perm[j]];



            if(j != 0){
                offset = offset + cantidadanterior;
            }
            cantidadanterior = organizacion->cantidaddeverticesporcolor[perm[j]];

            for(u32 k = 0; k < cantidaddeverticesEnEsteColor; ++k){
                
                FijarOrden((offset + k), G, iesimoarr[k]);

            }

            //memcpy((neworder+offset), iesimoarr, (organizacion->cantidaddeverticesporcolor[perm[j]] * sizeof(Vertice)));

            //liberamos la memoria de este array, porque ya no tiene utilidad.
            free(organizacion->arrayscolores[perm[j]]);
            
        }//end for(): por cada array...
    }

    //destruir cada array
    //EDIT: lo hacemos en el mismo loop de arriba para ahorrar

    //destruir coloresutilizados
    free(organizacion->tamanioarrporcolor);
    free(organizacion->cantidaddeverticesporcolor);
    free(organizacion->arrayscolores);
    free(organizacion);

    return ((char)1);
}
