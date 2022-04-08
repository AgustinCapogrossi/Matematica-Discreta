#include "UnleashHell.h"
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
	return (double)t.tv_sec + (double)t.tv_usec * 1e-6;
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
    //int64_t aInt = *((u32 *)a);
    //int64_t bInt = *((u32 *)b);
	u32 aInt = *((u32 *)a);
    u32 bInt = *((u32 *)b);
    // Al restarlos, se debe obtener un nÃºmero mayor, menor o igual a 0
    // Con esto ordenamos de manera descendente
    //return bInt - aInt;
	//CORRECION:
	//Si el resultado se devuelve como int, y los operandos a y b son unsigned int, puede haber underflow (en realidad, por ser unsigned, es un "wrap around") y se da vuelta el resultado, porque el maximo de un u32 es el doble que el maximo valor positivo de un int.

	//int64_t r = (bInt - aInt); //ORIGINAL

	// +----+----------------------------------------------------------------------------+
	// | <0 | The element pointed to by p1 goes before the element pointed to by p2      |
	// +----+----------------------------------------------------------------------------+
	// | 0  | The element pointed to by p1 is equivalent to the element pointed to by p2 |
	// +----+----------------------------------------------------------------------------+
	// | >0 | The element pointed to by p1 goes after the element pointed to by p2       |
	// +----+----------------------------------------------------------------------------+


	/*ORIGINAL
    if (r < 0) {
        return (-1);
    }
	*/

	/*ORIGINAL
    if (r > 0) {

        return (1);
        
    }
	*/

	if (aInt > bInt) {
        return (-1);
    }
	
	if (aInt < bInt) {
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

//-------------------------------------------------------------------------------//

//Reduce el arreglo arraypermutacion al tamaño de la cantidad de colores utilizados guardada en el Grafo
//Si no cambio, devuelve el mismo arreglo porque realloc() en ese caso no hace nada y devuelve el mismo puntero.
__attribute__((hot))u32* reduccirarregloPermutacion(Grafo G, u32* arraypermutacion){

	u32 coloresutilizados = Getcoloresutilizados(G);

	u32* temparreglo = realloc(arraypermutacion, (sizeof(u32) * coloresutilizados));
	if(temparreglo != NULL){
		//free(arraypermutacion);
		arraypermutacion = temparreglo;
	}
	else{
		//12 == ENOMEM exit code
		printf("No hay memoria suficiente\n");
		exit(12);
	}
	//hay que reinicializar porque el arreglo puede tener valores que son incorrectos, es decir colores que ya no son validos
	for (u32 j=0; j<coloresutilizados;j++){
		arraypermutacion[j]=j;
	}
	return(arraypermutacion);		
}


//-------------------------------------------------------------------------------//

int main(int argc,char *argv[]) {

	if (argc!=7){
		printf("Cantidad inesperada de parÃ¡metros\n");
		return (-1);

	}
	if(atoll(argv[5]) <= 0 && atoll(argv[4]) >0 && atoll(argv[3]) >0 ){
		printf("El quinto argumento no puede ser menor o igual a 0 ('cero'). Determina la probabilidad de la aleatorizacion en la estrategia 3\n");
		exit(1);
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


	printf("N vertices:%u\tN lados:%u\tDensidad:%f\n", vertices, lados, (float)((float)lados / (float)((float)((u64)vertices * (vertices-1))/2)) );


	printf("Delta de A es: %u\n", Delta(a));

	
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
	long int argv6toint = atol(argv[6]);
	for (int i = 0; i < argv1toint;i++){

		resetColores(a);

		AleatorizarVertices(a,(u32)argv6toint);



		start = get_time();
		u32 greedycolores = Greedy(a);
		end = get_time();
		printf("Loop:%i/%li Colores obtenidos: %u\n", (i+1), argv1toint, greedycolores);
		nGreedys = nGreedys+1;

		sumatotalgreedycolores+= (end - start);

		if (greedycolores < minimogreedycolores){
			//Entonces en esta iteracion encontramos un coloreo del grafo menor a los anteriores, lo tenemos que guardar.

			minimogreedycolores=greedycolores;

			/*Para guardar el orden actual, vamos a usar memcpy.
			Como mempcpy copia una cantidad de bytes, necesitamos saber cual es el tamaÃ±o en bytes de un solo vertice
			y despues multiplicar eso por la cantidad de vertices del grafo.
			Luego llamamos memcpy con ese valor y copiamos el orden*/
			//tamaniodeOrdenenBytes se calcula cuando hacemos el greedy con el orden natural. Idem la reserva de memoria para ordenMinimo
			
			memcpy(ordenMinimo, Orden(a), tamaniodeOrdenenBytes);

			/*
			//Este loop es el equivalente a hacer memcpy()

			Vertice const* ordenorig = Orden(a);
			for(u32 i = 0; i < vertices; ++vertices){
				ordenMinimo[i] = ordenorig[i];
			}
			*/
		}

		printf("Minimo Greedy: %u\n",minimogreedycolores);
	}
	Vertice* ordenoriginal = Orden(a);
	/*
	//El memcpy() de abajo es equivalente a este for()
	for(u32 i = 0; i < vertices; ++i){
		ordenoriginal[i] = ordenMinimo[i];
	}
	*/
	
	memcpy(ordenoriginal, ordenMinimo, tamaniodeOrdenenBytes);

	//*Antes de hacer este greedy, hay que despintar el grafo, sino da mal el coloreo.
	resetColores(a);

	Greedy(a);
	free(ordenMinimo);
	++nGreedys;

	
	printf("TiempoTotalColoresPrimero:%f  NCromatico:%u\nFin bloque 1: Greedys con orden aleatorio\n\n", sumatotalgreedycolores, minimogreedycolores);
	

//Inicio del bloque 2. Primero comprobamos que tengamos que tengamos que hacer algo, es decir, que el segundo argumento de ejecucion sea distinto de 0
if( atol(argv[2]) != 0){

	//Obtenemos la cantidad de colores utilizados. Esto viene determinado por el ultimo greedy del bloque anterior.
	u32 cantidadcoloresutilizados = Getcoloresutilizados(a);

	//Creamos un arreglo para las permutaciones que va a tomar OrdenPorBloqueDeColores
	u32* arreglocolores= (u32*)malloc(cantidadcoloresutilizados*sizeof(u32));

	//Llenamos el arreglo con el nÃºmero de colores utilizado
	assert(cantidadcoloresutilizados != 0);
	for (u32 j=0; j<cantidadcoloresutilizados;j++){
		arreglocolores[j]=j;
	}

	//Realizamos "b" iteraciones, que se corresponden con el parÃ¡metro argv[2]
	long int argv2toint = atol(argv[2]);
	for (int i=0; i<argv2toint;i++){
		randomizaru32s(arreglocolores,cantidadcoloresutilizados); //Permutamos los elementos del arreglo de colores

		OrdenPorBloqueDeColoresarr(a,arreglocolores);//Reordenamos el arreglo de orden del grafo, segun el orden de los colores que indica la permutacion

		resetColores(a); //despintamos el grafo para poder ejecutar greedy()

		start = get_time();
		u32 greedybloques = Greedy(a);
		end = get_time();
		++nGreedys;
		sumatotalgreedybloques += (end-start);
		u32 checkcoloresutilizados = Getcoloresutilizados(a);

		#ifndef NDEBUG
		//Segun la teoria, esto no deberia ser necesario ni activarse nunca
		//Si comentan el exit(1) de abajo, el proceso continua con ese coloreo incorrecto
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
				for(u32 k = cantidadcoloresutilizados; k < checkcoloresutilizados; ++k){
					arreglocolores[k] = k;
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

		#endif //!end debug

		if(checkcoloresutilizados < cantidadcoloresutilizados){

			//Si greedy() utilizo menos colores que la iteracion anterior, tenemos que reducir el arreglo de la permutacion para reflejar ese cambio

			arreglocolores = reduccirarregloPermutacion(a, arreglocolores);
			cantidadcoloresutilizados = checkcoloresutilizados;

		}


		printf("Loop:%i/%li Colores obtenidos: %u\n", (i+1), argv2toint,greedybloques);
	}
	
	free(arreglocolores);

}
	printf("Fin bloque 2: Greedys con reorden por bloque de color\n\n");


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
	
	Grafo grafoptimo = CopiarGrafo(a);

	nuevacantidadcoloresutilizados = Getcoloresutilizados(grafoptimo);
	nuevacantidadcoloresutilizados2 = nuevacantidadcoloresutilizados;
	nuevacantidadcoloresutilizados3 = nuevacantidadcoloresutilizados;

	//resetColores(grafoptimo);
	//EDIT: No hay que despintar la copia, sino nunca se va a poder reordenar por colores mas adelante.

	long int argv3toint = atol(argv[3]);
	long int argv4toint = atol(argv[4]);
	long int argv5toint = atol(argv[5]);
	

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
			//Si son todas las cantidades de colores utilizadas iguales, hacemos todo en un loop. Si no es el caso, mas abajo se hacen por separado.

			for (u32 j=0; j<nuevacantidadcoloresutilizados;j++){
				//OJO aca. La cantidad de colores significa que los colores van desde 0 a cantidad-1.
				//A las permutaciones de la segunda y tercer estrategia las cargamos ordenadas de mayor a menor. Es para que qsort ordene mas rapido despues. Lo ideal seria no ejecutar qsort si sabemos que ya estan ordenados los arreglos
				nuevoarreglocolores[j]=j;
				nuevoarreglocolores2[j]=(nuevacantidadcoloresutilizados -1 - j);
				nuevoarreglocolores3[j]=(nuevacantidadcoloresutilizados -1 - j);
			}

		}
		else{
			for (u32 j=0; j<nuevacantidadcoloresutilizados;j++){

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

		//Copiamos en H y W a grafoptimo, que es el grafo con el que menos colores pudimos pintar. Tambien se copia el orden del grafo en forma identica
		Grafo H = CopiarGrafo(grafoptimo);

		Grafo W = CopiarGrafo(grafoptimo);


		for(int l = 0; l<argv4toint;l++){ //"d" ciclos internos (argv[4])
			/*-----------------Estrategia 1-----------------*/
			
			assert(nuevacantidadcoloresutilizados != 0);
			randomizaru32s(nuevoarreglocolores,nuevacantidadcoloresutilizados); //Permutamos los elementos del arreglo de colores
			#ifndef NDEBUG
			start = get_time();
			#endif
	    	OrdenPorBloqueDeColoresarr(grafoptimo,nuevoarreglocolores);//Reordenamos aleatoriamente
			#ifndef NDEBUG
			end = get_time();
			printf("OrdenPorBloqueDeColoresarr() tardo %f segundos en estrategia1\n", (end-start));
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

				nuevoarreglocolores = reduccirarregloPermutacion(a, nuevoarreglocolores);
				nuevacantidadcoloresutilizados = checkcoloresutilizados;

			}




			//_______________________________________________//




			++nGreedys;
	    	tiempototalbloque_optimo += (end-start);
			minimocoloreobloques_optimo = minimocoloreobloques_optimo < greedybloques_optimo ? minimocoloreobloques_optimo : greedybloques_optimo;

			/*-----------------Fin1-----------------*/

			/*-----------------Estrategia 2-----------------*/
	   		
			if(l!=0){
				//mayormenor32s(nuevoarreglocolores2,nuevacantidadcoloresutilizados2);//Permutamos elementos de mayor a menor
				u32 tmp = nuevacantidadcoloresutilizados2 - 1;
				for(u32 i = 0; i < nuevacantidadcoloresutilizados2; ++i){
					nuevoarreglocolores2[i] = (tmp - i);
				}
			}
			#ifndef NDEBUG
			//revisamos que qsort con cmpfunction que ordena de mayor a menor realmente devuelva un orden descendente.
			for(u64 i = 0; i < nuevacantidadcoloresutilizados2 -1; ++i){
				if(nuevoarreglocolores2[i]<=nuevoarreglocolores2[i+1]){
					printf("error sort descendente\n");
					printf("nuevoarreglocolores2[i]=%u\tnuevoarreglocolores2[i+1]=%u\n", nuevoarreglocolores2[i], nuevoarreglocolores2[i+1]);
					exit(1);
				}
			}
			#endif
	   		
	    	OrdenPorBloqueDeColoresarr(H,nuevoarreglocolores2);//Reordenamos de mayor a menor
    
	    	resetColores(H);
    
	    	start = get_time();
	    	greedybloques_h = Greedy(H);
	    	end = get_time();
			//_______________________________________________//
			//
			checkcoloresutilizados = Getcoloresutilizados(H);
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


				nuevoarreglocolores2 = reduccirarregloPermutacion(H, nuevoarreglocolores2);
				nuevacantidadcoloresutilizados2 = checkcoloresutilizados;

			}




			//_______________________________________________//
			++nGreedys;
	    	tiempototalbloque_h += (end-start);
			minimocoloreobloques_h = minimocoloreobloques_h < greedybloques_h ? minimocoloreobloques_h : greedybloques_h;

			/*-----------------Fin2-----------------*/
			
			/*-----------------Estrategia 3-----------------*/
	    	
	    	//mayormenor32s(nuevoarreglocolores3,nuevacantidadcoloresutilizados3);

			u32 tmp = nuevacantidadcoloresutilizados3 - 1;
			for(u32 i = 0; i < nuevacantidadcoloresutilizados3; ++i){
				nuevoarreglocolores3[i] = (tmp - i);
			}



	    	/*Codigo que genera una probabilidad de 1/e*/
			static char srandsolo1vez = 0;
			u32 cantidadtosize = nuevacantidadcoloresutilizados3 - 1; //esto porque lo vamos a usar de indice de un arreglo, y el arreglo va de 0 a cantidad-1
	    	for (u32 i = 0; i<nuevacantidadcoloresutilizados3;i++){
	    		
				if(srandsolo1vez == 0){
					srand((u32)time(NULL));
					srandsolo1vez = 1;

				}
	    		
				//srand(argv6toint);
	    		//int probabilidad = rand()%(atol(argv[5]+1))+1; //Genera un numero al azar entre 1 y e, por lo que hay "e" elementos
				  long long int probabilidad = (rand()%(argv5toint)); //Genera un numero al azar entre 0 y e-1. Hay e elementos empezando desde el 0.

	    		if(probabilidad == (long long int)argv5toint -1){ //Si el valor es "e - 1" (porque contamos desde el 0 a los elementos, no desde 1), entonces hace el intercambio, e es 1 solo elemento de los "e" elementos
	    			u32 temp;					//Luego la posibilidad de que el valor aleatorio sea "e" es de 1/e
	    			//u32 posrand = rand()%nuevacantidadcoloresutilizados3;
					long long int posrandl = ((int64_t)rand() % ((int64_t)cantidadtosize - i + 1)) + i;
					u32 posrand = (u32)posrandl;
	    			temp = nuevoarreglocolores3[posrand];
	    			nuevoarreglocolores3[posrand] = nuevoarreglocolores3[i];
	    			nuevoarreglocolores3[i] = temp;
		    	}
		    }
		    /*Fin codigo que genera probabilidad 1/e*/	
	    	OrdenPorBloqueDeColoresarr(W,nuevoarreglocolores3);//Reordenamos
    
	    	resetColores(W);
    
	    	start = get_time();
	    	greedybloques_w = Greedy(W);
	    	end = get_time();
			//_______________________________________________//
			
			checkcoloresutilizados = Getcoloresutilizados(W);
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

				nuevoarreglocolores3 = reduccirarregloPermutacion(W, nuevoarreglocolores3);
				nuevacantidadcoloresutilizados3 = checkcoloresutilizados;
			

			}




			//_______________________________________________//
			++nGreedys;
	    	tiempototalbloque_w += (end-start);
			minimocoloreobloques_w = minimocoloreobloques_w < greedybloques_w ? minimocoloreobloques_w : greedybloques_w;
		
			/*-----------------Fin3-----------------*/
			printf("Loop%u\tG:%u\tH:%u\tW:%u\n", l, greedybloques_optimo, greedybloques_h, greedybloques_w);
			
	}//fin ciclo interno 'd' veces

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


		if(minimocoloreobloques_optimo <= minimocoloreobloques_h && minimocoloreobloques_optimo <= minimocoloreobloques_w){
			//entonces o <= h <= w || o <= w <= h
			//usar opt

			//Son todos ordenes equivalentes, entonces usamos el de optimo porque implica menos instrucciones.
			//liberar los otros 2.

			DestruccionDelGrafomod(H);
			H = NULL;
			DestruccionDelGrafomod(W);
			W = NULL;

		}
		else if(minimocoloreobloques_h < minimocoloreobloques_w){
			//entonces h <= w <= o || h <= o <= w
			//usar h
			#ifndef NDEBUG
			if(H == NULL || W == NULL){
				printf("Nuestra prueba de los colores esta mal hecha, porque es posible que se cumplan 2 condiciones a la vez\n");
				exit(1);
			}
			#endif
			Grafo temp = CopiarGrafo(H);

			DestruccionDelGrafomod(H);
			H = NULL;
			DestruccionDelGrafomod(W);
			DestruccionDelGrafomod(grafoptimo);
			grafoptimo = temp;

		}
		else{
			//entonces w <= h <= o || w <= o <= h
			//usar w
			#ifndef NDEBUG
			if(H == NULL || W == NULL){
				printf("Nuestra prueba de los colores esta mal hecha, porque es posible que se cumplan 2 condiciones a la vez\n");
				exit(1);
			}
			#endif
			Grafo temp = CopiarGrafo(W);

			DestruccionDelGrafomod(W);
			W = NULL;
			DestruccionDelGrafomod(H);
			H = NULL;
			DestruccionDelGrafomod(grafoptimo);
			grafoptimo = temp;


		}
		

	}//fin ciclo externo 'c' veces

	#ifndef NDEBUG
	printf("G absoluto:%u\tH absoluto:%u\tW absoluto:%u\n", absoluto_minimocoloreobloques_optimo, absoluto_minimocoloreobloques_h, absoluto_minimocoloreobloques_w);
	#endif
	printf("Cantidad de ejecuciones de Greedy() explicitas:%u\n", nGreedys);

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
	
	


	assert(a != NULL);
	assert(grafoptimo != NULL);
	DestruccionDelGrafomod(grafoptimo);
	if(a != NULL){
		//DestruccionDelGrafo(a);
		DestruccionDelGrafomod(a);

	}
	
	printf("END\n\n");
	return (0);
}