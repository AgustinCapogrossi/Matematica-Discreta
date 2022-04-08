#include "bitarray.h"
#include <assert.h>

#if defined(__INTEL_COMPILER) || defined(_MSC_VER) && defined(__clang__)
#include <intrin.h> //para _bitscanforward. No se puede poner en el bitarray.h porque sino tiene un conflicto en windows con winnt.h
#endif
bitarray createbitarray(u32 size, u32* uintscreados) {
	
	bitarray r = NULL;
	u32 divv = size / WIDTH;
	u32 resto = size % WIDTH;
	
	if (resto > 0) {

		r = (bitarray)calloc((size_t)(divv + (size_t)1), sizeof(u64));
		(*uintscreados) = divv + 1;

	}
	else {
		r = (bitarray)calloc((size_t)divv, sizeof(u64));
		(*uintscreados) = divv;
	}
	
	return (r);

}

bitarraysigned createbitarraysigned(u32 size, u32* ints64creados) {

	bitarraysigned r = NULL;
	u32 divv = size / WIDTH;
	u32 resto = size % WIDTH;

	if (resto > 0) {

		r = (bitarraysigned)malloc((size_t)(divv + (size_t)1) * sizeof(i64));
		(*ints64creados) = divv + 1;

	}
	else {
		r = (bitarraysigned)malloc((size_t)divv * sizeof(i64));
		(*ints64creados) = divv;
	}

	return (r);

}

bitarraysigned reallocbitarraysigned(bitarraysigned bas, u32 size, u32* ints64creados) {

	bitarraysigned temp = NULL;
	u32 divv = size / WIDTH;
	u32 resto = size % WIDTH;

	if (resto > 0) {

		temp = (bitarraysigned)realloc(bas, (size_t)(divv + (size_t)1) * sizeof(i64));
		(*ints64creados) = divv + 1;

	}
	else {
		temp = (bitarraysigned)realloc(bas ,(size_t)divv * sizeof(i64));
		(*ints64creados) = divv;
	}
	if(temp != NULL){
		bas = temp;
	}

	return (bas);

}

void destroybitarray(bitarray restrict br) {
	free(br);
	br = NULL;
}


void destroybitarraysigned(bitarraysigned restrict br) {
	free(br);
	br = NULL;
}

void setbitk(bitarray restrict br, u32 pos) {

	br[pos / WIDTH] |= (u64)((u64)1 << (pos % WIDTH));
}

void clearbitk(bitarray restrict br, u32 pos) {

	br[pos / WIDTH] &= ~((u64)1 << (pos%WIDTH));

}
/*
setbitk y clearbitk pero intentando usar funciones propias del compilador que supuestamente son mas 
rapidas
*/

void setbitkf(bitarraysigned restrict br, u32 const pos) {

	//i64 sink = -1;

	//_bittestandset64 toma como argumento una posicion empezando desde 0.(verificar)
#if defined(__GNUC__) || !defined(_MSC_VER) && defined(__clang__)
//#  Si estamos compilando con GCC, usar el algoritmo estandar, igual como en setbitk
	br[pos / WIDTH] |= (i64)((i64)1 << (pos % WIDTH));

#elif defined(__INTEL_COMPILER) || defined(_MSC_VER) || defined (__clang__) && defined(_MSC_VER)

	_bittestandset64(&(br[(i64)pos / WIDTH]), pos % WIDTH);
	
#endif
}

void clearbitkf(bitarraysigned restrict br, u32 const pos) {


	//i64 sink = -1;

	//_bittestandreset64 toma como argumento una posicion empezando desde 0.(verificar)

#ifdef __GNUC__
#ifndef __clang__

	br[pos / WIDTH] &= ~((u64)1 << (pos%WIDTH));
#endif // ! __CLANG__
#endif

#ifdef __clang__
#ifdef _WIN64
	//no lo puedo hacer andar. Esta definido en intrin.h pero no quiere.
	//EDIT: Clang solamente lo define si va a funcionar en windows
	_bittestandreset64(&(br[(i64)pos / WIDTH]), pos % WIDTH);
	//br[pos / WIDTH] &= ~((i64)1 << (pos%WIDTH));

#else
	br[pos / WIDTH] &= ~((i64)1 << (pos%WIDTH));
#endif

#endif // ! defined Clang

#if defined(__INTEL_COMPILER) || defined(_MSC_VER)

	_bittestandreset64(&(br[(i64)pos / WIDTH]), pos % WIDTH);

#endif

}


/**/
u32 getbittoint(bitarray restrict br, u32 pos) {

	u64 r = (br[pos / WIDTH] & ((u64)1 << ((u64)pos % WIDTH))) >> ((u64)pos % WIDTH);

	return ((u32)r);

}

u32 findfirst0(u64 const* restrict const br, u32 const cantidadbits) {

	u32 r = cantidadbits + 1; //sino tiene un 0 en array, devolver el tama�o mas 1
	
	for (u32 k = 0; k < cantidadbits; ++k) {
		//u64 temp = (br[k / WIDTH] & ((u64)1 << ((u64)k % WIDTH))) >> ((u64)k % WIDTH);
		//u64 temp2 = temp >> (k % WIDTH);
		//printf("temp:%llu\n", temp);
		if ((br[k / WIDTH] & ((u64)1 << (k % WIDTH))) == 0) {
			//printf("boom\n");
		//if (temp == 0) {
			r = k;
			break;
		}
	}


	return (r);
}

u32 findfirst0en32(u32 solo32bits) {

	u32 r = 33; //sino tiene un 0 en array, devolver el tama�o mas 1

	for (u32 k = 0; k < 32; ++k) {
		//u64 temp = (br[k / WIDTH] & ((u64)1 << ((u64)k % WIDTH))) >> ((u64)k % WIDTH);
		//u64 temp2 = temp >> (k % WIDTH);
		//printf("temp:%llu\n", temp);
		if ((solo32bits & ((u32)1 << (k % 32))) == 0) {
			//printf("boom\n");
		//if (temp == 0) {
			r = k;
			break;
		}
	}

	return (r);
}

#if defined(__POPCNT__) && ( (defined(__clang__) || defined(_MSC_VER )) || defined(__GNUC__))
u32 findfirst0fast(u64 const* restrict const br, u32 const cantidad_u64ints) {
	/*
	* POPCNT: int64_t _mm_popcnt_u64(unsigned __int64 a);
	* 
	* 
	*/

	u32 r = cantidad_u64ints*64 + 1; //sino tiene un 0 en array, devolver la cantidad de bits mas 1


	for (u32 k = 0; k < cantidad_u64ints; ++k) {

		u64 step = br[k];
		u64 rr = _mm_popcnt_u64(step);

		if (rr == 0) {
			return (k * WIDTH);
		}
		if (rr == WIDTH) {
			continue;
		}
		else {
			//busqueda binaria pero solo hacemos una division. No refina mas que eso
			u32* partes = (u32*)&step;
			u32 parte1 = partes[0];
			u32 parte2 = partes[1];
			u32 bitssetenparte1 = _mm_popcnt_u32(parte1); //least significant bits
			u32 bitssetenparte2 = _mm_popcnt_u32(parte2); //most significant bits
			
			if (bitssetenparte1 == 32 && bitssetenparte2 == 0) {
				r = (k * WIDTH) + 32;
				break;
			}

			if (bitssetenparte1 == 0 && bitssetenparte2 == 32) {
				r = (k * WIDTH);
				break;
			}
			
			if (bitssetenparte1 > 0 && bitssetenparte1 != 32 && bitssetenparte2 == 0) {

				r = findfirst0en32(parte1);
				
				//assert(r != 33);
				r += k * WIDTH;
				break;

			}
			else {
				if (bitssetenparte1 == 0 && bitssetenparte2 > 0 && bitssetenparte2 != 32) {
					
					r = findfirst0en32(parte2);
					
					//assert(r != 33);
					r += k * WIDTH;
					break;

				}
				else {

					r = findfirst0((br + k), 64);
					//assert(r != 65);
					r += k * WIDTH;
					break;

				}
			}
		}//end if() popcnt64 != 0

	} //end for()

	

	return (r);
}

u32 findfirst1fast(i64 const* restrict const bitarr, u32 const sizearray) {

	u32 resultado = (sizearray * 64) + 1;
	for (u32 index = 0; index < sizearray; ++index) {
		
		
		i64 rrpc = _mm_popcnt_u64(bitarr[index]);
		if (rrpc == 0) {
			continue;
		}
		if (rrpc == 64) {
			return(index * 64);
		}

		int32_t* partes = (int32_t*)&bitarr;
		int32_t parte1 = partes[0];
		int32_t parte2 = partes[1];

		u32 bitssetenparte1 = _mm_popcnt_u32(parte1);
		u32 bitssetenparte2 = _mm_popcnt_u32(parte2);

		if (bitssetenparte1 == 32 && bitssetenparte2 == 0) {
			resultado = (index * WIDTH) + 32;
			break;
		}

		if (bitssetenparte1 == 0 && bitssetenparte2 == 32) {
			resultado = (index * WIDTH);
			break;
		}


		
#ifdef __GNUC__
		int rrgcc = __builtin_ffsll(bitarr[index]);
		
		if (rrgcc != 0){
			--rrgcc; //esto porque ffsll(), si encuentra el primer bit en 1, devuelve el indice m�s 1. Sino devuelve 0 (no hay ningun bit en 1)
			resultado = (index * 64) + (u32)rrgcc;
			break;
		}
		
#elif __INTEL_COMPILER || _MSC_VER || __clang__
		//el compilador de Intel tambien define _MSC_VER, tener cuidado si hay alguna diferencia a tener en cuenta
		//esto lo hacemos porque GCC define otra funcion "intrinseca" igual a bitscan pero con otro nombre
		
		unsigned long rr = 65; //sabemos que 65 nunca puede ser una posicion
		unsigned char resultadobitscan = _BitScanForward64(&(rr), bitarr[index]);
		if (resultadobitscan != 0) {
			resultado = (index * 64) + rr;
			break;
		}
#else
		//usar busqueda con shift's
#endif
		/*
		else {
			//no habia ningun bit 1 en ese elemento del array
			continue;
		}
		*/
	}

	return (resultado);
}

/*----------------------------------------------------------------*/

u32 findfirst1en32(u32 solo32bits) {

	u32 r = 33; //sino tiene un 0 en array, devolver el tama�o mas 1

	for (u32 k = 0; k < 32; ++k) {
		//u64 temp = (br[k / WIDTH] & ((u64)1 << ((u64)k % WIDTH))) >> ((u64)k % WIDTH);
		//u64 temp2 = temp >> (k % WIDTH);
		//printf("temp:%llu\n", temp);
		if ((solo32bits & ((u32)1 << (k % 32))) == 1) {
			//printf("boom\n");
		//if (temp == 0) {
			r = k;
			break;
		}
	}

	return (r);
}

u32 findfirst1en32f(u32 solo32bits) {

	u32 r = 33; //sino tiene un 0 en array, devolver el tama�o mas 1
	long int solo32bitsl = (long int)solo32bits;


	#ifdef __GNUC__
		int rrgcc = __builtin_ffsl(solo32bitsl);
		
		if (rrgcc != 0){
			--rrgcc; //esto porque ffsll(), si encuentra el primer bit en 1, devuelve el indice m�s 1. Sino devuelve 0 (no hay ningun bit en 1)
			r = (u32)rrgcc;
		}
		
#elif __INTEL_COMPILER || _MSC_VER || __clang__
		//el compilador de Intel tambien define _MSC_VER, tener cuidado si hay alguna diferencia a tener en cuenta
		//esto lo hacemos porque GCC define otra funcion "intrinseca" igual a bitscan pero con otro nombre
		
		unsigned long rr = 65; //sabemos que 65 nunca puede ser una posicion
		unsigned char resultadobitscan = _BitScanForward(&(rr), solo32bitsl);
		if (resultadobitscan != 0) {
			r = rr;
		}
#else
		//usar busqueda con shift's

		for (u32 k = 0; k < 32; ++k) {
			//u64 temp = (br[k / WIDTH] & ((u64)1 << ((u64)k % WIDTH))) >> ((u64)k % WIDTH);
			//u64 temp2 = temp >> (k % WIDTH);
			//printf("temp:%llu\n", temp);
			if ((solo32bits & ((u32)1 << (k % 32))) == 1) {
				//printf("boom\n");
				//if (temp == 0) {
				r = k;
				break;
			}
		}



#endif


	return (r);
}


u32 findfirst1fastmod(i64 const* restrict const bitarr, u32 const sizearray) {

	u32 resultado = (sizearray * 64) + 1;
	for (u32 index = 0; index < sizearray; ++index) {
		
		
		i64 rrpc = _mm_popcnt_u64(bitarr[index]);
		if (rrpc == 0) {
			continue;
		}
		if (rrpc == 64) {
			return(index * 64);
		}

		int32_t* partes = (int32_t*)&bitarr;
		int32_t parte1 = partes[0];
		int32_t parte2 = partes[1];

		u32 bitssetenparte1 = _mm_popcnt_u32(parte1);
		u32 bitssetenparte2 = _mm_popcnt_u32(parte2);

		if (bitssetenparte1 == 32 && bitssetenparte2 == 0) {
			resultado = (index * WIDTH) + 32;
			break;
		}

		if (bitssetenparte1 == 0 && bitssetenparte2 == 32) {
			resultado = (index * WIDTH);
			break;
		}

		if (bitssetenparte1 > 0 && bitssetenparte1 != 32 && bitssetenparte2 == 0) {

				resultado = findfirst1en32f(parte1);
				
				//assert(r != 33);
				resultado += index * WIDTH;
				break;

			}
		else {
			if (bitssetenparte1 == 0 && bitssetenparte2 > 0 && bitssetenparte2 != 32) {
					
				resultado = findfirst1en32f(parte2);
					
				//assert(r != 33);
				resultado += index * WIDTH;
				break;

			}
		}




		/*-----------------------------------------*/
#ifdef __GNUC__
		int rrgcc = __builtin_ffsll(bitarr[index]);
		
		if (rrgcc != 0){
			--rrgcc; //esto porque ffsll(), si encuentra el primer bit en 1, devuelve el indice m�s 1. Sino devuelve 0 (no hay ningun bit en 1)
			resultado = (index * 64) + (u32)rrgcc;
			break;
		}
		
#elif __INTEL_COMPILER || _MSC_VER || __clang__
		//el compilador de Intel tambien define _MSC_VER, tener cuidado si hay alguna diferencia a tener en cuenta
		//esto lo hacemos porque GCC define otra funcion "intrinseca" igual a bitscan pero con otro nombre
		
		unsigned long rr = 65; //sabemos que 65 nunca puede ser una posicion
		unsigned char resultadobitscan = _BitScanForward64(&(rr), bitarr[index]);
		if (resultadobitscan != 0) {
			resultado = (index * 64) + rr;
			break;
		}
#else
		//usar busqueda con shift's
#endif
		/*
		else {
			//no habia ningun bit 1 en ese elemento del array
			continue;
		}
		*/
	}

	return (resultado);
}

/*------------------------------------------------------------------------*/

#endif