#CC=gcc
#CFLAGS=-g -std=c99 -Og -Wall -Wextra -fsanitize=address,undefined
#CFLAGSOPT=-std=c99 -O3 -march=native -flto -ffast-math
#DEPS = GrafoSt21.h vertice.h read_dimacs.h orden.h listaenlazada.h hashtable.h greedy.h
#OBJ = GrafoSt21.o vertice.o read_dimacs.o orden.o listaenlazada.o hashtable.o greedy.o main.o

#
# Compiler flags
#
CC     = gcc
#CC = clang
CFLAGS = -std=c99

#
# Project files
#
SRCS = GrafoSt21.c vertice.c read_dimacs.c orden.c listaenlazada.c hashtable.c hashtable_b.c greedy.c bipartito.c reorden.c main.c
OBJS = $(SRCS:.c=.o)
EXE  = TestDimacs

#
# Project files
#
SRCSTESTS = GrafoSt21.c vertice.c read_dimacs.c orden.c listaenlazada.c hashtable.c hashtable_b.c bitarray.c greedy.c greedyrapido.c bipartito.c reorden.c tests.c
OBJSTESTS = $(SRCSTESTS:.c=.o)
EXETESTS  = tests

#
# Tests build settings
#
DBGDIRTESTS = debug
DBGEXETESTS = $(DBGDIRTESTS)/$(EXETESTS)
DBGOBJSTESTS = $(addprefix $(DBGDIRTESTS)/, $(OBJSTESTS))
#DBGCFLAGS = -g -O2 -Wall -Wextra -march=native
DBGCFLAGSTESTS = -g -Og -Wall -Wextra -fsanitize=address,undefined,float-cast-overflow,bounds -ftrapv -march=native -Wunreachable-code -Wconversion -Wshadow
#DBGCFLAGS = -Wall -g3 -glldb -march=skylake -O0 -target x86_64-unknown-linux-gnu

#
# Debug build settings
#
DBGDIR = debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
#DBGCFLAGS = -g -O2 -Wall -Wextra -march=native
DBGCFLAGS = -g -Og -Wall -Wextra -fsanitize=address,undefined,float-cast-overflow,bounds -march=native -Wunreachable-code -Wconversion -Wshadow
#DBGCFLAGS = -Wall -g3 -glldb -march=skylake -O0 -target x86_64-unknown-linux-gnu



#
# Release build settings
#
RELDIR = release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
#-Wno-attributes
#
RELCFLAGS = -Wall -Wextra -Ofast -ffast-math -march=native -mtune=native -flto -DNDEBUG -fuse-linker-plugin --param=inline-min-speedup=1 -ftree-parallelize-loops=4
LDFLAGS = -fuse-linker-plugin -flto
#RELCFLAGS = -Wall -march=skylake -O2 -Ofast -target x86_64-unknown-linux-gnu -Rpass=loop-vectorize -ffast-math

#
# Testbitarray build settings
#


.PHONY: all clean debug prep release remake testbitarray tests

# Default build
all: prep debug

#
# Debug rules
#
debug: $(DBGEXE)

$(DBGEXE): $(DBGOBJS)
	$(CC) $(CFLAGS) $(DBGCFLAGS) -o $(DBGEXE) $^

$(DBGDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

#
# tests rules
#
tests: $(DBGEXETESTS)

$(DBGEXETESTS): $(DBGOBJSTESTS)
	$(CC) $(CFLAGS) $(DBGCFLAGSTESTS) -o $(DBGEXETESTS) $^

$(DBGDIRTESTS)/%.o: %.c
	$(CC) -c $(CFLAGS) $(DBGCFLAGSTESTS) -o $@ $<

#
# Release rules
#
release: $(RELEXE)

$(RELEXE): $(RELOBJS)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(RELEXE) $^

$(RELDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<


#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR) $(RELDIR) $(DBGDIRTESTS)


remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS) $(DBGEXETESTS) $(DBGOBJSTESTS)

cleanbitarray:
	rm -f $(BITARREXE) $(BITARROBJS)
