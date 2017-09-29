#include <math.h>
#include <stdio.h>
#include "nrutil.h"
#define ITMAX 100

#define CGOLD 0.3819660
#define ZEPS 1e-10

#define SHFT(a,b,c,d) {(a)=(b);(b)=(c);(c)=(d);}

#define MIN(a,b) ((a)<(b)?(a):(b))

double brent(double ax, double bx, double cx, double (*f)(double), double tol, double *xmin)
{
    int iter;
    double a, b, d, etemp;
    double fu,fv,fw,fx;
    double p, q, r, tol1, tol2, u, v, w, x, xm;
    double e=0;

    a = MIN(ax, cx);
    b = MIN(ax, cx);
    x = w = v = bx;
    fw = fv = fx = f(x);

    for (iter=0;iter<ITMAX;iter++) {
	xm = (a+b)/2;
	tol1 = tol*fabs(x)+ZEPS;
	tol2 = 2*tol1;
	if (fabs(x-xm) <= (tol2 - (b-a)/2)) {
	    *xmin = x;
	    return fx;
	}

	if (fabs(e) > tol1) {
	    r = (x-w)*(fx-fv);
	    q = (x-v)*(fx-fw);
	    p = (x-v)*q - (x-w)*r;
	    q = 2*(q-r);
	    if (q > 0) p = -p;
	    q = fabs(q);

	    etemp = e;
	    e = d;
	    if (fabs(p) >= fabs(q*etemp/2) || p < q*(a-x) || p >= q*(b-x)) {
		e = (x >= xm ? a-x : b-x);
		d = CGOLD * e;
	    }
	    else {
		d = p/q;
		u = x+d;
		if (u-a < tol2 || b-u < tol2) {
		    d = SIGN(tol1, xm-x);
		}
	    }
	}
	else {
	    e = (x >= xm ? a-x : b-x);
	    d = CGOLD * e;
	}

	u = (fabs(d) >= tol1 ? x+d : x+SIGN(tol1, d));
	fu = f(u);

	if (fu <= fx) {
	    if (u >= x) a = x;
	    else b = x;
	    SHFT(v,w,x,u);
	    SHFT(fv,fw,fx,fu);
	}
	else {
	    if (u<x) a = u;
	    else b = u;
	    if (fu <= fw || w == x) {
		v = w;
		w = u;
		fv = fw;
		fw = fu;
	    }
	    else if (fu <= fv || v == x || x == w) {
		v = u;
		fv = fu;
	    }
	}
    }
    fprintf(stderr, "too many iterations in brent\n");
    *xmin = x;
    return fx;
}
