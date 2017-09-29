/* draw a contour map of a function.
   
   the method used is to examine squares of the plot and to draw
   segments between the edges of the squares.
   
   for now, the segments are lines, but assuming that altitude varies
   bilinearly in each square could allow nicer curves to be drawn.  at
   the high end, using cubic splines to approximate the surface would
   allow lovely curves.
   
   */

#include <stdio.h>
#include <malloc.h>

#include "fun.h"

#define XSQUARES 60
#define YSQUARES 60

#define XMIN -20
#define XMAX  20

#define YMIN -20
#define YMAX  20

typedef struct {
    double x, y, z;
} point;

double xmin=XMIN, xmax=XMAX, ymin=YMIN, ymax=YMAX;
double zmin = 0, zmax = 0, zstep, zcount = 0;

int debug=0;

int findx(point *a, point *b, double z)
{
    double l;
    if (a->z > b->z) return findx(b, a, z);
    if (z <= a->z) {
	return (a->x - xmin)/(xmax-xmin) * 640;
    }
    if (z >= b->z) {
	return (b->x - xmin)/(xmax-xmin) * 640;
    }
    l = (z - a->z)/(b->z - a->z);
    return floor((a->x + l*(b->x - a->x) - xmin)/(xmax-xmin)*640);
}

int findy(point *a, point *b, double z)
{
    double l;
    if (a->z > b->z) return findy(b, a, z);
    if (z <= a->z) {
	return (a->y-ymin)/(ymax-ymin)*480;
    }
    if (z >= b->z) {
	return (b->y-ymin)/(ymax-ymin)*480;
    }
    l = (z - a->z)/(b->z - a->z);
    return floor((a->y + l*(b->y - a->y)-ymin)/(ymax-ymin)*480);
}

void step(point *p00, point *p01, point *p11, point *p10, double z)
{
    if (p00->z >= zmin && p00->z < zmax && p00->z < z && p01->z >= z) {
	if (debug) {
	    printf("%6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f %6.2f \n",
		   p00->x, p00->y, p01->x, p01->y,
		   p11->x, p11->y, p10->x, p10->y);
	    printf("%6d %6d %6d %6d %6d %6d %6d %6d \n",
		   findx(p00, p00, p00->z), findy(p00, p00, p00->z),
		   findx(p01, p00, p00->z), findy(p01, p00, p00->z),
		   findx(p11, p00, p00->z), findy(p11, p00, p00->z),
		   findx(p10, p00, p00->z), findy(p10, p00, p00->z));
	}
	if (p11->z <= z) {
	    printf("line 15 %d %d %d %d\n",
		   findx(p00, p01, z), findy(p00, p01, z),
		   findx(p01, p11, z), findy(p01, p11, z));
	}
	else if (p10->z <= z) {
	    printf("line 15 %d %d %d %d\n",
		   findx(p00, p01, z), findy(p00, p01, z),
		   findx(p11, p10, z), findy(p11, p10, z));
	}
	else {
	    printf("line 15 %d %d %d %d\n",
		   findx(p00, p01, z), findy(p00, p01, z),
		   findx(p10, p00, z), findy(p10, p00, z));
	}
    }
}

double round_down(double z, double step)
{
    return floor(z/step)*step;
}

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>=(b)?(a):(b))

#define min4(a,b,c,d) min(min(a,b),min(c,d))
#define max4(a,b,c,d) max(max(a,b),max(c,d))

int main(int argc, char *argv[])
{
    extern int getopt();
    extern char *optarg;
    funtab_entry *fun;
    int ch, errors=0;

    point p[XSQUARES+1][YSQUARES+1];
    state *s;

    int i, j, k;

    fun = funtab;
    ch = getopt(argc, argv, "f:x:y:z:l:");
    while ((ch) != EOF) {
	switch (ch) {
	case 'x':
	    if (sscanf(optarg, "%lf:%lf", &xmin, &xmax) != 2) {
		errors++;
	    }
	    break;
	case 'y':
	    if (sscanf(optarg, "%lf:%lf", &ymin, &ymax) != 2) {
		errors++;
	    }
	    break;
	case 'z':
	    if (sscanf(optarg, "%lf:%lf", &zmin, &zmax) != 2) {
		errors++;
	    }
	    break;
	case 'l':
	    if (sscanf(optarg, "%lf", &zcount) != 1) {
		errors++;
	    }
	    break;
	case 'f':
	    for (i=0;funtab[i].name;i++) {
		if (strcmp(funtab[i].name, optarg) == 0) {
		    if (debug) {
			printf("%s\n", funtab[i].name);
		    }

		    break;
		}
	    }
	    if (funtab[i].name) {
		fun = funtab+i;
		break;
	    }
	    errors++;
	    break;
	case '?':
	    errors++;
	    break;
	}
	ch = getopt(argc, argv, "f:x:y:z:l:");
    }

    if (errors) {
	fprintf(stderr, "usage: contour [-x min:max] [-y min:max] [-z min:max] [-f function]\n"
		"  -x xlow:xhigh specifies a range for x\n"
		"  -y ylow:yhigh specifies a range for y\n"
		"  -z zlow:zhigh specifies a range for z\n"
		"  -f selects a function to plot\n"
		"      f1-f5 are de-yong's functions\n"
		"      f6-f8 are bohachevsky's functions\n"
		"      sp is the fractal spiral\n");

	exit(1);
    }

    s = malloc(sizeof(*s));
    s->n = fun->dim;
    s->v = calloc(s->n, sizeof(s->v[0]));

    printf("#!./plotter\n\nset others $argc\n");

    for (i=0;i<XSQUARES+1;i++) {
	double x;
	x = xmin + ((double) i)*(xmax-xmin)/(XSQUARES+1);
	for (j=0;j<YSQUARES+1;j++) {
	    p[i][j].x = x;
	    p[i][j].y = ymin + ((double) j)*(ymax-ymin)/(YSQUARES+1);
	    s->v[0] = x; s->v[1] = p[i][j].y;
	    p[i][j].z = fun->f(s);
	}
    }

    if (debug) {
	for (i=0;i<XSQUARES+1;i++) {
	    for (j=0;j<YSQUARES+1;j++) {
		printf("%4.1f %4.1f", p[i][j].x, p[i][j].y);
		if (j%8 == 7) printf("\n");
		else printf(" ");
	    }
	    printf("\n");
	}
    }

    if (zmin >= zmax) {
	zmin = zmax = p[0][0].z;
	for (i=0;i<XSQUARES;i++) {
	    for (j=0;j<YSQUARES;j++) {
		if (p[i][j].z < zmin) zmin = p[i][j].z;
		if (p[i][j].z > zmax) zmax = p[i][j].z;
	    }
	}
    }
    if (zcount == 0) zcount = 50;
    zstep = (zmax-zmin)/zcount;

    for (i=0;i<XSQUARES;i++) {
	for (j=0;j<YSQUARES;j++) {
	    double z, z0, z1;
	    z0 = min4(p[i][j].z, p[i+1][j].z, p[i][j+1].z, p[i+1][j+1].z);
	    z1 = max4(p[i][j].z, p[i+1][j].z, p[i][j+1].z, p[i+1][j+1].z);
	    for (z=round_down(z0,zstep);z<z1; z+=zstep) {
		/*		debug = (i<3) && (j<3);*/
		if (z >= z0) {
		    step(&p[i][j], &p[i+1][j], &p[i+1][j+1], &p[i][j+1], z);
		    step(&p[i+1][j], &p[i+1][j+1], &p[i][j+1], &p[i][j], z);
		    step(&p[i+1][j+1], &p[i][j+1], &p[i][j], &p[i+1][j], z);
		    step(&p[i][j+1], &p[i][j], &p[i+1][j], &p[i+1][j+1], z);
		}
	    }
	}
    }
		
    fflush(stdout);
    getchar();
    return 0;
}
