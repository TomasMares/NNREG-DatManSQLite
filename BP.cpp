#include <iostream>
#include <stdlib.h>
#include<time.h>
#include <math.h>
#include "BP.h"
#include "Neuronka.h"
#include "general.h"

BP::BP(int c, double *w ){
	int i;
	poc_vah = c;
	grad1 = new double[poc_vah];
	grad2 = new double[poc_vah];
	grad3 = new double[poc_vah];
	Zvahy = new double[poc_vah];
	vahy = new double [poc_vah];
	CG = new double [poc_vah];
	CG2 = new double [poc_vah];
	for( i = 0; i < poc_vah; i++ ){
		grad2[i] = 0;
		CG[i] = 0;
		vahy[i] = w[i];
	}
	Pa = 0.001;
}
BP::BP(const vector<double> & weights){
	poc_vah = weights.size();
	grad1 = new double[poc_vah];
	grad2 = new double[poc_vah];
	grad3 = new double[poc_vah];
	Zvahy = new double[poc_vah];
	vahy = new double [poc_vah];
	CG = new double [poc_vah];
	CG2 = new double [poc_vah];
	for(int i = 0; i < poc_vah; i++ ){
		grad2[i] = 0;
		CG[i] = 0;
		vahy[i] = weights[i];
	}
	Pa = 0.001;
}
BP::~BP(){
	delete [] grad1;
	delete [] grad2;
	delete [] grad3;
	delete [] CG;
	delete [] CG2;
	delete [] Zvahy;
	delete [] vahy;
}
double BP::velikost(double *v, int n){
	double vel = 0;
	for( int i = 0; i < n; i++){
		vel += (*(v+i))*(*(v+i));
	}
	return sqrt(vel);
}
double BP::zmen_vahy(int m){
	int i;
	double pE, aE, a1, Norm;
	aE = nn->Grad(grad1);
	Norm = velikost(grad1, poc_vah);
	if(m%poc_vah == 0)
		CG_reset();
	a1 = aCG ();
	pE = nn->get_error(DM_TRAIN);
	//printf("\na0 = %g, pE = %g, aE = %g\n", a1, pE, aE);
	aE = line_search (a1, pE);
	for(i = 0; i < poc_vah; i++){
		vahy[i] -= Pa*CG[i];
		grad2[i] = grad1[i];
	}
	nn->reset_weights(vahy, 0);
	return aE;
}
double BP::max(double a, double b){
	if(a > b)
		return a;
	else
		return b;
}
double BP::min(double a, double b){
	if(a < b)
		return a;
	else
		return b;
}
double BP::odchylka(){
	int i;
	double SkaSou, vel1, vel2, odch;
	SkaSou = 0;
	vel1 = velikost(grad1, poc_vah);
	vel2 = velikost(grad2, poc_vah);
	if(vel1 == 0 || vel2 == 0){
		printf("1 - %g, 2 - %g\n", vel1, vel2);
		return 0;
	}
	for(i = 0; i < poc_vah; i++){
		SkaSou += grad1[i]*grad2[i];
	}
	odch = acos(SkaSou/(vel1*vel2));
	//printf("odch = %g\n", odch);
	return odch;
}
double BP::aCG(){
	double pom1 = 0;
	double pom3 = 0;
	double pom2 = 0;
	double Beta;
	for(int i = 0; i < poc_vah; i++){
		pom1 += grad1[i]*(grad1[i] - grad2[i]);
		pom3 += grad1[i]*(grad1[i]);
		pom2 += grad2[i]*grad2[i];
	}
	if(/*pom1 <= 0 ||*/ pom2 == 0){
		Beta = 0;
	}
	else{
		Beta = 1*pom3/pom2;
	}
	pom1 = 0;
	pom2 = 0;
	for(int i = 0; i < poc_vah; i++){
		CG2[i] = CG[i];
		CG[i] = grad1[i] + Beta*CG[i];
		pom1 += CG[i]*grad1[i];
		pom2 += CG2[i]*grad2[i];
	}
	if(pom2 == 0 || pom1 == 0){
		return 0.1;
	}
	else{
		if(Pa*pom2/pom1 > 0.1)
			return 0.1;
		else
			return Pa*pom2/pom1;
	}
}
void BP::CG_reset(){
	for(int i = 0; i < poc_vah; i++){
		CG[i] = 0;
	}
}
double BP::line_search(double a, double E3){
	double E1, E2, hW1, hW2, C2, cA, pA;
	int i, j;
	E2 = E3;
	nn->copy_weights(Zvahy);
	hW1 = 0;
	for(i = 0; i < poc_vah; i++){
		hW1 += -grad1[i]*CG[i];
	}
	hW2 = 0.1*fabs(hW1);
	hW1 = 0.0001*hW1;
	cA = a;
	pA = 0;
	for(i = 0; i < 10; i++){
		//printf("\ni = %d, cA = %g\n", i, cA);
		for(j = 0; j < poc_vah; j++){
			Zvahy[j] -= cA*CG[j];
		}
		nn->reset_weights(Zvahy, 0);
		E1 = nn->Grad(grad3);
		C2 = 0;
		for(j = 0; j < poc_vah; j++){
			C2 += -grad3[j]*CG[j];
		}
		if((E1 > E2) || (E1 > (E3 + cA*hW1))){
			E1 = Zoom(pA, cA, hW1, hW2, E3);
			//printf("\nexit1, E1 = %g, W1 = %g, E3 = %g, E2 = %g\n", E1, E3 + cA*hW1, E3, E2);
			nn->reset_weights(vahy, 0);
			return E1;
		}
		else if(fabs(C2) <= hW2){
			Pa = cA;
			nn->reset_weights(vahy, 0);
			//printf("\nexit2, E1 = %g, W1 = %g, E3 = %g, E2 = %g\n", E1, E3 + cA*hW1, E3, E2);
			return E1;
		}
		else if(C2 > 0){
			E1 = Zoom(pA, cA, hW1, hW2, E3);
			nn->reset_weights(vahy, 0);
			//printf("\nexit3, E1 = %g, W1 = %g, E3 = %g, E2 = %g\n", E1, E3 + cA*hW1, E3, E2);
			return E1;
		}
		E2 = E1;
		pA = cA;
		cA += cA;
		nn->reset_weights(vahy, 0);
		nn->copy_weights(Zvahy);
		if(cA > poc_vah){
			Pa = pA;
			return E1;
		}
	}
	Pa = pA;
	return E1;
}
double BP::Zoom(double Alow, double Ahigh, double hW1, double hW2, double E3){
	double cA, E1, C2;
	double E2 = E3;
	int i, j;
	for(i = 0; i < 10; i++){
		nn->reset_weights(vahy, 0);
		nn->copy_weights(Zvahy);
		cA = (Alow + Ahigh)/2;
		for(j = 0; j < poc_vah; j++){
			Zvahy[j] -= cA*CG[j];
		}
		nn->reset_weights(Zvahy, 0);
		E1 = nn->Grad(grad3);
		//printf("\nj = %d, cA = %g, E1 = %g, E2 = %g, E3 = %g\n", i, cA, E1, E2, E3);
		C2 = 0;
		for(j = 0; j < poc_vah; j++){
			C2 += -grad3[j]*CG[j];
		}
		if((E1 >= E2) || (E1 >= (E3 + cA*hW1))){
			Ahigh = cA;
			//printf("passes 1\n");
		}
		else if(fabs(C2) <= hW2){
			Pa = cA;
			//printf("passes 2\n");
			return E1;
		}
		/*else if((C2*(Ahigh - Alow)) >= 0){
			printf("passes 3\n");
			Ahigh = Alow;
		}*/
		else
			Alow = cA;
		E2 = E1;
	}
	Pa = cA;
	return E1;
}
