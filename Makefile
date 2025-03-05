CC=gcc
CFLAGS=-Wall -Wextra -g
OFLAGS=-O3
MFLAGS=-lm -lgmp

all: sqrt sqrtf

sqrt: sqrt.c
	$(CC) $(CFLAGS) $(OFLAGS) $< ../ydata/ydata.c ../ynotif/ynotif.c -o sqrt $(MFLAGS)

sqrtf: sqrt.f90
	gfortran $< -o $@

clean:
	rm -Rf sqrt sqrtf
