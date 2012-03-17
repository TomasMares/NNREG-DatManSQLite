#include <iostream>
#include "DataManager.h"
#include "Standartizor.h"
#include "Neuronka.h"
#include "NNtraining.h"
using namespace std;

//samp method
string method("SRCC");
//database columns
string testRow1("x1");
string testRow2("x2");
string testRow3("combLin");
string testRow4("sumQuad");
string testRow5("multPoly");
string testRow6("singLin");
string testRow7("rational");
string testRow8("multExp");
string testRow9("powComb");
string testRow10("combPoly");
string testRow11("log");
string testRow12("sumExp");
string testRow13("sumRatio");
string testRow14("singAtan");
string testRow15("combAtan");
string testRow16("floorOSum");
string testRow17("sumOFloor");
//tr & val restricts
string sqrestrict("samp = 'SRCC'");
string strict2("samp = 'RAND' AND (id BETWEEN 81 AND 90)");
//test restricts
string sqTestRestrict("samp = 'RAND' AND (id BETWEEN 91 AND 100)");

string database("samples.db");
string table("samples");

template<typename T> void releasePtrVec(vector<T*> & tRef){
	while(!(tRef.empty())){
		delete tRef.back();
		tRef.pop_back();
	}
}
int main(){
	vector<string> inp, outp, cmd;
	//input columns
	inp.push_back(testRow1);
	inp.push_back(testRow2);
	//output columns
	outp.push_back(testRow3);
	outp.push_back(testRow4);
	outp.push_back(testRow5);
	outp.push_back(testRow6);
	outp.push_back(testRow7);
	outp.push_back(testRow8);
	outp.push_back(testRow9);
	outp.push_back(testRow10);
	outp.push_back(testRow11);
	outp.push_back(testRow12);
	outp.push_back(testRow13);
	outp.push_back(testRow14);
	outp.push_back(testRow15);
	outp.push_back(testRow16);
	outp.push_back(testRow17);
	//restrictions
	cmd.push_back(sqrestrict);
	cmd.push_back(strict2);
	//composing database request
	dBPattern exper;
	exper.inpCols = inp;
	exper.outCols = outp;
	exper.path = database;
	exper.restrict = cmd;
	exper.table = table;

	//training
	DatManSQLite myDatMan(exper);
	myDatMan.applyStand();
	vector<int> conf;
	conf.push_back(inp.size()); conf.push_back(1); conf.push_back(1);
	vector<Neuronka*> myNNs;
	NNtraining nnTeacher;
	for(int i = 0; i < outp.size(); ++i){
		myNNs.push_back(new Neuronka(conf, i, &myDatMan));
		nnTeacher.SetNn(myNNs.back());
		nnTeacher.Tadapt(outp[i] + method, i);
	}

	//testing
	//create db request for testing data
	cmd.clear();
	cmd.push_back(sqTestRestrict);
	exper.restrict = cmd;
	//get standartization modes
	vector<Standartizor*> initStand = myDatMan.cloneAllStandartizors();
	DatManSQLite testData(exper, initStand);
	testData.applyStand();
	string resFile("testErrors");
	string postfix(".csv");
	string fileToPrint = resFile + method + postfix;
	ofstream out, outTwo;
	out.open(fileToPrint, ios_base::out);
	double testError;
	for(int i = 0; i < outp.size(); ++i){
		myNNs[i]->setData(&testData);
		testError = myNNs[i]->relative_error(DM_ALL);
		out << outp[i] << " " << testError << endl;
	}
	out.close();
	releasePtrVec(myNNs);
	releasePtrVec(initStand);
	return 0;
}