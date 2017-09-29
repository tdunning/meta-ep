#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "powell.h"
#include "fun.h"


state *create_state(int dim, double mut)
{
    state *r;

    r = malloc(sizeof(*r));
    r->n = dim;
    r->v = calloc(dim, sizeof(*r->v));
    r->m = calloc(dim, sizeof(*r->m));
    r->ball = mut;
    r->age = 0;
    r->score = 0;
    return r;
}

int compare_scores(state **a, state **b)
{
    if ((*a)->score > (*b)->score) {
	return 1;
    }
    else if ((*a)->score < (*b)->score) {
	return -1;
    }
    else return 0;
}

double limit(double low, double high, double x)
{
    if (x < low) x = low;
    if (x > high) x = high;
    return x;
}

double (*current_f)(state *);
state *start;

int evaluations;

double state_f(double *x)
{
    state sx;
    double r;
    int i;
    
    evaluations++;
    sx.n = start->n;
    sx.v = x;
    r = current_f(&sx);
    printf("%.5f ", r);
    for (i=0;i<sx.n;i++) {
	printf("%5.3f ", sx.v[i]);
    }
    printf("\n");
    return r;
}

#define sq(x) ((x) * (x))

void powell_opt(state *here, double (*f)(state *point))
{
    double **xi, *t;
    double v;
    int iter;

    int i, j;

    start = here;
    current_f = f;

    xi = calloc(here->n, sizeof(xi[0]));
    t = calloc(sq(here->n), sizeof(xi[0][0]));
    
    for (i=0;i<here->n;i++) {
	xi[i] = t + i*here->n;
	for (j=0;j<here->n;j++) {
	    xi[i][j] = 0;
	}
	xi[i][i] = 1;
    }
    
    evaluations = 0;
    powell0(here->v, xi, here->n, 1e-10, &iter, &v, state_f);

    printf("%.8f\n", v);
    printf("%d evaluations, %d iterations\n", evaluations, iter);
    free(xi);
    free(t);
}

int main(int argc, char *argv[])
{
    extern void srand48(long);
    extern double drand48();
    extern long time(long);
    extern int getopt();

    funtab_entry *fun;
    int i, ch;

    state *here;

    srand48(time(0));
    while ((ch = getopt(argc, argv, "f:")) != EOF) {
	switch (ch) {
	case 'f':
	    for (i=0;funtab[i].name;i++) {
		if (strcmp(funtab[i].name, optarg) == 0) break;
	    }
	    if (funtab[i].name) {
		fun = funtab+i;
		break;
	    }
	    fprintf(stderr, "can't understand function name %s\n", optarg);
	    
	    /* fall through into error handler */
	case '?':
	    fprintf(stderr, "usage: spiral [-a] [-x] [-S] [-V] [-G]\n"
		    "  -S means optimize SPIRAL function\n"
		    "  -V means optimize VALLEY function\n"
		    "  -G means optimize stretched gaussian\n");
	    exit(1);
	    break;
	}
    }

    here = create_state(fun->dim, 100.0);
    for (i=0;i<fun->dim;i++) {
	here->v[i] = fun->lower + drand48() * (fun->upper - fun->lower);
    }

    powell_opt(here, fun->f);

    return 0;
}
