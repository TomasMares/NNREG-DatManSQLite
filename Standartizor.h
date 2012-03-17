#ifndef _STANDARTIZOR_
#define _STANDARTIZOR_
#include <vector>
#include <iostream>
#include "Statistics.h"
using namespace std;

class Standartizor{
protected:
	Statistics fromStat, toStat;
	bool statset;
	Standartizor();
public:
	Standartizor(const Statistics & init, const Statistics & to);
	Standartizor(const Statistics & to);
	Standartizor(const Standartizor & s);
	virtual ~Standartizor();
	Standartizor & operator= (const Standartizor & s);
	virtual void updateSourceStat(const Standartizor* sPtr);
	virtual void setSourceStat(const Statistics & init);
	virtual double standartize(double subj) const {return subj;};
	virtual double standartizeInv(double subj) const{return subj;};
	virtual bool getStatset();
	Statistics getSourceStat(){return fromStat;};
	virtual Standartizor* newDeepCopy(){return new Standartizor(*this);};
};
class Linear : public Standartizor{
private :
	Linear();
public :
	Linear(const Statistics & init, const Statistics & to);
	Linear(const Statistics & to);
	Linear(const Linear &l);
	~Linear();
	Linear & operator= (const Linear & l);
	double standartize(double subj) const;
	double standartizeInv(double subj) const;
	Standartizor* newDeepCopy();
};
/*class Deviation : public Standartizor{
private :
	Deviation();
public :
	Deviation(const Statistics & init, const Statistics & to);
	Deviation(const Statistics & init);
	Deviation(const Deviation & dev);
	~Deviation();
	Deviation & operator= (const Deviation & dev);
	double standartize(double subj);
	double standartizeInv(double subj);
	Standartizor* newDeepCopy();
};*/
class Exponential : public Standartizor{
private :
	double base;
	Exponential();
public :
	Exponential(const Statistics & init, double nbase);
	Exponential(double nbase);
	Exponential(const Exponential & ex);
	~Exponential();
	Exponential & operator= (const Exponential & ex);
	double standartize(double subj) const;
	double standartizeInv(double subj) const;
	Standartizor* newDeepCopy();
};
class Logaritmic : public Standartizor{
private :
	double base;
	Logaritmic();
public :
	Logaritmic(const Statistics & init, double nbase);
	Logaritmic(double nbase);
	Logaritmic(const Logaritmic & ex);
	~Logaritmic();
	Logaritmic & operator= (const Logaritmic & ex);
	double standartize(double subj) const;
	double standartizeInv(double subj) const;
	Standartizor* newDeepCopy();
};
class CompositeST : public Standartizor{
private:
	vector<Standartizor*> Sprocedure;
	CompositeST();
public:
	CompositeST(vector<Standartizor*> stVec);
	CompositeST(const CompositeST & comp);
	~CompositeST();
	CompositeST & operator= (const CompositeST & comp);
	void updateSourceStat(const Standartizor* sPtr);
	void setSourceStat(const Statistics & init);
	double standartize(double subj) const;
	double standartizeInv(double subj) const;
	Standartizor* newDeepCopy();
};
#endif