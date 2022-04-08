#include "GrafoSt21.h"
#include "u32.h"
#include "read_dimacs.h"
#include "greedy.h"
#include "orden.h"
#include "greedyrapido.h"
#include "bipartito.h"
#include "reorden.h"
#include "vertice.h"
#include <stdint.h>
#include <time.h>
#include "greedyrapido.h"
#include "hashtable_b.h"




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

static double get_tticks()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	return (double)t.QuadPart;
}

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

void randomizaru32s(u32* restrict orden, u32 size) {
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

// inline u32 hashfuncfunc(u32 key, u32 buckets){
//     //#define hash2(key, buckets) (((key)*((key)+3)) % (buckets))
//     return((((u64)key)*(((u64)key)+3)) % buckets);
// }

int cmpfunction(const void *a, const void *b) {
    // Castear a enteros
    u32 aInt = *((u32 *)a);
    u32 bInt = *((u32 *)b);
    // Al restarlos, se debe obtener un número mayor, menor o igual a 0
    // Con esto ordenamos de manera descendente
    //return bInt - aInt;
	//CORRECION:
	//Si el resultado se devuelve como int, y los operandos a y b son unsigned int, puede haber underflow y se da vuelta el resultado, porque el maximo de un u32 es el doble que el maximo valor positivo de un int.

	//int64_t r = (bInt - aInt);

    /*
    if (r < 0) return -1;
    if (aa == bb) return 0;
    if (aa > bb) return 1;
    */

	if(aInt > bInt){
		return (-1);
	}
	if(aInt < bInt){
		return (1);
	}


	/*
    if (r < 0) {
        return (-1);
    }
    if (r > 0) {

        return (1);
        
    }
    */
   

    return (0);

	
}
	
void mayormenor32s(u32* restrict orden, u32 size) {
	//assert(orden != NULL);
    //--size; //porque los indices el array van de 0 a size-1
    qsort((void*)orden, size, sizeof(orden[0]), cmpfunction);
    //return (orden);
}

void setbit32tt(u32* br, u32 pos) {

	(*br) |= (u32)((u32)1 << (pos));
}

//////////////////
int main() {

	//u32 pruebabit = 7;

	// setbit32tt(&pruebabit, 1);
	// setbit32tt(&pruebabit, 0);
	// setbit32tt(&pruebabit, 2);

	// printf("pruebabit: %u\n", pruebabit);

	// for(uint_fast16_t i = 0; i < 32; ++i){
	// 	printf("bit_to_uint:%u\n",(pruebabit & ((u32)1 << (i))) );
	// }

	double startcarga;
	double endcarga;

	//u32 hash = ((u32) ((float)hashfunc((u64)3863311530, 4194304) + ( 0.5f * (float)4194304) + (((0.5f) * (0.5f)) * (float)4194304) )  % 4194304);

	//___________________________________//
	hashtable_t* tablab = inicializartabla(32);
	Vertice* arrprueba = (Vertice*)malloc(sizeof(Vertice) * 32);
	for(u32 i = 0; i < 32; ++i){
		if(i==0){
			arrprueba[i] = CrearVertice(UINT32_MAX, UINT32_MAX);
			insertarelemento(tablab, arrprueba[i]);

		}
		else{
			arrprueba[i] = CrearVertice(i, i);
			insertarelemento(tablab, arrprueba[i]);

		}
		
	}
	Vertice cocon = buscarelemento(tablab, UINT32_MAX);
	if(cocon != NULL){
		printf("\nN:%u\tC:%u\n", cocon->nombre, cocon->color);
	}
	else{
		printf("No sirve\n\n\n");
	}

	for(u32 i = 0; i < 32; ++i){
		free(arrprueba[i]->vecinos);
		free(arrprueba[i]);
		
	}
	destruirtablahash(tablab);
	free(arrprueba);

	//___________________________________//
	printf("\n\t\tSIZEOF hopnode_t:%lu\n\n", sizeof(hopnode_t));
	//___________________________________//
	hopnode_t* tablabb = inicializarhs(32);
	Vertice* arrpruebaa = (Vertice*)malloc(sizeof(Vertice) * 48);
	for(u32 i = 0; i < 48; ++i){
		if(i==0){
			arrpruebaa[i] = CrearVertice(UINT32_MAX, UINT32_MAX);
			insertarelementohs(tablabb, arrpruebaa[i]);

		}
		else{
			arrpruebaa[i] = CrearVertice(i+48, i+48);
			insertarelementohs(tablabb, arrpruebaa[i]);

		}
		
	}
	Vertice coconn = buscarelementohs(tablabb, UINT32_MAX);
	if(coconn != NULL){
		printf("\nN:%u\tC:%u\n", coconn->nombre, coconn->color);
	}
	else{
		printf("No sirve\n\n\n");
	}

	for(u32 i = 0; i < 48; ++i){
		free(arrpruebaa[i]->vecinos);
		free(arrpruebaa[i]);
		
	}
	destruirtablahashhs(tablabb);
	free(arrpruebaa);

	//___________________________________//

	


	startcarga = get_time();
	Grafo a = read_dimacs();
	endcarga = get_time();
	printf("Tiempo de carga (y sort() ):%f\n", (endcarga - startcarga));
	u32 lados = NumeroDeLados(a);
	u32 vertices = NumeroDeVertices(a);
	//Vertice* orden = Orden(a);
	Vertice* orden;

	printf("N vertices:%u\tN lados:%u\tDensidad:%f\n", vertices, lados, (double)((float)lados / (float)((float)((uint64_t)vertices * (vertices-1))/2)) );
	startcarga = get_time();
	Grafo b = CopiarGrafo(a);
	endcarga = get_time();
	printf("Tiempo de COPIA (y sort() ):%f\n", (endcarga - startcarga));
	startcarga = get_time();
	Grafo bhs = CopiarGrafomodhs(a);
	endcarga = get_time();
	printf("Tiempo de COPIA (y sort() ):%f\n", (endcarga - startcarga));
	
	Vertice* orden_a = OrdenNatural(a);
	Vertice* orden_b = OrdenNatural(b);
	Vertice* orden_bhs = OrdenNatural(bhs);

	for(u32 k = 0; k < NumeroDeVertices(a); ++k){
		if(orden_a[k]->nombre != orden_b[k]->nombre){
			printf("\n\t NO ES BIT EXACT\n\n");
			printf("Pos:%u\tNombre_a:%u\tNombre_b:%u\n", k, orden_a[k]->nombre, orden_b[k]->nombre);
			break;
		}
		if(orden_a[k]->nombre != orden_bhs[k]->nombre){
			printf("\n\t NO ES BIT EXACT BHS\n\n");
			printf("Pos:%u\tNombre_a:%u\tNombre_bhs:%u\n", k, orden_a[k]->nombre, orden_bhs[k]->nombre);
			break;
		}
		//printf("K:%u\tNombre:%u\n", k, orden_a[k]->nombre);


	}

	for(u32 k = 1; k < NumeroDeVertices(a); ++k){
		if(orden_a[k]->nombre < orden_a[k-1]->nombre){
			printf("Qsort anda mal!\n");
			exit(1);
		}
	}

	for(u32 k = 0; k < NumeroDeVertices(bhs); ++k){
		Vertice vert = orden_bhs[k];
		u32 grado = vert->grado;
		Vertice* vecinosvert = vert->vecinos->avecinos;
		for(u32 j = 0; j < grado; ++j){
			if(vecinosvert[j]->nombre != orden_a[k]->vecinos->avecinos[j]->nombre){
				printf("\n\n\t BOOOOOOOOOOOOM \n\n");
				exit(1);
			}

		}


	}

	for(u32 k = 0; k < NumeroDeVertices(b); ++k){
		Vertice vert = orden_b[k];
		u32 grado = vert->grado;
		Vertice* vecinosvert = vert->vecinos->avecinos;
		for(u32 j = 0; j < grado; ++j){
			if(vecinosvert[j]->nombre != orden_a[k]->vecinos->avecinos[j]->nombre){
				printf("\n\n\t BOOOOOOOOOOOOM B \n\n");
				exit(1);
			}

		}


	}

	startcarga = get_time();
	DestruccionDelGrafo(b);
	endcarga = get_time();
	printf("Tiempo de Destruccion de la Copia:%f\n", (endcarga - startcarga));
	DestruccionDelGrafo(bhs);
	//u32 cantidadvertices = NumeroDeVertices(a);
	//printf("numvert: %u\n", a->num_vertices);
	/*
	Grafo b = CopiarGrafo(a);
	printf("numvert: %u\n", NumeroDeVertices(b));
	Vertice* orden_a = Orden(a);
	Vertice* orden_b = Orden(b);
	for(u32 jj = 0; jj < vertices; ++jj){
		if(orden_b[jj]->nombre != orden_a[jj]->nombre){
			printf("Adios\n");
			exit(1);
		}
		
	}
	DestruccionDelGrafo(b);
	*/

	printf("Delta de A es: %u\n", Delta(a));



	/*_____________________________________*/
	//!Hay que probar usar un array con las direcciones de los colores de los vecinos.
	//!creo que no cambia nada: porque es un mov rbx, [punteroaVecino+offsetdelatributoColor].
	//*Una posible solucion es guardar en un bloom filter la combinacion de todos los colores de los vecinos.
	//*Pero si cambia el color de un vecino, hay que recalcularlo. Y propagar el nuevo color a todos los vecinos de ese vertice.
	//*Una funcion que pase de muchas dimensiones a 1 no se si sirve, porque tiene el mismo problema.

	/*_____________________________________*/

	//double start;
	//double end;
	//double sumatotal = 0;
	
	//start = get_time();
	////u32 rr = greedycrazy(a);
	//u32 rr = greedynaive2(a);
	///*Solamente hacerlo si es naive like*/
	//++rr;
	///*End*/
	//end = get_time();
	//printf("GreedyNaive2:%u\nTiempo:%f\n",rr, (end-start));
	
	/*
	Vertice* cur = OrdenNatural(a);
	for (u32 i = 0; i < cantidadvertices; ++i) {
		printf("Nombre i=%u ---> Color:%u \n", cur[i]->nombre, cur[i]->color);
	}
	
	
	

	*/
	/*
	u32 rrcontrol;
	u32 minimo = UINT32_MAX;
	
	
	for (int i = 1; i <= loops; ++i) {
		start = get_time();
		rrcontrol = greedynaive(a, orden);
		end = get_time();
		sumatotal += (end - start);

		minimo = minimo < rrcontrol + 1 ? minimo : rrcontrol + 1;
		
		//randomizarordenresetcolor(orden, cantidadvertices);

	}
	printf("TiempoTotal:%f\tNCromatico minimo:%u\n", sumatotal, minimo);
	
	
	resetColores(a);

	*/
	double start;
	double end;

	u32 rrcontrol;
	u32 rrgreedycolores;
	//u32 rrgreedycoloresbits;
	u32 rrgreedycoloresbitsavx2;

	u32 minimocontrol = UINT32_MAX;
	u32 minimogreedycolores = UINT32_MAX;
	//u32 minimogreedycoloresbits = UINT32_MAX;
	u32 minimogreedycoloresbitsavx2 = UINT32_MAX;

	double sumatotalcontrol = 0;
	double sumatotalgreedycolores = 0;
	//double sumatotalgreedycoloresbits = 0;
	double sumatotalgreedycoloresbitsavx2 = 0;
	///Aqui va la cantidad de loops de greedy's random
	u32 const loops = 0;
    printf("LOOPS:%u\n", loops);
	for (u32 i = 1; i <= loops; ++i) {
		
		if (i == 1) {
			//orden = OrdenNatural(a);
			for(u32 i = 0; i < vertices; ++i){
				FijarOrden(i, a, i);
			}
		}
		else {
			orden = Orden(a);
		}
		
		resetColores(a);
		start = get_time();
		rrcontrol = greedynaive(a, orden);
		end = get_time();
		sumatotalcontrol += (end - start);
		printf("END-START greedyCONTROL:%f\t COLORES:%u\n\n", (end - start), rrcontrol);
		minimocontrol = minimocontrol < rrcontrol ? minimocontrol : rrcontrol;
		//resetColores(a);
		

		
		resetColores(a);
		start = get_time();
		//rrgreedycolores = greedycoloresprimero(a, orden);
		rrgreedycolores = Greedy(a);
		end = get_time();

		

		
		
		sumatotalgreedycolores += (end - start);
		printf("END-START greedyColores:%f\t COLORES:%u\n", (end - start), rrgreedycolores);
		minimogreedycolores = minimogreedycolores < rrgreedycolores ? minimogreedycolores : rrgreedycolores;

		if (escoloreopropio(a) == 0){
			printf("NO es propio\n");
		}
		else{
			printf("SI es propio\n");
		}
		
		
		/*
		resetColores(a);
		start = get_time();
		rrgreedycoloresbits = greedycoloresprimerobitsallowed(a, orden);
		end = get_time();
		sumatotalgreedycoloresbits += (end - start);
		//printf("END-START greedyColores_BITS:%f\t COLORES:%u\n\n", (end - start),  rrgreedycoloresbits + 1);
		minimogreedycoloresbits = minimogreedycoloresbits < rrgreedycoloresbits ? minimogreedycoloresbits : rrgreedycoloresbits;
		*/

		
	 	
		//greedycoloresprimerobitsallowedavx2
		/*
		resetColores(a);
		start = get_time();
		//rrgreedycoloresbitsavx2 = greedycoloresprimerobitsallowedavx2(a, orden);
		rrgreedycoloresbitsavx2 = greedycoloresprimerobitsallowedavx2arr(a, orden);
		//rrgreedycoloresbitsavx2 = greedycoloresprimerobitsallowed(a, orden);
		end = get_time();
		sumatotalgreedycoloresbitsavx2 += (end - start);
		//printf("END-START greedyColores_BITS_AVX2:%f\t COLORES:%u\n\n", (end - start),  rrgreedycoloresbitsavx2 + 1);
		minimogreedycoloresbitsavx2 = minimogreedycoloresbitsavx2 < rrgreedycoloresbitsavx2 ? minimogreedycoloresbitsavx2 : rrgreedycoloresbitsavx2;
        */

		//proximo orden random
		//AleatorizarVertices(a, 0);
		AleatorizarVertices(a, 1);
		//randomizarorden(orden, vertices);
	}
	//printf("TiempoTotal:%f\tNCromatico minimo:%u\n", sumatotalcontrol, minimocontrol);
	printf("TiempoTotalColoresPrimero:%f  NCromatico:%u\n", sumatotalgreedycolores, minimogreedycolores);
	//printf("TiempoTotalColoresEnBITS_:%f  NCromatico:%u\n", sumatotalgreedycoloresbits, minimogreedycoloresbits);
	//printf("TiempoTotalColoresEnBITS_AVX2:%f  NCromatico minimo:%u\n", sumatotalgreedycoloresbitsavx2, minimogreedycoloresbitsavx2);


	///*Tests Bipartito.
	//test de bipartito2 con copia
	Grafo tbipart2 = CopiarGrafo(a);
	resetColores(tbipart2);

	//prueba de aleatorizarVertice2
	// Vertice* ordd = Orden(tbipart2);

	// for(u32 i = 0; i < vertices; ++i){
	// 	printf("V=%u\t\tNombre:%u\n", i, ordd[i]->nombre);
	// }


	

	// start = get_time();
	// AleatorizarVertices(tbipart2, 3);
	// end = get_time();
	// printf("AleatorizarVertices tiempo:%f\n", (end-start));

	start = get_time();
	AleatorizarVertices(tbipart2, 3);
	end = get_time();
	printf("AleatorizarVertices2 tiempo:%f\n", (end-start));



	

	// printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nAFTER\n\n\n\n\n\n");
	// ordd = Orden(tbipart2);
	// for(u32 i = 0; i < vertices; ++i){
	// 	printf("V=%u\t\tNombre:%u\n", i, ordd[i]->nombre);
	// }

	char rbipart = Bipartito(tbipart2);
	printf("EsBipartito:%i\n",rbipart);
	
	char rescoloreopropio = escoloreopropio(tbipart2);
	printf("Bipartito-resultadoEsColoreoPropio:%i\n",rescoloreopropio);
	u32 cantidadcoloresbipartito = 0; 
	for(u32 i = 0; i < vertices; ++i){
		u32 tempcolor = Color(i, tbipart2);
		if(tempcolor > cantidadcoloresbipartito){
			cantidadcoloresbipartito = tempcolor;
		}
	}
	++cantidadcoloresbipartito; //1 porque es una cantidad, no el maximo color
	printf("Bipartito-cantidad de colores utilizados:%u\n", cantidadcoloresbipartito);
	
	
	DestruccionDelGrafo(tbipart2);



	///*END tests Bipartito

	///*START test reordenporbloque

	/**/

	resetColores(a);
	srand((u32)time(NULL));
	u32 seedparaaleatorizar = rand();
	AleatorizarVertices(a, seedparaaleatorizar);
	u32 colutiltestordenbloque = Greedy(a);
	u32* permutacion = (u32*)malloc(sizeof(u32) * colutiltestordenbloque);
	u32 cantccolores = colutiltestordenbloque;
	for(u32 ab = 0; ab < cantccolores; ++ab){
		permutacion[ab] = ab;
	}

	//randomizaru32s(permutacion, cantccolores);

	mayormenor32s(permutacion, cantccolores);
	for(u32 k = 1; k < cantccolores; ++k){
		if(permutacion[k] > permutacion[k-1]){
			printf("Qsort inverso anda mal!\n");
			exit(1);
		}
		//printf("Kinverso:%u\tNombre:%u\n", k, permutacion[k-1]);
	}

	//char resultadoreordenporcolor = OrdenPorBloqueDeColores(a, permutacion);
	start = get_time();
	char resultadoreordenporcolor = OrdenPorBloqueDeColores(a, permutacion);
	end = get_time();
	printf("resultadoreordenporcolor:%i\t CantidaddeColores:%u\tTiempo:%f\n\n", resultadoreordenporcolor, cantccolores, (end-start));
	Vertice* ord = Orden(a);
	/*
	printf("Permutacion:\n");
	for(u32 pp = 0; pp < cantccolores; ++pp){
		printf("Pos=%u\tColor=%u\n", pp, permutacion[pp]);
	}
	*/

	///*TEST DE OrdenPorBloqueDeColores()
	u32 cursorcolorpermutacion = 0;
	u32 colorenpermutacion;
	for(u32 ii = 0; ii < vertices; ++ii){
		Vertice verto = ord[ii]; //lo pongo en una variable asi es mas facil verlo en el debugger
		//colorenpermutacion = permutacion[cursorcolorpermutacion]; //lo pongo en una variable asi es mas facil verlo en el debugger
		//printf("VColor:%u\t\tPColor:%u\n",verto->color,permutacion[cursorcolorpermutacion]);
		//printf("VColor:\t%u\n",verto->color);
		if(verto->color != permutacion[cursorcolorpermutacion]){
			++cursorcolorpermutacion;
			colorenpermutacion = permutacion[cursorcolorpermutacion];
			if(verto->color != colorenpermutacion){
				//entonces el reorden por bloques no siguio el orden de la permutacion indicado.
				printf("Error en OrdenPorBloqueDeColores\tColorenPermutacion:%u\n", colorenpermutacion);
				break;
			}
		}
		//printf("ii:%u\tColor:%u\n", ii, verto->color);
	}

	free(permutacion);

	resetColores(a);
	start = get_time();
	rrgreedycolores = Greedy(a);
	end = get_time();
	printf("Checktime:%f\n",(end-start));

	///*END test reordenporbloque
	/**/







	/*

	//resetColores(a);
	printf("Test randomseed\n");
	
	Vertice* orden1 = (Vertice*)malloc(sizeof(Vertice) * vertices);
	Vertice* orden2 = (Vertice*)malloc(sizeof(Vertice) * vertices);
	if (orden1 != NULL && orden2 != NULL){
		copiaarray(Orden(a), orden1, vertices);
		copiaarray(Orden(a), orden2, vertices);
		randomizarordenconseed(orden1, vertices, 1);
		randomizarordenconseed(orden2, vertices, 1);

		for (u32 j = 0; j < vertices; ++j) {
			if (orden1[j]->nombre != orden2[j]->nombre) {
				printf("Para j:%u son distintos ordenes\n", j);
				break;

			}
		}
		printf("END test\n");
	}

	free(orden1);
	free(orden2);
	
	*/

	/*
	minimo = UINT32_MAX;
	sumatotal = 0;
#pragma loop(hint_parallel(2))
	for (int i = 1; i <= 10; ++i) {
		//psets nuevoset = setuparraygreedy();
		start = get_time();
		//rrcontrol = greedynaive3(a, nuevoset, orden);
		rrcontrol = greedynaive2(a,  orden);
		end = get_time();
		minimo = minimo < rrcontrol + 1 ? minimo : rrcontrol + 1;
		sumatotal += (end - start);
		randomizarordenresetcolor(orden, cantidadvertices);
		//destruirsetcolor(nuevoset);

	}
	printf("TiempoTotal2:%f\tNCromatico2 minimo:%u\n", sumatotal, minimo);
*/

	//printf("GreedyControl:%u\nTiempo:%f\n", rrcontrol +1 , (end - start));

	//printf("Resetting\n");
	//resetColores(a);
	//printf("DONE\n");

	//printf("Reordering...\n");
	////randomizarorden(OrdenNatural(a), cantidadvertices);
	//setOrdenNatural(a, randomizarorden(OrdenNatural(a), cantidadvertices));
	//printf("DONE\n");

	//start = get_time();
	//rrcontrol = greedynaive(a);
	//end = get_time();



	//printf("GreedyControl22:%u\nTiempo:%f\n", rrcontrol + 1, (end - start));

	/*
	plista2d list2d = setuplistagreedy();
	start = get_time();
	u32 rr2 = greedylista(a, list2d);
	end = get_time();
	
	printf("Greedy2:%u\nTiempo:%f\n", rr2, (end - start));
	destruirlist2d(list2d);
	*/

	/*
	psets nuevoset = setuparraygreedy();
	start = get_time();
	u32 rr3 = greedyarray(a, nuevoset);
	end = get_time();
	printf("Greedy3:%u\nTiempo:%f\n", rr3, (end - start));
	destruirsetcolor(nuevoset);
	*/

	/*
	printf("Resetting\n");
	resetColores(a);
	printf("DONE\n");
	
	psets nuevoset = setuparraygreedy();
	start = get_time();
	u32 rr3 = greedynaive3(a, nuevoset);
	end = get_time();
	printf("GreedyNaive3:%u\nTiempo:%f\n", rr3 + 1, (end - start));
	destruirsetcolor(nuevoset);
	*/

	//printf("El 2do vecino del tercer vertice cargado:%u\n", iesimoVecino(a->vertices[2],2)->nombre);
	
	DestruccionDelGrafo(a);
	//DestruccionDelGrafomod(b);
	//DestruccionDelGrafo(b);
	//testHash();

	printf("END\n\n");
	return (0);
}