CC=gcc
LDLIBS = -lm
CFLAGS = -g -Wall

all: meta scale centroid spiral powell_opt plotter

meta: meta.o fun.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

contour: contour.o fun.o
	$(CC) $(CFLAGS) $^ -o $@ -lm

plotter: plotter.o
	$(CC) $(CFLAGS) $^ -o $@ -ltcl -lvga -lm

powell_opt.o brent.o linmin.o mnbrak.o powell.o : powell.h
powell_opt.o: fun.h
powell_opt : fun.o powell_opt.o brent.o linmin.o mnbrak.o powell.o 
