#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    int n;			/* number of direct parameters */
    int age;			/* number of generations that this one has lasted */
    double *v;			/* n values for the direct */
    double ball;		/* uncorrelated mutation rate */
    double *m;			/* correlated mutation direction */
    double score;		/* fitness for sorting */
} state;

double drand48();

#define sq(x) ((x) * (x))

/* cool spiral function */
double spiral(state *s)
{
    double r, f, x, y;
    if (fabs(s->v[0]) > 1000 || fabs(s->v[1]) > 1000) {
	return 200;
    }

    x = s->v[0]-5;
    y = s->v[1]-3.1;
    r = log(x*x+y*y);
    f = 1-exp(-(sq(x*cos(r)+y*sin(r))));
    return f+r/10;
}

double gaussian(state *s, double stretch)
{
    double r, x, y;
    x = (s->v[0]-5) + (s->v[1]-3.1);
    y = (s->v[1]-3.1) - (s->v[0]-5);
    
    r = stretch*x*x+y*y;
    if (fabs(s->v[0]) > 1000 || fabs(s->v[1]) > 1000) {
	return -exp(-r/10) + 20;
    }
    else {
	return -exp(-r/10);
    }
}

double valley(state *s)
{
    double r, x, y;
    x = s->v[0]-5;
    y = s->v[1]-3.1;
    r = x*x+y*y;
    r = 2*exp(-r) + (x+y)*(x+y)/100;
    return (x-1)*(x-1) + r*r;
}

double gauss(void)
{
    int i;
    double x;

    x = 0;
    for (i=0;i<12;i++) {
	x += drand48();
    }
    return x-6;
}

/* pick a point which is uniformly distributed on a sphere */
double *sphere(int n)
{
    int i;
    double *r, sum;

    n += (n%2);			/* round up to an even dimension */
    r = calloc(n, sizeof(r[0]));
    
    sum = 0;
    for (i=0;i<n;i++) {
	r[i] = gauss();
	sum += r[i]*r[i];
    }

    sum = sqrt(sum);
    for (i=0;i<n;i++) {
	r[i] = r[i]/sum;
    }
    return r;
}

double mag(double *x, int n)
{
    int i;
    double sum;

    sum = 0;
    for (i=0;i<n;i++) {
	sum += x[i]*x[i];
    }
    return sqrt(sum);
}

void mutate(state *x)
{
    int i;
    double err;

    /* the directional mutation rate gets changed */
    for (i=0;i<x->n;i++) {
	x->m[i] += x->ball * gauss();
    }

    /* then the non-directional mutation rate gets tweaked..
       note that there is some coupling which
       keeps the mutation cloud from getting too terribly skinny */
    x->ball = - (x->ball + mag(x->m, x->n)/10) * log(1-drand48());

    
    /* mutate the direct variables */
    err = 1+gauss();
    for (i=0;i<x->n;i++) {
	x->v[i] += x->ball * gauss() + x->m[i] * err;
    }

}

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

void duplicate(state *r, state *x)
{
    r->ball = x->ball;
    memcpy(r->v, x->v, x->n * sizeof(*r->v));
    memcpy(r->m, x->m, x->n * sizeof(*r->m));
    r->age = 0;
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

enum {
    SPIRAL, VALLEY, STRETCHED
    };

double limit(double low, double high, double x)
{
    if (x < low) x = low;
    if (x > high) x = high;
    return x;
}

int main(int argc, char *argv[])
{
    extern void srand48(long);
    extern long time(long);
    extern int getopt();
    int mutate_all=0, fun_switch=SPIRAL, age_best=0;
    int ch;
    int oldx[400], oldy[400];
    double x, max;

    state *pop[200];
    int i, j, k;
    int calmings;

    ch = getopt(argc, argv, "SVGax");
    while ((ch) != EOF) {
	switch (ch) {
	case 'a':
	    mutate_all = 1;
	    break;
	case 'x':
	    age_best = 1;
	    break;
	case 'S':
	    fun_switch = SPIRAL;
	    break;
	case 'V':
	    fun_switch = VALLEY;
	    break;
	case 'G':
	    fun_switch = STRETCHED;
	    break;
	case '?':
	    fprintf(stderr, "usage: spiral [-a] [-x] [-S] [-V] [-G]\n"
		    "  -a means mutate entire population\n"
		    "  -x means age the survivors\n"
		    "  -S means optimize SPIRAL function\n"
		    "  -V means optimize VALLEY function\n"
		    "  -G means optimize stretched gaussian\n");
	    exit(1);
	    break;
	}
	ch = getopt(argc, argv, "SVGax"); 
    }
    printf("#!./plotter\n\nset others $argc\n");

    srand48(time(0));
    for (i=0;i<200;i++) {
	pop[i] = create_state(2, 100.0);
	if (fun_switch == SPIRAL) {
	    pop[i]->v[0] = 0;
	    pop[i]->v[1] = -23;
	    pop[i]->ball = 0.1;
	}
    }
    for (i=0;i<20;i++) {
	if (i) {
	    duplicate(pop[i], pop[0]);
	    mutate(pop[i]);
	}
	switch (fun_switch) {
	case SPIRAL:
	    pop[i]->score = spiral(pop[i]);
	    break;
	case VALLEY:
	    pop[i]->score = valley(pop[i]);
	    break;
	case STRETCHED:
	    pop[i]->score = gaussian(pop[i], 10.0);
	    break;
	}
    }

    calmings = 0;
    for (i=0;i<100;i++) {
	printf("dot 15 ");
	for (j=0;j<20;j++) {
	    oldx[j] = pop[j]->v[0]*10 + 320;
	    oldy[j] = pop[j]->v[1]*10 + 240;
	    printf("%d %d ", oldx[j], oldy[j]);
	}
	printf("\nif {$others} {\n");
	for (j=20;j<200;j+=20) {
	    printf("dot 10 ");
	    for (k=j;k<j+20;k++) {
		oldx[k] = pop[k]->v[0]*10 + 320;
		oldy[k] = pop[k]->v[1]*10 + 240;
		printf("%d %d ", oldx[k], oldy[k]);
	    }
	    printf("\n");
	}
	printf("}\n");

	
	/* spawn the top few */
	for (j=20;j<200;j++) {
	    duplicate(pop[j], pop[j%20]);
	}

	/* old members get calmer */
	if (age_best) {
	    for (j=0;j<20;j++) {
		if (pop[j]->age > 1) {
		    for (k=0;k<pop[j]->n;k++) {
			pop[j]->m[k] /= 2;
		    }
		    pop[j]->ball /= 2;
		    calmings++;
		}
		/* and older */
		pop[j]->age++;
	    }
	}

	for (j=0;j<200;j++) {
	    if (mutate_all || j >= 20) {
		mutate(pop[j]);
		switch (fun_switch) {
		case SPIRAL:
		    pop[j]->score = spiral(pop[j]);
		    break;
		case VALLEY:
		    pop[j]->score = valley(pop[j]);
		    break;
		case STRETCHED:
		    pop[j]->score = gaussian(pop[j], 10.0);
		    break;
		}
	    }
	}
	qsort(pop, 200, sizeof(*pop), compare_scores);

	if (isatty(1)) {
	    printf("generation %d:\n", i);
	    for (j=0;j<3;j++) {
		double r, x, y, u, v;
		x = pop[j]->v[0];
		y = pop[j]->v[1];

		u = pop[j]->m[0];
		v = pop[j]->m[1];
		r = pop[j]->ball;

		printf("%6.3f %6.3f %6.3f %6.3f %6.3f %6.6f %d\n",
		       x, y, u, v, r, pop[j]->score, pop[j]->age);
	    }
	    printf("\n");
	}
	else {
	    printf("\nif {$others} {\n");
	    for (j=20;j<200;j+=20) {
		printf("dot 0 ");
		for (k=j;k<j+20;k++) {
		    printf("%d %d ", oldx[k], oldy[k]);
		}
		printf("\n");
	    }
	    printf("}\n");
	}
	max = 0;
	for (j=0;j<20;j++) {
	    x = hypot(mag(pop[j]->v, pop[j]->n), pop[j]->ball);
	    if (max < x) max = x;
	}
	if (max < 0.1) break;
    }
    fprintf(stderr, "%d calmings\n", calmings);
    return 0;
}
