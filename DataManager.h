/*
 * DataManager.h
 *
 *  Created on: 17.12.2010
 *      Author: Tom
 */

#ifndef DATAMANAGER_H_
#define DATAMANAGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Statistics.h"
#include "Standartizor.h"
#include "DataManager.h"
#include "sqlite3.h"
using namespace std;

template<typename T> T minimum(T op1, T op2);
template<typename T> T maximum(T op1, T op2);

enum dataset{DM_ALL, DM_TRAIN, DM_VALID};

const double TARMAX = 1;
const double TARMIN = -1;
const double TARMEAN = 0;
const double TARDEV = 0.5;
const double BDIFF = 1000;
const double LBASE = 10;

typedef struct columnInfo{
	vector<string> inpCols;
	vector<string> outCols;
	vector<string> restrict;
	string table;
	string path;
} dBPattern;

typedef struct toLearn{
	double output;
	int bIndex;
	vector<double>::iterator inputs;
} learnPattern;

class DatManIterator;

class DatManSQLite{
private :
	vector<vector<vector<double>>> data;
	vector<Statistics> datStats;
	vector<Standartizor*> datStandart; 
	bool standartized;
	void CheckStat();
	DatManSQLite();
	DatManSQLite(const DatManSQLite & dat);
	void calculateStats();
	int borderIndex;
public :
	//DatManSQLite(const string& inpFile, const string& outFile, const vector<Standartizor*>& inpStand, const vector<Standartizor*>& outStand, int sets = 3);
	//DatManSQLite(const string& inpFile, const string& outFile, const DatManSQLite & statSource);
	DatManSQLite(const dBPattern & patt, const vector<Standartizor*>& stand);
	DatManSQLite(const dBPattern & patt);
	~DatManSQLite();
	static vector<string> createSelect(const dBPattern & info);
	void printData(ofstream & out);
	double Enorm(int col);
	void CrossMove();
	Statistics getColumnStat(int colIndex);
	Standartizor* cloneStandartizor(int colIndex,bool isOutput);
	vector<Standartizor*> cloneAllStandartizors(); 
	void applyStand();
	void applyInvStand();
	DatManIterator getIterator(dataset set, int colIndex);
};

class DatManIterator {
private :
	vector<vector<vector<double>>>::iterator startSetIt;
	vector<vector<vector<double>>>::iterator currentSetIt;
	vector<vector<vector<double>>>::iterator endSetIt;
	vector<vector<double>>::iterator currentRow;
	int index, colIndex;
	double eNorm, pattSum;
public :
	DatManIterator(vector<vector<vector<double>>>::iterator newStart,
				   vector<vector<vector<double>>>::iterator newEnd,
				   int newIndex, int col, double norm)
	{
		startSetIt = newStart;
		endSetIt = newEnd;
		index = newIndex;
		colIndex = col;
		int sum = 0;
		startIt();
		while(currentSetIt < endSetIt){
			sum += currentSetIt->size();
			++currentSetIt;
		}
		eNorm = norm;
		pattSum = (double)sum;
		startIt();
	};
	bool startIt(){
		currentSetIt = startSetIt;
		currentRow = (*currentSetIt).begin();
		return true;
	};
	bool moveToNextPattern(learnPattern & toWrite){
		if(currentSetIt == endSetIt){
			startIt();
			return false;
		}
		toWrite.output = *((currentRow->begin()) + colIndex);
		toWrite.bIndex = index;
		toWrite.inputs = currentRow->begin();
		if(currentRow < (currentSetIt->end() - 1)){
			++currentRow;
			return true;
		}
		else{
			if(++currentSetIt < endSetIt){
				currentRow = currentSetIt->begin();
			}
			return true;
		}
	};
	double getENorm(){
		return eNorm;
	}
	double getPattSum(){
		return pattSum;
	}
};
#endif /* DATAMANAGER_H_ */
