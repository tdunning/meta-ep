#include <stdlib.h>

#include "nrutil.h"
#include "powell.h"

#define TOL 2e-8

int ncom;
double *pcom, *xicom, (*nrfunc)(double*), *xt;;

double f1dim(double x)
{
    int j;
    double f;

    for (j=0;j<ncom;j++) {
	xt[j] = pcom[j] + x*xicom[j];
    }
    f = nrfunc(xt);
    return f;
}

void linmin(double *p, double *xi, int n, double *fret, double (*func)(double *))
{
    int j;

    double xx, xmin, fx, fb, fa, bx, ax;

    ncom = n;
    pcom = calloc(n, sizeof(double));
    xicom = calloc(n, sizeof(double));
    xt = calloc(ncom, sizeof(double));

    nrfunc = func;
    for (j=0;j<n;j++) {
	pcom[j] = p[j];
	xicom[j] = xi[j];
    }

    ax = 0;
    xx = 1;
    mnbrak(&ax, &xx, &bx, &fa, &fx, &fb, f1dim);
    *fret = brent(ax, xx, bx, f1dim, TOL, &xmin);
    for (j=0;j<n;j++) {
	xi[j] *= xmin;
	p[j] += xi[j];
    }

    free(xt);
    free(xicom);
    free(pcom);
}
