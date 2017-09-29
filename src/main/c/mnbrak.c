#include <math.h>
#include "nrutil.h"

#define GOLD 1.618034

#define GLIMIT 100
#define TINY 1e-20

#define SHFT(a,b,c,d) {(a)=(b);(b)=(c);(c)=(d);}

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb, double *fc, double (*func)(double))
{
    double ulim, u, r, q, fu, dum;

    *fa = func(*ax);
    *fb = func(*bx);

    if (*fb > *fa) {
	SHFT(dum, *ax, *bx, dum);
	SHFT(dum, *fb, *fa, dum);
    }
    *cx = (*bx) + GOLD*(*bx - *ax);
    *fc = func(*cx);

    while (*fb > *fc) {
	r = (*bx - *ax) * (*fb - *fc);
	q = (*bx - *cx) * (*fb - *fa);
	u = *bx - ((*bx - *cx)*q - (*bx - *ax)*r) /
	    (2 * SIGN(MAX(fabs(q-r), TINY), q-r));
	ulim = (*bx) + GLIMIT * (*cx - *bx);

	if ((*bx-u)*(u-*cx) > 0) {
	    fu = func(u);
	    if (fu < *fc) {
		*ax = *bx;
		*bx = u;
		*fa = *fb;
		*fb = fu;
		return;
	    }
	    else if (fu > *fb) {
		*cx = u;
		*fc = fu;
		return;
	    }

	    u = *cx + GOLD*(*cx - *bx);
	    fu = func(u);
	}
	else if ((*cx - u) * (u - ulim) > 0) {
	    fu = func(u);
	    if (fu < *fc) {
		SHFT(*bx, *cx, u, *cx + GOLD*(*cx - *bx));
		SHFT(*fb, *fc, fu, func(u));
	    }
	}
	else if ((u-ulim) * (ulim-*cx) >= 0) {
	    u = ulim;
	    fu = func(u);
	}
	else {
	    u = (*cx) + GOLD*(*cx - *bx);
	    fu = func(u);
	}
	SHFT(*ax, *bx, *cx, u);
	SHFT(*fa, *fb, *fc, fu);
    }
}
		     
	     
		    
