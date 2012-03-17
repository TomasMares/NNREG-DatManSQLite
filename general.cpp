#include "general.h"
#include "mtwister.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int rseed=-1;

void initialize_randoms ( void )
{
    init_genrand(rseed);
}

int random_int ( int olow , int ohigh )
{
    return genrand_int32()%( ohigh-olow+1 )+olow ;
}

double random_double ( double olow , double ohigh )
{
    return genrand_real1()*( ohigh-olow )+olow;
}
double maximum(double a, double b){
	if(a > b)
		return a;
	else
		return b;
}
double minimum(double a, double b){
	if(a < b)
		return a;
	else
		return b;
}
double signum(double a){
	if(a > 0)
		return 1;
	else if(a < 0)
		return -1;
	else
		return 0;
}
//pøevzato z http://en.literateprograms.org/Box-Muller_transform
double Normal_random(double stddev, double mean){
	double x, y, r, d;
	do{
		x = random_double(-1, 1);
		y = random_double(-1, 1);
		r = x*x + y*y;
	}while(r == 0 || r > 1);
	d = sqrt(-2*log(r)/r);
	return (x*d*stddev + mean);
}
void NormRV (double size, double *v, int dim){
	int i;
	double S = 0;
	for(i = 0; i < dim; i++){
		v[i] = random_double(-1, 1);
		S += v[i]*v[i];
	}
	if(S < 0.00000001)
		S = 1;
	else
		S = sqrt(S);
	for(i = 0; i < dim; i++){
		v[i] = v[i]/S*size;
	}
	return;
}
int Msolve(double **A, double *x, int n){//A - matice soustavy s pravou stranou n*(n + 1), x øešeni
	int i, j, k, max;
	double *temp, partS;
	for(i = 0; i < n; i++){
		max  = i;
		for(j = (i + 1); j < n; j++){
			if(fabs(A[j][i]) > fabs(A[max][i]))
				max = j;
		}
		temp = A[i];
		A[i] = A[max];
		A[max] = temp;
		if(A[i][i] == 0)
			return 0;
		for(j = (i + 1); j < n; j++){
			for(k = n; k >= i; k--){
				A[j][k] = A[j][k] - A[i][k]*A[j][i]/A[i][i];
			}
		}
	}
	x[n - 1] = A[n - 1][n]/A[n - 1][n - 1];
	for(j = (n - 2); j >= 0; j--){
		partS = 0;
		for(k = (j + 1); k < n; k++)
			partS += A[j][k]*x[k];
		x[j] = (A[j][n] - partS)/A[j][j];
	}
	return 1;
}
void LoadM(double **A, int n){
	int i, j;
	FILE *mat;
	mat = fopen(".\\src\\Matrix.dat", "r");
	if(mat == NULL){
		printf("unable to load matrix\n");
		return;
	}
	for(i = 0; i < n; i++){
		for(j = 0; j <= n; j++){
			fscanf(mat, "%lf", &(A[i][j]));
		}
	}
	return;
}
void PrintM(double **A, int n){
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 0; j <= n; j++){
			printf("%lf ",A[i][j]);
		}
		printf("\n");
	}
}
void CQSort(int acordto, double **Array, int Lrow, int Hrow){
	int m, i;
	double *temp;
	if(Lrow < (Hrow - 1)){
		m = Lrow;
		for(i = (Lrow + 1); i < Hrow; i++){
			if(Array[i][acordto] < Array[m][acordto]){
				m++;
				temp = Array[m];
				Array[m] = Array[i];
				Array[i] = temp;
				temp = Array[m];
				Array[m] = Array[m - 1];
				Array[m - 1] = temp;
			}
			else if((m + 1) < Hrow){
				if(Array[i][acordto] == Array[m + 1][acordto]){
					m++;
				}
			}
		}
		CQSort(acordto, Array, Lrow, m);
		CQSort(acordto, Array, (m + 1), Hrow);
	}
	return;
}
void Assrank(int col, double **A, int n, int m){
	if((col + 1) >= m)
		return;
	int j, i, p;
	double srank, rank;
	i = 0;
	p = 1;
	while(i < n){
		srank = 0;
		p = 0;
		for(j = i; (j < n) && (A[i][col] == A[j][col]); j++){
			p++;
			srank += (j + 1);
		}
		rank = (double)srank/p;
		for(j = i; (j < (i + p)) && (j < n); j++){
			A[j][col + 1] = rank;
		}
		i += p;
	}
	return;
}
double Spearman(double *a, double *b, int n){
	double **help, spcoef, mean, sda, sdb;
	int i, cols;
	cols = 4;
	help = new double* [n];
	for(i = 0; i < n; i++){
		help [i] = new double [cols];
		help[i][0] = a[i];
		help[i][2] = b[i];
	}
	CQSort(0, help, 0, n);
	Assrank(0, help, n, cols);
	CQSort(2, help, 0, n);
	Assrank(2, help, n, cols);
	mean = (double)(n + 1)/2;
	spcoef = sda = sdb = 0;
	for(i = 0; i < n; i++){
		sda += (help[i][1] - mean)*(help[i][1] - mean);
		sdb += (help[i][3] - mean)*(help[i][3] - mean);
		spcoef += (help[i][1] - mean)*(help[i][3] - mean);
	}
	if(sda == 0 || sdb == 0)
		return 0;
	sda = sqrt(sda/n);
	sdb = sqrt(sdb/n);
	for(i = 0; i < n; i++){
		delete [] help[i];
	}
	delete [] help;
	return spcoef/sda/sdb/n;
}
/*
author: Anicka Kucerova, Czech Technical University in Prague
version 0.0.1
16 Mar 2009
*/

