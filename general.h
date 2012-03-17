#ifndef __GENERAL__
#define __GENERAL__

#define MinDouble -0.3274927429751972E+308
#define MaxDouble +0.3274927429751972E+308

#define sqr( x ) ((x)*(x))
#define _MIN_(x, y) (((x)<(y)) ? (x) : (y))

typedef double* p_double ;
typedef p_double* pp_double ;
typedef int* p_int ;
typedef char* p_char;

void initialize_randoms ( void ) ;
int random_int ( int olow , int ohigh ) ;
double random_double ( double olow , double ohigh ) ;
double Normal_random(double stddev, double mean);
double maximum(double a, double b);
double minimum(double a, double b);
double signum(double a);
void NormRV (double size, double *v, int dim);
void errmsg ( p_char omsg ) ;
void LoadM(double **A, int n);
void PrintM(double **A, int n);
int Msolve(double **A, double *x,int n);
void CQSort(int acordto, double **Array, int Lrow, int Hrow);
//CQSort - seøadí dvourozm. pole podle sloupce acordto - øazení je stabilní
void Assrank(int col, double **A, int n, int m);
//Assrank - neboli assign rank pøiøazuje seøazenému poli ranky - øeší duplicity
double Spearman(double *A, double *B, int n);
//Spearman - vypoète spearmanovu korelaci mezi vektorem A a B

#endif

/*
author: Anicka Kucerova, Czech Technical University in Prague
version 0.0.1
16 Mar 2009
*/

