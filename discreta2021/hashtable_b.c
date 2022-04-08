#include "hashtable_b.h"
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef uint64_t u64;

static u32 TABLE_SIZE;
static u32 TABLE_SIZE_HS;
//u32 const PRIMEG = 678964193;
//u32 PRIME;

//https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html
u64 hashfuncfunc(u32 key, u32 buckets){
    //#define hash2(key, buckets) (((key)*((key)+3)) % (buckets))
    return((u64)(((u64)key)*(((u64)key)+3)) % buckets);
}

inline u64 int64hash(u64 u) {
    //Returns hash of u as a 64-bit integer.
    u64 v = u * 3935559000370003845LL + 2691343689449507681LL;
    v ^= v >> 21; v ^= v << 37; v ^= v >> 4;
    v *= 4768777513237032717LL;
    v ^= v << 20; v ^= v >> 41; v ^= v << 5;
    return v;
}

inline u32 int32hash(u64 u){
    return (u32)(int64hash(u) & 0xffffffff);
}

u32 nextPowerOf2(u32 n)
{
    u32 p = 1;
    if (n && !(n & (n - 1))) {
        return n;
    }
 
    while (p < n) {
        p <<= 1;
    }
     
    return p;
}



// function to calculate first hash
inline u32 hash1(u32 key){
        return (key % TABLE_SIZE);
}
static u32 th_hs;
inline u32 hash1_hs(u32 key){
        //return (int32hash(key) % th_hs);
        //return (key % (TABLE_SIZE_HS - 32));
        return (key % th_hs);
}
static float const A = 0.618033989F;
static float d = 0;
inline u32 hash3_hs(u32 key){
    if(d == 0){
        d = (float)floorf(A * (float)UINT32_MAX);
    }
    u32 x = (u32)((float)key*d);
    return(x%th_hs);

}

  
// function to calculate second hash
//int hash2(int key){
//return (PRIME - (key % PRIME));
//}

//inicializa tabla hash con esquema open addressing con "sondeo cuadratico"
hashtable_t* inicializartabla(u32 sizeTable){
    //Vamos a usar el doble del espacio necesario, asi la tabla tiene un factor de ocupacion medio.
    //Tambien es necesario usar al menos el doble del tamanio necesario para garantizar que va a encontrar un lugar vacio a la hora de insertar
    //Vamos a usar de tamaño la potencia de 2 igual o superior al valor, y despues lo multiplicamos por 2. Potencia de 2 porque con eso podemos garantizar que vamos a encontrar un lugar si usamos una funcion hash que tiene cierta manera
    //Para mas, leer: https://es.wikipedia.org/wiki/Tabla_hash#Resoluci%C3%B3n_de_colisiones
    //En especifico, estamos usando "sondeo cuadratico". Descripto en ingles en: https://en.wikipedia.org/wiki/Quadratic_probing
    //Por que podemos usar un tamaño de tabla que no es un numero primo? Porque en general funciona: https://stackoverflow.com/questions/1145217/why-should-hash-functions-use-a-prime-number-modulus
    //La multiplicacion por O.5*i y i*(0.5^2) se debe a que esto garantiza que vamos a visitar todos los lugares de la tabla: https://research.cs.vt.edu/AVresearch/hashing/quadratic.php
    u32 const multiplicador = 2;
    //este assert es para verificar que haya overflow cuando multipliquemos a sizeTable por 2.
    //Aprendi que si usamos la opcion "-ftrapv" en gcc, el compilador emite codigo que verifica que cada operacion aritmetica no haga overflow. Esto hubiese sido muy bueno para descubrir errores de overflow, porque en los grafos chicos no tenemos vertices con nombres cercanos al limite de un u32, entonces descubrimos errores tarde.
    assert((UINT32_MAX/sizeTable) >= 2);
    TABLE_SIZE = (nextPowerOf2(sizeTable) * multiplicador);

    hashtable_t* tabla = (hashtable_t*)malloc(sizeof(hashtable_t));
    tabla->currsize = 0;
    tabla->arr = (Vertice*)calloc(TABLE_SIZE, sizeof(Vertice));
    if(tabla->arr == NULL){
        exit(12);
    }
    //inicializar todos en NULL lo hace calloc


    return(tabla);
}

char insertarelemento(hashtable_t* tabla, Vertice vert){
    char r = 0;
    //este assert es para revisar que no haya overflow cuando usamos hash2. Para nuestro proyecto, es imposible porque ningun nombre es supera a uint32_max
    //pero si usamos a hash2 sin los casts a (u64) a los argumentos, esto falla en los grafos con nombres grandes.
    //Dicho de otra manera, si el nombre del vertice es superior a 65534, la cuenta de la funcion hash (nombrevert)*(nombrevert+3) da mayor al numero maximo posible que de puede guardar en un u32. Entonces se guarda la diferencia y no sirve.
    //Por ese motivo, usamos la funcion hash1, que es el clasico: key % longitudtabla.

    //assert(vert->nombre < (UINT64_MAX/(vert->nombre + 3))); //Este assert es si usamos hash2

    for(u32 i = 0; i < TABLE_SIZE; ++i){
        //u32 hash = ((u32) ((float)hash2((u64)vert->nombre, (u64)TABLE_SIZE) + ( 0.5f * (float)i) + (((0.5f) * (0.5f)) * (float)i) )  % TABLE_SIZE);
        u32 hash = ((u32) ((float)hash1(vert->nombre) + ( 0.5F * (float)i) + (((0.5F) * (0.5F)) * (float)i) )  % TABLE_SIZE);
        if (tabla->arr[hash] == NULL){
            //entonces podemos insertar el valor aca
            tabla->arr[hash] = vert;
            #ifndef NDEBUG
            ++tabla->currsize;
            #endif
            return(1);//exito
        }

    }
    return(r);//todo mal, la tabla esta llena, lo que no deberia ser posible dado que su tamaño es mas del doble de lo necesario
}

Vertice buscarelemento(hashtable_t* tabla, u32 nombrevertice){
    Vertice r = NULL;

    for(u32 i = 0; i < TABLE_SIZE; ++i){
        //u32 hash = ((u64) ((double)hashfuncfunc(nombrevertice, TABLE_SIZE) + ( 0.5 * (double)i) + (((0.5) * (0.5)) * (double)i) )  % TABLE_SIZE);
        //u32 hash = ((u32) ((float)hash2((u64)nombrevertice, (u64)TABLE_SIZE) + ( 0.5f * (float)i) + (((0.5f) * (0.5f)) * (float)i) )  % TABLE_SIZE);
        u32 hash = ((u32) ((float)hash1(nombrevertice) + ( 0.5F * (float)i) + (((0.5F) * (0.5F)) * (float)i) )  % TABLE_SIZE);
        Vertice candidate = tabla->arr[hash];
        if(candidate != NULL){
            if(candidate->nombre == nombrevertice){
                return(candidate);//fue encontrado
            }
        }
        else{
            break;
        }
    }
    return(r);//no fue encontrado
}

//-------------------------------------------------------------------------//


//inicializa tabla hash con esquema open addressing con "sondeo cuadratico"
hashtable2_t* inicializartabla2(u32 sizeTable){
    //Vamos a usar el doble del espacio necesario, asi la tabla tiene un factor de ocupacion medio.
    //Tambien es necesario usar al menos el doble del tamanio necesario para garantizar que va a encontrar un lugar vacio a la hora de insertar
    //Vamos a usar de tamaño la potencia de 2 igual o superior al valor, y despues lo multiplicamos por 2. Potencia de 2 porque con eso podemos garantizar que vamos a encontrar un lugar si usamos una funcion hash que tiene cierta manera
    //Para mas, leer: https://es.wikipedia.org/wiki/Tabla_hash#Resoluci%C3%B3n_de_colisiones
    //En especifico, estamos usando "sondeo cuadratico". Descripto en ingles en: https://en.wikipedia.org/wiki/Quadratic_probing
    //Por que podemos usar un tamaño de tabla que no es un numero primo? Porque en general funciona: https://stackoverflow.com/questions/1145217/why-should-hash-functions-use-a-prime-number-modulus
    //La multiplicacion por O.5*i y i*(0.5^2) se debe a que esto garantiza que vamos a visitar todos los lugares de la tabla: https://research.cs.vt.edu/AVresearch/hashing/quadratic.php
    u32 const multiplicador = 2;
    //este assert es para verificar que haya overflow cuando multipliquemos a sizeTable por 2.
    //Aprendi que si usamos la opcion "-ftrapv" en gcc, el compilador emite codigo que verifica que cada operacion aritmetica no haga overflow. Esto hubiese sido muy bueno para descubrir errores de overflow, porque en los grafos chicos no tenemos vertices con nombres cercanos al limite de un u32, entonces descubrimos errores tarde.
    //Si se usa Clang, se puede usar -fsanitize=unsigned-integer-overflow, que es mas util para esto en el caso de usar unsigned integers.
    
    assert((UINT32_MAX/sizeTable) >= 2);
    TABLE_SIZE = (nextPowerOf2(sizeTable) * multiplicador);

    hashtable2_t* tabla = (hashtable2_t*)malloc(sizeof(hashtable2_t));
    tabla->currsize = 0;
    tabla->arr = (vertOrd_t*)calloc(TABLE_SIZE, sizeof(vertOrd_t));
    if(tabla->arr == NULL){
        //exit(12);
        free(tabla);
        return(NULL);
    }
    //inicializar todos en NULL lo hace calloc


    return(tabla);
}

char insertarelemento2(hashtable2_t* tabla, u32 nombre , u32 posOrden){
    char r = 0;
    ++posOrden;
    //este assert es para revisar que no haya overflow cuando usamos hash2. Para nuestro proyecto, es imposible porque ningun nombre es supera a uint32_max
    //pero si usamos a hash2 sin los casts a (u64) a los argumentos, esto falla en los grafos con nombres grandes.
    //Dicho de otra manera, si el nombre del vertice es superior a 65534, la cuenta de la funcion hash (nombrevert)*(nombrevert+3) da mayor al numero maximo posible que de puede guardar en un u32. Entonces se guarda la diferencia y no sirve.
    //Por ese motivo, usamos la funcion hash1, que es el clasico: key % longitudtabla.

    //assert(vert->nombre < (UINT64_MAX/(vert->nombre + 3))); //Este assert es si usamos hash2

    //Guardamos la posicion en ordenNatural, pero sumandole 1, para poder usar el 0 como valor para indicar vacio.

    for(u32 i = 0; i < TABLE_SIZE; ++i){
        //u32 hash = ((u32) ((float)hash2((u64)vert->nombre, (u64)TABLE_SIZE) + ( 0.5f * (float)i) + (((0.5f) * (0.5f)) * (float)i) )  % TABLE_SIZE);
        u32 hash = ((u32) ((float)hash1(nombre) + ( 0.5F * (float)i) + (((0.5F) * (0.5F)) * (float)i) )  % TABLE_SIZE);
        if (tabla->arr[hash].posOrden == 0){ // 0 significa vacio
            //entonces podemos insertar el valor aca
            tabla->arr[hash].posOrden = posOrden;
            tabla->arr[hash].nombre = nombre;
            #ifndef NDEBUG
            ++tabla->currsize;
            #endif
            return(1);//exito
        }

    }
    return(r);//todo mal, la tabla esta llena, lo que no deberia ser posible dado que su tamaño es mas del doble de lo necesario
}

u32 buscarelemento2(hashtable2_t* tabla, u32 nombrevertice){
    //Vertice r = NULL;

    for(u32 i = 0; i < TABLE_SIZE; ++i){
        //u32 hash = ((u64) ((double)hashfuncfunc(nombrevertice, TABLE_SIZE) + ( 0.5 * (double)i) + (((0.5) * (0.5)) * (double)i) )  % TABLE_SIZE);
        //u32 hash = ((u32) ((float)hash2((u64)nombrevertice, (u64)TABLE_SIZE) + ( 0.5f * (float)i) + (((0.5f) * (0.5f)) * (float)i) )  % TABLE_SIZE);
        u32 hash = ((u32) ((float)hash1(nombrevertice) + ( 0.5F * (float)i) + (((0.5F) * (0.5F)) * (float)i) )  % TABLE_SIZE);
        vertOrd_t posicionentabla = tabla->arr[hash];
        if(posicionentabla.posOrden != 0){
            if(posicionentabla.nombre == nombrevertice){
                return(posicionentabla.posOrden-1);//fue encontrado
            }
        }
        else{
            break;
        }
    }
    return(UINT32_MAX);//no fue encontrado
}

void destruirtablahash2(hashtable2_t* tabla){
    assert(tabla != NULL);
    assert(tabla->arr != NULL);
    free(tabla->arr);
    free(tabla);
    tabla = NULL;
}



//-------------------------------------------------------------------------//
//Todo esto de abajo es para otro esquema de tabla hash: hopscotch hashing. Es un poco mas rapido pero cuando el factor de ocupacion se acerca a 0.9 puede fallar en guardar un vertice. Si bien eso nunca pasa en la practica porque tiene el doble de capacidad que la necesaria, al principio no andaba bien. Ahora anda bien pero no fue usada

void destruirtablahash(hashtable_t* tabla){
    assert(tabla->arr != NULL);
    free(tabla->arr);
    free(tabla);
    tabla = NULL;
}

inline void setbit(u16* br, u32 pos) {

	(*br) |= (u16)((u16)1 << (pos));
}


inline void setbit32(u32* br, u32 pos) {

	(*br) |= (u32)((u32)1 << (pos));
}


char getbitinpositionx(u16 bits, u16 pos){

    if ((bits & ((u16)1 << (pos))) == 1){
        return(1);
    }
    else{
        return(0);
    }

}

char getbitinpositionx32(u32 bits, u32 pos){

    if ((bits & ((u32)1 << (pos))) == 1){
        return(1);
    }
    else{
        return(0);
    }

}

u16 findxbit1en16(u16 solo16bits, u16 iesimo) {

	u16 r = 17; //sino tiene un 0 en array, devolver el tama�o mas 1
    u16 contador = 0;
    //--iesimo; //porque las posiciones dentro del u16 empiezan en 0,1,2,...,15.

	for (u16 k = 0; k < 16; ++k) {
		//u64 temp = (br[k / WIDTH] & ((u64)1 << ((u64)k % WIDTH))) >> ((u64)k % WIDTH);
		//u64 temp2 = temp >> (k % WIDTH);
		//printf("temp:%llu\n", temp);
		if ((solo16bits & ((u16)1 << (k))) == 1) {
            ++contador;
            if(contador == iesimo){
                r=k;
                break;
            }
            //else
            
		}
	}

	return (r);
}

u32 findxbit1en32(u32* solo32bits, u16 iesimo) {

	u32 r = 0; //sino tiene un 0 en array, devolver el tama�o mas 1
    u32 contador = 0;
    //--iesimo; //porque las posiciones dentro del u16 empiezan en 0,1,2,...,15.

   if((*solo32bits) != 0){
	    for (u32 k = 0; k < 32; ++k) {
	    	//u64 temp = (br[k / WIDTH] & ((u64)1 << ((u64)k % WIDTH))) >> ((u64)k % WIDTH);
	    	//u64 temp2 = temp >> (k % WIDTH);
	    	//printf("temp:%llu\n", temp);
	    	//if ((((*solo32bits) & ((u32)1 << (k))) >> (k)) == 1) { //Shift, mask y despues shift de vuelta es muy lento y lo podemos reemplazar por algo mas simple
            if ((((*solo32bits) & ((u32)1 << (k))) ) != 0) {
                ++contador;
                if(contador == iesimo){
                    return(k);
                }
                //else

	    	}
            else{
            }
	    }
    }

	return (r);
}

//u16 MAXDISTANCE = 16;
u32 MAXDISTANCE = 32;
u32 const MULTIPLICADORHS = 2;

//hopscotch tiene un problema: si el nombre de un vertice + 32 es mayor a uint32_max, hay un overflow(wrap around) (que no chequeamos en runtime porque gasta tiempo) y el algoritmo falla en insertar un vertice.
hopnode_t* inicializarhs(u32 size){
    //multiplicado el tamaño por 2
    assert((UINT32_MAX/size) >= 2);
    if(size < 32){
       size += 32;
    }
    size += 32;
    
    TABLE_SIZE_HS = (size * MULTIPLICADORHS);//le sumamos 32 porque esa es la distancia maxima. Entonces si justo el hash da el numero maximo, y tenemos que reubicar un valor vecino, vamos a hacer overflow en el array. Si hacemos un check cada vez, es muy caro. Mejor agrandar aca y en hash1()
    TABLE_SIZE_HS += 32;
    //th_hs = TABLE_SIZE_HS - 64;
    th_hs = TABLE_SIZE_HS;
    //TABLE_SIZE_HS = nextPowerOf2(size) * 2;
    hopnode_t* tabla = (hopnode_t*)calloc((TABLE_SIZE_HS), sizeof(hopnode_t));
    
    return(tabla);    
}

char insertarelementohs(hopnode_t* tabla, Vertice vert){
    char r = 0;
    //char next = 0;
    u32 hash = hash1_hs(vert->nombre);
    if(tabla[hash].vert_p == NULL){
        //se puede insertar aca mismo.
        tabla[hash].vert_p = vert;
        //setbit(&tabla[hash].hops, 0); //settea en la posicion[0] el bit en 1. Empieza desde 0.
        setbit32(&tabla[hash].hops, 0); //settea en la posicion[0] el bit en 1. Empieza desde 0.
        return(1);
    }
    //else if(tabla[hash].hops != UINT16_MAX){
    //if(tabla[hash].hops != UINT32_MAX){
    else if(tabla[hash].hops != UINT32_MAX){
        //entonces hay algun elemento en esta posicion cuyo hash es el mismo o es alguno cercano y termino en esta posicion. Hay que buscar otro lugar. Eso depende de los hops
        //for(u16 i = 1; i < MAXDISTANCE; ++i){
        //for(u32 i = 1; i < MAXDISTANCE; ++i){
        for(u32 i = findxbit1en32(&tabla[hash].hops, 1); i < MAXDISTANCE; ++i){
            if(tabla[hash + i].vert_p == NULL){
                //podemos insertarlo aca.
                tabla[hash + i].vert_p = vert;
                //marcamos el bit correspondiente en hops
                //setbit(&tabla[hash].hops, (i));
                setbit32(&tabla[hash].hops, (i));
                //listo
                return(1);
            }
        }
        //next = 1;
        //exit(1);
    }
    //if(next == 1){
            //si llegamos hasta aca, no pudimos insertar el elemento en ningun lugar dentro de la maxima distancia, entonces hay que hacer un eviction de un elemento cercano.
            //para eso primero buscamos un lugar desocupado, y despues vamos probando que elemento que puede ser ubicado ahi podemos mover.
            
    for(u32 i = (hash + 1 + MAXDISTANCE); i < (hash + 1 + MAXDISTANCE*2); ++i){//maxdistance*2 porque en el peor caso, el elemento <posicion elemento argumento + maxdistance> es el que hay que mover, y dicho elemento solamente puede ser movido en el peor caso hasta maxdistance + maxdistance = maxdistance*2
        if(tabla[i].vert_p == NULL){ //if(i <= TABLE_SIZE_HS && tabla[i].vert_p == NULL){
            //entonces podriamos intentar mover algun elemento a este lugar.
            u32 posicioncandidata = i;
            for(u32 j = (posicioncandidata - MAXDISTANCE); j < posicioncandidata; ++j){
                u16 contador = 0;
                //if(tabla[j].hops != 0 && (getbitinpositionx(tabla[j].hops, (MAXDISTANCE -1 - contador)) == 0) && tabla[j].hops != UINT16_MAX){
                if(tabla[j].hops != 0 && (getbitinpositionx32(tabla[j].hops, (MAXDISTANCE -1 - contador)) == 0) && tabla[j].hops != UINT32_MAX){
                    //lo movemos a la posicion candidata. Y en j guardamos nuestro vertice
                    tabla[posicioncandidata].vert_p = tabla[j].vert_p;
                    tabla[posicioncandidata].hops = 0;
                    //setbit(&tabla[j].hops, (MAXDISTANCE - 1 - contador));
                    setbit32(&tabla[j].hops, (MAXDISTANCE - 1 - contador));
                    //guardamos nuestro vertice en la posicion liberada.
                    tabla[j].vert_p = vert;
                    assert((j-hash) <= MAXDISTANCE );
                    //setbit(&tabla[hash].hops, (j - hash - 1)); // (j - hash) === hash - hash + MAXDISTANCE + contador_i - MAXDISTANCE + eloffsetdecontadorj. La pregunta es: como es que esta garantizado que contador_i+eloffsetdecontadorj <= MAXDISTANCE? Si no se cumpliera eso, agregar un elemento no estaria garantizado
                    setbit32(&tabla[hash].hops, (j - hash - 1));
                    //printf("\t\tWe did it\n\n");
                    return(1);
                }

                ++contador;

            }//endfor interno

        }//endif

    }//endfor externo
        
    printf("No se pudo insertar, tenes que hacer un rehash\n");

        //}//end else|if|lo que habia
    
    return(r);
}

Vertice buscarelementohs(hopnode_t* tabla, u32 nombrevert){
    u32 hash = hash1_hs(nombrevert);
    //for(u16 i = findxbit1en16(tabla[hash].hops, 1); i < MAXDISTANCE; ++i){
    u32 tt = findxbit1en32(&tabla[hash].hops, 1);
    //for(u32 i = findxbit1en32(tabla[hash].hops, 1); i < MAXDISTANCE; ++i){
    for(u32 i = tt; i < MAXDISTANCE; ++i){
        if(tabla[hash + i].vert_p != NULL && tabla[hash + i].vert_p->nombre == nombrevert){
            return(tabla[hash + i].vert_p);
        }
    }
    return(NULL);
}

void destruirtablahashhs(hopnode_t* tabla){

    free(tabla);

}