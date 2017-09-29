CC=gcc
LDLIBS = -lm
CFLAGS = -g -Wall

all: meta scale centroid spiral powell_opt 

meta: meta.o fun.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

contour: contour.o fun.o
	$(CC) $(CFLAGS) $^ -o $@ -lm


powell_opt.o brent.o linmin.o mnbrak.o powell.o : powell.h
powell_opt.o: fun.h
powell_opt : fun.o powell_opt.o brent.o linmin.o mnbrak.o powell.o 

clean:
	rm -f *.o meta scale centroid spiral powell_opt
