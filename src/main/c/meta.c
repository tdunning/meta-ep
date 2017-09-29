#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

#include "getopt.h"
#include "fun.h"

typedef struct {
    int old_ep;
    int meta_mutate;
    int mutation_cooling;
    int recorded_step;
    int directional_mutation;
    int crossover;
    double mutation_rate;
    double decay;
} evo_options;

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

void mutate(state *x, evo_options opt)
{
    int i;
    double err;

    if (opt.meta_mutate) {
	/* the directional mutation rate gets changed */
	if (opt.directional_mutation) {
	    if (opt.recorded_step) {
		/* update our direction */
		err = 1+gauss();
		for (i=0;i<x->n;i++) {
		    x->m[i] = err * x->m[i] + x->ball * gauss();
		}

		/* then the non-directional mutation rate gets tweaked..
		   note that there is some coupling which
		   keeps the mutation cloud from getting too terribly skinny */
		x->ball = - 1.1 * (x->ball + mag(x->m, x->n)/10) * log(1-drand48());

		/* with recorded steps, we just take the current step */
		for (i=0;i<x->n;i++) {
		    x->v[i] += x->m[i];
		}
	    }
	    else {
		/* without recorded steps, we use the step to pick a random
		   mutation */
		err = 1+gauss();
		for (i=0;i<x->n;i++) {
		    x->v[i] += err * x->m[i] + x->ball * gauss();
		}

		/* update our direction */
		err = 1+gauss();
		for (i=0;i<x->n;i++) {
		    x->m[i] = err * x->m[i] + x->ball * gauss();
		}

		/* then the non-directional mutation rate gets tweaked..
		   note that there is some coupling which
		   keeps the mutation cloud from getting too terribly skinny */
		x->ball = - (x->ball + mag(x->m, x->n)/10) * log(1-drand48());
	    }
	}
	else {			/* non-directional mutation */
	    double dx, sum;

	    if (opt.meta_mutate == 1) {
		sum = 0;
		for (i=0;i<x->n;i++) {
		    dx = x->ball * gauss();
		    sum = sum + dx*dx;
		    x->v[i] += dx;
		}
		if (opt.recorded_step) {
		    x->ball = sqrt(sum/x->n);
		}
		else {
		    x->ball = - x->ball * exp(1-drand48());
		}
	    }
	    else {
		if (opt.recorded_step) {
		    /* with recorded step, we find a step to take and then
		       set the mutation vector using that step */
		    sum = 0;
		    for (i=0;i<x->n;i++) {
			dx = x->m[i] * gauss();
			sum = sum + dx*dx;
			x->m[i] = dx;
			x->v[i] += dx;
		    }
		    x->ball = sqrt(sum/x->n);
		}
		else {
		    /* without recorded step, we find a step to take and then
		       set the mutation vector by self-similar mutation */
		    sum = 0;
		    for (i=0;i<x->n;i++) {
			dx = x->m[i] * gauss();
			sum = sum + dx*dx;
			x->v[i] += dx;
			x->m[i] = - x->m[i] * exp(1-drand48());
		    }
		    x->ball = sqrt(sum/x->n);
		}
	    }
	}
    }
    else {			/* use only the global mutation rate */
	for (i=0;i<x->n;i++) {
	    x->v[i] += opt.mutation_rate * gauss();
	}
	x->ball = opt.mutation_rate;
    }
	
}

state *create_state(funtab_entry *f,
		    double xmin, double xmax, double ymin, double ymax,
		    int x_coord, int y_coord)
{
    state *r;
    int dim, i;

    r = malloc(sizeof(*r));
    dim = f->dim;
    r->n = dim;
    r->v = calloc(dim, sizeof(*r->v));
    r->m = calloc(dim, sizeof(*r->m));
    for (i=0;i<dim;i++) {
	if (i == x_coord) {
	    r->v[i] = uniform(xmin, xmax);
	}
	else if (i == y_coord) {
	    r->v[i] = uniform(ymin, ymax);
	}
	else {
	    r->v[i] = uniform(f->lower, f->upper);
	}
	r->m[i] = (xmax-xmin)/10;
    }
    /* the initial mutation rate is pretty low... meta mutation will increase
       it if that helps */
    r->ball = hypot(xmax-xmin, ymax-ymin)/10;
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

void crossover(state *r, state *x, state **others, int survivors)
{
    r->ball = x->ball;
    memcpy(r->v, x->v, x->n * sizeof(*r->v));
    memcpy(r->m, x->m, x->n * sizeof(*r->m));
    r->age = 0;

    /* first parent determines whether to mate */
    r->parents = -1.1 * r->parents * log(1-drand48());
    if (r->parents < -log(1-drand48())) {
	int i, n;
	double mix;
	n = floor(survivors * drand48());
	mix = drand48();
	for (i=0;i<r->n;i++) {
	    r->v[i] = mix * r->v[i] + (1-mix) * others[n]->v[i];
	    r->m[i] = mix * r->m[i] + (1-mix) * others[n]->m[i];
	}
    }
}

int compare_scores(const void *pa, const void *pb)
{
    state **a, **b;

    a = (state**) pa;
    b = (state**) pb;
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

double xmin=0, xmax=0, ymin=0, ymax=0;

int disp_x(double x)
{
    return floor(640 * (x-xmin)/(xmax-xmin));
}

int disp_y(double y)
{
    return floor(480 * (y-ymin)/(ymax-ymin));
}

char *option_string(evo_options opt)
{
    static char r[200];

    r[0] = 0;
    if (opt.old_ep) {
	strcat(r, "EP ");
    }
	
    if (opt.crossover) {
	strcat(r, "CROSSOVER ");
    }
    if (opt.meta_mutate) {
	strcat(r, "META ");
	if (opt.recorded_step) {
	    strcat(r, "RECORDED ");
	}
	if (opt.directional_mutation) {
	    strcat(r, "DIRECTIONAL ");
	}
    }
    else if (opt.mutation_cooling) {
	sprintf(r, "COOLED M=%.4f DECAY=%.3f", opt.mutation_rate, opt.decay);
    }
    else {
	strcat(r, "HUH?");
    }
    return r;
}

void permute(void *p, int n_elements, int size)
{
    void *t;
    int i, j;

    t = malloc(size);
    for (i=1;i<n_elements;i++) {
	j = floor((i+1)*drand48());
	if (i != j) {
	    memcpy(                t, (char *) p+i*size, size);
	    memcpy((char *) p+i*size, (char *) p+j*size, size);
	    memcpy((char *) p+j*size,          t,        size);
	}
    }
    free(t);
}

int main(int argc, char *argv[])
{
    extern void qsort();
    extern void srand48(long);
    extern long time(long);
    extern int getopt();
    extern char *optarg;
    int errors=0, unknown_option=0;
    char *message = NULL;

    funtab_entry *fun;
    int ch, graphics=0;
    double llx=0, lly=0, urx=0, ury=0;
    int x_coord=0, y_coord=1;

    static evo_options opt = {
	0,			/* old_ep */
	1,			/* meta_mutate */
	0,			/* mutation_cooling */
	0,			/* recorded_step */
	0,			/* directional_mutation */
	0,			/* crossover */
	0,			/* mutation_rate */
	0.5,			/* decay rate */
    };
    state **pop;
    int i, j, k, n;
    int calmings, evaluations;
    double parents;
    int max_generation=200;
    int population_size=200, survivors=20;
    int found_limits=0;

    fun = funtab;
    while ((ch = getopt(argc, argv, "C:p:e:G:c:mMrdgf:i:x:y:X:Y:")) != EOF) {
	switch (ch) {
	case 'C':
	    sscanf(optarg, "%lf", &parents);
	    opt.crossover = 1;
	    break;
	case 'p':
	    sscanf(optarg, "%d:%d", &survivors, &population_size);
	    break;
	case 'G':
	    sscanf(optarg, "%d", &max_generation);
	    break;
	case 'g':
	    graphics = 1;
	    break;
	case 'x':
	    if (sscanf(optarg, "%lf:%lf", &xmin, &xmax) != 2) {
		message = "can't parse x limits";
		errors++;
	    }
	    found_limits++;
	    break;
	case 'y':
	    if (sscanf(optarg, "%lf:%lf", &ymin, &ymax) != 2) {
		message = "can't parse y limits";
		errors++;
	    }
	    found_limits++;
	    break;
	case 'i':
	    if (sscanf(optarg, "%lf:%lf,%lf:%lf",
		       &llx, &lly, &urx, &ury) == 4) {
		double t;
		if (llx > urx) {
		    t = llx;
		    llx = urx;
		    urx = t;
		}
		if (lly > ury) {
		    t = lly;
		    lly = ury;
		    ury = t;
		}
	    }
	    else if (sscanf(optarg, "%lf:%lf", &llx, &lly) == 2) {
		urx = llx+0.001;
		ury = lly+0.001;
	    }
	    else {
		message = "can't parse initial point";
		errors++;
	    }
	    break;
	case 'X':
	    if (sscanf(optarg, "%d", &x_coord) != 1) {
		message = "can't understand x dimension number";
		errors++;
	    }
	    break;
	case 'Y':
	    if (sscanf(optarg, "%d", &y_coord) != 1) {
		message = "can't understand x dimension number";
		errors++;
	    }
	    break;
	    /* normal meta mutation */
	case 'e':
	    sscanf(optarg, "%d", &opt.old_ep);
	    if (opt.old_ep > population_size-1) {
		opt.old_ep = population_size-1;
	    }
	    break;
	case 'm':
	    opt.meta_mutate = 1;
	    opt.mutation_cooling = 0;
	    opt.recorded_step = 0;
	    break;
	    /* recorded step meta mutation */
	case 'M':
	    opt.meta_mutate = 2;
	    opt.mutation_cooling = 0;
	    opt.recorded_step = 0;
	    break;
	    /* recorded step meta mutation */
	case 'r':
	    opt.meta_mutate = 1;
	    opt.mutation_cooling = 0;
	    opt.recorded_step = 1;
	    break;
	    /* directional mutation */
	case 'd':
	    opt.directional_mutation = 1;
	    break;

	case 'c':
	    opt.meta_mutate = 0;
	    opt.mutation_cooling = 1;
	    opt.recorded_step = 0;
	    sscanf(optarg, "%lf", &opt.decay);
	    break;

	case 'f':
	    for (i=0;funtab[i].name;i++) {
		if (strcmp(funtab[i].name, optarg) == 0) break;
	    }
	    if (funtab[i].name) {
		fun = funtab+i;
		break;
	    }
	    message = "can't understand function name";
	    
	    /* fall through into error handler */
	case '?':
	    unknown_option = ch;
	    errors++;
	    break;
	}
    }
    if (!found_limits) {
	xmin = fun->lower;
	xmax = fun->upper;
	ymin = fun->lower;
	ymax = fun->upper;
    }

    if (llx == urx) {
	llx = xmin;
	urx = xmax;
    }
    if (lly == ury) {
	lly = ymin;
	ury = ymax;
    }

    if (errors) {
	if (unknown_option) {
	    fprintf(stderr, "unknown option: %c\n", unknown_option);
	}
	if (message) {
	    fprintf(stderr, "%s\n", message);
	}
	fprintf(stderr, "usage: meta [-a] [-x] [-f function]\n"
		"  -e means to use 1-1 EP\n"
		"  -C parents indicates the average number of parent to use\n"
		"  -c decay specifies a fixed exponential cooling schedule\n"
		"  -m indicates that meta-evolution should be used\n"
		"  -r indicates that recorded step meta-evolution is desired\n"
		"  -d indicates that directional mutation is desired\n"
		"  -x xlow:xhigh specifies a range for x\n"
		"  -y ylow:yhigh specifies a range for y\n"
		"  -X dim specifies which dimension to put on x axis\n"
		"  -Y dim specifies which dimension to put on y axis\n"
		"  -a means age the survivors\n"
		"  -f selects a function to optimize\n"
		"      f1-f5 are de-yong's functions\n"
		"      f6-f8 are bohachevsky's functions\n"
		"      sp is the fractal spiral\n");
	exit(1);
    }

    srand48(time(0));
    evaluations = 0;
    pop = calloc(population_size, sizeof(pop[0]));
    for (i=0;i<population_size;i++) {
	pop[i] = create_state(fun, llx, urx, lly, ury, x_coord, y_coord);
	pop[i]->score = (fun->f)(pop[i]);
	pop[i]->parents = parents;
	evaluations++;
    }
    permute(pop, population_size, sizeof(*pop));
    qsort(pop, population_size, sizeof(*pop), compare_scores);

    if (opt.mutation_cooling) {
	opt.mutation_rate = pop[0]->ball;
    }

    if (graphics) {
	printf("set g [gstate]\nscale %.3f %.3f\ntranslate %.3f %.3f\n"
	       "set h [gstate]\n",
	       100.0/(xmax-xmin), 100.0/(ymax-ymin), -xmin, -ymin);
    }

    calmings = 0;
    for (i=0;i<max_generation && pop[0]->score > fun->target;i++) {
	if (!graphics) {
	    printf("%d %.8g %.8g\n", i,
		   pop[0]->score, pop[0]->ball);
	    fflush(stdout);
	}
	else {
	    printf("clear\ngstate $g\ntranslate 0 90\nscale 0.03\n"
		   "text 15 0 0 {%s}\n"
		   "text 15 0 -100 TARGET=%.4G\ntext 15 0 -200 BEST=%.4G\n"
		   "text 15 0 -300 GENERATION=%d\n"
		   "text 15 0 -500 PARENTS=%.2f\n"
		   "gstate $h\n", option_string(opt),
		   fun->target, pop[0]->score, i, pop[0]->parents);
	    if (opt.old_ep && opt.old_ep < survivors) n = opt.old_ep;
	    else n = survivors;
	    if (n > 200) n = 200;
	    for (j=0;j<n;j+=20) {
		printf("dot 15 ");
		for (k=j;k<n && k<j+20;k++) {
		    printf("%.4f %.4f ",
			   pop[k]->v[x_coord], pop[k]->v[y_coord]);
		}
		printf("\n");
		fflush(stdout);
	    }
	    if (opt.old_ep && opt.old_ep < population_size) n = opt.old_ep;
	    else n = population_size;
	    if (n > 200) n = 200;
	    if (n > survivors+50) n = survivors+50;
	    for (j=survivors;j<n;j+=20) {
		printf("dot 15 ");
		for (k=j;k<n && k<j+20;k++) {
		    printf("%.4f %.4f ",
			   pop[k]->v[x_coord], pop[k]->v[y_coord]);
		}
		printf("\n");
		fflush(stdout);
	    }
	}

	if (opt.mutation_cooling) {
	    opt.mutation_rate *= opt.decay; 
	}

	if (opt.old_ep) {
	    for (j=0;j<opt.old_ep;j++) {
		duplicate(pop[opt.old_ep], pop[j]);
		mutate(pop[opt.old_ep], opt);
		pop[opt.old_ep]->score = (fun->f)(pop[opt.old_ep]);
		if (pop[j]->score > pop[opt.old_ep]->score) {
		    state *t;
		    t = pop[j];
		    pop[j] = pop[opt.old_ep];
		    pop[opt.old_ep] = t;
		}
	    }
	    permute(pop, opt.old_ep, sizeof(*pop));
	    qsort(pop, opt.old_ep, sizeof(*pop), compare_scores);
	}
	else {
	    for (j=survivors;j<population_size;j++) {
		if (opt.crossover) {
		    crossover(pop[j], pop[j%survivors], pop, survivors);
		}
		else {
		    duplicate(pop[j], pop[j%survivors]);
		}
	    }

	    /* note that the unmutated members are at the end to encourage
	       diversity in flat landscapes */
	    for (j=0;j<population_size-survivors;j++) {
		mutate(pop[j], opt);
	    }

	    /* we re-evaluate all members of the population to deal with
	       stochastic functions */
	    for (j=0;j<population_size;j++) {
		pop[j]->score = (fun->f)(pop[j]);
		evaluations++;
	    }
	    qsort(pop, population_size, sizeof(*pop), compare_scores);
	}
    }
    fprintf(stderr, "%d calmings\n%d evaluations\n%.12g is best point found\n",
	    calmings, evaluations, pop[0]->score);
    return 0;
}
