#include <stdio.h>
#include <math.h>
#include <malloc.h>

typedef struct {
    int n;			/* number of direct parameters */
    int age;			/* number of generations that this one has lasted */
    double *v;			/* n values for the direct */
    double ball;		/* uncorrelated mutation rate */
    double *m;			/* correlated mutation direction */
    double parents;		/* average number of parents */
    double score;		/* fitness for sorting */
} state;

typedef double (*function)(state *s);

typedef struct {
    char *name;
    int dim;
    function f;
    double lower, upper;
    double target;
} funtab_entry;

extern funtab_entry funtab[];

double drand48();

/* return a normally distributed random variable with mean 0, sd = 1 */
double gauss(void);

double uniform(double a, double b);

/* a long narrow quadratic value */
double valley(state *s);

/* cool spiral function */
double spiral(state *s);

/* de Yong's functions */
double fun_1(state *s);
double fun_1a(state *s);
double fun_2(state *s);
double fun_3(state *s);
double fun_4(state *s);
double fun_5(state *s);

/* Bohachevky's gallery of rogues */
double fun_6(state *s);
double fun_7(state *s);
double fun_8(state *s);

/* GA contest functions */
double con_1(state *s);
double con_2(state *s);
double con_3(state *s);
double con_4(state *s);
double con_5(state *s);


