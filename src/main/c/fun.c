#include <math.h>

#include "fun.h"

double uniform(double a, double b)
{
    return a + drand48()*(b-a);
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

funtab_entry funtab[] = {
    {"valley", 2, valley, -10, 10, -1},
    {"sp", 2, spiral, -30,    30, 1e-6},
    {"f1", 3, fun_1,  -5.12,  5.12, -1},
    {"f1a",  5, fun_1a,  -5.12,  5.12, -1},
    {"f1b", 10, fun_1a,  -5.12,  5.12, -1},
    {"f1c", 20, fun_1a,  -5.12,  5.12, -1},
    {"f2", 2, fun_2,  -2.048, 2.048, 1e-6},
    {"f3", 5, fun_3,  -5.12,  5.12, -30+1e-6},
    {"f4", 30, fun_4, -1.28,  1.28, -100},
    {"f5", 2, fun_5, -65.536,65.536, -100},
    {"f6", 2, fun_6, -50, 50, -1},
    {"f7", 2, fun_7, -50, 50, 1e-6},
    {"f8", 2, fun_8, -50, 50, -0.3+1e-6},
    {"c1_5",  5, con_1, -50, 50, 1e-6},
    {"c1_10",10, con_1, -50, 50, 1e-6},
    {"c2_5",  5, con_2, -50, 50, 1e-4},
    {"c2_10",10, con_2, -50, 50, 1e-4},
    {"c3_5",  5, con_3, -50, 50, -9},
    {"c3_10",10, con_3, -50, 50, -9},
    {"c4_5",  5, con_4, -50, 50, -4.687}, 
    {"c4_10",10, con_4, -50, 50, -9.66},
    {"c5_5",  5, con_5,   0, 10, -1.4},
    {"c5_10",10, con_5,   0, 10, -1.4},

/*    "branin", 2, branin, -5, 15, 0.397887,*/
/*    "gp", 2, fgold, -2, 2, 3,*/
/*    "rastrigin", 2, rastrigin, -5.12, 5.12, 0,*/
/*    "camelback", 2, camelback, -3, 3, -1.0316,*/

    {NULL, 0, NULL, 0, 0, 0}
};

/* these next four functions are taken from:
   http://www.systemtechnik.tu-ilmenau.de/~pohlheim/GA_Toolbox/gaindex.html

   which seems to be a MATLAB toolbox for GAs.

   (thanks to mark davis for finding them)

   */

#define sq(x) ((x) * (x))

/* Branin

The Branin rcos function [Bra72] is a global optimization test function. The function has 3 global optima. 

function definition 
     fBran(x1,x2)=a·(x2-b·x1^2+c·x1-d)^2+e·(1-f)·cos(x1)+e; 
     a=1, b=5.1/(4·pi^2), c=5/pi, d=6, e=10, f=1/(8·pi); 
     -5<=x1<=10, 0<=x2<=15. 
global minimum 
     f(x1,x2)=0.397887; (x1,x2)=(-pi,12.275), (pi,2.275),(9.42478,2.475). 
     */

/*
double branin(state *s)
{
    double a=1, b=5.1/(4*M_PI*M_PI), c=5/M_PI, d=6, e=10, f=1/(8*M_PI); 
    return a*sq(x2-b·sq(x1)+c·x1-d)+e·(1-f)*cos(x1)+e; 
}
*/

/*
Goldstein-Price:

The Goldstein-Price function [GP71] is a global optimization test function. 

function definition 
     fGold(x1,x2)=[1+(x1+x2+1)^2·(19-14·x1+3·x1^2-14·x2+6·x1·x2+3·x2^2)]·[30+(2·x1-3·x2)^2·(18-32·x1+12·x1^2+48·x2-36·x1·x2+27·x2^2)];

     -2<=x(i)<=2, i=1:2. 
global minimum 
     f(x1,x2)=3; (x1,x2)=(0,-1). 
     */

/*
double gp(state *s)
{
    return (1+sq(x1+x2+1)*
	    (19-14*x1+3*sq(x1)-14*x2+6*x1*x2+3*sq(x2)))*
	(30+sq(2*x1-3*x2)*(18-32*x1+12*sq(x1)+48*x2-36*x1*x2+27*sq(x2)));
}
*/

/*
  Rastrigin:

Rastrigin's function is based on function 1 with the addition of
cosine modulation to produce many local minima. Thus, the test
function is highly multimodal.  However, the location of the minima
are regularly distributed.

function definition 
     f6(x)=10*n+sum(x(i)^2-10*cos(2*pi*x(i))), i=1:n;
     -5.12<=x(i)<=5.12. 

global minimum 
     f(x)=0; x(i)=0, i=1:n. 

     */

/*
double rastrigin(double *s)
{
    int i;
    return 10*n+sum(x(i)^2-10*cos(2*M_PI*x(i))), i=1:n;
}
*/

/*
  Camelback:

The 2-D Six-hump camel back function [DS78] is a global optimization
test function. Within the bounded region are six local minima, two of
them are global minima.

function definition 
     fSixh(x1,x2)=(4-2.1*x1^2+x1^4/3)*x1^2+x1*x2+(-4+4*x2^2)*x2^2; 
     -3<=x1<=3, -2<=x2<=2. 
global minimum 
     f(x1,x2)=-1.0316; (x1,x2)=(-0.0898,0.7126), (0.0898,-0.7126). 
     */
/*
double camelback(double *s)
{
    return (4-2.1*sq(x1)+sq(sq(x1))/3)*sq(x1)+x1*x2+(-4+4*sq(x2))*sq(x2);
}
*/

/* a long narrow quadratic value */
double valley(state *s)
{
    double x, y;
    if (fabs(s->v[0]) > 100 || fabs(s->v[1]) > 100) {
	return 200;
    }
    x = s->v[0]-1 + s->v[1]-2;
    y = 100*(-s->v[0]+1 + s->v[1]-2);
    return sq(x) + sq(y);
}

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
#if 0
    /* this has a slightly bowl shape with a constant depth value.  at the
       origin, the bowl steepens and there is a singularity at the center */
    f = 1-exp(-(sq(x*cos(r)+y*sin(r))));
    return f+r/10;
#endif
    /* this version is mostly flat, but the valley deepens toward the center */
    f = 1-exp(-(sq(x*cos(r)+y*sin(r))))/(1+x*x+y*y);
    return f;
}

double fun_1(state *s)
{
    double x, y, z;
    x = s->v[0];
    y = s->v[1];
    z = s->v[2];
    return sq(x) + sq(y) + sq(z);
}

double fun_1a(state *s)
{
    int i;
    double sum;

    sum = 0;
    for (i=0;i<s->n;i++) {
	sum += sq(s->v[i]);
    }
    return sum;
}

double fun_2(state *s)
{
    double x, y;
    x = s->v[0];
    y = s->v[1];
    return 100*sq(sq(x)-sq(y)) + sq(1-x);
}

double fun_3(state *s)
{
    int i;
    double sum, x;
    sum = 0;
    for (i=0;i<5;i++) {
	x = s->v[i];
	if (x < -5.12) return 1000;
	sum += floor(x);
    }
    return sum;
}

double fun_4(state *s)
{
    double sum;
    double x;
    int i;
    sum = gauss();
    for (i=0;i<30;i++) {
	x = s->v[i];
	x = sq(x);
	sum += i*sq(x);
    }
    return sum;
}

double a[25][2] = {
{-32, -32}, {-16, -32}, {0, -32}, {16, -32}, {32, -32}, 
{-32, -16}, {-16, -16}, {0, -16}, {16, -16}, {32, -16}, 
{-32, 0}, {-16, 0}, {0, 0}, {16, 0}, {32, 0}, 
{-32, 16}, {-16, 16}, {0, 16}, {16, 16}, {32, 16}, 
{-32, 32}, {-16, 32}, {0, 32}, {16, 32}, {32, 32},
};

double fun_5(state *s)
{
    int j;
    double x, y, sum;
    x = s->v[0];
    y = s->v[1];

    sum = 1/500.0;
    for (j=0;j<25;j++) {
	sum += 1/(j+1 + pow(x-a[j][0], 6) + pow(x-a[j][1], 6));
    }
    return 1/sum;
}

double fun_6(state *s)
{
    double x, y;
    x = s->v[0];
    y = s->v[1];
    /* the goofy form of the following is to avoid roundoff in constants */
    return (10*x*x + 20 * y*y - 3*cos(3*M_PI*x) - 4*cos(4*M_PI*y) + 7)/10;
}

double fun_7(state *s)
{
    double x, y;
    x = s->v[0];
    y = s->v[1];
    return sq(x) + 2 * sq(y) - 0.3*(cos(3*M_PI*x)*cos(4*M_PI*y)) + 0.3;
}

double fun_8(state *s)
{
    double x, y;
    x = s->v[0];
    y = s->v[1];
    return sq(x) + 2 * sq(y) - 0.3*(cos(3*M_PI*x) + cos(4*M_PI*y)) + 0.3;
}

/* ICEO contest functions.  the raw versions come first, then the
   versions compatible with the optimization functions */

/*
 * 1st ICEO test function
 *
 * Problem 1: The Sphere model
 *
 */

double f_sphere(double x[], int n)
{
    int i;
    double Sum;

    for (Sum=0.0,i=0;i<n;i++) {
	Sum += x[i]*x[i];	
    }

    return (Sum);
}

/*
 * 1st ICEO test function
 *
 * Problem 2: The Griewank's function
 *
 */

#define D	4000.0

double f_Griew(double x[], int n)
{
    int i;

    double Val1,    Val2,    Sum;

    for (Val1 = 0.0, Val2 = 1.0, i = 0; i < n; i++) {
	Val1 += x[i] * x[i];
	Val2 *= cos(x[i] / sqrt((double) (i + 1)) );
    }

    Sum = Val1 / D - Val2 + 1.0;
    return (Sum);
}

/*
 * 1st ICEO test function
 *
 * Problem 3: Shekel's Foxholes
 *
 */

#include "shekel.h"

double shekel(double *x, int n)
{
    int	i, j;
    double sp, h, result = 0.0;

    for (i = 0; i < 30; i++) {
	sp = 0.0;
	for (j = 0; j < n; j++) {
	    h   = x[j] - shekel_a[i][j];
	    sp += h * h;
	}
	result += 1.0 / (sp + shekel_c[i]);
    }
    return( - result);
}


/*
 * 1st ICEO test function
 *
 * Problem 4: The Michalewitz's function 
 *
 */

#define m  10.0

double Micha(x, n) /* Michalewitz */
double x[]; 
int n;
{ 
    double u;
    int i;

    u=0; 
    for (i=0;i<n;i++) {
	u = u + sin(x[i])
	    * pow(sin((i+1)*x[i]*x[i]/M_PI),2.0*m);
    }

    return(-u); 
}

#include "langerman.h"

/*
 * 1st ICEO test function
 *
 * Problem 5: The Langerman's function 
 *
 */

double min = 500;


double SqrDst(double x1[], double x2[], int n)
{
    double dist = 0.0, d;
    int i;

    dist = 0;

    for (i=0;i<n;i++) {
	d = x1[i] - x2[i];
	dist += d*d;
    }

    return (dist);
}


double langerman(double x[], int n)
{
    int i;

    double Sum, dist;
	
    Sum = 0;
    for (i=0;i<5;i++) {
	dist = SqrDst( x, langerman_a[i], n );
	Sum -= langerman_c[i] * (exp(-dist/M_PI) * cos( M_PI * dist ) );
    }

    return (Sum);
}

/* ICEO contest interface functions */
double con_1(state *s)
{
    return f_sphere(s->v, s->n);
}

double con_2(state *s)
{
    return f_Griew(s->v, s->n);
}
	
double con_3(state *s)
{
    return shekel(s->v, s->n);
}

double con_4(state *s)
{
    return Micha(s->v, s->n);
}

double con_5(state *s)
{
    return langerman(s->v, s->n);
}

   
