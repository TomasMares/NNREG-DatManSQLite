/*
 * DataManager.cpp
 *
 *  Created on: 17.12.2010
 *      Author: Tom
 */


#include "DataManager.h"

DatManSQLite::DatManSQLite(){
	standartized = false;
}
//chybi osetreni chyb !!!
DatManSQLite::DatManSQLite(const dBPattern & patt, const vector<Standartizor*>& stand)
{
	standartized = false;
	//try to open database
	sqlite3* handle;
	int check = sqlite3_open(patt.path.c_str(), &handle);
	if(check){
		cout << "couldn't open database" << endl;
	}
	sqlite3_stmt* myStmt;
	borderIndex = patt.inpCols.size();
	vector<string> cmdSelects = DatManSQLite::createSelect(patt);
	for(auto itSelects = cmdSelects.begin(); itSelects < cmdSelects.end(); ++itSelects){
		//prepare querry
		sqlite3_prepare(handle, (*itSelects).c_str(), (*itSelects).length(), &myStmt, NULL);
		int nCols = sqlite3_column_count(myStmt);
		check = sqlite3_step(myStmt);
		vector<vector<double>> tempSet;
		vector<double> tempRow (nCols);
		while(check == SQLITE_ROW){
			for(int col = 0; col < nCols; col++){
				tempRow[col] = sqlite3_column_double(myStmt, col);
				cout << tempRow[col] << " ";
			}
			cout << endl;
			tempSet.push_back(tempRow);
			check = sqlite3_step(myStmt);
		}
		data.push_back(tempSet);
		sqlite3_finalize(myStmt);
		cout << endl << "next to handle" << endl;
	}
	sqlite3_close(handle);
	//with a bit of luck done
	calculateStats();
	auto it = stand.begin();
	for(int i = 0; it < stand.end(); ++it, ++i){
		datStandart.push_back((*it)->newDeepCopy());
		if((*it)->getStatset() == false){
			(datStandart.back())->setSourceStat(datStats[i]);
		}
	}
}

DatManSQLite::DatManSQLite(const dBPattern & patt)
{
	standartized = false;
	//try to open database
	sqlite3* handle;
	int check = sqlite3_open(patt.path.c_str(), &handle);
	if(check){
		cout << "couldn't open database" << endl;
	}
	sqlite3_stmt* myStmt;
	borderIndex = patt.inpCols.size();
	vector<string> cmdSelects = DatManSQLite::createSelect(patt);
	for(auto itSelects = cmdSelects.begin(); itSelects < cmdSelects.end(); ++itSelects){
		//prepare querry
		sqlite3_prepare(handle, (*itSelects).c_str(), (*itSelects).length(), &myStmt, NULL);
		int nCols = sqlite3_column_count(myStmt);
		check = sqlite3_step(myStmt);
		vector<vector<double>> tempSet;
		vector<double> tempRow (nCols);
		while(check == SQLITE_ROW){
			for(int col = 0; col < nCols; col++){
				tempRow[col] = sqlite3_column_double(myStmt, col);
				cout << tempRow[col] << " ";
			}
			cout << endl;
			tempSet.push_back(tempRow);
			check = sqlite3_step(myStmt);
		}
		data.push_back(tempSet);
		sqlite3_finalize(myStmt);
		cout << endl << "next to handle" << endl;
	}
	sqlite3_close(handle);
	//with a bit of luck done - ADD ERROR HANDLING
	//initializing standartizors
	Statistics statInit(TARMIN, TARMAX, TARMEAN, TARDEV);
	calculateStats();
	Standartizor* linInit = new Linear(statInit);
	Standartizor* logInit = new Logaritmic(LBASE);
	vector<Standartizor*> compInit; compInit.push_back(logInit); compInit.push_back(linInit);
	int index = 0;
	for(auto itSt = datStats.begin(); itSt < datStats.end(); ++itSt){
		if((*itSt).getMin() > 0 && ((*itSt).getMax()/(*itSt).getMin()) > BDIFF){
			datStandart.push_back(new CompositeST(compInit));
		}
		else{
			datStandart.push_back(linInit->newDeepCopy());
		}
		datStandart.back()->setSourceStat(*itSt);
		++index;
	}
	delete linInit;
	delete logInit;
}

vector<string> DatManSQLite::createSelect(const dBPattern & info){
	vector<string> toProcess = info.inpCols;
	toProcess.insert(toProcess.end(), info.outCols.begin(), info.outCols.end());
	stringstream create(stringstream::in | stringstream::out);
	vector<string> commands;
	for(auto itRestrict = info.restrict.begin(); itRestrict < info.restrict.end(); ++itRestrict){
		create << "SELECT ";
		auto itCol = toProcess.begin();
		for(; itCol < (toProcess.end() - 1); ++itCol){
			create << (*itCol) << ", ";
		}
		create << (*itCol) << " ";
		create << "FROM " << info.table << " " << "WHERE ";
		create << *itRestrict;
		commands.push_back(create.str());
		create.str("");
	}
	return commands;
}

DatManSQLite::~DatManSQLite(){
	while(datStandart.size() > 0){
		delete datStandart.back();
		datStandart.pop_back();
	}
}

void DatManSQLite::printData(ofstream & out){
	for(auto itSet = data.begin(); itSet < data.end(); ++itSet){
		for(auto itRow = (*itSet).begin(); itRow < (*itSet).end(); ++itRow){
			for(auto itCol = (*itRow).begin(); itCol < (*itRow).end(); ++itCol){
				out << (*itCol) << " ";
			}
			out << endl;
		}
	}
	return;
}
void DatManSQLite::calculateStats(){
	int alloc = data[0][0].size();
	int i;
	vector<double> min(alloc), max(alloc), mean(alloc), dev(alloc);
	for(int j = 0; j < alloc; ++j){
		min[j] = data[0][0][j];
		max[j] = min[j];
		mean[j] = 0;
		dev[j] = 0;
	}
	int nSum = 0;
	for(auto itSet = data.begin(); itSet < data.end(); ++itSet){
		for(auto itRow = (*itSet).begin(); itRow < (*itSet).end(); ++itRow){
			int col = 0;
			for(auto itCol = (*itRow).begin(); itCol < (*itRow).end(); ++itCol, ++col){
				if((*itCol) < min[col]){
					min[col] = (*itCol);
				}
				else if((*itCol) > max[col]){
					max[col] = (*itCol);
				}
				mean[col] += (*itCol);
			}
			++nSum;
		}
	}
	for(i = 0; i < alloc; ++i){
		mean[i] = mean[i]/(double)nSum;
	}
	for(auto itSet = data.begin(); itSet < data.end(); ++itSet){
		for(auto itRow = (*itSet).begin(); itRow < (*itSet).end(); ++itRow){
			int col = 0;
			for(auto itCol = (*itRow).begin(); itCol < (*itRow).end(); ++itCol, ++col){
				dev[col] += ((*itCol) - mean[col])*((*itCol) - mean[col]);
			}
		}
	}
	Statistics toPrint;
	for(int i = 0; i < alloc; ++i){
		dev[i] = sqrt(dev[i]/(double)nSum);
		datStats.push_back(Statistics::createStatistics(min[i], max[i], mean[i], dev[i]));
		toPrint = datStats.back();
		toPrint.printStat();
	}
	return;
}

double DatManSQLite::Enorm(int col){
	int resCol = col%data[0][0].size();
	double upMin, upMax;
	Statistics stat(datStandart[resCol]->getSourceStat());
	if(resCol < datStandart.size() && standartized){
		upMin = datStandart[resCol]->standartize(stat.getMin());
		upMax = datStandart[resCol]->standartize(stat.getMax());
		return (upMax - upMin); 
	}
	else{
		upMin = stat.getMin();
		upMax = stat.getMax();
		return (upMax - upMin);
	}
}
/*
void DatManSQLite::CrossMove(){
	double **Ihelp, **Ohelp;
	int Shelp, i;
	Shelp = SetS[0];
	Ihelp = inputs[0];
	Ohelp = outputs[0];
	for(i = 0; i < (Nsets - 1); i++){
		inputs[i] = inputs[i + 1];
		outputs[i] = outputs[i + 1];
		SetS[i] = SetS[i + 1];
	}
	inputs[Nsets - 1] = Ihelp;
	outputs[Nsets - 1] = Ohelp;
	SetS[Nsets - 1] = Shelp;
}
int DatManSQLite::SumTS(char *set){
	if(*(set + 1) == 'r')
		return Nd - SetS[Nsets - 1];
	else if(*(set + 1) == 'e')
		return SetS[Nsets - 1];
	else
		return Nd;
}
Statistics DatManSQLite::getColumnStat(int index, char set){
	int inxToRet;
	if(set == 'i'){
		inxToRet = index%IDim;
		if(inxToRet < inpStandart.size() && inStandartized){
			double nmax, nmin;
			nmin = inpStandart[inxToRet]->standartize(inpStat[inxToRet].getMin());
			nmax = inpStandart[inxToRet]->standartize(inpStat[inxToRet].getMax());
			return Statistics::createStatistics(nmin, nmax, 0, 1);
		}
		else{
			return inpStat[inxToRet];
		}
	}
	else{
		inxToRet = index%ODim;
		if(inxToRet < outStandart.size() && outStandartized){
			double nmax, nmin;
			nmin = outStandart[inxToRet]->standartize(outStat[inxToRet].getMin());
			nmax = outStandart[inxToRet]->standartize(outStat[inxToRet].getMax());
			return Statistics::createStatistics(nmin, nmax, 0, 1);
		}
		else{
			return outStat[inxToRet];
		}
	}
}
vector<Statistics> DatManSQLite::getSetStat(char set){
	int dim;
	if(set == 'i'){
		dim = IDim;
	}
	else{
		dim = ODim;
	}
	vector<Statistics> statVec;
	for(int i = 0; i < dim; ++i){
		statVec.push_back(getColumnStat(i, set));
	}
	return statVec;
}*/
vector<Standartizor*> DatManSQLite::cloneAllStandartizors(){
	vector<Standartizor*> result;
	for(auto it = datStandart.begin(); it < datStandart.end(); ++it){
		result.push_back((*it)->newDeepCopy());
	}
	return result;
}
Standartizor* DatManSQLite::cloneStandartizor(int colIndex, bool isOutput){
	int inxToRet = colIndex;
	if(isOutput){
		inxToRet += borderIndex;
	}
	return datStandart[inxToRet]->newDeepCopy();
}
void DatManSQLite::applyStand(){
	if(standartized){
		return;
	}
	for(auto itSet = data.begin(); itSet < data.end(); ++itSet){
		for(auto itRow = (*itSet).begin(); itRow < (*itSet).end(); ++itRow){
			auto itSt = datStandart.begin();
			for(auto itCol = (*itRow).begin(); itCol < (*itRow).end() && itSt < datStandart.end(); ++itCol, ++itSt){
				(*itCol) = (*itSt)->standartize(*itCol); 
			}
		}
	}
	standartized = true;
	return;
}
void DatManSQLite::applyInvStand(){
	if(!standartized){
		return;
	}
	for(auto itSet = data.begin(); itSet < data.end(); ++itSet){
		for(auto itRow = (*itSet).begin(); itRow < (*itSet).end(); ++itRow){
			auto itSt = datStandart.begin();
			for(auto itCol = (*itRow).begin(); itCol < (*itRow).end() && itSt < datStandart.end(); ++itCol, ++itSt){
				(*itCol) = (*itSt)->standartizeInv(*itCol); 
			}
		}
	}
	standartized = false;
	return;
}
DatManIterator DatManSQLite::getIterator(dataset set, int colIndex){
	double enorm = Enorm(colIndex);
	if(set == DM_TRAIN){
		DatManIterator it(data.begin(), (data.end() - 1), borderIndex, colIndex + borderIndex, enorm);
		return it;
	}
	else if(set == DM_VALID){
		DatManIterator it((data.end() - 1), data.end(), borderIndex, colIndex + borderIndex, enorm);
		return it;
	}
	else{
		DatManIterator it(data.begin(), data.end(), borderIndex, colIndex + borderIndex, enorm);
		return it;
	}
}
template<typename T> T minimum(T op1, T op2){
	return (op1 < op2) ? op1 : op2; 
}
template<typename T> T maximum(T op1, T op2){
	return (op1 > op2) ? op1 : op2; 
}
