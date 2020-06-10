FLAGS = -Wall
COMPILO=gcc

OBJ = 	data.o 		\
		atis.o		\
		format.o	\
		read_airports.o \
		plane.o		\
		route.o	\
		semaphores.o \
		signals.o \
		tower.o \
		main.o

all : build run

run :
	./Simul_BSL

build: $(OBJ)
	$(COMPILO) -o Simul_BSL $(OBJ) -lpthread -lrt

test:
	rm -f test
	$(COMPILO) -o test test.c $(FLAGS)
	./test

clean:
	rm -f *.o core

clear: clean
	rm -f Simul_BSL
	rm -f test

%.o : %.c
	$(COMPILO) -o $@ -c $< $(FLAGS)