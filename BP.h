#include "neuronka.h"
#include "DataManager.h"
#include <vector>
using namespace std;

#ifndef BP_H_
#define BP_H_
class BP
{
protected :
	double *grad1;
	double *grad2;
	double *grad3;
	double *vahy;
	double velP;
	double *Zvahy;
	double *CG;
	double *CG2;
	int poc_vah;
	double line_search(double a, double CGsiz);
	double OLS(double a, double E);
	double Zoom(double a1, double a2, double hW1, double hW2, double E3);
	double OZoom(double a1, double a2, double hW1, double hW2, double E3);
	double aCG();
public :
	double Pa;
	Neuronka *nn;
	BP(int c, double *w);
	BP(const vector<double> & weights);
	~BP();
	double velikost(double *g,int n);
	double zmen_vahy(int m);
	double ChangeOW(int m);
	double max(double a, double b);
	double min(double a, double b);
	double odchylka();
	void CG_reset();
};
#endif
