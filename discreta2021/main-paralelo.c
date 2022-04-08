#include "GrafoSt21.h"
#include "u32.h"
#include "read_dimacs.h"
#include "greedy.h"
#include "orden.h"
//#include "greedyrapido.h"
#include "bipartito.h"
#include "reorden.h"
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <omp.h>

#ifndef TIMEFUNC
#define TIMEFUNC
#ifdef _WIN64

#include <windows.h>
double get_time()
{
	LARGE_INTEGER t;
	LARGE_INTEGER f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / (double)f.QuadPart;
}
/*
static double get_tticks()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	return (double)t.QuadPart;
}
*/

#else

#include <sys/time.h>
#include <sys/resource.h>

static inline double get_time()
{
	struct timeval t;
	//struct timezone tzp;
	gettimeofday(&t, NULL);
	return t.tv_sec + t.tv_usec * 1e-6;
}

#endif

#endif




void randomizaru32s(u32* restrict orden, u32 size) {
	assert(orden != NULL);
    srand((u32)time(NULL));
    --size; //porque los indices el array van de 0 a size-1
    for (u32 i = 0; i < size; ++i) {
        u32 temp = orden[i];
        long long int posicionrand = ((int64_t)rand() % ((int64_t)size - i + 1)) + i;
        orden[i] = orden[posicionrand];
        orden[posicionrand] = temp;
    }
    //return (orden);
}

int cmpfunction(const void *a, const void *b) {
    // Castear a enteros
    u32 aInt = *((u32 *)a);
    u32 bInt = *((u32 *)b);
    // Al restarlos, se debe obtener un número mayor, menor o igual a 0
    // Con esto ordenamos de manera descendente
    //return bInt - aInt;
	//CORRECION:
	//Si el resultado se devuelve como int, y los operandos a y b son unsigned int, puede haber underflow y se da vuelta el resultado, porque el maximo de un u32 es el doble que el maximo valor positivo de un int.

	int64_t r = (bInt - aInt);

    /*
    if (r < 0) return -1;
    if (aa == bb) return 0;
    if (aa > bb) return 1;
    */

    if (r < 0) {
        return (-1);
    }
    if (r > 0) {

        return (1);
        
    }
    
   

    return (0);

	
}
	
void mayormenor32s(u32* restrict orden, u32 size) {
	assert(orden != NULL);
    //--size; //porque los indices el array van de 0 a size-1
    qsort((void*)orden, size, sizeof(orden[0]), cmpfunction);
    //return (orden);
}

u32 checkcoloresutilizados(Grafo G, u32* arraypermutacion,u32 cantidadanterior){

	u32 coloresutilizados = Getcoloresutilizados(G);
	if(coloresutilizados < cantidadanterior){
		//entonces hay que achicar el arreglo que contiene la permutacion.
		u32* temparreglo = realloc(arraypermutacion, (sizeof(u32) * coloresutilizados));
		if(temparreglo != NULL){
			arraypermutacion = temparreglo;
		}
		else{
			//12 == ENOMEM exit code
			printf("No hay memoria suficiente\n");
			exit(12);
		}
		//nuevacantidadcoloresutilizados = checkcoloresutilizados;
		//hay que reinicializar porque el arreglo puede tener valores que son incorrectos, es decir colores que ya no son validos
		for (u32 j=0; j<coloresutilizados;j++){
			arraypermutacion[j]=j;
		}		
	}
	return(coloresutilizados);
}

int main(int argc,char *argv[]) {

	if (argc!=7){
		printf("Cantidad inesperada de parámetros\n");
		return (-1);

	}

	double startcarga;
	double endcarga;

	startcarga = get_time();
	Grafo a = read_dimacs();
	if (a == NULL){
		printf("No se pudo leer el archivo. Puede que este mal formateado.\n");
		exit(1);
	}
	endcarga = get_time();
	printf("Tiempo de carga (y sort() ):%f\n", (endcarga - startcarga));
	u32 lados = NumeroDeLados(a);
	u32 vertices = NumeroDeVertices(a);
	//Vertice* orden = Orden(a);
	
	//Vertice* orden;

	printf("N vertices:%u\tN lados:%u\tDensidad:%f\n", vertices, lados, (double)((float)lados / (float)((float)(vertices * (vertices-1))/2)) );
	//u32 cantidadvertices = NumeroDeVertices(a);
	//printf("numvert: %u\n", a->num_vertices);
	//Grafo b = CopiarGrafo(a);
	//printf("numvert: %u\n", b->num_vertices);

	printf("Delta de A es: %u\n", Delta(a));

	//Vertice* cur = OrdenNatural(a);
	//for (u32 i = 0; i < cantidadvertices; ++i) {
	//	printf("Itemnro:%u Nombre i=%u ---> Color:%u \n", i, cur[i]->nombre, cur[i]->color);
	//}
	
	char esBipartito = Bipartito(a);
	if (esBipartito){
		printf("El grafo es Bipartito.\n");
		#ifndef NDEBUG
		char r = escoloreopropio(a);
		if(r!=1){
			printf("Bipartito() no devolvio un coloreo propio\n");
		}
		#endif
		return -1;
	}
	else{
		printf("El grafo no es Bipartito.\n");
	}
	resetColores(a);

	double start;
	double end;

	int nGreedys = 0;
	u32 minimogreedycolores = UINT32_MAX;
	double sumatotalgreedycolores = 0;
	double sumatotalgreedybloques = 0;

	Vertice* ordenMinimo = NULL;

	/*------------------------------------------------------------------------------------------*/
	//Hacemos greedy con orden natural.
	//Para eso copiamos el array de ordenNatural en el array de orden
	//tambien lo copias en ordenMinimo;

	
	unsigned long long tamaniodeOrdenenBytes = sizeof(Vertice) * vertices;
	ordenMinimo = (Vertice*)malloc(tamaniodeOrdenenBytes);
	assert(ordenMinimo != NULL);

	Vertice* ordennaturaldelgrafo = OrdenNatural(a);
	Vertice* ordendelgrafo = Orden(a);

	for(u32 i = 0; i < vertices; ++i){
		
		ordendelgrafo[i] = ordennaturaldelgrafo[i];
		ordenMinimo[i] = ordennaturaldelgrafo[i];
	}
	//Ejecutamos Greedy
	u32 resultadoGreedyOrdNatural = Greedy(a);
	printf("Greedy() con orden natural:%u\n\n", resultadoGreedyOrdNatural);
	#ifndef NDEBUG
	char r = escoloreopropio(a);
	if(r!=1){
		printf("Greddy() con orden natural no devolvio un coloreo propio\n");
	}
	#endif
	//Actualizamos minimogreedycolores
	minimogreedycolores = minimogreedycolores < resultadoGreedyOrdNatural ? minimogreedycolores : resultadoGreedyOrdNatural;

	//hacemos tantos greedys con orden aleatorio como el argumento a===argv[1] lo indique

	long int argv1toint = atol(argv[1]);
	//bool semaforoverde = true;
	for (int i = 0; i < argv1toint;i++){

		resetColores(a);

		//if(semaforoverde == true){
		AleatorizarVertices(a,(u32)atoll(argv[6]));
			//Vertice* ordenaleatorio = Orden(a);

		//}


		start = get_time();
		u32 greedycolores = Greedy(a);
		end = get_time();
		printf("Loop:%i/%li Colores obtenidos: %u\n", (i+1), argv1toint, greedycolores);
		nGreedys = nGreedys+1;

		sumatotalgreedycolores+= (end - start);

		if (minimogreedycolores <= greedycolores){

			//minimogreedycolores=minimogreedycolores;
			//semaforoverde = true;
			
		}
		else{

			minimogreedycolores=greedycolores;

			/*Para guardar el orden actual, vamos a usar memcpy.
			Como mempcpy copia una cantidad de bytes, necesitamos saber cual es el tamaño en bytes de un solo vertice
			y despues multiplicar eso por la cantidad de vertices del grafo.
			Luego llamamos memcpy con ese valor y copiamos el orden*/
			//tamaniodeOrdenenBytes se calcula cuando hacemos el greedy con el orden natural. Idem la reserva de memoria para ordenMinimo
			
			memcpy(ordenMinimo, Orden(a), tamaniodeOrdenenBytes);
			/*
			Vertice const* ordenorig = Orden(a);
			for(u32 i = 0; i < vertices; ++vertices){
				ordenMinimo[i] = ordenorig[i];
			}
			*/
			//semaforoverde = false;
		}
		//minimogreedycolores = minimogreedycolores < greedycolores ? minimogreedycolores : greedycolores;
		printf("Minimo Greedy: %u\n",minimogreedycolores);
	}
	Vertice* ordenoriginal = Orden(a);
	//Como no tenemos acceso directo al atributo orden del grafo, vamos a tener que copiar elemento por elemento
	
	/*
	for(u32 i = 0; i < vertices; ++i){
		ordenoriginal[i] = ordenMinimo[i];
	}
	*/
	
	//*Prueba memcpy()
	memcpy(ordenoriginal, ordenMinimo, tamaniodeOrdenenBytes);

	//*End prueba memcpy()

	//*Antes de hacer este greedy, hay que despintar el grafo, sino da mal el coloreo.
	resetColores(a);

	Greedy(a);
	free(ordenMinimo);
	++nGreedys;

	
	printf("TiempoTotalColoresPrimero:%f  NCromatico:%u\nFin bloque 1: Greedys con orden aleatorio\n\n", sumatotalgreedycolores, minimogreedycolores);

	//Obtenemos la cantidad de colores utilizados
	u32 cantidadcoloresutilizados = Getcoloresutilizados(a);

	//Creamos un arreglo para las permutaciones que va a tomar OrdenPorBloqueDeColores
	u32* arreglocolores= (u32*)malloc(cantidadcoloresutilizados*sizeof(u32));

	//Llenamos el arreglo con el número de colores utilizado
	for (u32 j=0; j<cantidadcoloresutilizados;j++){
		arreglocolores[j]=j;
	}

	//Realizamos "b" iteraciones, que se corresponden con el parámetro argv[2]
	long int argv2toint = atol(argv[2]);
	for (int i=0; i<argv2toint;i++){
		randomizaru32s(arreglocolores,cantidadcoloresutilizados); //Permutamos los elementos del arreglo de colores
		//printf("Reordenando en el bloque del argv A\n");
		OrdenPorBloqueDeColores(a,arreglocolores);//Reordenamos
		//printf("Fin reorden bloque del argv A\n");

		resetColores(a);

		start = get_time();
		u32 greedybloques = Greedy(a);
		end = get_time();
		++nGreedys;
		sumatotalgreedybloques += (end-start);
		u32 checkcoloresutilizados = Getcoloresutilizados(a);

		#ifndef NDEBUG
		//Segun la teoria, esto no deberia ser necesario ni activarse nunca
		if(checkcoloresutilizados>cantidadcoloresutilizados && i>0){
			printf("Greedy actual utilizo mas colores que el greedy anterior. No debe suceder\n");
			exit(1);
		}

		if(checkcoloresutilizados>cantidadcoloresutilizados){
			//hay que agrandar el arreglo de permutacion.
			printf("Fail. No deberia suceder. Bloque 1\n");
			u32* temparreglo = realloc(arreglocolores, (sizeof(u32) * checkcoloresutilizados));
			if(temparreglo != NULL){
				arreglocolores = temparreglo;
				for(u32 i = cantidadcoloresutilizados; i < checkcoloresutilizados; ++i){
					arreglocolores[i] = i;
				}
				cantidadcoloresutilizados = checkcoloresutilizados;
				//el arreglo quedo listo con los nuevos numeros cargados

			}
			else{
				//12 == ENOMEM exit code
				exit(12);
			}
			

		}

		//check si greedy dio un coloreo propio al menos
		if(!escoloreopropio(a)){
			printf("NO ES COLOREO PROPIO. ALGO ESTA MAL. Linea %u, archivo %s\n", __LINE__, __FILE__);
			exit(1);

		}

		#endif

		if(checkcoloresutilizados < cantidadcoloresutilizados){
			//hay que achicar la permutacion.
			u32* temparreglo = realloc(arreglocolores, (sizeof(u32) * checkcoloresutilizados));
			if(temparreglo != NULL){
				arreglocolores = temparreglo;
			}
			#ifndef NDEBUG
			else{
				//12 == ENOMEM exit code
				exit(12);
			}
			#endif
			
			cantidadcoloresutilizados = checkcoloresutilizados;
			//hay que reinicializar porque el arreglo puede tener valores que son incorrectos ahora
			for (u32 j=0; j<cantidadcoloresutilizados;j++){
				arreglocolores[j]=j;
			}
			
			//lo anterior lo podriamos evitar si a Ordenporbloquedecolores() le pudiesemos pasar un tamaño de permutacion, no que lo calcule con Getcoloresutilizados()

		}


		printf("Loop:%i/%li Colores obtenidos: %u\n", (i+1), argv2toint,greedybloques);
	}
	
	free(arreglocolores);
	printf("Fin bloque 2: Greedys con reorden por bloque de color\tTiempo total greedy():%f\n\n", sumatotalgreedybloques);


	u32 nuevacantidadcoloresutilizados;
	u32 nuevacantidadcoloresutilizados2;
	u32 nuevacantidadcoloresutilizados3;
	u32 cantidadcoloresutilizadosanterior = 0;
	u32 cantidadcoloresutilizadosanterior2 = 0;
	u32 cantidadcoloresutilizadosanterior3 = 0;
	u32* nuevoarreglocolores = NULL;
	u32* nuevoarreglocolores2= NULL;
	u32* nuevoarreglocolores3= NULL;
	#ifndef NDEBUG
	u32 absoluto_minimocoloreobloques_optimo = UINT32_MAX;
	u32 absoluto_minimocoloreobloques_h = UINT32_MAX;
	u32 absoluto_minimocoloreobloques_w = UINT32_MAX;
	#endif
	u32 greedybloques_optimo = UINT32_MAX;
	u32 greedybloques_h = UINT32_MAX;
	u32 greedybloques_w = UINT32_MAX;
	double tiempototalbloque_optimo = 0;
	double tiempototalbloque_h = 0;
	double tiempototalbloque_w = 0;
	
	//Grafo grafoptimo = CopiarGrafo(a);
	Grafo grafoptimo = CopiarGrafomodhs(a);
	nuevacantidadcoloresutilizados = Getcoloresutilizados(grafoptimo);
	nuevacantidadcoloresutilizados2 = nuevacantidadcoloresutilizados;
	nuevacantidadcoloresutilizados3 = nuevacantidadcoloresutilizados;
	//Ojo que CopiarGrafo copia a los vertices de forma exacta.
	//entonces hay que despintar a la copia.
	//resetColores(grafoptimo);
	//EDIT: No hay que despintar la copia, sino nunca se va a poder reordenar por colores mas adelante.

	long int argv3toint = atol(argv[3]);
	long int argv4toint = atol(argv[4]);
	long int argv5toint = atol(argv[5]);
	//long int argv6toint = atol(argv[6]);
	for(int k = 0; k<argv3toint;++k){ //"c" ciclos externos (argv[3])
	#ifndef NDEBUG
	printf("\nLOOPK=%u\tArgumento=%li\n",k+1, argv3toint);
	#endif

		u32 minimocoloreobloques_optimo = UINT32_MAX;
		u32 minimocoloreobloques_h = UINT32_MAX;
		u32 minimocoloreobloques_w = UINT32_MAX;
		cantidadcoloresutilizadosanterior = nuevacantidadcoloresutilizados;
		cantidadcoloresutilizadosanterior2 = nuevacantidadcoloresutilizados2;
		cantidadcoloresutilizadosanterior3 = nuevacantidadcoloresutilizados3;
		nuevacantidadcoloresutilizados = Getcoloresutilizados(grafoptimo);
		nuevacantidadcoloresutilizados2 = nuevacantidadcoloresutilizados;
		nuevacantidadcoloresutilizados3 = nuevacantidadcoloresutilizados;
		if(k == 0){
			nuevoarreglocolores = (u32*)malloc(nuevacantidadcoloresutilizados*sizeof(u32));
			nuevoarreglocolores2 = (u32*)malloc(nuevacantidadcoloresutilizados2*sizeof(u32));
			nuevoarreglocolores3 = (u32*)malloc(nuevacantidadcoloresutilizados3*sizeof(u32));
		}
		else{
			if(nuevacantidadcoloresutilizados > cantidadcoloresutilizadosanterior){
				#ifndef NDEBUG
				printf("Esto nunca deberia ejecutarse.\nArchivo:%s\tLinea:%u\n",__FILE__,__LINE__);
				#endif
				
				u32* temp1 = (u32*)realloc(nuevoarreglocolores, (nuevacantidadcoloresutilizados * sizeof(u32)));
				if((temp1 != NULL)){
					nuevoarreglocolores = temp1;

				}
				else{
					printf("REALLOC de nuevoarreglocolores* salio mal. Exit\n");
					exit(12);
				}
			}
			if(nuevacantidadcoloresutilizados2 > cantidadcoloresutilizadosanterior2){
				#ifndef NDEBUG
				printf("Esto nunca deberia ejecutarse.\nArchivo:%s\tLinea:%u\n",__FILE__,__LINE__);
				#endif
				
				u32* temp1 = (u32*)realloc(nuevoarreglocolores2, (nuevacantidadcoloresutilizados2 * sizeof(u32)));
				if((temp1 != NULL)){
					nuevoarreglocolores2 = temp1;


				}
				else{
					printf("REALLOC de nuevoarreglocolores* salio mal. Exit\n");
					exit(12);
				}
			}
			if(nuevacantidadcoloresutilizados3 > cantidadcoloresutilizadosanterior3){
				#ifndef NDEBUG
				printf("Esto nunca deberia ejecutarse.\nArchivo:%s\tLinea:%u\n",__FILE__,__LINE__);
				#endif
				
				u32* temp1 = (u32*)realloc(nuevoarreglocolores3, (nuevacantidadcoloresutilizados3 * sizeof(u32)));
				if((temp1 != NULL)){
					nuevoarreglocolores3 = temp1;

				}
				else{
					printf("REALLOC de nuevoarreglocolores* salio mal. Exit\n");
					exit(12);
				}
			}


		}
		
		if(nuevacantidadcoloresutilizados == nuevacantidadcoloresutilizados2 && nuevacantidadcoloresutilizados2 == nuevacantidadcoloresutilizados3){
			for (u32 j=0; j<nuevacantidadcoloresutilizados;j++){
				//OJO aca. La cantidad de colores significa que los colores van desde 0 a cantidad-1.
				//A las permutaciones de la segunda y tercer estrategia las cargamos ordenadas de mayor a menor. Es para ahorrarse un paso, todavia hay que hacer el check mas abajo
				nuevoarreglocolores[j]=j;
				nuevoarreglocolores2[j]=(nuevacantidadcoloresutilizados -1 - j);
				nuevoarreglocolores3[j]=(nuevacantidadcoloresutilizados -1 - j);
			}

		}
		else{
			for (u32 j=0; j<nuevacantidadcoloresutilizados;j++){
				//OJO aca. La cantidad de colores significa que los colores van desde 0 a cantidad-1.
				nuevoarreglocolores[j]=j;
			}
			for (u32 j=0; j<nuevacantidadcoloresutilizados2;j++){
				//OJO aca. La cantidad de colores significa que los colores van desde 0 a cantidad-1.
				nuevoarreglocolores2[j]=(nuevacantidadcoloresutilizados2 -1 - j);
			}
			for (u32 j=0; j<nuevacantidadcoloresutilizados3;j++){
				//OJO aca. La cantidad de colores significa que los colores van desde 0 a cantidad-1.
				nuevoarreglocolores3[j]=(nuevacantidadcoloresutilizados3 -1 - j);
			}


		}

		/*
		//Aca estamos haciendo dos alias del puntero nuevoarreglocolores.
		u32* nuevoarreglocolores2 = nuevoarreglocolores;
		u32* nuevoarreglocolores3 = nuevoarreglocolores;
		*/

		Grafo H;
		Grafo W;
		#pragma omp parallel
		{

		#pragma omp sections
		{

		#pragma omp section
		{
		//Grafo H = CopiarGrafo(grafoptimo);
		//H = CopiarGrafo(grafoptimo); //ultimo que anda bien
		//Grafo H = CopiarGrafomodhs(grafoptimo);
		H = CopiarGrafomodhs(grafoptimo);
		}//!end omp section

		#pragma omp section
		{
		//Grafo W = CopiarGrafo(grafoptimo);
		//W = CopiarGrafo(grafoptimo); //ultimo que anda bien
		//Grafo W = CopiarGrafomodhs(grafoptimo);
		W = CopiarGrafomodhs(grafoptimo);
		}//!end omp section

		}//!end omp sections

		}//!end omp parallel
		

		
		for(int l = 0; l<argv4toint;l++){ //"d" ciclos internos (argv[4])

		#pragma omp parallel private(l)
		{

		

		#pragma omp sections
		{
			/*-----------------Estrategia 1-----------------*/
			
			#pragma omp section
			{

			randomizaru32s(nuevoarreglocolores,nuevacantidadcoloresutilizados); //Permutamos los elementos del arreglo de colores
			#ifndef NDEBUG
			start = get_time();
			#endif
	    	OrdenPorBloqueDeColores(grafoptimo,nuevoarreglocolores);//Reordenamos aleatoriamente
			#ifndef NDEBUG
			end = get_time();
			printf("OrdenPorBloqueDeColores() tardo %f segundos en estrategia1\n", (end-start));
			#endif
    
	    	resetColores(grafoptimo);
    
	    	start = get_time();
	    	greedybloques_optimo = Greedy(grafoptimo);
	    	end = get_time();

			//_______________________________________________//

			u32 checkcoloresutilizados = Getcoloresutilizados(grafoptimo);
			
			#ifndef NDEBUG
			if(checkcoloresutilizados>nuevacantidadcoloresutilizados){
				printf("Fail. No deberia suceder. Bloque 2, estrategia 1\n");
				//hay que agrandar el arreglo de permutacion.
				u32* temparreglo = realloc(nuevoarreglocolores, (sizeof(u32) * checkcoloresutilizados));
				if(temparreglo != NULL){
					nuevoarreglocolores = temparreglo;
					for(u32 i = nuevacantidadcoloresutilizados; i < checkcoloresutilizados; ++i){
						nuevoarreglocolores[i] = i;
					}
					nuevacantidadcoloresutilizados = checkcoloresutilizados;
					//el arreglo quedo listo con los nuevos numeros cargados

				}
				else{
					//12 == ENOMEM exit code
					printf("No hay memoria suficiente\n");
					exit(12);
				}

			}

			//check si greedy dio un coloreo propio al menos
			if(!escoloreopropio(a)){
				printf("NO ES COLOREO PROPIO. ALGO ESTA MAL. Linea %u, archivo %s\n", __LINE__, __FILE__);
				exit(1);

			}

			#endif

			if(checkcoloresutilizados < nuevacantidadcoloresutilizados){
				//hay que achicar la permutacion.

				u32* temparreglo = realloc(nuevoarreglocolores, (sizeof(u32) * checkcoloresutilizados));
				if(temparreglo != NULL){
					nuevoarreglocolores = temparreglo;
				}
				else{
					//12 == ENOMEM exit code
					printf("No hay memoria suficiente\n");
					exit(12);
				}
				nuevacantidadcoloresutilizados = checkcoloresutilizados;
				//hay que reinicializar porque el arreglo puede tener valores que son incorrecto ahora
				for (u32 j=0; j<nuevacantidadcoloresutilizados;j++){
					nuevoarreglocolores[j]=j;
				}
			
				//lo anterior lo podriamos evitar si a Ordenporbloquedecolores() le pudiesemos pasar un tamaño de permutacion, no que lo calcule con Getcoloresutilizados()

			}




			//_______________________________________________//




			++nGreedys;
	    	tiempototalbloque_optimo += (end-start);
			minimocoloreobloques_optimo = minimocoloreobloques_optimo < greedybloques_optimo ? minimocoloreobloques_optimo : greedybloques_optimo;


			}//!end pragma omp section
			/*-----------------Fin1-----------------*/

			/*-----------------Estrategia 2-----------------*/
			#pragma omp section
			{
	   		
			if(l!=0){
				//mayormenor32s(nuevoarreglocolores2,nuevacantidadcoloresutilizados2);//Permutamos elementos de mayor a menor
				u32 tmp = nuevacantidadcoloresutilizados2 - 1;
				for(u32 i = 0; i < nuevacantidadcoloresutilizados2; ++i){
					nuevoarreglocolores2[i] = (tmp - i);
				}
			}
	   		
	    	char dd = OrdenPorBloqueDeColores(H,nuevoarreglocolores2);//Reordenamos de mayor a menor
			if(dd == 0){
				printf("Shit\n");
			}
    
	    	resetColores(H);
    
	    	start = get_time();
	    	greedybloques_h = Greedy(H);
	    	end = get_time();

			//_______________________________________________//
			//esto no se podria paralelizar si usamos las mismas variables

			u32 checkcoloresutilizados = Getcoloresutilizados(H);

			#ifndef NDEBUG
			if(checkcoloresutilizados>nuevacantidadcoloresutilizados2){
				//hay que agrandar el arreglo de permutacion.
				printf("Fail. No deberia suceder. Bloque 2, estrategia 2\n");
				u32* temparreglo = realloc(nuevoarreglocolores2, (sizeof(u32) * checkcoloresutilizados));
				if(temparreglo != NULL){
					nuevoarreglocolores2 = temparreglo;
					for(u32 i = nuevacantidadcoloresutilizados2; i < checkcoloresutilizados; ++i){
						nuevoarreglocolores2[i] = i;
					}
					nuevacantidadcoloresutilizados2 = checkcoloresutilizados;
					//el arreglo quedo listo con los nuevos numeros cargados

				}
				else{
					//12 == ENOMEM exit code
					exit(12);
				}

			}
			#endif

			if(checkcoloresutilizados < nuevacantidadcoloresutilizados2){
				//hay que achicar la permutacion.
				u32* temparreglo = realloc(nuevoarreglocolores2, (sizeof(u32) * checkcoloresutilizados));
				if(temparreglo != NULL){
					nuevoarreglocolores2 = temparreglo;
				}
				else{
					//12 == ENOMEM exit code
					printf("No hay memoria suficiente\n");
					exit(12);
				}
				nuevacantidadcoloresutilizados2 = checkcoloresutilizados;
				//hay que reinicializar porque el arreglo puede tener valores que son incorrecto ahora
				for (u32 j=0; j<nuevacantidadcoloresutilizados2;j++){
					nuevoarreglocolores2[j]= nuevacantidadcoloresutilizados2 - 1 - j;
				}
			
				//lo anterior lo podriamos evitar si a Ordenporbloquedecolores() le pudiesemos pasar un tamaño de permutacion, no que lo calcule con Getcoloresutilizados()

			}




			//_______________________________________________//
			++nGreedys;
	    	tiempototalbloque_h += (end-start);
			minimocoloreobloques_h = minimocoloreobloques_h < greedybloques_h ? minimocoloreobloques_h : greedybloques_h;

			/*-----------------Fin2-----------------*/

			}//!End pragma omp section
			
			/*-----------------Estrategia 3-----------------*/
			#pragma omp section
			{
	    	
	    	//mayormenor32s(nuevoarreglocolores3,nuevacantidadcoloresutilizados3);
			u32 tmp = nuevacantidadcoloresutilizados3 - 1;
			for(u32 i = 0; i < nuevacantidadcoloresutilizados3; ++i){
				nuevoarreglocolores3[i] = (tmp - i);
			}
	    	/*Codigo que genera una probabilidad de 1/e*/
			static char onlyonce = 0;
	    	for (u32 i = 0; i<nuevacantidadcoloresutilizados3;i++){
	    		
				if(onlyonce == 0){
					srand(time(NULL));
					onlyonce = 1;
				}
	    		
				//srand(argv6toint);
	    		//int probabilidad = rand()%(atol(argv[5]+1))+1; //Genera un numero al azar entre 1 y e, por lo que hay "e" elementos
				  int probabilidad = rand()%((int)argv5toint+1)+1;

	    		if(probabilidad == argv5toint){ //Si el valor es "e", entonces hace el intercambio, e es 1 solo elemento de los "e" elementos
	    			u32 temp;					//Luego la posibilidad de que el valor aleatorio sea "e" es de 1/e
	    			//u32 posrand = rand()%nuevacantidadcoloresutilizados3;
					long long int posrandl = ((int64_t)rand() % ((int64_t)tmp - i + 1)) + i;
					u32 posrand = (u32)posrandl;
	    			temp = nuevoarreglocolores3[posrand];
	    			nuevoarreglocolores3[posrand] = nuevoarreglocolores3[i];
	    			nuevoarreglocolores3[i] = temp;
		    	}
		    }
		    /*Fin codigo que genera probabilidad 1/e*/	
	    	OrdenPorBloqueDeColores(W,nuevoarreglocolores3);//Reordenamos
    
	    	resetColores(W);
    
	    	start = get_time();
	    	greedybloques_w = Greedy(W);
	    	end = get_time();
			//_______________________________________________//
			
			u32 checkcoloresutilizados = Getcoloresutilizados(W);
			#ifndef NDEBUG
			if(checkcoloresutilizados>nuevacantidadcoloresutilizados3){
				//hay que agrandar el arreglo de permutacion.
				printf("Fail. No deberia suceder. Bloque 2, estrategia 3\n");
				u32* temparreglo = realloc(nuevoarreglocolores3, (sizeof(u32) * checkcoloresutilizados));
				if(temparreglo != NULL){
					nuevoarreglocolores3 = temparreglo;
					for(u32 i = nuevacantidadcoloresutilizados3; i < checkcoloresutilizados; ++i){
						nuevoarreglocolores3[i] = i;
					}
					nuevacantidadcoloresutilizados3 = checkcoloresutilizados;
					//el arreglo quedo listo con los nuevos numeros cargados

				}
				else{
					//12 == ENOMEM exit code
					printf("No hay memoria suficiente\n");
					exit(12);
				}

			}
			#endif

			if(checkcoloresutilizados < nuevacantidadcoloresutilizados3){
				//hay que achicar la permutacion.
				u32* temparreglo = realloc(nuevoarreglocolores3, (sizeof(u32) * checkcoloresutilizados));
				if(temparreglo != NULL){
					nuevoarreglocolores3 = temparreglo;
				}
				else{
					//12 == ENOMEM exit code
					printf("No hay memoria suficiente\n");
					exit(12);
				}
				nuevacantidadcoloresutilizados3 = checkcoloresutilizados;
				//hay que reinicializar porque el arreglo puede tener valores que son incorrecto ahora
				for (u32 j=0; j<nuevacantidadcoloresutilizados3;j++){
					nuevoarreglocolores3[j]= nuevacantidadcoloresutilizados3 - 1 - j;
				}
			
				//lo anterior lo podriamos evitar si a Ordenporbloquedecolores() le pudiesemos pasar un tamaño de permutacion, no que lo calcule con Getcoloresutilizados()

			}




			//_______________________________________________//
			++nGreedys;
	    	tiempototalbloque_w += (end-start);
			minimocoloreobloques_w = minimocoloreobloques_w < greedybloques_w ? minimocoloreobloques_w : greedybloques_w;

			}//!End pragma omp section
			/*-----------------Fin3-----------------*/
			


		}//!pragmaomp sections

		}//!pragmaomp parallel

		//printf("Loop%u\tG:%u\tH:%u\tW:%u\n", l, greedybloques_optimo, greedybloques_h, greedybloques_w);
	}//fin ciclo interno 'd' veces
	
	
	


		printf("Loop externo:%u\tG:%u\tH:%u\tW:%u\n", k+1, greedybloques_optimo, greedybloques_h, greedybloques_w);
		printf("Tiempos acumulados hasta el momento\nGreedy_optimo:%f\tGreedy_h:%f\tGreedy_w:%f\n", tiempototalbloque_optimo, tiempototalbloque_h, tiempototalbloque_w);
		#ifndef NDEBUG
		absoluto_minimocoloreobloques_optimo = absoluto_minimocoloreobloques_optimo < minimocoloreobloques_optimo ? absoluto_minimocoloreobloques_optimo : minimocoloreobloques_optimo;
		absoluto_minimocoloreobloques_h = absoluto_minimocoloreobloques_h < minimocoloreobloques_h ? absoluto_minimocoloreobloques_h : minimocoloreobloques_h;
		absoluto_minimocoloreobloques_w = absoluto_minimocoloreobloques_w < minimocoloreobloques_w ? absoluto_minimocoloreobloques_w : minimocoloreobloques_w;
		#endif
		

		//hora de elegir al mejor para usarlo en la proxima iteracion
		#ifndef NDEBUG
		if(minimocoloreobloques_optimo == minimocoloreobloques_h){
			printf("Greedy sobre bloque 1 (optimo) es igual a Greedy sobre bloque 2 (h)\n");
		}
		if(minimocoloreobloques_optimo == minimocoloreobloques_w){
			printf("Greedy sobre bloque 1 (optimo) es igual a Greedy sobre bloque 3 (w)\n");
		}
		if(minimocoloreobloques_h == minimocoloreobloques_w){
			printf("Greedy sobre bloque 2 (h) es igual a Greedy sobre bloque 3 (w)\n");
		}
		#endif
		//esta comparacion nos da el menor de los 3. Pero si son los 3 iguales, nos da el numero del bloque w.
		//? La especificacion no dice como 'desempatar' si 2 valores o los 3 son iguales. Que orden elegimos?

		//u32 rrr = minimocoloreobloques_optimo < minimocoloreobloques_h ? minimocoloreobloques_optimo : minimocoloreobloques_h;
		//rrr = rrr < minimocoloreobloques_w ? rrr : minimocoloreobloques_w;
		/*
		char who =  minimocoloreobloques_optimo <= minimocoloreobloques_h ? 'o' : 'h';
		char who2 = minimocoloreobloques_optimo <= minimocoloreobloques_w ? 'o' : 'w';
		char who3 = minimocoloreobloques_h <= minimocoloreobloques_w ? 'h' : 'w';
		*/
		//char semaforoverde = 1;

		if(minimocoloreobloques_optimo <= minimocoloreobloques_h && minimocoloreobloques_optimo <= minimocoloreobloques_w){
			//entonces o <= h <= w || o <= w <= h
			//usar opt

			//Son todos ordenes equivalentes, entonces usamos el de optimo porque implica menos instrucciones.
			//liberar los otros 2.

			
			#pragma omp parallel
			{

			#pragma omp sections
			{

			#pragma omp section
			{
			//DestruccionDelGrafo(H);
			DestruccionDelGrafo(H);
			H = NULL;
			}
			#pragma omp section
			{
			//DestruccionDelGrafo(W);
			DestruccionDelGrafo(W);
			W = NULL;
			}
			} //!end omp sections
			} //!end omp parallel

		}
		else if(minimocoloreobloques_h < minimocoloreobloques_w){
			//entonces h <= w <= o || h <= o <= w
			//usar h
			#ifndef NDEBUG
			if(H == NULL || W == NULL){
				printf("WASTED\n");
				exit(1);
			}
			#endif
			//Grafo temp = CopiarGrafo(H); //ultimo que anda bien
			Grafo temp = CopiarGrafomodhs(H);
			#pragma omp parallel
			{

			#pragma omp sections
			{

			#pragma omp section
			{
			DestruccionDelGrafo(H);
			
			H = NULL;
			}

			#pragma omp section
			{

			DestruccionDelGrafo(W);
			
			}

			#pragma omp section
			{
			DestruccionDelGrafo(grafoptimo);
			
			}

			} //!end omp sections
			} //!end omp parallel

			grafoptimo = temp;

		}
		else{
			//entonces w <= h <= o || w <= o <= h
			//usar w
			#ifndef NDEBUG
			if(H == NULL || W == NULL){
				printf("WASTED\n");
				exit(1);
			}
			#endif
			//Grafo temp = CopiarGrafo(W); //ultimo que anda bien
			Grafo temp = CopiarGrafomodhs(W);
			#pragma omp parallel
			{

			#pragma omp sections
			{

			#pragma omp section
			{
			DestruccionDelGrafo(W);
			
			W = NULL;
			}

			#pragma omp section
			{
			DestruccionDelGrafo(H);
			
			H = NULL;
			}

			#pragma omp section
			{
			DestruccionDelGrafo(grafoptimo);
			
			}

			} //!end omp sections
			} //!end omp parallel

			grafoptimo = temp;



		}
		/*
		if(who == 'o' && who2 == 'o'){
			//usar opt

			//Son todos ordenes equivalentes, entonces usamos el de optimo porque implica menos instrucciones.
			//liberar los otros 2.
			DestruccionDelGrafo(H);
			H = NULL;
			DestruccionDelGrafo(W);
			W = NULL;
			semaforoverde = 0;
		}
		if(who == 'h' && who3 == 'h' && semaforoverde == 1){
			//usar h
			#ifndef NDEBUG
			if(H == NULL || W == NULL){
				printf("WASTED\n");
				exit(1);
			}
			#endif
			Grafo temp = CopiarGrafo(H);
			DestruccionDelGrafo(H);
			H = NULL;
			DestruccionDelGrafo(W);
			DestruccionDelGrafo(grafoptimo);
			grafoptimo = temp;
			semaforoverde = 0;

		}
		if(who2 == 'w' && who3 == 'w' && semaforoverde == 1){
			//usar w
			#ifndef NDEBUG
			if(H == NULL || W == NULL){
				printf("WASTED\n");
				exit(1);
			}
			#endif
			Grafo temp = CopiarGrafo(W);
			DestruccionDelGrafo(W);
			W = NULL;
			DestruccionDelGrafo(H);
			H = NULL;
			DestruccionDelGrafo(grafoptimo);
			grafoptimo = temp;
			semaforoverde = 0;

		}
		*/

		// if(minimocoloreobloques_optimo == minimocoloreobloques_h && minimocoloreobloques_optimo == minimocoloreobloques_w){
		// 	//Son todos ordenes equivalentes, entonces usamos el de optimo porque implica menos instrucciones.
		// 	//liberar los otros 2.
		// 	DestruccionDelGrafo(H);
		// 	H = NULL;
		// 	DestruccionDelGrafo(W);
		// 	W = NULL;
		// 	break;


		// }
		// if(minimocoloreobloques_optimo <= minimocoloreobloques_h && minimocoloreobloques_optimo <= minimocoloreobloques_w){
		// 	//liberar los otros 2.
		// 	DestruccionDelGrafo(H);
		// 	H = NULL;
		// 	DestruccionDelGrafo(W);
		// 	W = NULL;
		// 	break;

		// }
		// if(minimocoloreobloques_h < minimocoloreobloques_optimo){

		// }

		// //TODO: usar un enum{} para who, sino no se entiende nada.
		// //char who = 0;

		// if (rrr == minimocoloreobloques_optimo){
		// 	//who = 1;
		// 	//liberar los otros 2.
		// 	DestruccionDelGrafo(H);
		// 	H = NULL;
		// 	DestruccionDelGrafo(W);
		// 	W = NULL;
			
		// }
		// if (rrr == minimocoloreobloques_h){
		// 	//who = 2;
		// 	Grafo temp = CopiarGrafo(H);
		// 	DestruccionDelGrafo(H);
		// 	H = NULL;
		// 	free(W);
		// 	DestruccionDelGrafo(W);
		// 	DestruccionDelGrafo(grafoptimo);
		// 	grafoptimo = temp;

			
		// }
		// if (rrr == minimocoloreobloques_w){
		// 	//who = 3;
		// 	Grafo temp = CopiarGrafo(W);
		// 	DestruccionDelGrafo(W);
		// 	W = NULL;
		// 	DestruccionDelGrafo(H);
		// 	H = NULL;
		// 	DestruccionDelGrafo(grafoptimo);
		// 	grafoptimo = temp;

		// }
	
	// if(k == (argv3toint - 1)){
	// 	//hay que liberar lo que quedo
	// 	free(nuevoarreglocolores);
	// 	nuevoarreglocolores = NULL;
	// 	free(nuevoarreglocolores2);
	// 	nuevoarreglocolores2 = NULL;
	// 	free(nuevoarreglocolores3);
	// 	nuevoarreglocolores3 = NULL;
	// 	printf("G absoluto:%u\tH absoluto:%u\tW absoluto:%u\n\nFIN ciclo externo\n", absoluto_minimocoloreobloques_optimo, absoluto_minimocoloreobloques_h, absoluto_minimocoloreobloques_w);
	// }
		


	}//fin ciclo externo 'c' veces
	
	if(nuevoarreglocolores != NULL){
		free(nuevoarreglocolores);
		nuevoarreglocolores = NULL;

	}

	if(nuevoarreglocolores2 != NULL){
		free(nuevoarreglocolores2);
		nuevoarreglocolores2 = NULL;
	}

	if(nuevoarreglocolores3 != NULL){
		free(nuevoarreglocolores3);
		nuevoarreglocolores3 = NULL;
	}
	#ifndef NDEBUG
	printf("G absoluto:%u\tH absoluto:%u\tW absoluto:%u\n", absoluto_minimocoloreobloques_optimo, absoluto_minimocoloreobloques_h, absoluto_minimocoloreobloques_w);
	#endif
	printf("Cantidad de ejecuciones de Greedy() explicitas:%u\n", nGreedys);
	


	assert(a != NULL);
	assert(grafoptimo != NULL);
	//DestruccionDelGrafo(grafoptimo);
	DestruccionDelGrafo(grafoptimo);
	if(a != NULL){
		//DestruccionDelGrafo(a);
		DestruccionDelGrafo(a);

	}
	
	printf("END\n\n");
	return (0);
}
