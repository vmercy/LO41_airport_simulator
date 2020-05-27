FLAGS = -Wall
COMPILO=gcc

OBJ = 	data.o 		\
		atis.o		\
		main.o

all : build run

run :
	./Simul_BSL

build: $(OBJ)
	$(COMPILO) -o Simul_BSL $(OBJ)

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