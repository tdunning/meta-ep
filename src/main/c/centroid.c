#include <stdio.h>

int main(int argc, char *argv[])
{
    int i, n;
    double x, y;

    double xm = 0;
    double ym = 0;
    double xscale, yscale;

    char line[3000];

    if (argc != 3) {
	fprintf(stderr, "usage: centroid xscale yscale  < data\n");
	exit(1);
    }
    i = 1;
    sscanf(argv[i++], "%lf", &xscale);
    sscanf(argv[i++], "%lf", &yscale);

    n = 0;
    while (n < 1000 &&
	   fgets(line, sizeof(line), stdin)) {
	sscanf(line, "%lf %lf", &x, &y);
	xm += x;
	ym += y;
	n++;
    }
    xm /= n;
    ym /= n;

    printf("%.5f %.5f\n", xm, ym);
    return 0;
}

