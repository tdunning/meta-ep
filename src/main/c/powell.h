void powell0(double *p, double **xi, int n, double ftol, int *iter,
	    double *fret, double (*func)(double*));

void powell(double *p, double **xi, int n, double ftol, int *iter,
	    double *fret, double (*func)(double*));

double brent(double ax, double bx, double cx, double (*f)(double), double tol, double *xmin);

void mnbrak(double *ax, double *bx, double *cx, double *fa, double *fb, double *fc, double (*func)(double));

void linmin(double p[], double xi[], int n,
	    double *fret, double (*func)(double[]));

