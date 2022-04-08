#include "hashtable_b.h"
#include "vertice.h"
#include <stdint.h>
#if (defined(_WIN64) && defined(_MSC_VER)) || defined(__clang__)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "read_dimacs.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>


//char* nextline(FILE* fp) {
//    static u32 linenum = 0;
//    static fpos_t position = NULL;
//    char* buff = (char*)malloc(sizeof(char) * DIMACS_MAXLINE_SIZE);
//
//    fp = fopen("q10.corr.txt", "r"); // opening an existing file
//    if (fp == NULL)
//    {
//        //printf("Could not open file %s\n", filename);
//        return NULL;
//    }
//    
//    if (linenum == 0) { //first time
//
//        position = fgetpos(fp, &position);
//        ++linenum;
//
//        return (fgets(buff, DIMACS_MAXLINE_SIZE, fp));
//    }
//
//    fsetpos(fp, position);
//
//}
//plista* tablahash = NULL;
hashtable_t* tablab = NULL;

//u32 miembrosHash[3000];
u32 cantidad = 0;
u32 cargados = 0;
Grafo graph;

/*
static uint64_t seed;
static u32 seed2;




void inithf2() {
    srand(floor((uint64_t)327.0f * (uint64_t)rand()));
    seed = rand();

    seed2 = rand();
    

}


uint64_t hashfunc2(uint64_t k, u32 m) {

    //uint64_t seed2 = floor((uint64_t)33427.0f * (uint64_t)UINT32_MAX);

    uint64_t r;

    uint64_t s = seed;//floor((uint64_t)33427.0f * (uint64_t)UINT32_MAX);

    //printf("S es: %lu\n", s);
    
    
    uint64_t x = k * s;

    //printf("X es: %lu\n", x);

    r = (x ^ seed) ;

    u32 rr = (u32)r % m;

    
    //r = x % (uint64_t)m;


    //printf("R es: %lu\n", r);

    //printf("RR es: %u\n\n\n", rr);


    return (r);
}

void testHash() {
    inithf2();
    for (u32 i = 0; i < 30; ++i) {
        u32 r = ((hashfunc(i, 90)) ^ seed2) % 90;
        printf("R es: %u\n", r);
    }
    printf("\n\n\n");
    //inithf2();
    //for (u32 i = 0; i < 30; ++i) {
    //    uint64_t r = hashfunc2((uint64_t)i, 90);
    //    //printf("R es: %lu\n", r);
    //}

    //uint64_t r = hashfunc2((uint64_t)29, 90);
    u32 r2 = ((hashfunc(29, 90)) ^ seed2) % 90;

    printf("R2 es:%u\n\n", r2);

}
*/
Grafo read_dimacs() {

    //FILE* fp;
    //prototipo
    Vertice buffer = NULL;
    u32 indicebuffer = 0;
    //
    char* c;
    u32 edges = 0;
    u32 vertices = 0;
    char* verticesChar = (char*)calloc((size_t)LONGITUDCIFRA, sizeof(char));
    char keyword[] = "edge";
    /*
    printf("Opening the file in read mode\n");
    fp = fopen("Octo.txt", "r"); // opening an existing file
    if (fp == NULL)
    {
        printf("Could not open file \n");
        return 1;
    }
    */
    //printf("Reading the file\n");
    char* buff = (char*)malloc(sizeof(char) * DIMACS_MAXLINE_SIZE);
    
    while (1)
    {
        c = fgets(buff, DIMACS_MAXLINE_SIZE, stdin); // reading the file
        //c = fgets(buff, DIMACS_MAXLINE_SIZE, fp);
        if (c == NULL) {

            //break;
            free(buff);
            free(verticesChar);
            #ifndef NDEBUG
            printf("No se pudo leer una linea desde stdin.\n");
            #endif
            return(NULL);

        }


        if (c[0] == 'c') {

            //printf("Un comentario\n");
            continue;

        }


        if (c[0] == 'p') {

            //printf("Empieza p\n");
            
            bool errorkeyword = 0; // Esto quedo del viejo main. En realidad no se usa porque si hay algun problema, el programa termina antes de tiempo
            char* cc = &(c[2]); //saltea el espacio entre 'p' y 'edge'
            for (u32 i = 0; i < 4; ++i) {

                //printf("cc%d es: %c\n", i, *cc);


                if (*(cc) != keyword[i]) {
                    
                    errorkeyword = 1; 
                    //break;
                    #ifndef NDEBUG
                    printf("Se buscaba la letra \"%c\" de \"edge\" y no se encontro\n",keyword[i]);
                    #endif
                    free(buff);
                    free(verticesChar);
                    return(NULL);

                }

                //printf("errorkeyword es:%c\n",(char)errorkeyword);

                ++cc;

            }

            if (!errorkeyword) {

                ++cc;
                //printf("ccc es: %c\n", *cc);
                //char digitotemporal[10];
                if (sscanf(cc, "%u", &vertices) == EOF) {
                    printf("Error leyendo p line\n");
                    #ifndef NDEBUG
                    printf("Se buscaba \"p edge <numero1> <numero2>\" pero no se encontro a <numero1>\n");
                    #endif
                    return(NULL);

                }
                    
                //char* itoa(int value, char* str, int base);

                //_itoa(vertices, verticesChar, 10);
                //no usar itoa
                sprintf(verticesChar, "%u", vertices);

                u32 ttt = (u32)strlen(verticesChar);
                //printf("TTT es:%d\n", ttt);
                //u32 len = strspn(cc, vertices);

                //printf("len es:%d\n", len);
                
                cc = cc + ttt;

                while ((*cc) == ' ' || (*cc) == '\t') { //para descartar los espacios o tabs entre n y m.
                    ++cc;
                }

                //printf("cc edge es:%c\n\n",*cc);
                if (sscanf(cc, "%u", &edges) == EOF) {
                    printf("Error leyendo p line\n");
                    #ifndef NDEBUG
                    printf("Se buscaba \"p edge <numero1> <numero2>\" pero no se encontro a <numero2>\n");
                    #endif
                    return(NULL);

                }
                    
                
                //printf("edges1 es:%d\n\n", edges);
                //bool erroredge = 0;
                
                //while ((*c) != '\n') //seguir a la proxima linea e ignorar cualquier otra cosa
                //    c = fgetc(fp);

                /// Esto es para ver si son digitos decimales
                /// 

                //for (u32 lineas = 0; lineas < edges; ++lineas) {

                //    c = fgetc(fp);
                //    
                //    if (c == 'e') {
                //        c = fgetc(fp); //espacio
                //        c = fgetc(fp);
                //        
                //        while ((int)c > 47 && (int)c < 58) {

                //        }
                //    }



                //}
                

            }
            else {
                printf("El archivo no contiene una linea \'p edge\' valida\n");
                return (NULL);
            }

            //printf("Hay %d vertices. Hay %d lados\n\n", vertices, edges);

            //Hacer lo que corresponda con inicializacion del grafo

            //tablahash = createTable((3*vertices));
            tablab = inicializartabla(vertices);

            //assert(tablahash != NULL);
            assert(tablab != NULL);

            graph = InicializacionDelGrafo(vertices, edges);

            //prototipo

            buffer = Reservarmemoria(vertices);
            if(buffer == NULL){
                printf("Buffer == NULL\n");
                exit(12);
            }
        

            

            //
            break;
        }

        if (c[0] == 'e') {

            printf("La linea empieza con e\nBreak\n");
            break;
            //return (1);

        }
        //printf("Char is %c\n", c);
    }

    char* verticeChar = (char*)calloc((size_t)120, sizeof(char));
    long unsigned int kkkk = 0;
    u32 errores = 0;

    for (u32 i = 0; i < edges; ++i) {
        ++kkkk;

        u32 v;
        u32 w;

        c = fgets(buff, DIMACS_MAXLINE_SIZE, stdin);
        //c = fgets(buff, DIMACS_MAXLINE_SIZE, fp);

        //printf("C: %s\n", buff);

        if (c[0] != 'e') {
            printf("Error al leer una linea de lado\n");
            ++errores;
            break;
        }

        ++c; //ya leimos 'e'
        
        if (c[0] == '\t') {
            printf("hay un tab extra entre e y el int\n");
            ++c; //para saltear el espacio
        }
        
        if (sscanf(c, "%u", &v) == EOF) {
            printf("Error leyendo e line\n");
            ++errores;
            //free(buffer);
            return (NULL);

        }
            

        ++c;


        while ((*c) == ' ' || (*c) == '\t') { //para descartar los espacios o tabs entre v y w.
            ++c;
        }
        
        //no usar itoa porque puede el nombre del vertice puede hacer overflow dentro de itoa y devuelve negativo
        //_itoa(v, verticeChar, 10);

        sprintf(verticeChar, "%u", v);

        //printf("verticeChar es:%s\n", verticeChar);

        u32 offset = (u32)strlen(verticeChar);
        //printf("Offset es:%d\n", offset);

        c = c + offset;

        if (sscanf(c, "%u", &w) == EOF) {
            printf("Error leyendo e line\n");
            ++errores;
            //free(buffer);
            return (NULL);

        }
            

        //printf("e v:%u w:%u\ti:%u\tCargados:%u\n\n", v, w, i, cargados);


        //Hacer lo que corresponda por cada edge encontrado

        //

        //

        Vertice nuevovertice;
        Vertice nuevovecino;

        //Vertice busqueda = existeVertice(tablahash, v);
        Vertice busqueda = buscarelemento(tablab, v);
        

        if (busqueda == NULL) {

            //nuevovertice = CrearVertice(v, UINT32_MAX);
            //prototipo
            nuevovertice = CrearVerticeb(v, UINT32_MAX, buffer, &indicebuffer);

            //printf("Creando lista\n");
            nuevovertice->vecinos->lvecinos = crearlista();
            //printf("Lista creada\n");

            //u32 resultt = insertVertice(tablahash, nuevovertice);

            //insertVertice(tablahash, nuevovertice);
            #ifndef NDEBUG
            char checkh = insertarelemento(tablab, nuevovertice);
            if(checkh == 0){
                printf("Hay algun error al insertar el elemento nro:%u\n", nuevovertice->nombre);
                exit(1);
            }
            #else
            insertarelemento(tablab, nuevovertice);
            #endif

            //miembrosHash[cantidad] = v;
            //++cantidad;
            //DEBUG
            //graph->vertices[cargados] = nuevovertice;
            //DEBUG
            Addvertice(graph, nuevovertice, cargados);
            ++cargados;

            //printf("Vertice nu %u CREADO\n",v);

        }
        else {
            nuevovertice = busqueda;
        }



        //esto es por si hay definido un edge tal que: e v w; donde v es igual a w ( un vertice sobre si mismo)
        if (v != w) {

            //Vertice busquedavecino = existeVertice(tablahash, w);
            Vertice busquedavecino = buscarelemento(tablab, w);

            if (busquedavecino == NULL) {
                //nuevovecino = CrearVertice(w, UINT32_MAX);

                //prototipo
                nuevovecino = CrearVerticeb(w, UINT32_MAX, buffer, &indicebuffer);

                nuevovecino->vecinos->lvecinos = crearlista();
                //u32 resultt2 = insertVertice(tablahash, nuevovecino);

                //insertVertice(tablahash, nuevovecino);
                #ifndef NDEBUG
                char checkh2 = insertarelemento(tablab, nuevovecino);
                if(checkh2 == 0){
                    printf("Hay algun error al insertar el elemento nro:%u\n", nuevovecino->nombre);
                    exit(1);
                }
                #else
                insertarelemento(tablab, nuevovecino);
                #endif


                //miembrosHash[cantidad] = w;
                //++cantidad;

                //printf("Vertice vec %u CREADO\n", w);
                //DEBUG
                //graph->vertices[cargados] = nuevovecino;
                //DEBUG
                Addvertice(graph, nuevovecino, cargados);
                ++cargados;


            }
            else {
                nuevovecino = busquedavecino;
            }

            //nuevovertice.agregarnuevovecino

            insertarnodo(nuevovertice->vecinos->lvecinos, nuevovecino);

            //printf("Nuevo vecino agregado\n");

            //

            //nuevovecino.agregarnuevovecino

            insertarnodo(nuevovecino->vecinos->lvecinos, nuevovertice);
        }
        else { // entonces es un vertice sobre si mismo: solamente lo agregamos como vecino de si mismo.
            insertarnodo(nuevovertice->vecinos->lvecinos, nuevovertice);
            //printf("edge sobre si mismo\n");
        }
        


        

    }
    //DEBUG
    //printf("Leidos:%lu\tCargados:%u\tInserts:%u\tErrores:%u\n\n", kkkk, cargados, cuantosinserts(), errores);
    //DEBUG
    Vertice* que;
    que = Miembros(graph);
    u32 cantidaddevecinosagregada = edges * 2;
    /*
    for(u32 i = 0; i < vertices; ++i){
        cantidaddevecinosagregada += que[i]->vecinos->lvecinos->cantidad;
    }
    */
    Vertice* buffervecindades = Reservarmemoriavecindades(cantidaddevecinosagregada);
    u32 indicebuffervecindades = 0;
    for (u32 i = 0; i < cargados; ++i) {

        //printf("Miembrohash %u es:\t%u\n", i, miembrosHash[i]);
        //que = getVertice(tablahash, miembrosHash[i]);
        //printf("vecinoslista%u:%u\n", i,que[i]->vecinos->lvecinos->cantidad);
        
        listatoarrayvecinosmod(que[i], buffervecindades, &indicebuffervecindades);

        //Vertice ttest = iesimoVecino(que, 2);
        //if (ttest != NULL) {

        //
        //    printf("El IESIMO vecino es:%u\n", ttest->nombre);

        //}

        //printf("Grado (segun la lista):%u\n\n", que->vecinos->lvecinos->cantidad);
        //que = DestVerticeUnitario(que);

        
    }

    /*Ya podemos liberar la memoria utilizada, el grafo ya esta cargado*/
    //assert(tablahash != NULL);
    assert(tablab != NULL);
    //destroyTable(tablahash);
    destruirtablahash(tablab);
    assert(buff != NULL);
    free(buff);
    assert(verticeChar != NULL);
    free(verticeChar);
    assert(verticesChar != NULL);
    free(verticesChar);
    /*Fin free()'s*/


    //DEBUG
    //graph->DeltaG = Delta(graph);
    Setdelta(graph, Delta(graph));
    //copiaarray(graph->vertices, graph->orden, graph->num_vertices);
    copiavertaorden(graph);
    copiavertaordenNatural(graph);
    
    Vertice* naturalord = OrdenNatural(graph);
    //printf("Comienza sort()\n");
    //Vertice* st = isOrdenNatural(naturalord, NumeroDeVertices(graph));
    Vertice* st = ordenqsort(naturalord, NumeroDeVertices(graph));
    //printf("Termino sort()\n");
    //quicksort: Falta probarlo bien, no funciona bien

    //Vertice* st = qsOrdenNatural(naturalord, 0, NumeroDeVertices(graph));
    setOrdenNatural(graph, st);


    //DEBUG

    //DEBUG
    //printf("\n\n\tCANTIDAD:%u\n\n", cargados);
    //printf("Closing stdin\n");
    //DEBUG

    //for (u32 k = 0; k<cantidad; ++k){
    //    Vertice vertice = getVertice(tablahash, hashfunc((miembrosHash[k]), (3 * vertices)));
    //    free(vertice);
    //}
    
    //printf("Returning readdimacs()\n");

    //fclose(fp);
    return (graph);

}
