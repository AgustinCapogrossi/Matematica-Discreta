
#include "GrafoSt21.h" 
#include "hashtable.h"
#include "listaenlazada.h"
#include "orden.h"
#include "read_dimacs.h"
#include "vertice.h"
#include <string.h> //para poder usar memcpy
#include "hashtable_b.h"
#include <assert.h>

// #ifndef TIMEFUNC
// #define TIMEFUNC
// #ifdef _WIN64

// #include <windows.h>
// double get_time()
// {
// 	LARGE_INTEGER t;
// 	LARGE_INTEGER f;
// 	QueryPerformanceCounter(&t);
// 	QueryPerformanceFrequency(&f);
// 	return (double)t.QuadPart / (double)f.QuadPart;
// }
// /*
// static double get_tticks()
// {
// 	LARGE_INTEGER t;
// 	QueryPerformanceCounter(&t);
// 	return (double)t.QuadPart;
// }
// */

// #else

// #include <sys/time.h>
// #include <sys/resource.h>

// static inline double get_time()
// {
// 	struct timeval t;
// 	//struct timezone tzp;
// 	gettimeofday(&t, NULL);
// 	return t.tv_sec + t.tv_usec * 1e-6;
// }

// #endif

// #endif

/*___________________________________________________________________________*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~ Estructura del grafo. ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*___________________________________________________________________________*/
struct GrafoSt {
	Vertice* vertices;  /*|Arreglo de punteros a vertices. |*/
	Vertice* orden;         /*|Orden que va ha tomar Greedy.   |*/
	Vertice* ordenNatural; /*|Orden ordenado por nombre   |*/
	u32 DeltaG; /*|Delta del grafo   |*/
	u32 coloresutilizados; /*|Cantidad de colores utilizados para pintar al grafo. Lo actualiza el ultimo algoritmo de coloreo que haya coloreado el grafo  |*/
	u32 num_vertices;   /*|Cantidad de vertices.		   |*/
	u32 m_lados;        /*|Cantidad de lados.			   |*/
};

/*___________________________________________________________________________*/
/*~~~~~~~~~~|Funciones de Construccion/Destruccion/Copia del Grafo|~~~~~~~~~~*/
/*___________________________________________________________________________*/
Vertice* ArrayOrdenVertices (u32 num_vertices) {
	//Reserva memoria para una cantidad num_vertices de Vertice's
	Vertice* orden = (Vertice*)malloc(num_vertices * sizeof(Vertice));

    return orden;
}
Grafo ConstruccionDelGrafo() {
	//Es una funcion "wrapper" de read_dimacs()
	Grafo G = read_dimacs();
	return (G);
}
Grafo InicializacionDelGrafo(u32 num_vertices, u32 m_lados){
	//Inicializa la estructura GrafoSt
	//Reserva espacio para el array de vertices, y ambos arrays de orden:orden y ordenNatural
    Grafo G = calloc(1, sizeof(struct GrafoSt));
    assert(G != NULL);
	
	G->num_vertices = num_vertices;
    G->m_lados = m_lados;
	G->vertices = ArrayVertices(num_vertices);

	/* El orden inicial es igual al orden de carga 
	ordenNatural: esto se carga y ordena despues, al final en read_dimacs()*/
	G->orden = ArrayOrdenVertices(num_vertices);
	G->ordenNatural = ArrayOrdenVertices(num_vertices);
	G->DeltaG = 0;
	

    assert(G->vertices != NULL);
    assert(G->orden != NULL);
	assert(G->ordenNatural != NULL);
    return (G);
}

void DestruccionDelGrafo_old(Grafo G){
	//Libera toda la memoria reservada por InicializacionDelGrafo()
	//Tambien destruye todos los vertices que tiene el grafo
		
	u32 cantidad = G->num_vertices;
	G->vertices = DestVertices(G->vertices, cantidad);

	/*Como Vertices* orden es una permutacion de Vertices, y
	a esos vertices los acabamos de eliminar, entonces solamente resta
	liberar la memoria del array orden*/


	free(G->vertices);

	free(G->orden);
	free(G->ordenNatural);
	G->orden = NULL;
	G->vertices = NULL;
	G->ordenNatural = NULL;

	free(G);
	G = NULL;
}

void copiaarray_orig(Vertice* arrayorigen, Vertice* arraydestino, u32 longitud) {
	//Copia un array de Vertice's desde origen a destino

	for (u32 i = 0; i < longitud; ++i) {
		arraydestino[i] = arrayorigen[i];
	}

}

void copiaarray(Vertice* arrayorigen, Vertice* arraydestino, u32 longitud) {
	//Copia un array de Vertice's desde origen a destino
	memcpy(arraydestino, arrayorigen, (sizeof(Vertice) * longitud));

}

void copiavertaorden(Grafo G) {
	//Copia el array de vertices de G en la memoria reservada para el orden de G
	//Funcion 'wrapper' de copiaarray()

	copiaarray(G->vertices, G->orden, G->num_vertices);

}

void copiavertaordenNatural(Grafo G) {
	//Copia el array de vertices de G en la memoria reservada para el ordenNatural de G
	//Funcion 'wrapper' de copiaarray()

	copiaarray(G->vertices, G->ordenNatural, G->num_vertices);

}

Grafo CopiarGrafolista(Grafo G){

	plista* hasht = createTable(G->num_vertices);
	 

	Grafo H = calloc(1, sizeof(struct GrafoSt)); 

	assert(H != NULL);
	 
	H->num_vertices = G->num_vertices;
	H->m_lados = G->m_lados;
	H->coloresutilizados = G->coloresutilizados;
	
	H->vertices = (Vertice *)malloc(H->num_vertices * sizeof(Vertice));
	size_t sizeVerticeSt = sizeof(struct VerticeSt);

	for(u32 i = 0; i < H->num_vertices; ++i){
		H->vertices[i] = (Vertice)malloc(sizeof(struct VerticeSt));
		H->vertices[i] = memcpy(H->vertices[i], G->vertices[i], sizeVerticeSt);
		H->vertices[i]->vecinos = (impvecinos)malloc(sizeof(union implementacionvecinos));
		nuevaVecindad(H->vertices[i], H->vertices[i]->grado);
		insertVertice(hasht, H->vertices[i]);
	}


	//no se puede copiar directo, porque necesitamos vertices nuevos
	//Aqui creamos una copia de cada vertice. Luego completamos el array de vecinos de cada uno
	//Para eso usamos una nueva tabla hash, y la usamos de la misma manera que en read_dimacs()

	//Aqui usamos la tabla hash para encontrar la direccion de cada vecino
	for (u32 i = 0; i < H->num_vertices; ++i) {
		for (u32 k = 0; k < G->vertices[i]->grado; ++k) {
			H->vertices[i]->vecinos->avecinos[k] = existeVertice(hasht, G->vertices[i]->vecinos->avecinos[k]->nombre);	
		}
	}

	H->orden = ArrayOrdenVertices(H->num_vertices);
	copiaarray(H->vertices, H->orden, H->num_vertices);

	H->ordenNatural = ArrayOrdenVertices(H->num_vertices);
	copiaarray(H->vertices, H->ordenNatural, H->num_vertices);
	//Ordenar ordenNatural, asi puede ser usado despues de crear la copia
	ordenqsort(H->ordenNatural, H->num_vertices);

	H->DeltaG = G->DeltaG;

	destroyTable(hasht); //se puede hacer antes
	
	return H;	 

}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*___________________________________________________________________________*/
/*~~~~~~~~~~|Funciones para extraer informacion de datos del Grafo|~~~~~~~~~~*/
/*___________________________________________________________________________*/

//Devuelve el número de vértices del Grafo.
u32 NumeroDeVertices(Grafo G){
    assert(G != NULL);
    return G->num_vertices;
}


//Devuelve el número de lados o aristas del Grafo.
u32 NumeroDeLados(Grafo G){
    assert(G != NULL);
    return G->m_lados;
}

//Devuelve el orden en el que se evalúa el Grafo.
Vertice* Orden(Grafo G){
	return G->orden;
}

//Devuelve el arreglo de vertices que son parte del Grafo.
Vertice* Miembros(Grafo G) {
	return G->vertices;
}

//Agrega un nuevo miembro (vertice) al Grafo, en la posicion pos
void Addvertice(Grafo G, Vertice vert, u32 pos) {

	G->vertices[pos] = vert;

}

/*Establece el grado mas alto del grafo G*/
void Setdelta(Grafo G, u32 delt) {
	G->DeltaG = delt;
}
/*Devuelve el grado mas alto del grafo G*/
u32 GetDelta(Grafo G) {
	return (G->DeltaG);
}


//Calcula Delta del Grafo.

u32 Delta(Grafo G){
	u32 maximo = 0;
	Vertice vv = NULL;
	for (u32 i = 0; i<G->num_vertices; i++){
		vv = G->vertices[i];
		if (vv == NULL) { //esto hay que sacarlo, porque no se supone que haya un vertice con direccion NULL
			continue;
		}
		if (maximo < gradoVertice(vv)) {
			maximo = gradoVertice(G->vertices[i]);
		}
	}
	return (maximo);
}
//Devuelve el array ordenNatural
Vertice* OrdenNatural(Grafo G) {
	return (G->ordenNatural);
}
//Establece el array ordenNatural en el ordenr dado
void setOrdenNatural(Grafo G, Vertice* ordenr) {
	G->ordenNatural = ordenr;
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*___________________________________________________________________________*/
/*~~~~~~~~~~~~|Funciones para extraer información de los Vertices|~~~~~~~~~~~*/
/*___________________________________________________________________________*/
//son todas funciones por posicion en el orden vigente

//Devuelve el nombre del vértice.
extern inline u32 Nombre(u32 i,Grafo G){

	//Lo hacemos leyendo directo

	return(G->orden[i]->nombre);
}

//Devuelve el color del vértice.
extern inline u32 Color(u32 i,Grafo G){
	u32 color;
	if (i <= G->num_vertices){

		//Lo hacemos leyendo directo

		color = G->orden[i]->color;
	}
	else{
		color = UINT32_MAX;
	}
	    
    return color;
    
}

//Devuelve el grado del vértice.
extern inline u32 Grado(u32 i,Grafo G){
	u32 grado;
	if (i<=G->num_vertices){

		//Lo hacemos leyendo directo

		grado = G->orden[i]->grado;
	}
	else{
		grado = UINT32_MAX;
	}
	    
    return grado;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*___________________________________________________________________________*/
/*~~~~~|Funciones para extraer información de los vecinos de un vértice|~~~~~*/
/*___________________________________________________________________________*/
//Devuelve el color del vecino j-ésimo en el orden interno del elemento i-ésimo en el orden general del Grafo.
extern inline u32 ColorVecino(u32 j,u32 i,Grafo G){
	return(G->orden[i]->vecinos->avecinos[j]->color);
}

//Devuelve el nombre del vecino j-ésimo en el orden interno del elemento i-ésimo en el orden general del Grafo.
u32 NombreVecino(u32 j,u32 i,Grafo G){

	return(G->orden[i]->vecinos->avecinos[j]->nombre);
}

//Devuelve el orden general del vecino j-ésimo en el orden interno del elemento i-ésimo en el orden general del Grafo.
#if defined(__GNUC__)
//Sirve para que no salga el warning por no retornar un valor en todos los exits de la funcion
//De igual forma, emitimos un mensaje que no es un warning para avisarle al usuario cuando compila
//Esto es tanto para clang como para gcc

#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic push
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
#endif

//mensaje
#pragma message "-Wreturn-type ignorado en OrdenVecino() [GrafoSt21.c]"

u32 OrdenVecino(u32 j,u32 i,Grafo G){

	return(G->orden[i]->vecinos->avecinos[j]->ordenRelativo);
	
}

#ifdef __GNUC__
//esto lo hacemos para que solamente se ignore ese tipo de warning en esta funcion, y solo esta funcion.
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif


//Calcula el peso del lado conformado por el elemento i-ésimo del orden general del grafo y su j-ésimo vecino.
//u32 PesoLadoConVecino(u32 j,u32 i,Grafo G){
//	Vertice verticei = G->orden[i];
//	u32* vecinosi = VecinosVertice(verticei);
//	return pesototal;
//}	


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


/*___________________________________________________________________________*/
/*~~~~~~~~~~~~~~|Funciones para modificar datos de los vértices|~~~~~~~~~~~~~*/
/*___________________________________________________________________________*/

//asigna color x al v'ertice i del orden interno Orden
__attribute__ ((hot, nonnull (3))) inline char FijarColor(u32 x,u32 i,Grafo G){
	if(i<G->num_vertices){
		//pintarVertice(G->orden[i],x);

		//Lo hacemos directo
		G->orden[i]->color = x;
		return 0;
	}
		return 1;

	
}

//FijarOrden es destructivo: no guarda el vertice que estaba en la posicion N. Eso lo tiene que hacer el que llama a esta funcion
char FijarOrden(u32 i,Grafo G,u32 N){

	//FijarOrden es destructivo: no guarda el vertice que estaba en la posicion N. Eso lo tiene que hacer el que llama a esta funcion

	if (i<G->num_vertices && N<G->num_vertices){

		G->orden[i] = G->ordenNatural[N];
		G->orden[i]->ordenRelativo = i;
		return 0;
	}
	else{
		return 1;
	}
}
//Aleatoriza de forma deterministica segun el argumento u32 R
//Siempre retorna 0 (no hubo problema) porque asume que Grafo G esta bien conformado (no tiene vertices NULL)
char AleatorizarVertices_directo(Grafo G,u32 R){
	randomizarordenconseed_directo(Orden(G), NumeroDeVertices(G), R);
    return ((char)0);
}

static char srandunavez = 0;

//-------------------------------------------------//

inline void randomizarpos(u32* restrict orden, u32 size) {
    --size; //porque los indices el array van de 0 a size-1 //EDIT: No es por eso, es porque le sumamos 1 a posicionrand.
    for (u32 i = 0; i < size; ++i) {
        u32 temp = orden[i];
        long long int posicionrand = ((int64_t)rand() % ((int64_t)size - i + 1)) + i;
        orden[i] = orden[posicionrand];
        orden[posicionrand] = temp;
    }

}

//-------------------------------------------------//

inline void randomizarordenconseed(u32* arrayAux, u32 size, unsigned int seed){



    if(srandunavez == 0){
        srand(seed);
        ++srandunavez;
    }

    for(u32 i = 0; i < size; ++i){
        arrayAux[i] = i;
    }

    randomizarpos(arrayAux, size);


}

char AleatorizarVertices(Grafo G,u32 R){
	//randomizarordenconseed(Orden(G), NumeroDeVertices(G), R);
	u32 size = NumeroDeVertices(G);
	u32* arrayAux = (u32*)malloc(sizeof(u32) * size);

	if(arrayAux == NULL){
		return((char)1);
	}

	randomizarordenconseed(arrayAux, size, R);
	for(u32 i = 0; i < size; ++i){
		//pone al vertice que esta en el orden natural I en el lugar random de Orden(interno) que indica arrayAux.
		FijarOrden(i, G, arrayAux[i]);
		
	}

	free(arrayAux);
    return ((char)0);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void DestruccionDelGrafo(Grafo G){
	//Libera toda la memoria reservada por InicializacionDelGrafo()
	//Tambien destruye todos los vertices que tiene el grafo
		
	u32 cantidad = G->num_vertices;

	DestVerticesmod(G->vertices, cantidad);

	/*Como Vertices* orden es una permutacion de Vertices, y
	a esos vertices los acabamos de eliminar, entonces solamente resta
	liberar la memoria del array orden*/


	free(G->vertices);	//NOT==>Se hace en DesVerticesmod.

	free(G->orden);
	free(G->ordenNatural);
	G->orden = NULL;
	G->vertices = NULL;
	G->ordenNatural = NULL;

	free(G);
	G = NULL;

}

//_____________________________________________________________________________________//

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//Copia el grafo, pero los vertices y los arrays de cada vertice con los punteros a sus vecinos estan todos juntos, en dos bloques respectivos
//Lo hacemos asi para que la destruccion del grafo y la copia sea mas rapida.
Grafo CopiarGrafo(Grafo G){
	//este copiagrafo usa la nueva tabla con quadratic probing

	u32 nvert = G->num_vertices;

	hashtable_t* tablab = inicializartabla(nvert);

	Grafo H = calloc(1, sizeof(struct GrafoSt)); 

	assert(H != NULL);
	
	Vertice buffer = Reservarmemoria(nvert);
	if(buffer == NULL){
		printf("No se pudo reservar la memoria. Archivo:%s Linea:%i\n", __FILE__, __LINE__);
		exit(12);
	}
	Vertice* buffervecindades = Reservarmemoriavecindades((G->m_lados * 2));
	if(buffervecindades == NULL){
		printf("No se pudo reservar la memoria. Archivo:%s Linea:%i\n", __FILE__, __LINE__);
		exit(12);
	}
	u32 indicebuffervecindades = 0;
	size_t sizeVerticeSt = sizeof(struct VerticeSt);

	memcpy(buffer, (G->vertices[0]), (nvert * sizeVerticeSt));


	 
	H->num_vertices = nvert;
	H->m_lados = G->m_lados;
	H->coloresutilizados = G->coloresutilizados;
	

	H->vertices = (Vertice *)malloc(H->num_vertices * sizeof(Vertice));
	

	for(u32 i = 0; i < nvert; ++i){

		H->vertices[i] = &(buffer[i]);
		
		H->vertices[i]->vecinos = (impvecinos)malloc(sizeof(union implementacionvecinos));

		nuevaVecindadmod(H->vertices[i], H->vertices[i]->grado, buffervecindades, &indicebuffervecindades);

		insertarelemento(tablab, H->vertices[i]);
	}

	H->ordenNatural = ArrayOrdenVertices(H->num_vertices);
	copiaarray(H->vertices, H->ordenNatural, H->num_vertices);
	//Ordenar ordenNatural, asi puede ser usado despues de crear la copia
	ordenqsort(H->ordenNatural, H->num_vertices);

	H->orden = ArrayOrdenVertices(H->num_vertices);


	//no se puede copiar directo, porque necesitamos vertices nuevos
	//Aqui creamos una copia de cada vertice. Luego completamos el array de vecinos de cada uno
	//Para eso usamos una nueva tabla hash, y la usamos de la misma manera que en read_dimacs()

	//Aqui usamos la tabla hash para encontrar la direccion de cada vecino
	//Usamos el mismo loop para establecer el orden a uno identico al del grafo original
	#ifndef NDEBUG
	//double start = get_time();
	#endif
	for (u32 i = 0; i < H->num_vertices; ++i) {
		for (u32 k = 0; k < G->vertices[i]->grado; ++k) {
			//H->vertices[i]->vecinos->avecinos[k] = existeVertice(hasht, G->vertices[i]->vecinos->avecinos[k]->nombre);
			H->vertices[i]->vecinos->avecinos[k] = buscarelemento(tablab, G->vertices[i]->vecinos->avecinos[k]->nombre);
			assert(H->vertices[i]->vecinos->avecinos[k] != NULL);
			
		}

		//aqui copiamos el orden original en la nueva copia. Para eso reutilizamos la tabla hash.
		H->orden[i] = buscarelemento(tablab, G->orden[i]->nombre);
		H->orden[i]->ordenRelativo = i;
		
	}
	#ifndef NDEBUG
	//double end = get_time();
	//printf("%f segundos buscando en hash\n", (end-start));
	#endif

	destruirtablahash(tablab);	

	H->DeltaG = G->DeltaG;
	
	return H;	 

}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//Copia el grafo, pero los vertices y los arrays de cada vertice con los punteros a sus vecinos estan todos juntos, en dos bloques respectivos
//Lo hacemos asi para que la destruccion del grafo y la copia sea mas rapida.
Grafo CopiarGrafomodhs(Grafo G){
	//esta funcion copiar usa la implementacion de hopscotch hashing para la tabla hash

	u32 nvert = G->num_vertices;

	hopnode_t* tablab = inicializarhs(nvert);

	Grafo H = calloc(1, sizeof(struct GrafoSt)); 

	assert(H != NULL);
	
	Vertice buffer = Reservarmemoria(nvert);
	if(buffer == NULL){
		printf("No se pudo reservar la memoria. Archivo:%s Linea:%i\n", __FILE__, __LINE__);
		exit(12);
	}
	Vertice* buffervecindades = Reservarmemoriavecindades((G->m_lados * 2));
	if(buffervecindades == NULL){
		printf("No se pudo reservar la memoria. Archivo:%s Linea:%i\n", __FILE__, __LINE__);
		exit(12);
	}
	u32 indicebuffervecindades = 0;
	size_t sizeVerticeSt = sizeof(struct VerticeSt);

	memcpy(buffer, (G->vertices[0]), (nvert * sizeVerticeSt));


	 
	H->num_vertices = nvert;
	H->m_lados = G->m_lados;
	H->coloresutilizados = G->coloresutilizados;
	
	H->vertices = (Vertice *)malloc(H->num_vertices * sizeof(Vertice));
	

	for(u32 i = 0; i < nvert; ++i){
		H->vertices[i] = &(buffer[i]);
		
		H->vertices[i]->vecinos = (impvecinos)malloc(sizeof(union implementacionvecinos));

		nuevaVecindadmod(H->vertices[i], H->vertices[i]->grado, buffervecindades, &indicebuffervecindades);
		insertarelementohs(tablab, H->vertices[i]);
	}

	H->ordenNatural = ArrayOrdenVertices(H->num_vertices);
	copiaarray(H->vertices, H->ordenNatural, H->num_vertices);
	//Ordenar ordenNatural, asi puede ser usado despues de crear la copia
	ordenqsort(H->ordenNatural, H->num_vertices);

	H->orden = ArrayOrdenVertices(H->num_vertices);



	//no se puede copiar directo, porque necesitamos vertices nuevos
	//Aqui creamos una copia de cada vertice. Luego completamos el array de vecinos de cada uno
	//Para eso usamos una nueva tabla hash, y la usamos de la misma manera que en read_dimacs()

	//Aqui usamos la tabla hash para encontrar la direccion de cada vecino
	#ifndef NDEBUG
	//double start = get_time();
	#endif
	for (u32 i = 0; i < H->num_vertices; ++i) {
		for (u32 k = 0; k < G->vertices[i]->grado; ++k) {
			//H->vertices[i]->vecinos->avecinos[k] = existeVertice(hasht, G->vertices[i]->vecinos->avecinos[k]->nombre);
			H->vertices[i]->vecinos->avecinos[k] = buscarelementohs(tablab, G->vertices[i]->vecinos->avecinos[k]->nombre);
			assert(H->vertices[i]->vecinos->avecinos[k] != NULL);
		}
		H->orden[i] = buscarelementohs(tablab, G->orden[i]->nombre);
		
	}
	#ifndef NDEBUG
	//double end = get_time();
	//printf("%f segundos buscando en hash\n", (end-start));
	#endif

	destruirtablahashhs(tablab);
	
	H->DeltaG = G->DeltaG;

	return H;	 

}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

