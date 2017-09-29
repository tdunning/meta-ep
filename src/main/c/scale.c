#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int i;
    double x, y, u, v, r;

    double xm, ym;
    double xscale, yscale, uscale, rscale;

    char line[4000];

    if (argc != 7) {
	fprintf(stderr, "usage: scale xoffset yoffset xscale yscale uscale rscale <data\n");
	exit(1);
    }
    i = 1;
    sscanf(argv[i++], "%lf", &xm);
    sscanf(argv[i++], "%lf", &ym);
    sscanf(argv[i++], "%lf", &xscale);
    sscanf(argv[i++], "%lf", &yscale);
    sscanf(argv[i++], "%lf", &uscale);
    sscanf(argv[i++], "%lf", &rscale);

    while (fgets(line, sizeof(line), stdin)) {
	double m, rx, ry;

	x = y = u = v = r = 0;
	sscanf(line, "%lf %lf %lf %lf %lf", &x, &y, &u, &v, &r);

	r *= rscale;
	m = hypot(u,v);

	if (m > 1e-8) {
	    rx = -r*v/m;
	    ry = r*u/m;
	}
	else {
	    rx = ry = 0;
	}
	printf("%.0f %.0f %.0f %.0f %.0f %.0f\n", 
	       (x-xm)*xscale, (y-ym)*yscale, u*uscale, v*uscale, rx, ry);
    }
    return 0;
}

