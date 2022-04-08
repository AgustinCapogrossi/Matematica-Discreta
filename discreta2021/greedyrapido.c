#include "greedyrapido.h"
#include "GrafoSt21.h"
#include "bitarray.h"
#include <stdint.h>
#include <xmmintrin.h>
#include <assert.h>

// <summary>
/// Greedy colores primero pero el array de forbiddencolors implementado con bits
/// </summary>
/// <param name="G">Grafo G</param>
/// <param name="orden">Vertice* Orden</param>
/// <returns>Cantidad de colores utilizado por greedy.
#ifdef __POPCNT__
u32 greedycoloresprimerobits(Grafo G, const Vertice* restrict orden) {

	u32 sizearrayVecinos = Delta(G) + 1; // por teorema de Brooks podemos asumir que nunca vamos a necesitar mas de Δ + 1 colores

    u32 uint64creados = 0;
	bitarray forbiddencolors = createbitarray(sizearrayVecinos, &uint64creados);
	u32 cantidadVert = NumeroDeVertices(G);

	//Guarda el color mas alto utilizado. No es la cantidad de colores utilizada!
	u32 maxcolor = 0;

	//pintar el primero
	orden[0]->color = 0;

	//Por cada vertice, hacer:
	for (u32 i = 1; i < cantidadVert; ++i) {

		Vertice restrict vert = orden[i];
		Vertice* arrayVec = vert->vecinos->avecinos;
		u32 grado = vert->grado;

		//reset array
		//si el compilador optimiza bien, usa memset()
		//i > 1 asi la primera vez no lo hace, porque calloc() ya dejo todo en 0
		//Memset es equivalente a hacer el for comentado abajo:
		/*
		for (u32 k = 0; k < sizearrayVecinos && i > 1; ++k) {
			forbiddencolors[k] = false;

		}
		*/
		if (i > 1) {

			memset(forbiddencolors,'\0', uint64creados);

		}


		//Busca cada vecino, para establecer que colores se pueden utilizar y cuales no.
		//Si el color C no se puede utilizar, entonces forbiddencolors[C] = true;

		u32 vecinospintadosyavisitados = 0;
		for (u32 k = 0; k < grado; ++k) {

			u32 colorvecino = arrayVec[k]->color;
			if (colorvecino != UINT32_MAX) {
				//entonces el vecino esta pintado
				++vecinospintadosyavisitados;
				//forbiddencolors[colorvecino] = true;
				setbitk(forbiddencolors, colorvecino);
				//maxcolor = maxcolor > colorvecino ? maxcolor : colorvecino;
			}
			if (vecinospintadosyavisitados == i) {
				break;
			}
		}

		u32 j = findfirst0fast(forbiddencolors, uint64creados);
		assert(j != (uint64creados*64) + 1);
		//Pintar el vertice del color j
		vert->color = j;
		//Actualizar maxcolor
		maxcolor = maxcolor > j ? maxcolor : j;


	}
	free(forbiddencolors);

	//Setcoloresutilizados(G, (maxcolor + 1));
	return (maxcolor + 1);
}
#endif

#ifdef __SSE4_1__
#define HABILITARPRIMEROPINTADO
#else
#ifdef __AVX__
#define HABILITARPRIMEROPINTADO
#endif
#endif

#ifdef HABILITARPRIMEROPINTADO
u32 primeropintado(Vertice const* restrict const arrVecinos, u32 const size) {

	u32 resultado = 0;
	if (size == 1) {

		if (arrVecinos[0]->color != UINT32_MAX) {
			resultado = 0;
			return (resultado);
		}

	}
	else {
		//u32 steps = (size / 2);
		u32 restolineal = (size % 2);
		u32 limit = size - restolineal;
		unsigned char encontrado = 0;
		for (u32 index = 0; index < limit; index += 2) {
			//Convertir 2 u32 a 2 i64 y guardarlos como un __m128i
			//i64 elementos[2] = { (i64)arrcolores[index], (i64)arrcolores[index + 1] };

			//__m128i packde2;  
			/*_mm_i64gather_epi64(elementos, packde2, 1);*/
			__m128i coloresde2vecinos = _mm_set_epi64x((i64)(arrVecinos[index]->color), (i64)(arrVecinos[index + 1]->color));
			__m128i colordespintado = _mm_set_epi64x((i64)UINT32_MAX, (i64)UINT32_MAX);
			__m128i resultadocomp = _mm_cmpeq_epi64(coloresde2vecinos, colordespintado);
			//Recordar que _mm_extract devuelve en posicion 0 al resultado de cmpeq(arrcolores[index+1], uint32_max)
			// y en posicion 1 al resultado de cmpeq(arrcolores[index+0], uint32_max).
			// Es decir, lo devuelve en orden inverso al que cargamos los datos con _mm_set()
			//Por eso en la siguiente linea el primer resultado que pedimos es el que esta en resultadocomp[posicion 1] y no el de que esta en
			// resultadocomp[posicion 0].
			// Si resultadocomp[posicion 1] es distinto a -1, entonces significa que arrcolores[index + 0] esta pintado. Entonces devolvemos index.
			i64 rescoloriesimo = _mm_extract_epi64(resultadocomp, 1);
			if (rescoloriesimo != (i64)-1) {
				//esta pintado
				encontrado = 1;
				resultado = index;
				break;

			}
			rescoloriesimo = _mm_extract_epi64(resultadocomp, 0); //ahora el segundo elemento
			if (rescoloriesimo != (i64)-1) {
				encontrado = 1;
				resultado = index + 1;
				break;
			}



		}//end for()
		if (encontrado == 0 && restolineal == 1) {
			//hacer parte lineal
			if (arrVecinos[size - 1]->color != UINT32_MAX) {
				resultado = size - 1;
				return (resultado);
			}

		}

	}



	return (resultado);
}

#endif


#ifdef __SSE2__
u32 primeropintadosse4ints(Vertice const* restrict const arrVecinos, u32 const size) {
	u32 const stride = 4;
	u32 resultado = 0;
	if (size == 1) {

		if (arrVecinos[0]->color != UINT32_MAX) {
			resultado = 0;
			return (resultado);
		}

	}
	else {
		//u32 steps = (size / stride);
		u32 restolineal = (size % stride);
		u32 limit = size - restolineal;
		unsigned char encontrado = 0;
		for (u32 index = 0; index < limit; index += stride) {

			//Convertir 2 u32 a 2 i64 y guardarlos como un __m128i
			//i64 elementos[2] = { (i64)arrcolores[index], (i64)arrcolores[index + 1] };

			//__m128i packde2;  
			/*_mm_i64gather_epi64(elementos, packde2, 1);*/
			__m128i coloresde2vecinos = _mm_set_epi32((i32)(arrVecinos[index]->color), (i32)(arrVecinos[index + 1]->color), (i32)(arrVecinos[index + 2]->color), 
				(i32)(arrVecinos[index + 3]->color));
			__m128i colordespintado = _mm_set_epi32((i32)UINT32_MAX, (i32)UINT32_MAX, (i32)UINT32_MAX, (i32)UINT32_MAX);
			__m128i resultadocomp = _mm_cmpeq_epi32(coloresde2vecinos, colordespintado);
			//Recordar que _mm_extract devuelve en posicion 0 al resultado de cmpeq(arrcolores[index+1], uint32_max)
			// y en posicion 1 al resultado de cmpeq(arrcolores[index+0], uint32_max).
			// Es decir, lo devuelve en orden inverso al que cargamos los datos con _mm_set()
			//Por eso en la siguiente linea el primer resultado que pedimos es el que esta en resultadocomp[posicion 1] y no el de que esta en
			// resultadocomp[posicion 0].
			// 
			// Si resultadocomp[posicion 1] es distinto a -1, entonces significa que arrcolores[index + 0] esta pintado. Entonces devolvemos index.
			// Addendum:
			//_mm_extract_epi32 fue agregada en las instrucciones SSE 4.1. Todas las demas instrucciones SIMD de esta funcion son de SSE 2.
			// Como es mejor que sea compatible con mas procesadores, hay que usar _mm_store_si128(), que es un movdqa memoria128, xmm128.
			// Tarda el mismo tiempo o un ciclo menos en la mayoria de los procesadores, nos ahorra tiempo porque saca todo junto y es de sse2.
			//
//#define __STDC_VERSION__ 201112L
#ifdef __STDC_VERSION__

#if __STDC_VERSION__ >= 201112L
			_Alignas(16) i32 vector[4];
			_mm_store_si128((__m128i*)vector, resultadocomp);

			//else
#else
			//si estamos usando un estandar anterior a C11, como C99, usamos _mm_storeu_si128(), que no necesita memoria alineada
			i32 vector[4];
			_mm_storeu_si128((__m128i*)vector, resultadocomp);
#endif

#endif
			for (u32 indice = 0; indice < 4; ++indice) {
				//no hacemos un cast, para ahorrar
				//entonces el valor debe ser distinto a -1. i32 -1 ==> 11111111111111111111111111111111
				if (vector[3 - indice] != (i32)-1) {
					//esta pintado
					encontrado = 1;
					resultado = index + indice;
					break;
				}
			}
			if (encontrado == 1) {
				break;
			}
			/*
			i64 rescoloriesimo = _mm_extract_epi32(resultadocomp, 3);
			if (rescoloriesimo != (i32)-1) {
				//esta pintado
				encontrado = 1;
				resultado = index;
				break;

			}
			rescoloriesimo = _mm_extract_epi32(resultadocomp, 2); //ahora el segundo elemento
			if (rescoloriesimo != (i32)-1) {
				encontrado = 1;
				resultado = index + 1;
				break;
			}
			rescoloriesimo = _mm_extract_epi32(resultadocomp, 1); //ahora el segundo elemento
			if (rescoloriesimo != (i32)-1) {
				encontrado = 1;
				resultado = index + 2;
				break;
			}
			rescoloriesimo = _mm_extract_epi32(resultadocomp, 0); //ahora el segundo elemento
			if (rescoloriesimo != (i32)-1) {
				encontrado = 1;
				resultado = index + 3;
				break;
			}
			*/


		}//end for()
		if (encontrado == 0 && restolineal < stride) {
			//hacer parte lineal
			for (u32 k = (size - restolineal); k < size; ++k) {
				if (arrVecinos[k]->color != UINT32_MAX) {
					resultado = k;
					break;
				}
			}

		}

	}



	return (resultado);
}
#endif

//Con un inicio del array arbitrario
#ifdef __SSE2__
inline i64 primeropintadosse4intsmod(Vertice const* restrict const arrVecinos, u32 const size, u32 const posicioninicial) {
	u32 const stride = 4;
	i64 resultado = -1;
    //u32 ultimovecinorevisado = 0;//
	if (size == 1) {

		if (arrVecinos[0]->color != UINT32_MAX) {
			resultado = 0;
			return (resultado);
		}

	}
    if ((size - posicioninicial) < 4){
        //entonces lo hagamos con un for() normal
        for(u32 u = posicioninicial; u < size; ++u){
            if (arrVecinos[u]->color != UINT32_MAX){
                //entonces el vecino esta pintado
                resultado = u;
                return (resultado);
            }
        }
    }else {
		//u32 steps = (size / stride);
		//u32 restolineal = ((size - posicioninicial) % stride);
        //u32 restolineal = size % stride;
		u32 limit = size - 4;
		unsigned char encontrado = 0;
		for (u32 index = posicioninicial; index < limit; index += stride) {

            //ultimovecinorevisado = index + 3;
			//Convertir 2 u32 a 2 i64 y guardarlos como un __m128i
			//i64 elementos[2] = { (i64)arrcolores[index], (i64)arrcolores[index + 1] };

			//__m128i packde2;  
			/*_mm_i64gather_epi64(elementos, packde2, 1);*/
			__m128i coloresde2vecinos = _mm_set_epi32((i32)(arrVecinos[index]->color), (i32)(arrVecinos[index + 1]->color), (i32)(arrVecinos[index + 2]->color), (i32)(arrVecinos[index + 3]->color));
			__m128i colordespintado = _mm_set_epi32((i32)UINT32_MAX, (i32)UINT32_MAX, (i32)UINT32_MAX, (i32)UINT32_MAX);
			__m128i resultadocomp = _mm_cmpeq_epi32(coloresde2vecinos, colordespintado);
			//Recordar que _mm_extract devuelve en posicion 0 al resultado de cmpeq(arrcolores[index+1], uint32_max)
			// y en posicion 1 al resultado de cmpeq(arrcolores[index+0], uint32_max).
			// Es decir, lo devuelve en orden inverso al que cargamos los datos con _mm_set()
			//Por eso en la siguiente linea el primer resultado que pedimos es el que esta en resultadocomp[posicion 1] y no el de que esta en
			// resultadocomp[posicion 0].
			// 
			// Si resultadocomp[posicion 1] es distinto a -1, entonces significa que arrcolores[index + 0] esta pintado. Entonces devolvemos index.
			// Addendum:
			//_mm_extract_epi32 fue agregada en las instrucciones SSE 4.1. Todas las demas instrucciones SIMD de esta funcion son de SSE 2.
			// Como es mejor que sea compatible con mas procesadores, hay que usar _mm_store_si128(), que es un movdqa memoria128, xmm128.
			// Tarda el mismo tiempo o un ciclo menos en la mayoria de los procesadores, nos ahorra tiempo porque saca todo junto y es de sse2.
			//
//#define __STDC_VERSION__ 201112L
#ifdef __STDC_VERSION__

#if __STDC_VERSION__ >= 201112L
			_Alignas(16) i32 vector[4];
			_mm_store_si128((__m128i*)vector, resultadocomp);

			//else
#else
			//si estamos usando un estandar anterior a C11, como C99, usamos _mm_storeu_si128(), que no necesita memoria alineada
			i32 vector[4];
			_mm_storeu_si128((__m128i*)vector, resultadocomp);
#endif

#endif
			#ifdef __clang__
			#pragma clang loop distribute(enable)
			#endif
			for (u32 indice = 0; indice < 4; ++indice) {
				//no hacemos un cast, para ahorrar
				//entonces el valor debe ser distinto a -1. i32 -1 ==> 11111111111111111111111111111111
				if (vector[3 - indice] != (i32)-1) {
					//esta pintado
					encontrado = 1;
					resultado = index + indice;
					break;
				}
			}
			if (encontrado == 1) {
				break;
			}
#ifdef __GNUC__
			__builtin_prefetch(arrVecinos[index+4], 0, _MM_HINT_T1);

#else
			_mm_prefetch((char const*)arrVecinos[index+4], _MM_HINT_T1);
#endif
			/*
			i64 rescoloriesimo = _mm_extract_epi32(resultadocomp, 3);
			if (rescoloriesimo != (i32)-1) {
				//esta pintado
				encontrado = 1;
				resultado = index;
				break;

			}
			rescoloriesimo = _mm_extract_epi32(resultadocomp, 2); //ahora el segundo elemento
			if (rescoloriesimo != (i32)-1) {
				encontrado = 1;
				resultado = index + 1;
				break;
			}
			rescoloriesimo = _mm_extract_epi32(resultadocomp, 1); //ahora el segundo elemento
			if (rescoloriesimo != (i32)-1) {
				encontrado = 1;
				resultado = index + 2;
				break;
			}
			rescoloriesimo = _mm_extract_epi32(resultadocomp, 0); //ahora el segundo elemento
			if (rescoloriesimo != (i32)-1) {
				encontrado = 1;
				resultado = index + 3;
				break;
			}
			*/


		}//end for()
		if (encontrado == 0) {
			//hacer parte lineal
			for (u32 k = limit; k < size; ++k) {
				if (arrVecinos[k]->color != UINT32_MAX) {
					resultado = k;
					break;
				}
			}

		}

	}



	return (resultado);
}
#endif
//ENDINICIOARBITRARIO
#ifdef __AVX2__
u32 primeropintadoavx2(Vertice const* const restrict arrVecinos, u32 const size) {
	u32 resultado = 0;
	if (size == 1) {

		if (arrVecinos[0]->color != UINT32_MAX) {
			resultado = 0;
			return (resultado);
		}

	}
	else {
		//u32 steps = (size / 8);
		u32 restolineal = (size % 8);
		u32 limit = size - restolineal;
		unsigned char encontrado = 0;
		u32 index = 0;
		for (index = 0; index < limit; index += 8) {
			//Convertir 2 u32 a 2 i64 y guardarlos como un __m128i
			//i64 elementos[2] = { (i64)arrcolores[index], (i64)arrcolores[index + 1] };

			//__m128i packde2;  
			/*_mm_i64gather_epi64(elementos, packde2, 1);*/
			//__m128i coloresde2vecinos = _mm_set_epi64x((i64)(arrcolores[index]), (i64)(arrcolores[index + 1]));

			/*RECORDAR: UINT32_MAX EQUIVALE A TODOS LOS BITS EN 1. EN UN SIGNED INT ESO EQUIVALE A -1 (MENOS UNO)*/

			__m256i const coloresde8vecinos = _mm256_set_epi32((int32_t)(arrVecinos[index]->color), (int32_t)(arrVecinos[index + 1]->color), 
				(int32_t)(arrVecinos[index + 2]->color), (int32_t)(arrVecinos[index + 3]->color), (int32_t)(arrVecinos[index + 4]->color),
				(int32_t)(arrVecinos[index + 5]->color), (int32_t)(arrVecinos[index + 6]->color), (int32_t)(arrVecinos[index + 7]->color));

			//__m128i colordespintado = _mm_set_epi64x((i64)UINT32_MAX, (i64)UINT32_MAX);
			__m256i const colordespintado = _mm256_set_epi32((int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX),
				(int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX));

			//__m128i resultadocomp = _mm_cmpeq_epi64(coloresde2vecinos, colordespintado);
			__m256i resultadocomp2 = _mm256_cmpeq_epi32(coloresde8vecinos, colordespintado);

			 

			/*
			* EXTRACT_EPI32 tiene como segundo argumento un int inmediato (un int literal, no guardado en una variable)
			* ESO PORQUE ES UN MACRO
            * Por eso usamos _mm256_store. Ademas, hace todo junto y ahorra ciclos
			* */

			//_Alignas() solo esta definido en C11 o superior
			//no esta definido en C99. Para eso hay que incluir <stdalign.h>, pero creo que solo funciona en GNU GCC
			//LEER para entender mas: https://stackoverflow.com/questions/13257166/print-a-m128i-variable
			// 
			//sin embargo, si en reemplazo usamos _mm256_storeu_si256(), no hace falta alinear la memoria, entonces lo podemos hacer sin 
			//necesitar incluir algun header extra, pero es algunos ciclos mas lento

#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112L
			_Alignas(16) u32 vector[8];
			_mm256_store_si256((__m256i*)vector, resultadocomp2);
#else
			u32 vector[8];
			_mm256_storeu_si256((__m256i*)vector, resultadocomp2);
#endif
#endif
			//Guarda en vector los resultados, pero al reves, porque asi estaban guardados en la memoria
			//Eso porque la arquitectura AMD64 (X64) es little-endian: Guarda en memoria empezando desde los bits menos significativos a mas significativos
			//Entonces '123456' se guarda en binario '654321'. Pero despues las instrucciones las devuelven los bits bien interpretados, segun su tipo
			//vector[0] == resultadocomp(arrcolores[index + 7], UINT32_MAX)
			//vector[1] == resultadocomp(arrcolores[index + 6], UINT32_MAX)
			//vector[2] == resultadocomp(arrcolores[index + 5], UINT32_MAX)
			//....
			//vector[7] == resultadocomp(arrcolores[index + 0], UINT32_MAX)

			for (int32_t ii = 0; ii < 8; ++ii) {
				u32 elemento = vector[(7-ii)];

				if (elemento != UINT32_MAX) {
					encontrado = 1;
					resultado = index + ii;
					break;
				}
			}
			if (encontrado == 1) {
				break;
			}
			
			



			/*
			i64 rescoloriesimo = _mm_extract_epi64(resultadocomp, 0);
			if (rescoloriesimo != (i64)-1) {
				//esta pintado
				encontrado = 1;
				resultado = index;
				break;

			}
			rescoloriesimo = _mm_extract_epi64(resultadocomp, 1); //ahora el segundo elemento
			if (rescoloriesimo != (i64)-1) {
				encontrado = 1;
				resultado = index + 1;
				break;
			}
			*/


		}//end for()
		if (encontrado == 0 && restolineal < 8) {
			//hacer parte lineal
			for (u32 k = (size-restolineal); k < size; ++k) {
				if (arrVecinos[k]->color != UINT32_MAX) {
					resultado = k;
					break;
				}
			}

		}

	}



	return (resultado);
}

/*MODIFICACION PARA USAR ARRAY DE VERTICS VECINOS*/

i64 primeropintadoavx2mod(Vertice const* const restrict arrVecinos, u32 const size, u32 const posicioninicial) {
	i64 resultado = -1;
	if (size == 1) {

		if (arrVecinos[0]->color != UINT32_MAX) {
			resultado = 0;
			return (resultado);
		}

	}
    if ((size - posicioninicial) < 8){
        //entonces lo hagamos con un for() normal
        for(u32 u = posicioninicial; u < size; ++u){
            if (arrVecinos[u]->color != UINT32_MAX){
                //entonces el vecino esta pintado
                resultado = u;
                return (resultado);
            }
        }
    } else {
		//u32 steps = (size / 8);
		//u32 restolineal = (size % 8);
		u32 limit = size - 8;
		unsigned char encontrado = 0;
		u32 index = 0;
		for (index = posicioninicial; index < limit; index += 8) {
			//Convertir 2 u32 a 2 i64 y guardarlos como un __m128i
			//i64 elementos[2] = { (i64)arrcolores[index], (i64)arrcolores[index + 1] };

			//__m128i packde2;  
			/*_mm_i64gather_epi64(elementos, packde2, 1);*/
			//__m128i coloresde2vecinos = _mm_set_epi64x((i64)(arrcolores[index]), (i64)(arrcolores[index + 1]));

			/*RECORDAR: UINT32_MAX EQUIVALE A TODOS LOS BITS EN 1. EN UN SIGNED INT ESO EQUIVALE A -1 (MENOS UNO)*/

			__m256i const coloresde8vecinos = _mm256_set_epi32((int32_t)(arrVecinos[index]->color), (int32_t)(arrVecinos[index + 1]->color), 
				(int32_t)(arrVecinos[index + 2]->color), (int32_t)(arrVecinos[index + 3]->color), (int32_t)(arrVecinos[index + 4]->color),
				(int32_t)(arrVecinos[index + 5]->color), (int32_t)(arrVecinos[index + 6]->color), (int32_t)(arrVecinos[index + 7]->color));

			//__m128i colordespintado = _mm_set_epi64x((i64)UINT32_MAX, (i64)UINT32_MAX);
			__m256i const colordespintado = _mm256_set_epi32((int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX),
				(int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX));

			//__m128i resultadocomp = _mm_cmpeq_epi64(coloresde2vecinos, colordespintado);
			__m256i resultadocomp2 = _mm256_cmpeq_epi32(coloresde8vecinos, colordespintado);

			 

			/*
			* EXTRACT_EPI32 tiene como segundo argumento un int inmediato (un int literal, no guardado en una variable)
			* ESO PORQUE ES UN MACRO
            * Por eso usamos _mm256_store. Ademas, hace todo junto y ahorra ciclos
			* */

			//_Alignas() solo esta definido en C11 o superior
			//no esta definido en C99. Para eso hay que incluir <stdalign.h>, pero creo que solo funciona en GNU GCC
			//LEER para entender mas: https://stackoverflow.com/questions/13257166/print-a-m128i-variable
			// 
			//sin embargo, si en reemplazo usamos _mm256_storeu_si256(), no hace falta alinear la memoria, entonces lo podemos hacer sin 
			//necesitar incluir algun header extra, pero es algunos ciclos mas lento

#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112L
			_Alignas(16) u32 vector[8];
			_mm256_store_si256((__m256i*)vector, resultadocomp2);
#else
			u32 vector[8];
			_mm256_storeu_si256((__m256i*)vector, resultadocomp2);
#endif
#endif
			//Guarda en vector los resultados, pero al reves, porque asi estaban guardados en la memoria
			//Eso porque la arquitectura AMD64 (X64) es little-endian: Guarda en memoria empezando desde los bits menos significativos a mas significativos
			//Entonces '123456' se guarda en binario '654321'. Pero despues las instrucciones las devuelven los bits bien interpretados, segun su tipo
			//vector[0] == resultadocomp(arrcolores[index + 7], UINT32_MAX)
			//vector[1] == resultadocomp(arrcolores[index + 6], UINT32_MAX)
			//vector[2] == resultadocomp(arrcolores[index + 5], UINT32_MAX)
			//....
			//vector[7] == resultadocomp(arrcolores[index + 0], UINT32_MAX)

			for (int32_t ii = 0; ii < 8; ++ii) {
				u32 elemento = vector[(7-ii)];

				if (elemento != UINT32_MAX) {
					encontrado = 1;
					resultado = index + ii;
					break;
				}
			}
			if (encontrado == 1) {
				break;
			}
			
			



			/*
			i64 rescoloriesimo = _mm_extract_epi64(resultadocomp, 0);
			if (rescoloriesimo != (i64)-1) {
				//esta pintado
				encontrado = 1;
				resultado = index;
				break;

			}
			rescoloriesimo = _mm_extract_epi64(resultadocomp, 1); //ahora el segundo elemento
			if (rescoloriesimo != (i64)-1) {
				encontrado = 1;
				resultado = index + 1;
				break;
			}
			*/


		}//end for()
		if (encontrado == 0) {
			//hacer parte lineal
			for (u32 k = limit; k < size; ++k) {
				if (arrVecinos[k]->color != UINT32_MAX) {
					resultado = k;
					break;
				}
			}

		}

	}



	return (resultado);
}

/*END MOD*/


/*AVX ARRAY*/

inline void primeropintadoavx2arr(Vertice const* const restrict arrVecinos, u32 const size, u32* const arrayrespuesta,u32* const sizerespuesta, const u32 cantidadmaximaposible) {
	//i64 resultado = -1;
    //u32 entradasocupadas = 0;
    (*sizerespuesta) = 0;


	if (size == 1) {

		if (arrVecinos[0]->color != UINT32_MAX) {
			arrayrespuesta[(*sizerespuesta)] = arrVecinos[0]->color;
            //++entradasocupadas;
            ++(*sizerespuesta);
			return;
		}

	}
    if ((size) < 8){
        //entonces lo hagamos con un for() normal
        for(u32 u = 0; u < size; ++u){
            if (arrVecinos[u]->color != UINT32_MAX){
                //entonces el vecino esta pintado
                arrayrespuesta[(*sizerespuesta)] = arrVecinos[u]->color;
                ++(*sizerespuesta);
            }
        }
        return;
    } else {
		//u32 steps = (size / 8);
		u32 restolineal = (size % 8);
		u32 limit = size - restolineal;
		unsigned char maximoposible = 0;
		u32 index = 0;
		for (index = 0; index < limit; index += 8) {
			//Convertir 2 u32 a 2 i64 y guardarlos como un __m128i
			//i64 elementos[2] = { (i64)arrcolores[index], (i64)arrcolores[index + 1] };

			//__m128i packde2;  
			/*_mm_i64gather_epi64(elementos, packde2, 1);*/
			//__m128i coloresde2vecinos = _mm_set_epi64x((i64)(arrcolores[index]), (i64)(arrcolores[index + 1]));

			/*RECORDAR: UINT32_MAX EQUIVALE A TODOS LOS BITS EN 1. EN UN SIGNED INT ESO EQUIVALE A -1 (MENOS UNO)*/

			__m256i const coloresde8vecinos = _mm256_set_epi32((int32_t)(arrVecinos[index]->color), (int32_t)(arrVecinos[index + 1]->color), 
				(int32_t)(arrVecinos[index + 2]->color), (int32_t)(arrVecinos[index + 3]->color), (int32_t)(arrVecinos[index + 4]->color),
				(int32_t)(arrVecinos[index + 5]->color), (int32_t)(arrVecinos[index + 6]->color), (int32_t)(arrVecinos[index + 7]->color));

			//__m128i colordespintado = _mm_set_epi64x((i64)UINT32_MAX, (i64)UINT32_MAX);
			__m256i const colordespintado = _mm256_set_epi32((int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX),
				(int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX), (int32_t)(UINT32_MAX));

			//__m128i resultadocomp = _mm_cmpeq_epi64(coloresde2vecinos, colordespintado);
			__m256i resultadocomp2 = _mm256_cmpeq_epi32(coloresde8vecinos, colordespintado);

			 

			/*
			* EXTRACT_EPI32 tiene como segundo argumento un int inmediato (un int literal, no guardado en una variable)
			* ESO PORQUE ES UN MACRO
            * Por eso usamos _mm256_store. Ademas, hace todo junto y ahorra ciclos
			* */

			//_Alignas() solo esta definido en C11 o superior
			//no esta definido en C99. Para eso hay que incluir <stdalign.h>, pero creo que solo funciona en GNU GCC
			//LEER para entender mas: https://stackoverflow.com/questions/13257166/print-a-m128i-variable
			// 
			//sin embargo, si en reemplazo usamos _mm256_storeu_si256(), no hace falta alinear la memoria, entonces lo podemos hacer sin 
			//necesitar incluir algun header extra, pero es algunos ciclos mas lento

#ifdef __STDC_VERSION__
#if __STDC_VERSION__ >= 201112L
			_Alignas(16) u32 vector[8];
			_mm256_store_si256((__m256i*)vector, resultadocomp2);
#else
			u32 vector[8];
			_mm256_storeu_si256((__m256i*)vector, resultadocomp2);
#endif
#endif
			//Guarda en vector los resultados, pero al reves, porque asi estaban guardados en la memoria
			//Eso porque la arquitectura AMD64 (X64) es little-endian: Guarda en memoria empezando desde los bits menos significativos a mas significativos
			//Entonces '123456' se guarda en binario '654321'. Pero despues las instrucciones las devuelven los bits bien interpretados, segun su tipo
			//vector[0] == resultadocomp(arrcolores[index + 7], UINT32_MAX)
			//vector[1] == resultadocomp(arrcolores[index + 6], UINT32_MAX)
			//vector[2] == resultadocomp(arrcolores[index + 5], UINT32_MAX)
			//....
			//vector[7] == resultadocomp(arrcolores[index + 0], UINT32_MAX)
			#ifdef __clang__
			#pragma clang loop distribute(enable)
			#endif
			for (int32_t ii = 0; ii < 8; ++ii) {
				u32 elemento = vector[(7-ii)];

				if (elemento != UINT32_MAX) {
					//encontrado = 1;
                    arrayrespuesta[(*sizerespuesta)] = arrVecinos[index + ii]->color;
                    ++(*sizerespuesta);
					//resultado = index + ii;
					//break;
				}
			}
			if ((*sizerespuesta) > cantidadmaximaposible) {
                maximoposible = 1;
				break;
			}
			
			



			/*
			i64 rescoloriesimo = _mm_extract_epi64(resultadocomp, 0);
			if (rescoloriesimo != (i64)-1) {
				//esta pintado
				encontrado = 1;
				resultado = index;
				break;

			}
			rescoloriesimo = _mm_extract_epi64(resultadocomp, 1); //ahora el segundo elemento
			if (rescoloriesimo != (i64)-1) {
				encontrado = 1;
				resultado = index + 1;
				break;
			}
			*/

			/*----------------------------------*/
			//Prefetch sin unroll del ultimo loop
			//el unroll del ultimo loop ahorra un prefetch que si o si se desperdicia.
			//Si el compilador optimiza bien, esto es innecesario
			#ifdef __GNUC__
			__builtin_prefetch(arrVecinos[index+4], 0, _MM_HINT_T1);

			#else
			_mm_prefetch((char const*)arrVecinos[index+4], _MM_HINT_T1);
			#endif



			/*----------------------------------*/


		}//end for()
		if (maximoposible == 0) {
			//hacer parte lineal
			for (u32 k = limit; k < size; ++k) {
				if (arrVecinos[k]->color != UINT32_MAX) {
					arrayrespuesta[(*sizerespuesta)] = arrVecinos[k]->color;
                    ++(*sizerespuesta);
				}
			}

		}

	}



	//return (resultado);
}

//! AVX ARRAY
//! 
//! 


u32 greedycoloresprimerobitsallowedavx2arr(Grafo G, const Vertice* restrict const orden) {

	//u32 sizearrayVecinos = GetDelta(G) + 1; // por teorema de Brooks podemos asumir que nunca vamos a necesitar mas de Δ + 1 colores
	u32 sizearrayallowedcolors = 1;
	u32 sizearrayVecinos = 1;
	//bool* restrict forbiddencolors = (bool*)calloc(sizearrayVecinos, sizeof(bool)); //este array de bool tiene tantas posiciones como colores posibles (Δ + 1)
	u32 int64creados = 0;
	bitarraysigned allowedcolors = createbitarraysigned(sizearrayallowedcolors, &int64creados);

	u32* arrcoloresvecinos = (u32*)malloc(sizeof(u32) * sizearrayVecinos);
	u32 sizerespuesta;

	u32 cantidadVert = NumeroDeVertices(G);

	//Guarda el color mas alto utilizado. No es la cantidad de colores utilizada!
	u32 maxcolor = 0;

	//pintar el primero
	orden[0]->color = 0;
	for (u32 i = 1; i < cantidadVert; ++i) {

		

		//llenamos de unos todos los bits de "allowedcolors"
		//memset toma como segundo argumento 1 byte. Nosotros le pasamos 4 bytes pero adentro de la funcion hay un cast, no hay problema.
		//En el tercer argumento, va la cantidad de veces que va a llenar allowedcolors con ese byte.
		//Como allowedcolors es un array de int64_t: 1 byte == 8bits. 64bits/8bits = 8. Necesitamos 8 bytes por cada posicion de allowedcolors, por eso se multiplica por 8.
		memset(allowedcolors, (i32)-1, (int64creados * 8));

		/*
		* Esto que sigue es la alternativa a usar memset(). -1 es equivalente a 0xffffffffffffffff. Eso es equivalente a tener todo un entero de 64bits con los bits en 1.
		for (u32 k = 0; k < int64creados; ++k) {
			allowedcolors[k] = -1;
		}
		*/
		//u32 color = 0;
		Vertice restrict vert = orden[i];
		Vertice* arrayVec = vert->vecinos->avecinos;
		u32 grado = vert->grado;

		if(sizearrayVecinos <= grado){
			//realloc
			u32* temparray = (u32*)realloc(arrcoloresvecinos, (sizeof(u32)*grado));
			if(temparray != NULL){
				arrcoloresvecinos = temparray;
				u32 tempsize = sizearrayVecinos;
				sizearrayVecinos = grado;
				//inicializar las posiciones nuevas en color "despintado" ==> UINT32_MAX.
				for(u32 j = tempsize; j < sizearrayVecinos; ++j){
					arrcoloresvecinos[j] = UINT32_MAX;
				}



			}
		}

		//Busca cada vecino, para establecer que colores se pueden utilizar y cuales no.
		//Si el color C no se puede utilizar, entonces allowedcolors[C] = false;

		//primeropintadoavx2arr() devuelve un array con los colores de los vecinos pintados. Con eso despues sabemos cuales colores estan permitidos y cuales no.

		primeropintadoavx2arr(arrayVec, grado, arrcoloresvecinos, &sizerespuesta, i);

		for (u32 l = 0; l < sizerespuesta; ++l) {
			u32 colorvecino = arrcoloresvecinos[l];

			if((int64creados*64) <= colorvecino){
				//hay que agrandar el array
				//esto es peligroso porque como argumento estamos usando el color del vecino, que maliciosamente podria ser muy grande

				u32 tempint64creados = int64creados;

				allowedcolors = reallocbitarraysigned(allowedcolors, colorvecino, &int64creados);
					//poner todos los bits en 1 en los ints nuevos
				for (u32 k = tempint64creados; k < int64creados; ++k) {
					allowedcolors[k] = (i64)-1;

				}
			}

			//allowedcolors[colorvecino / WIDTH] &= ~((u64)1 << (colorvecino%WIDTH));
			clearbitkf(allowedcolors, colorvecino);
			arrcoloresvecinos[l] = UINT32_MAX; //ya que estamos, vamos reiniciando arrcoloresvecinos en este loop, elemento por elemento
		}

#ifdef __POPCNT__

		u32 j = findfirst1fastmod(allowedcolors, int64creados);
#else

#endif
//assert(j != (int64creados * 64) + 1);
//j devuelve la posicion empezando por cero (0)
//Pintar el vertice del color j
		vert->color = j;
		//Actualizar maxcolor
		maxcolor = maxcolor > j ? maxcolor : j;


	}
	free(allowedcolors);
	free(arrcoloresvecinos);
	//Setcoloresutilizados(G, (maxcolor + 1));
	return (maxcolor + 1);
}




#endif

#ifdef __SSE2__

#ifdef __POPCNT__

/// <summary>
/// Greedy colores primero pero el array de forbiddencolors implementado con bits
/// </summary>
/// <param name="G">Grafo G</param>
/// <param name="orden">Vertice* Orden</param>
/// <returns>Maximo color utilizado por greedy. Recordar que empieza con el color 0. Por lo tanto, la cantidad es retorno + 1.
/// No hacemos esa suma dentro de la funcion</returns>
u32 greedycoloresprimerobitsallowed(Grafo G, const Vertice* restrict const orden) {

	//u32 sizearrayVecinos = GetDelta(G) + 1; // por teorema de Brooks podemos asumir que nunca vamos a necesitar mas de Δ + 1 colores
	u32 sizearrayVecinos = 1;
	//bool* restrict forbiddencolors = (bool*)calloc(sizearrayVecinos, sizeof(bool)); //este array de bool tiene tantas posiciones como colores posibles (Δ + 1)
	u32 int64creados = 0;
    bitarraysigned allowedcolors = createbitarraysigned(sizearrayVecinos, &int64creados);
	
	u32 cantidadVert = NumeroDeVertices(G);

	//Guarda el color mas alto utilizado. No es la cantidad de colores utilizada!
	u32 maxcolor = 0;

	//pintar el primero
	orden[0]->color = 0;
	for (u32 i = 1; i < cantidadVert; ++i) {

		//memset(allowedcolors, UCHAR_MAX, int64creados);
        //memset() no sirve para esto, porque no llena de 1's al array, no me acuerdo ahora porque

		//for (u32 k = 0; k < int64creados; ++k) {
		//	allowedcolors[k] = -1;

		//}

		memset(allowedcolors, (i32)-1, (int64creados * 8));

		//u32 color = 0;
		Vertice restrict vert = orden[i];
		Vertice* arrayVec = vert->vecinos->avecinos;
		u32 grado = vert->grado;

		//reset array
		//si el compilador optimiza bien, usa memset()
		//i > 1 asi la primera vez no lo hace, porque calloc() ya dejo todo en 0
		/*
		for (u32 k = 0; k < sizearrayVecinos && i > 1; ++k) {
			forbiddencolors[k] = false;

		}
		*/
		//Busca cada vecino, para establecer que colores se pueden utilizar y cuales no.
		//Si el color C no se puede utilizar, entonces forbiddencolors[C] = true;

		u32 vecinospintadosyavisitados = 0;
        i64 posicionvecinopintado = 0;
        while(vecinospintadosyavisitados < i){
            if (grado < 4){
                for (u32 j = 0; j < grado; ++j){
                    u32 colorvecino = arrayVec[j]->color;
                    if (colorvecino != UINT32_MAX){
                        allowedcolors[colorvecino / WIDTH] &= ~((i64)1 << (colorvecino%WIDTH));
                         ++posicionvecinopintado;
                        ++vecinospintadosyavisitados;

                    }
                }
                break;

            }
            
            posicionvecinopintado = primeropintadosse4intsmod(arrayVec, grado, posicionvecinopintado);
            if (posicionvecinopintado == -1){
                //no hay mas vecinos pintados
                break;
            }
            u32 colorvecino = arrayVec[posicionvecinopintado]->color;

			/*_____________________________________________________*/

			if((int64creados*64) <= colorvecino){
					//hay que agrandar el array
					//esto es peligroso porque como argumento estamos usando el color del vecino, que maliciosamente podria ser muy grande

					u32 tempint64creados = int64creados;

					allowedcolors = reallocbitarraysigned(allowedcolors, colorvecino, &int64creados);
					//poner todos los bits en 1 en los ints nuevos
					for (u32 k = tempint64creados; k < int64creados; ++k) {
						allowedcolors[k] = (i64)-1;

					}
				}

			/*_____________________________________________________*/

            //clearbitkf(allowedcolors, colorvecino);
            //allowedcolors[colorvecino / WIDTH] &= (i64)((i64)1 << (colorvecino % WIDTH)); //CLEAR

			//ESTA linea anda perfecto pero vamos a probar usar clearbitkf
            //allowedcolors[colorvecino / WIDTH] &= ~(i64)((u64)1 << (u64)(colorvecino%WIDTH));

			clearbitkf(allowedcolors, colorvecino);

            ++posicionvecinopintado;
            ++vecinospintadosyavisitados;
            
        }

        /*LOOP QUE HAY QUE REEMPLAZAR A CONTINUACION*/
        /*
		for (u32 k = 0; k < grado; ++k) {
			//i64 sink = -1;
			u32 colorvecino = arrayVec[k]->color;
			if (colorvecino != UINT32_MAX) {
				//entonces el vecino esta pintado
				++vecinospintadosyavisitados;
				//forbiddencolors[colorvecino] = true;
				//_bittestandreset64(&(allowedcolors[colorvecino/WIDTH]), (colorvecino % WIDTH));

				allowedcolors[colorvecino / WIDTH] |= (i64)((i64)1 << (colorvecino % WIDTH)); //CLEAR
				
				//maxcolor = maxcolor > colorvecino ? maxcolor : colorvecino;
			}
			if (vecinospintadosyavisitados == i) {
				break;
			}
		}
        */
		
		//u32 j = findfirst0fast(forbiddencolors, uint64creados);
        //#ifdef __POPCNT__

		//Si no anda findfirst1fastmod(), usar findfirst1fast() porque anda perfecto
		u32 j = findfirst1fastmod(allowedcolors, int64creados);
        //#else

        //#endif
		//assert(j != (int64creados * 64) + 1);
		//j devuelve la posicion empezando por cero (0)
		//Pintar el vertice del color j
		vert->color = j;
		//Actualizar maxcolor
		maxcolor = maxcolor > j ? maxcolor : j;


	}
	free(allowedcolors);
	//Setcoloresutilizados(G, (maxcolor + 1));
	return (maxcolor + 1);
}

/*GREEDY CON AVX2*/

u32 greedycoloresprimerobitsallowedavx2(Grafo G, const Vertice* restrict const orden) {

	//u32 sizearrayVecinos = Delta(G) + 1; // por teorema de Brooks podemos asumir que nunca vamos a necesitar mas de Δ + 1 colores
	u32 sizearrayVecinos = 1;
	//bool* restrict forbiddencolors = (bool*)calloc(sizearrayVecinos, sizeof(bool)); //este array de bool tiene tantas posiciones como colores posibles (Δ + 1)
	u32 int64creados = 0;
    bitarraysigned allowedcolors = createbitarraysigned(sizearrayVecinos, &int64creados);
	
	u32 cantidadVert = NumeroDeVertices(G);

	//Guarda el color mas alto utilizado. No es la cantidad de colores utilizada!
	u32 maxcolor = 0;

	//pintar el primero
	orden[0]->color = 0;
	for (u32 i = 1; i < cantidadVert; ++i) {

		//memset(allowedcolors, UCHAR_MAX, int64creados);
        //memset() no sirve para esto, porque no llena de 1's al array, no me acuerdo ahora porque
		for (u32 k = 0; k < int64creados; ++k) {
			allowedcolors[k] = -1;

		}

		//u32 color = 0;
		Vertice restrict vert = orden[i];
		Vertice* arrayVec = vert->vecinos->avecinos;
		u32 grado = vert->grado;

		//reset array
		//si el compilador optimiza bien, usa memset()
		//i > 1 asi la primera vez no lo hace, porque calloc() ya dejo todo en 0
		/*
		for (u32 k = 0; k < sizearrayVecinos && i > 1; ++k) {
			forbiddencolors[k] = false;

		}
		*/
		//Busca cada vecino, para establecer que colores se pueden utilizar y cuales no.
		//Si el color C no se puede utilizar, entonces forbiddencolors[C] = true;

		u32 vecinospintadosyavisitados = 0;
        i64 posicionvecinopintado = 0;
        //u32 posicionanterior = 0;
        while(vecinospintadosyavisitados < i){
            posicionvecinopintado = primeropintadoavx2mod(arrayVec, grado, posicionvecinopintado);
            if (posicionvecinopintado == -1){
                break;
            }
            u32 colorvecino = arrayVec[posicionvecinopintado]->color;
			/*_____________________________________________________*/

			if((int64creados*64) <= colorvecino){
					//hay que agrandar el array
					//esto es peligroso porque como argumento estamos usando el color del vecino, que maliciosamente podria ser muy grande

					u32 tempint64creados = int64creados;

					allowedcolors = reallocbitarraysigned(allowedcolors, colorvecino, &int64creados);
					//poner todos los bits en 1 en los ints nuevos
					for (u32 k = tempint64creados; k < int64creados; ++k) {
						allowedcolors[k] = (i64)-1;

					}
				}

			/*_____________________________________________________*/

            //clearbitkf(allowedcolors, colorvecino);
            //allowedcolors[colorvecino / WIDTH] &= (i64)((i64)1 << (colorvecino % WIDTH)); //CLEAR
            allowedcolors[colorvecino / WIDTH] &= ~((i64)1 << (colorvecino%WIDTH));
            //posicionanterior = posicionvecinopintado;
            ++posicionvecinopintado;
            ++vecinospintadosyavisitados;
        }

        /*LOOP QUE HAY QUE REEMPLAZAR A CONTINUACION*/
        /*
		for (u32 k = 0; k < grado; ++k) {
			//i64 sink = -1;
			u32 colorvecino = arrayVec[k]->color;
			if (colorvecino != UINT32_MAX) {
				//entonces el vecino esta pintado
				++vecinospintadosyavisitados;
				//forbiddencolors[colorvecino] = true;
				//_bittestandreset64(&(allowedcolors[colorvecino/WIDTH]), (colorvecino % WIDTH));

				allowedcolors[colorvecino / WIDTH] |= (i64)((i64)1 << (colorvecino % WIDTH)); //CLEAR
				
				//maxcolor = maxcolor > colorvecino ? maxcolor : colorvecino;
			}
			if (vecinospintadosyavisitados == i) {
				break;
			}
		}
        */
		
		//u32 j = findfirst0fast(forbiddencolors, uint64creados);
        //#ifdef __POPCNT__

		u32 j = findfirst1fast(allowedcolors, int64creados);
        //#else

        //#endif
		//assert(j != (int64creados * 64) + 1);
		//j devuelve la posicion empezando por cero (0)
		//Pintar el vertice del color j
		vert->color = j;
		//Actualizar maxcolor
		maxcolor = maxcolor > j ? maxcolor : j;


	}
	free(allowedcolors);
	
	//Setcoloresutilizados(G, (maxcolor + 1));
	return (maxcolor + 1);
}

/* END GREEDY CON AVX2*/





#endif // !__POPCNT__

#endif // !__SSE2__