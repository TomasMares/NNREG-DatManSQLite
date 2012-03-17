#ifndef NEURONKA_H_
#define NEURONKA_H_
#include <math.h>
#include "DataManager.h"
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Neuronka
{
private:
	double ***NS, ***Gr;
	int *T;
	int nw;
	int v;
	int col;
	/*
	 * promìnná col specifikuje sloupec v poli outputs ( které je souèástí tøídy DataManager)
	 * na který se neuronka specializuje ( pøedpokládám, že nadále budeme používat neuronky
	 * s jedním výstupem.
	 */
    int realloc_NS ( unsigned int new_nodes ); // -> pomocná metoda pro add_neurons
    void clean();
	DatManSQLite *myData;
public:
	Neuronka(const vector<int> & conf, int col, DatManSQLite* data);
	Neuronka ();
	~Neuronka ();
	void Ninitialize(int L, int *Conf,unsigned int col = 0);
	void nnInitialize(const vector<int> &, int);
	void show ();
	int NoW(unsigned int NewN);
	int CoL();
	double NgW();
	int Config(int *conf);
	int Lays();
	vector<int> getConfig();
    double propagate(const double* in) const;
	double propagate(const vector<double> &input) const;
	double propagate(vector<double>::iterator it);
	//chybové funkce
	double get_error(dataset);
	double relative_error(dataset);
	int OHN();
	void BSort(int layer);
	void BSort();
	//double MaxE (dataset set);
	void set_weights (double S, unsigned int new_nodes);
	void reset_weights ( double *w, int new_nodes );
	bool reset_weights(vector<double>& weights, int newNodes);
	void copy_weights ( double *w );
	vector<double> getWeights();
	void print_weights ();
	void NguWid (int n);
	double NumG (double *g);
	double Grad(double *g);
	void load_weights (); // -> zatím neimplementováno, mìla by ukládat váhy do souboru
	bool add_neurons ( unsigned int n );
	void GControl();
	void controlInPrint(const string& file);
	void setData(DatManSQLite* data){myData = data;};
};
#endif /* NEURONKA_H_ */
