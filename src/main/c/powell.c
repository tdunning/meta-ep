#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "powell.h"
#include "nrutil.h"

#define ITMAX 200

void powell(double *p, double **xi, int n, double ftol, int *iter,
	    double *fret, double (*func)(double*))
{
    int count;
    int i, j;
    double fp, fpt, *pt, *ptt, *xit;

    pt = calloc(n, sizeof(double));
    ptt = calloc(n, sizeof(double));
    xit = calloc(n, sizeof(double));

    fp = func(p);
    for (count=0;count<ITMAX;count++) {
	for (j=0;j<n;j++) pt[j] = p[j]; /* save initial point */
	fpt = fp;

	for (i=0;i<n;i++) {	/* loop over all directions in the set */
	    for (j=0;j<n;j++) {
		xit[j] = xi[j][i]; /* copy the direction */
	    }
	    printf("0 0 linmin %d\n", i);
	    linmin(p, xit, n, &fp, func);
	}

	/* minimize along the average of all previous directions */
	for (j=0;j<n;j++) {
	    xit[j] = p[j]-pt[j];
	}
	    printf("0 0 conjugate %.3e %.3e\n", xit[0], xit[1]);
	linmin(p, xit, n, &fp, func);

	if (2.0 * fabs(fp-fpt) < ftol*(fabs(fp)+fabs(fpt))) {
	    *iter = count;
	    *fret = fp;
	    free(xit);
	    free(ptt);
	    free(pt);
	    return;
	}
    }
    fprintf(stderr, "too many iterations\n");
    exit(1);
}


void powell0(double *p, double **xi, int n, double ftol, int *iter,
	    double *fret, double (*func)(double*))
{
    int i, ibig, j;
    double del, fp, fptt, t, *pt, *ptt, *xit;

    pt = calloc(n, sizeof(double));
    ptt = calloc(n, sizeof(double));
    xit = calloc(n, sizeof(double));
    *fret = func(p);
    for (j=0;j<n;j++) pt[j] = p[j]; /* save initial point */
    for (*iter=1;;(*iter)++) {
	fp = (*fret);
	ibig = 0;
	del = 0.0;		/* will be the biggest function decrease */

	for (i=0;i<n;i++) {	/* loop over all directions in the set */
	    for (j=0;j<n;j++) {
		xit[j] = xi[j][i]; /* copy the direction */
	    }

	    fptt = (*fret);
	    linmin(p, xit, n, fret, func);
	    if (fabs(fptt-(*fret)) > del) {
		del = fabs(fptt-(*fret));
		ibig = i;
	    }
	}

	if (2.0 * fabs(fp-(*fret)) < ftol*(fabs(fp)+fabs(*fret)) && *iter>3) {
	    free(xit);
	    free(ptt);
	    free(pt);
	    return;
	}

	if (*iter == ITMAX) {
	    fprintf(stderr, "Too many iterations\n");
	    exit(1);
	}
	for (j=0;j<n;j++) {	/* construct the extrapolated point */
	    ptt[j] = 2*p[j] - pt[j]; /* and the average direction */
	    xit[j] = p[j] - pt[j]; /* save old starting point */
	    pt[j] = p[j];
	}
	fptt = func(ptt);	/* value at extrapolated point */
	if (fptt < fp) {
	    t = 2*(fp - 2*(*fret) + fptt) * SQR(fp - (*fret) - del)
		- del*SQR(fp-fptt);
	    if (t < 0) {
		linmin(p, xit, n, fret, func);
		for (j=0;j<n;j++) {
		    xi[j][ibig] = xi[j][n-1];
		    xi[j][n-1] = xit[j];
		}
	    }
	}
    }
}
