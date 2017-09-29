/* clone of the drand48 procedures */

double drand48(void);
double erand48(unsigned short xsubi[3]);
long nrand48(unsigned short xsubi[]);
long lrand48(void);
long jrand48(unsigned short xsubi[3]);
long mrand48(void);
void srand48(long seedval);
unsigned short *seed48(unsigned short seed16v[3]);
void lcong48(unsigned short param[7]);

