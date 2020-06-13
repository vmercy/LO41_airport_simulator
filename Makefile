FLAGS = -Wall
LINKFLAGS = -lpthread -lrt

COMPILO=gcc

OBJ = 	data.o 		\
		atis.o		\
		format.o	\
		read_airports.o \
		plane.o		\
		route.o	\
		signals.o \
		main.o

all : build run

run :
	./Simul_BSL

build: $(OBJ)
	$(COMPILO) -o Simul_BSL $(OBJ) $(LINKFLAGS)

test: clear_test
	$(COMPILO) -o test test.c $(FLAGS) $(LINKFLAGS)
	./test

interface: clear_interface
	$(COMPILO) -o Simul_BSL_interface interface.c $(FLAGS) $(LINKFLAGS)
	./Simul_BSL_interface

clear_interface:
	rm -f Simul_BSL_interface

clear_test:
	rm -f test

clean:
	rm -f *.o core

clear: clean
	rm -f Simul_BSL
	rm -f test

%.o : %.c
	$(COMPILO) -o $@ -c $< $(FLAGS)