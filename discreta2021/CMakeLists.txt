cmake_minimum_required(VERSION 3.10)

# set the project name
project(TestDimacs C)

#Especifico de cada compilador:
# Configuracion para Clang
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
        if(CLANG_TIDY_COMMAND)
            message(STATUS "Se va a utilizar clang-tidy")
            set(CMAKE_C_CLANG_TIDY clang-tidy;-header-filter=.;-checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*,-clang-analyzer-apiModeling.*,-clang-analyzer-osx.*,-clang-analyzer-webkit.*,bugprone-*,clang-analyzer-unix.*,-clang-analyzer-optin.portability.*,-readability-*,-readability-magic-numbers,-clang-analyzer-security.insecureAPI.*,-readability-isolate-declaration;)
            #set(CMAKE_CXX_CLANG_TIDY "" CACHE STRING "" FORCE) #esto limpia la variable para todo los proyectos de forma recursiva
        endif()

        
    endif()#!buildtype is debug
    if(WIN32)
    #if(CMAKE_C_COMPILER MATCHES "clang-cl\.exe$")
    #if(CMAKE_C_COMPILER MATCHES "clang\.exe$")
    #add_compile_options("-Wall -Wextra -g3 -glldb -march=skylake -target x86_64-pc-win32 -fsanitize=address -fsanitize-address-use-after-scope")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -glldb -O0 -g --target=x86_64-pc-win32 -march=skylake -mtune=skylake -mavx2 -fshort-enums -ftrapv")
    #set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -glldb --target=x86_64-pc-win32 -march=skylake")
    #Para ver la vectorizacion, usar -Rpass=loop-vectorize -Rpass-analysis=vectorize -Rpass-missed=vectorize
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -DNDEBUG -flto=full -march=skylake -mtune=skylake -mavx2 -Ofast --target=x86_64-pc-win32 -ffast-math -fassociative-math -fno-signed-zeros -freciprocal-math -fno-unwind-tables")
    set(LDFLAGS "${LDFLAGS} -flto=full")
    #endif()
    else()
    #-fsanitize=address -fsanitize-address-use-after-scope
	set(CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -g -g3 -glldb -march=skylake -mavx2 -O0 -fno-optimize-sibling-calls -target x86_64-unknown-linux-gnu -fsanitize=address,undefined,float-cast-overflow,bounds -fsanitize-address-use-after-scope -fsanitize=unsigned-integer-overflow")
    #Usar estas flags para ver porque no vectoriza algun loop: -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize
    #Para usar profile data de instrumentacion, usar: -fprofile-instr-use=code.profdata
    #Para compilar para genenrar profile data con instrumentacion, usar: -fprofile-instr-generate. Despues export LLVM_PROFILE_FILE="code-%m.profraw". Ejecutar el programa.
    #Luego hacer llvm-profdata merge -output=code.profdata code-*.profraw. Y despues usar -fprofile-instr-use
    #-fsave-optimization-record=yaml
    #-fprofile-instr-use=code.profdata -fsave-optimization-record=yaml
    #-flto=full
    #-fsanitize=address,undefined,float-cast-overflow,bounds -fsanitize-address-use-after-scope -fsanitize=unsigned-integer-overflow
    #-g3 -flto=full -glldb
	set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto=full -DNDEBUG -Wall -march=skylake -mtune=skylake -mavx2 -Ofast -target x86_64-unknown-linux-gnu -ffast-math -freciprocal-math -Rpass=loop-vectorize -fprofile-use=code2.profdata") #-fsanitize=address,undefined,float-cast-overflow,bounds -fsanitize-address-use-after-scope -fsanitize=unsigned-integer-overflow
    set(LDFLAGS "${LDFLAGS} flto=full -fprofile-use=code2.profdata")
    #set(LDFLAGS "${LDFLAGS} -pg -g3 -fopenmp")
    endif() #!target system is Windows32 or Windows64
endif()
#!configuracion para Clang

#Configuracion para GCC
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        find_program(CLANG_TIDY_COMMAND NAMES clang-tidy)
        if(CLANG_TIDY_COMMAND)
            message(STATUS "Se va a utilizar clang-tidy")
            set(CMAKE_C_CLANG_TIDY clang-tidy;-header-filter=.;-checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*,-clang-analyzer-apiModeling.*,-clang-analyzer-osx.*,-clang-analyzer-webkit.*,bugprone-*,clang-analyzer-unix.*,-clang-analyzer-optin.portability.*,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling;)
        endif()
	endif()
    set(CMAKE_C_FLAGS_DEBUG "-g3 -O0 -Wall -Wextra -march=native -mtune=native -fsanitize=address,undefined,float-cast-overflow,bounds,bool")
    #-fsanitize=address,undefined
    #-ftrapv
    #set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -march=native")
	#set(CMAKE_C_FLAGS_RELEASE "-O3 -Ofast -march=native -mtune=native -flto -ffast-math -fprofile-use=/home/leo/Famaf/Discreta2/2021/proy/discreta2021/build")
    # -fopt-info-inline-optimized-missed=gccinline.txt
    #-freg-struct-return
    set(CMAKE_C_FLAGS_RELEASE "-Ofast -flto -fuse-linker-plugin -march=native -mtune=native -ffast-math -DNDEBUG --param=inline-min-speedup=1 -ftree-parallelize-loops=4 -fgcse-sm -fgcse-las -fipa-pta -fprofile-use=/home/leo/Famaf/Discreta2/2021/proy/discreta2021/build")

    #set(LDFLAGS "-flto -fuse-linker-plugin -fprofile-use=/home/leo/Famaf/Discreta2/2021/proy/discreta2021/build")
    #-flto -fuse-linker-plugin
    #-g --gc-sections -g3
    set(LDFLAGS "${LDFLAGS} -flto -fuse-linker-plugin -fprofile-use=/home/leo/Famaf/Discreta2/2021/proy/discreta2021/build")
    #set(LDFLAGS "${LDFLAGS} -pg -g2 -fopenmp")
endif()
#!configuracion para gcc
if(CMAKE_C_COMPILER_ID STREQUAL "IntelLLVM;Intel")
endif()

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
endif()

#Peligroso
file(GLOB HEADERS "./*.h")
#Peligroso
file(GLOB SOURCES "./*.c")

#$<FILTER:list,INCLUDE|EXCLUDE,regex>
#<FILTER:SOURCES,EXCLUDE,"LArray.c">
#list(FILTER SOURCES EXCLUDE REGEX 'LArray')
#list(REMOVE_ITEM SOURCES "LArray.c")
#list(REMOVE_ITEM HEADERS "LArray.h")
list(FILTER SOURCES EXCLUDE REGEX "LArray")
list(FILTER HEADERS EXCLUDE REGEX "LArray")
#list(FILTER HEADERS EXCLUDE REGEX "RomaVictor")
list(FILTER HEADERS EXCLUDE REGEX "greedyrapido.h")
list(FILTER HEADERS EXCLUDE REGEX "bitarray.h")
#Esto es para cuando no vamos a hacer tests
list(FILTER SOURCES EXCLUDE REGEX "bitarray.c")
list(FILTER SOURCES EXCLUDE REGEX "greedyrapido.c")
list(FILTER SOURCES EXCLUDE REGEX "tests")
list(FILTER SOURCES EXCLUDE REGEX "main-paralelo.c")
list(FILTER SOURCES EXCLUDE REGEX "main-rv.c")

##Esto se decide mas bien cuando el usuario corre cmake. Sino solamente estamos definiendo un valor por defecto
##Para saber el valor que uso el usuario, hay que leer la variable CMAKE_CURRENT_BINARY_DIR 
##set(CMAKE_BINARY_DIR "CMAKE_SOURCE_DIR/build")


#FLAGS
#if()
    
#endif()
#Para apagar GNU gcc C extensions: las extenciones que agrega gcc a C
#set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS ON)
#Para que cualquier compilador tenga como flag std=c11. Tambien se puede usar c99 o 14
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)


# add the executable
add_executable(TestDimacs ${SOURCES})

list(FILTER SOURCES EXCLUDE REGEX "main.c")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fopenmp -Ofast -flto -march=native -mtune=native")
set(LDFLAGS "${LDFLAGS} -fopenmp")
add_executable(TestDimacs-paralelo main-paralelo.c ${SOURCES})

# test Rv

# list(FILTER SOURCES EXCLUDE REGEX "main.c")
# list(FILTER SOURCES EXCLUDE REGEX "main-paralelo.c")
# list(FILTER SOURCES EXCLUDE REGEX "bitarray.c")
# list(FILTER SOURCES EXCLUDE REGEX "greedyrapido.c")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Ofast -flto -march=native -mtune=native")
# add_executable(TestDimacs-rv main-rv.c ${SOURCES})

# Agregar el nombre

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME_RELEASE "TestDimacs")
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME_DEBUG "TestDimacs")


#Configuracion para hacer tests con Ctest.
set(DIMACSTXT_DIR "/home/leo/Famaf/Discreta2/2021/proy/archivos_dimacs")
if(WIN32)
set(DIMACSTXT_DIR "C:\\Users\\LEO\\source\\repos\\TestDimacs\\TestDimacs")
endif()
enable_testing()
#include(Dart) es para que no salga un error mal configurado por ctest
include(Dart)
set (SOURCESTEST ${SOURCES})
list(FILTER SOURCESTEST EXCLUDE REGEX "main")
list(FILTER SOURCESTEST EXCLUDE REGEX "tests")
##list(APPEND SOURCESTEST "tests.c")
add_library(TestDimacs_lib OBJECT ${SOURCESTEST})
add_executable(tests tests.c $<TARGET_OBJECTS:TestDimacs_lib>)

get_target_property(EXECUTABLE_DEBUG_NAME TestDimacs OUTPUT_NAME_DEBUG) #no lo uso

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")


    add_test(NAME kmtxt CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         #COMMAND tests "< ${DIMACSTXT_DIR}/KM.txt")
         #COMMAND ${SHELLCOMMAND} "/C $<TARGET_FILE:tests> < ${DIMACSTXT_DIR}\\KM.txt")
         COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:tests> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:tests> < ${DIMACSTXT_DIR}\\KM.txt")
         #COMMAND ${SHELLCOMMAND} "-Command \{Start-Process $<TARGET_FILE:tests> -RedirectStandardInput ${DIMACSTXT_DIR}\\KM.txt -NoNewWindow -Wait\}")

    set_tests_properties(kmtxt PROPERTIES
        PASS_REGULAR_EXPRESSION "NCromatico([ \t\r])?(\=|\:)?([ \t\r])?1000")

#Octo.txt
    add_test(NAME octotxt CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        #COMMAND ${SHELLCOMMAND} "-Command \{Start-Process $<TARGET_FILE:tests> -RedirectStandardInput ${DIMACSTXT_DIR}\\Octo.txt -NoNewWindow -Wait\}")
        #COMMAND ${SHELLCOMMAND} "/C $<TARGET_FILE:tests> < ${DIMACSTXT_DIR}\\Octo.txt")
        COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:tests> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:tests> < ${DIMACSTXT_DIR}\\Octo.txt")

    set_tests_properties(octotxt PROPERTIES
        PASS_REGULAR_EXPRESSION "NCromatico([ \t\r])?(\=|\:)?([ \t\r])?339")

#Esbipartito1
    add_test(NAME esbipartito1 CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        #COMMAND ${SHELLCOMMAND} "-Command \{Start-Process $<TARGET_FILE:tests> -RedirectStandardInput ${DIMACSTXT_DIR}\\Octo.txt -NoNewWindow -Wait\}")
        #COMMAND ${SHELLCOMMAND} "/C $<TARGET_FILE:tests> < ${DIMACSTXT_DIR}\\Octo.txt")
        COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:tests> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:tests> < ${DIMACSTXT_DIR}\\GRD22154753987.txt")

    set_tests_properties(esbipartito1 PROPERTIES
        PASS_REGULAR_EXPRESSION "EsBipartito:1")
#Esbipartito2
        add_test(NAME esbipartito2 CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        #COMMAND ${SHELLCOMMAND} "-Command \{Start-Process $<TARGET_FILE:tests> -RedirectStandardInput ${DIMACSTXT_DIR}\\Octo.txt -NoNewWindow -Wait\}")
        #COMMAND ${SHELLCOMMAND} "/C $<TARGET_FILE:tests> < ${DIMACSTXT_DIR}\\Octo.txt")
        COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:tests> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:tests> < ${DIMACSTXT_DIR}\\DG100.txt")

    set_tests_properties(esbipartito2 PROPERTIES
        PASS_REGULAR_EXPRESSION "EsBipartito:1")
#Esbipartitocoloreopropio1
add_test(NAME esbipartitocoloreopropio1 CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        #COMMAND ${SHELLCOMMAND} "-Command \{Start-Process $<TARGET_FILE:tests> -RedirectStandardInput ${DIMACSTXT_DIR}\\Octo.txt -NoNewWindow -Wait\}")
        #COMMAND ${SHELLCOMMAND} "/C $<TARGET_FILE:tests> < ${DIMACSTXT_DIR}\\Octo.txt")
        COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:tests> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:tests> < ${DIMACSTXT_DIR}\\GRD22154753987.txt")

    set_tests_properties(esbipartitocoloreopropio1 PROPERTIES
        PASS_REGULAR_EXPRESSION "Bipartito-resultadoEsColoreoPropio:1")

#Esbipartitocoloreopropio2
add_test(NAME esbipartitocoloreopropio2 CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        #COMMAND ${SHELLCOMMAND} "-Command \{Start-Process $<TARGET_FILE:tests> -RedirectStandardInput ${DIMACSTXT_DIR}\\Octo.txt -NoNewWindow -Wait\}")
        #COMMAND ${SHELLCOMMAND} "/C $<TARGET_FILE:tests> < ${DIMACSTXT_DIR}\\Octo.txt")
        COMMAND ${CMAKE_COMMAND} -E chdir $<TARGET_FILE_DIR:tests> $ENV{ComSpec} /c "$<TARGET_FILE_NAME:tests> < ${DIMACSTXT_DIR}\\DG100.txt")

    set_tests_properties(esbipartitocoloreopropio2 PROPERTIES
        PASS_REGULAR_EXPRESSION "Bipartito-resultadoEsColoreoPropio:1")






else()
#ES LINUX

    set(SHELLCOMMAND "sh")

    add_test(NAME kmtxt CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         #COMMAND tests "< ${DIMACSTXT_DIR}/KM.txt")
         COMMAND "${SHELLCOMMAND}" -c "$<TARGET_FILE:tests> < ${DIMACSTXT_DIR}/KM.txt")
         #COMMAND "C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe"
         #Start-Process .\tests.exe -RedirectStandardInput "C:\Users\LEO\source\repos\TestDimacs\TestDimacs\KM.txt" -NoNewWindow -Wait
        

set_tests_properties(kmtxt PROPERTIES
        PASS_REGULAR_EXPRESSION "NCromatico([ \t\r])?(\=|\:)?([ \t\r])?1000")

#Octo.txt
add_test(NAME octotxt CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND "${SHELLCOMMAND}" -c "$<TARGET_FILE:tests> < ${DIMACSTXT_DIR}/Octo.txt")

set_tests_properties(octotxt PROPERTIES
        PASS_REGULAR_EXPRESSION "NCromatico([ \t\r])?(\=|\:)?([ \t\r])?339")

        #add_test(NAME test1 COMMAND sh -c "$<TARGET_FILE:tests> < ${DIMACSTXT_DIR}/KM.txt")

#Esbipartito1
add_test(NAME esbipartito1 CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND "${SHELLCOMMAND}" -c "$<TARGET_FILE:tests> < ${DIMACSTXT_DIR}/DG100.txt")

set_tests_properties(esbipartito1 PROPERTIES
        PASS_REGULAR_EXPRESSION "EsBipartito:1")

#Esbipartito2
add_test(NAME esbipartito2 CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND "${SHELLCOMMAND}" -c "$<TARGET_FILE:tests> < ${DIMACSTXT_DIR}/GRD22154753987.txt")

set_tests_properties(esbipartito2 PROPERTIES
        PASS_REGULAR_EXPRESSION "EsBipartito:1")

#Esbipartitocoloreopropio1
add_test(NAME esbipartitocoloreopropio1 CONFIGURATIONS Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMAND "${SHELLCOMMAND}" -c "$<TARGET_FILE:tests> < ${DIMACSTXT_DIR}/DG100.txt")

set_tests_properties(esbipartitocoloreopropio1 PROPERTIES
        PASS_REGULAR_EXPRESSION "Bipartito-resultadoEsColoreoPropio:1")

#Esbipartitocoloreopropio2
add_test(NAME esbipartitocoloreopropio2 CONFIGURATIONS Debug
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
 COMMAND "${SHELLCOMMAND}" -c "$<TARGET_FILE:tests> < ${DIMACSTXT_DIR}/GRD22154753987.txt")

set_tests_properties(esbipartitocoloreopropio2 PROPERTIES
PASS_REGULAR_EXPRESSION "Bipartito-resultadoEsColoreoPropio:1")


endif()







#!configuracion de ctest
