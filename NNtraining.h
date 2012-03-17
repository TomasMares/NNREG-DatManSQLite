/*
 * NNtraining.h
 *
 *  Created on: 28.12.2010
 *      Author: Tom
 */

#ifndef NNTRAINING_H_
#define NNTRAINING_H_
#include "Neuronka.h"
#include "BP.h"
#include<math.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "NNtraining.h"
#include "DataManager.h"
using namespace std;

class NNtraining{
private :
	int CGitL, Check, AdL;
	/*
	 * RPROPitL aj znaèí limity poètu iterací v tréninkových metodách
	 */
	double MinDec, adaptMinDec;
	Neuronka *Nn;
	string adaptResFile;
	void initAdResF(const string& init, int it);
	double TrRes(double E, int i);
	double AdRes(char *f, char *);
	double findBestExtendedW(int trials, int newNodes, vector<double>& bestW, double prevE = 100);
	bool AdRes(int, double, double);
public :
	NNtraining();
	~NNtraining();
	void SetNn(Neuronka *Nn);
	//SetNn nastavuje ukazatel na neuronku, která se má trénovat
	double CGtr(int Nstart);
	void Tadapt(const string& name, int it);
	//Tadapt - adaptace topologie
	//TrRes a AdRes tisknou do pøíslušných souborù výsledky tréninku vah resp. adaptace topologie
};
#endif /* NNTRAINING_H_ */
