#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "Neuronka.h"
#include "general.h"

using namespace std;
Neuronka::Neuronka(){
	T = NULL;
	NS = NULL;
	nw = 0;
	v = 0;
	col = 0;
	myData = NULL;
}
Neuronka::Neuronka(const vector<int> & conf, int nCol, DatManSQLite* data){
	myData = data;
	T = NULL;
	NS = NULL;
	nw = 0;
	v = 0;
	col = nCol;
	nnInitialize(conf, col);
}
Neuronka::~Neuronka(){
		int i,n;
		if(NS != NULL){
			for(i=0;i<(v-1);i++){
				for(n=0;n<=(T[i]);n++){
					delete [] NS [i][n];
					delete [] Gr [i][n];
				}
				delete [] NS [i];
				delete [] Gr [i];
			}
			for(i=0;i<(T[(v-1)]);i++){
				delete [] NS [(v-1)][i];
				delete [] Gr [(v-1)][i];
			}
			delete [] NS[(v-1)];
			delete [] Gr[(v-1)];
			delete [] NS;
			delete [] Gr;
		}
		if(T != NULL)
			delete [] T;
}
void Neuronka::clean(void){
	int i,n;
	if(NS != NULL){
		for(i=0;i<(v-1);i++){
			for(n=0;n<=(T[i]);n++){
				delete [] NS [i][n];
				delete [] Gr [i][n];
			}
			delete [] NS [i];
			delete [] Gr [i];
		}
		for(i=0;i<(T[(v-1)]);i++){
			delete [] NS [(v-1)][i];
			delete [] Gr [(v-1)][i];
		}
		delete [] NS[(v-1)];
		delete [] Gr[(v-1)];
		delete [] NS;
		delete [] Gr;
	}
	if(T != NULL)
		delete [] T;
	T = NULL;
	NS = NULL;
}
void Neuronka::Ninitialize(int layers, int *Conf, unsigned int cl){
	clean();
	int i,k;
	v = layers;
	T = new int[v+1];
	for(i = 0; i < v; i++){
		T[i] = Conf[i];
	}
	T[v] = 0;
	nw = 0;
	for(k = 0; k < (v-1); k++){
		nw += (T[k]+1)*T[k+1];
	}
	NS = new double**[v];
	Gr = new double** [v];
	for(i = 0; i < (v-1); i++){
	    NS[i] = new double*[T[i]+1];
	    Gr[i] = new double* [T[i]+1];
	          for( k = 0; k <= (T[i]); k++){
	              NS[i][k] = new double[T[i+1]+1];
	              Gr[i][k] = new double [T[(i+1)]+1];
	          }
	}
	NS[(v-1)] = new double*[T[v-1]];
	Gr[(v-1)] = new double*[T[v-1]];
	for(i = 0; i < T[(v-1)]; i++){
		NS[(v-1)][i] = new double[1];
		Gr[v-1][i] = new double[1];
	}
	for(i = 0; i < (v-1); i++){
		NS[i][(T[i])][0] = -1;
	}
	col = cl;
}
void Neuronka::nnInitialize(const vector<int> & Conf, int cl){
	clean();
	int i,k;
	v = Conf.size();
	T = new int[v+1];
	for(i = 0; i < v; i++){
		T[i] = Conf[i];
	}
	T[v] = 0;
	nw = 0;
	for(k = 0; k < (v-1); k++){
		nw += (T[k]+1)*T[k+1];
	}
	NS = new double**[v];
	Gr = new double** [v];
	for(i = 0; i < (v-1); i++){
	    NS[i] = new double*[T[i]+1];
	    Gr[i] = new double* [T[i]+1];
	          for( k = 0; k <= (T[i]); k++){
	              NS[i][k] = new double[T[i+1]+1];
	              Gr[i][k] = new double [T[(i+1)]+1];
	          }
	}
	NS[(v-1)] = new double*[T[v-1]];
	Gr[(v-1)] = new double*[T[v-1]];
	for(i = 0; i < T[(v-1)]; i++){
		NS[(v-1)][i] = new double[1];
		Gr[v-1][i] = new double[1];
	}
	for(i = 0; i < (v-1); i++){
		NS[i][(T[i])][0] = -1;
	}
	col = cl;
}
double Neuronka::propagate( const double* p ) const{
	if(NS == NULL){
		return 0;
	}
	int i,k,j, control;
	double y;
	for(j = 0; j < T[0]; j++){
		NS[0][j][0] = *(p + j);
	}
    for(i = 1; i < v; i++){
        for(k = 0; k < (T[i]); k++){
        	y = 0;
        	for(j = 0; j <= (T[i-1]); j++){
                y += (NS[(i-1)][j][0]*NS[(i-1)][j][k+1]);
            	}
        	if(i < (v - 1)){
        		NS[i][k][0] = 1/(1+exp(-y));
        	}
        	else
        		NS[i][k][0] = y;
        }
    }
    return NS[(v - 1)][0][0];
}
double Neuronka::propagate(const vector<double> &input)const{
	if(NS == NULL || input.size() < T[0]){
		throw exception("cannot propagate this vector");
	}
	int i,k,j;
	double y;
	for(j = 0; j < T[0]; j++){
		NS[0][j][0] = input[j];
	}
    for(i = 1; i < v; i++){
        for(k = 0; k < (T[i]); k++){
        	y = 0;
        	for(j = 0; j <= (T[i-1]); j++){
                y += (NS[(i-1)][j][0]*NS[(i-1)][j][k+1]);
            	}
        	if(i < (v - 1)){
        		NS[i][k][0] = 1/(1+exp(-y));
        	}
        	else
        		NS[i][k][0] = y;
        }
    }
    return NS[(v - 1)][0][0];
}
double Neuronka::propagate(vector<double>::iterator it){
	if(NS == NULL){
		throw exception("cannot propagate this vector");
	}
	int i,k,j;
	double y;
	for(j = 0; j < T[0]; j++){
		NS[0][j][0] = *(it++);
	}
    for(i = 1; i < v; i++){
        for(k = 0; k < (T[i]); k++){
        	y = 0;
        	for(j = 0; j <= (T[i-1]); j++){
                y += (NS[(i-1)][j][0]*NS[(i-1)][j][k+1]);
            	}
        	if(i < (v - 1)){
        		NS[i][k][0] = 1/(1+exp(-y));
        	}
        	else
        		NS[i][k][0] = y;
        }
    }
    return NS[(v - 1)][0][0];
}
int Neuronka::NoW(unsigned int NewN){
	/*if(NewN != 0)
		return (T[0] + 1)*T[1];
	else*/
		return nw;
}
int Neuronka::CoL(){
	return col;
}
void Neuronka::show(){
	printf("\nnw = %d", nw);
	printf("\ncol = %d", col);
	return;
}
//metody pro práci s daty
//chybové funkce
double Neuronka::get_error (dataset set){
	if(NS == NULL){
		return - 1;
	}
	else if(myData == NULL){
		printf("no data set\n");
		return -1;
	}
	double av_error = 0;
	double currentOutput;
	DatManIterator myIt = myData->getIterator(set, col);
	learnPattern toLearn;
	while(myIt.moveToNextPattern(toLearn)){
		currentOutput = propagate(toLearn.inputs);
		av_error += (currentOutput - toLearn.output)*(currentOutput - toLearn.output);
	}
	return (av_error/myIt.getPattSum()/2);
}
double Neuronka::relative_error (dataset set){
	if(NS == NULL){
		return - 1;
	}
	else if(myData == NULL){
		printf("no data set\n");
		return -1;
	}
	double av_error = 0;
	double currentOutput;
	DatManIterator myIt = myData->getIterator(set, col);
	learnPattern toLearn;
	while(myIt.moveToNextPattern(toLearn)){
		currentOutput = propagate(toLearn.inputs);
		av_error += fabs(currentOutput - toLearn.output);
	}
	double check = myIt.getPattSum();
	check = myIt.getENorm();
	return (av_error/(myIt.getENorm()*myIt.getPattSum()));
}
/*
double Neuronka::MaxE (char *set){
}
*/
//metody pro práci s vahami
void Neuronka::set_weights (double S, unsigned int new_n){
	if(NS == NULL){
		return;
	}
	int k, j, i, w;
	w = 0;
	if( new_n == 0 || v > 3){
		for( k = 0; k < (v - 1); k++){
			for(j = 0; j <= T[k]; j++){
				for(i = 1; i <= T[k + 1]; i++){
					NS[k][j][i] = random_double(-S, S);
				}
			}
		}
	}
	else{
		for(j = 0; j <= T[0]; j++){
			for( i = (T[1] - new_n + 1); i <= T[1]; i++){
				NS [0][j][i] = random_double(-S, S);
				w++;
			}
		}
		for(j = (T[1] - new_n); j < T[1]; j++){
			for( i = 1; i <= T[2]; i++){
				NS [1][j][i] = random_double(-S, S);
				w++;
			}
		}
	}
}
void Neuronka::reset_weights( double *w, int new_n){
	if(NS == NULL){
		return;
	}
	int k,j,i;
	if(new_n == 0){
		for(k = 0; k < (v-1); k++){
			for(j = 0; j <= T[k]; j++){
				for(i = 1; i <= T[k+1]; i++){
						NS[k][j][i] = *w;
						w++;
				}
			}
		}
	}
	else{
		for(j = 0; j <= T[0]; j++){
			for(i = (T[1] - new_n + 1); i <= T[1]; i++){
				NS [0][j][i] = *w;
				w++;
			}
		}
		for(j = (T[1] - new_n); j < T[1]; j++){
			for(i = 1; i <= T[2]; i++){
				NS [1][j][i] = *w;
				w++;
			}
		}
	}
}
bool Neuronka::reset_weights(vector<double>& weights, int new_n){
	if(NS == NULL){
		return false;
	}
	int k,j,i;
	vector<double>::iterator w;
	w = weights.begin();
	if(new_n == 0){
		for(k = 0; k < (v-1); k++){
			for(j = 0; j <= T[k]; j++){
				for(i = 1; i <= T[k+1] && w < weights.end(); i++){
						NS[k][j][i] = *w;
						++w;
				}
			}
		}
	}
	else{
		for(j = 0; j <= T[0]; j++){
			for(i = (T[1] - new_n + 1); i <= T[1] && w < weights.end(); i++){
				NS [0][j][i] = *w;
				++w;
			}
		}
		for(j = (T[1] - new_n); j < T[1] && w < weights.end(); j++){
			for(i = 1; i <= T[2]; i++){
				NS [1][j][i] = *w;
				++w;
			}
		}
	}
	return true;
}
void Neuronka::copy_weights ( double *weights ){
	if(NS == NULL){
		return;
	}
	int k, j, i;
	for(k = 0; k < (v-1); k++){
		for(j = 0; j <= T[k]; j++){
			for(i = 1; i <= T[k+1]; i++){
				*weights = NS[k][j][i];
				weights++;
			}
		}
	}
	return;
}
vector<double> Neuronka::getWeights(){
	vector<double> weights(nw);
	if(NS == NULL){
		return weights;
	}
	vector<double>::iterator wIterator = weights.begin();
	int k, j, i;
	for(k = 0; k < (v-1); k++){
		for(j = 0; j <= T[k]; j++){
			for(i = 1; i <= T[k+1]; i++){
				*wIterator = (NS[k][j][i]);
				++wIterator;
			}
		}
	}
	return weights;
}
void Neuronka::print_weights (){
	if(NS == NULL){
		return;
	}
	int k, j, i;
	FILE *we;
	we = fopen("weights.dat", "a");
	for( k = 0; k < ( v-1 ); k++){
		for( i = 1; i <= (T[(k+1)]); i++){
			for( j = 0; j <= T[k]; j++){
				fprintf (we,"%g ", NS [k][j][i]);
			}
			fprintf(we, "\n");
		}
	}
	fprintf(we, "\n_________\n");
	return;
}
void Neuronka::NguWid(int new_n = 0){
	if(NS == NULL){
		return;
	}
	int i,j;
	double size;
	double b;
	b = pow((double) T[1], 1/(double) T[0]);
	//b *= random_double(0.5, 1.5);
	if(new_n == 0)
		new_n = T[1];
	for(i = (T[1] - new_n + 1); i <= T[1]; i++){
		size = 0;
		for(j = 0; j < T[0]; j++){
			size += pow(NS[0][j][i], 2);
		}
		size = sqrt(size);
		for(j = 0; j < T[0]; j++){
			NS[0][j][i] *= b/size;
		}
		NS[0][T[0]][i] = random_double(-b, b);//NS[0][T[0]][i]/size*b;
	}
	NS[1][T[1]][1] = 0;
	return;
}
//metody spojené s uèením
bool Neuronka::add_neurons(unsigned int new_n ){
	if(NS == NULL || v > 3){
		return false;
	}
	int i, j, k, n, nnw, cl;
	nnw = nw + new_n*(T[0] + 1 + T[2]);
	DatManIterator it = myData->getIterator(DM_TRAIN, col);
	if(nnw > it.getPattSum()){
		return false;
	}
	double *weights;
	weights = new double [nw];
	int *old_T, *iniT;
	old_T = new int [v];
	iniT = new int [v];
	for( i = 0; i < v; i++ ){
		old_T [i] = T [i];
		iniT[i] = T[i];
	}
	copy_weights( weights );
	cl = col;
	iniT[1] += new_n;
	Ninitialize(v, iniT, cl);
    n = 0;
    for( k = 0; k < (v - 1); k++ ){
    	for(j = 0; j < old_T [k]; j++){
    		for(i = 1; i <= old_T[k+1]; i++){
    			NS [k][j][i] = weights[n];
    			n++;
    		}
    	}
		for(i = 1; i <= old_T[k+1]; i++){
			NS [k][T[k]][i] = weights [n];
			n++;
		}
    }
    for ( k = 0; k < (v - 1); k++){
    	NS [k][(T[k])][0] = -1;
    }
    n = 0;
    n++;
    set_weights (1, new_n );
    delete [] weights;
    delete [] iniT;
    delete [] old_T;
    return true;
}
double Neuronka::NumG(double *grad){
	if(NS == NULL || myData == NULL){
		return -1;
	}
	double *weights;
	weights = new double [nw];
	double dw = 0.00000001;
	int i;
	double E2, E1;
	E1 = get_error(DM_TRAIN);
	copy_weights(weights);
	for(i = 0; i < nw; i++){
		weights[i] += dw;
		reset_weights(weights, 0);
		E2 = get_error(DM_TRAIN);
		grad[i] = (E2 - E1)/dw;
		weights[i] -= dw;
		reset_weights(weights, 0);
	}
	delete [] weights;
	return E1;
}
double Neuronka::Grad(double *g){
	if(NS == NULL || myData == NULL){
		return - 1;
	}
	int a, k, j, i, n, S;
	double prum_chyba = 0;
	for(j = 0; j < nw; j++){
		g[j] = 0;
	}
	for( k = 0; k < (v-1); k++){
		for( j = 0; j <= T[k]; j++ ){
			Gr [k][j][0] = 0;
		}
	}
	DatManIterator myDatIt = myData->getIterator(DM_TRAIN, col);
	learnPattern pattToLearn;
	double funcValue;
	while(myDatIt.moveToNextPattern(pattToLearn)){
		funcValue = propagate(pattToLearn.inputs);
		Gr[(v-1)][0][0] = (funcValue - pattToLearn.output);
		prum_chyba += Gr[(v-1)][0][0]*Gr[(v-1)][0][0]/2;
		for( k = (v-2); k > (v - 3); k-- ){
			for( j = 0; j <= (T[k]); j++ ){
				for( i = 1; i <= (T[k+1]); i++ ){
						Gr[k][j][0] += Gr[(k+1)][(i-1)][0]*NS[k][j][i];
						Gr[k][j][i] =  Gr[(k+1)][(i-1)][0]*NS[k][j][0];
				}
			}
		}
		for( k = (v-3); k >= 0; k-- ){
			for( j = 0; j <= (T[k]); j++ ){
				for( i = 1; i <= (T[k+1]); i++ ){
						Gr[k][j][0] += (Gr[(k+1)][(i-1)][0]*NS[k+1][i-1][0]*(1 - NS[k+1][i-1][0])*NS[k][j][i]);
						Gr[k][j][i] =  (Gr[(k+1)][(i-1)][0]*NS[k+1][i-1][0]*(1 - NS[k+1][i-1][0])*NS[k][j][0]);
				}
			}
		}
		for( k = (v-2); k >= 0; k--){
			for( j = 0; j<= (T[k]); j++){
				Gr [k][j][0] = 0;
			}
		}
		a=0;
		for( k = 0; k < (v-1); k++ ){
			for( j = 0; j <= (T[k]); j++ ){
				for( i = 1; i <= (T[k+1]); i++){
					g[a] += (Gr[k][j][i]/myDatIt.getPattSum());
					a++;
				}
			}
		}
	}
	return prum_chyba/myDatIt.getPattSum();
}
double Neuronka::NgW(){
	return T[1];
}
void Neuronka::BSort(int l){
	int i, IMax, I1, I2, j;
	double max;
	double *help;
	help = new double[(T[l] + 1 + T[l + 2])];
	I1 = 1;
	I2 = T[l + 1];
	while(I2 > I1){
		max = NS[l][T[l]][I1];
		IMax = I1;
		for(i = (I1 + 1); i <= I2; i++){
			if(NS[l][T[l]][i] > max){
				max = NS[l][T[l]][i];
				IMax = i;
			}
		}
		//cout << "IMax = " << IMax << " max = " << max << "\n";
		for(j = 0; j <= T[l]; j++){
			help[j] = NS[l][j][IMax];
			NS[l][j][IMax] = NS[l][j][I2];
			NS[l][j][I2] = help[j];
			//cout << "j = " << j << "\n";
		}
		for(i = 1; i <= T[l + 2]; i++){
			help[i - 1] = NS[l + 1][IMax - 1][i];
			NS[l + 1][IMax - 1][i] = NS[l + 1][I2 - 1][i];
			NS[l + 1][I2 - 1][i] = help[i - 1];
			//cout << "i = " << i << "\n";
		}
		I2--;
	}
	delete [] help;
	return;
}
void Neuronka::BSort(){
	int i;
	for(i = 0; i < (v - 1); i++){
		BSort(i);
	}
	return;
}
void Neuronka::GControl(){
	int i, help;
	help = (T[v - 2] + 1)*T[v - 1];
	double *NGrad, *AGrad;
	NGrad = new double[nw];
	AGrad = new double[nw];
	NumG(NGrad);
	Grad(AGrad);
	printf("\n\n");
	for(i = (nw - 1); i >= 0; i --){
		printf("%g ", NGrad[i]);
	}
	printf("\n");
	for(i = (nw - 1); i >= 0; i --){
		printf("%g ", AGrad[i]);
	}
	printf("\n");
	printf("\n");
	delete [] NGrad;
	delete [] AGrad;
}
int Neuronka::OHN(){
	return T[v - 2];
}
int Neuronka::Lays(){
	return v;
}
int Neuronka::Config(int *conf){
	for(int i = 0; i < v; i++){
		conf[i] = T[i];
	}
	return v;
}
vector<int> Neuronka::getConfig(){
	vector<int> config;
	for(int i = 0; i < v; i++){
		config.push_back(T[i]);
	}
	return config;
}

/*
void Neuronka::controlInPrint(const string& file){
	ofstream out;
	out.open(file, ios_base::out);
	vector<double> iRow;
	for(int set = 0; set < Data->Nsets; ++set){
		for(int row = 0; row < Data->SetS[set]; ++row){
			iRow = Data->inputLine(set, row);
			int col = 0;
			for(auto it = iRow.begin(); it < iRow.end(); ++it, ++col){
				if(selMask[col]){
					out << *it << " ";
				}
			}
			out << endl;
		}
	}
	out.close();
	return;
}
*/