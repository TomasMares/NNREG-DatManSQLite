/*
 * NNtraining.cpp
 *
 *  Created on: 28.12.2010
 *      Author: Tom
 */
#include "NNtraining.h"

NNtraining::NNtraining(){
	CGitL = 5000;
	Check = 100;
	AdL = 3;
	MinDec = 0.99;
	adaptMinDec = 0.99;
	Nn = NULL;
}
NNtraining::~NNtraining(){
}
void NNtraining::SetNn(Neuronka *N){
	Nn = N;
}
double NNtraining::CGtr (int Nstart){
	if(Nn == NULL){
		return -1;
	}
	int n, nw;
	nw = Nn->NoW(0);
	vector<double> bestW(nw);
	vector<double> recW(nw);
	double E_tr1, avE1, avE2, tE1, tE2, mintE, trE;
	bestW = Nn->getWeights();
	recW = Nn->getWeights();
	E_tr1 = 10;
	avE1 = 0;
	avE2 = 100;
	BP bp1(recW);
	bp1.nn = Nn;
	mintE = tE2 = Nn->relative_error(DM_VALID);
	tE1 = 0;
	for(n = 0; n < CGitL && E_tr1 > 0; n++){
		E_tr1 = bp1.zmen_vahy(n);
		avE1 += E_tr1;
		if(n%10 == 0){
			trE = Nn->relative_error(DM_TRAIN);
			tE1 = Nn->relative_error(DM_VALID);
			if((avE1/avE2 > MinDec && n > 99) /*|| (tE1 > 2*mintE && n > 9)*/){
				Nn->reset_weights(bestW, 0);
				return mintE;
			}
			if(tE1 < mintE){
				mintE = tE1;
				bestW = Nn->getWeights();
			}
			avE2 = avE1;
			tE2 = tE1;
			tE1 = 0;
			avE1 = 0;
		}
	}
	Nn->reset_weights(bestW, 0);
	return mintE;
}
void NNtraining::Tadapt (const string& name, int it){
	if(Nn == NULL){
		return;
	}
	initAdResF(name, it);
	int stop1, nw, col;
	int step = 1;
	double bestRecVErr, besteverVErr, recTrErr;
	nw = Nn->NoW(0);
	col = Nn->CoL();
	vector<double> bestWRec(nw);
	vector<double> besteverW(nw);
	vector<int> nnConfig(3);
	nnConfig = Nn->getConfig();
	besteverVErr = 1000;
	findBestExtendedW(4, 0, bestWRec);
	int stop2 = 0;
	while(true){
		Nn->reset_weights (bestWRec, 0);
		CGtr(0);
		bestRecVErr = Nn->relative_error(DM_VALID);
		recTrErr = Nn->relative_error(DM_TRAIN);
		AdRes(nw, recTrErr, bestRecVErr);
		if(bestRecVErr > adaptMinDec*besteverVErr){
			stop2++;
		}
		cout << endl << stop2 << " " << besteverVErr << " " <<bestRecVErr;
		if(bestRecVErr < besteverVErr){
			nnConfig = Nn->getConfig();
			besteverW = Nn->getWeights();
			besteverVErr = bestRecVErr;
		}
		if(!Nn->add_neurons(step) || stop2 >= 3){
			break;
		}
		nw = Nn->NoW(0);
		bestRecVErr = findBestExtendedW(4, 0, bestWRec);
		findBestExtendedW(4, step, bestWRec, bestRecVErr);
	}
	Nn->nnInitialize(nnConfig, col);
	Nn->reset_weights(besteverW, 0);
	return;
}
double NNtraining::findBestExtendedW(int trials, int newNodes, vector<double>& bestW, double prevE){
	double recValErr;
	double minValErr = prevE;
	vector<double> weights;
	for(int i = 0; i < trials; ++i){
		Nn->set_weights(1, newNodes);
		//Nn->NguWid(newNodes);
		weights = Nn->getWeights();
		CGtr(0);
		recValErr = Nn->relative_error(DM_VALID);
		if (recValErr < minValErr){
			minValErr = recValErr;
			bestW = Nn->getWeights();
		}
		Nn->reset_weights(weights, 0);
	}
	return minValErr;
}
void NNtraining::initAdResF(const string& init, int it){
	stringstream tmp;
	int col = Nn->CoL();
	tmp << init << it;
	adaptResFile = tmp.str();
	adaptResFile += ".csv";
	ofstream out;
	out.open(adaptResFile, ios_base::out);
	out << "\"nw\"" << " \"trError\"" << " \"valError\"" << endl;
	out.close();
}
bool NNtraining::AdRes(int nw, double trErr, double valErr){
	if(Nn == NULL){
		return false;
	}
	ofstream results;
	results.open(adaptResFile, ios_base::app);
	results << nw << " " << trErr << " " << valErr << endl;
	results.close();
	return true;
}
double NNtraining::TrRes (double Etr, int i){
	if(Nn == NULL){
		return -1;
	}
	FILE *Tres;
	char Tfile [100];
	double Ete, ETmax, TTER;
	sprintf (Tfile, "results_%d.dat", i);
	Tres = fopen(Tfile, "w");
	//fprintf(Tres, "\nEtr \nEte \nEva \nETmax \nTTER \n");
	//Ete = sqrt(Nn->relative_error("test"));
	//TTER = Ete/sqrt(Etr);
	//ETmax = Nn->ODSpear("train");
	fprintf(Tres, " %g\n", Etr);
	fclose(Tres);
	return 0;
}
