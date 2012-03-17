#include "Standartizor.h"

Standartizor::Standartizor(const Statistics & init, const Statistics & to){
	fromStat = init;
	toStat = to;
	statset = true;
}
Standartizor::Standartizor(const Statistics & to){
	toStat = to;
	statset = false;
}
Standartizor::Standartizor(const Standartizor & s){
	fromStat = s.fromStat;
	toStat = s.toStat;
	statset = s.statset;
}
Standartizor::Standartizor(){
	statset = false;
}
Standartizor & Standartizor::operator= (const Standartizor & s){
	if(&s != this){
		this->fromStat = s.fromStat;
		this->toStat = s.toStat;
		statset = s.statset;
		return *this;
	}
	else{
		return *this;
	}
}
void Standartizor::setSourceStat(const Statistics & init){
	if(!statset){
		fromStat = init;
		statset = true;
	}
	else{
		throw exception ("statistics already set");
	}
}
void Standartizor::updateSourceStat(const Standartizor* sPtr){
	if(sPtr){
		double min, max, dev, mean;
		min = sPtr->standartize(sPtr->fromStat.getMin());
		max = sPtr->standartize(sPtr->fromStat.getMax());
		dev = fromStat.getStddev();
		mean = fromStat.getMean();
		fromStat = Statistics::createStatistics(min, max, mean, dev);
	}
	else{
		throw exception("updateSourceStat null pointer");
	}
	return;
}
Standartizor::~Standartizor(){}
bool Standartizor::getStatset(){
	return statset;
}
Linear::Linear(const Statistics & init, const Statistics & to) : Standartizor(init, to){
}
Linear::Linear(const Statistics & to) : Standartizor(to){
}
Linear::Linear(const Linear &l) : Standartizor(l){
}
Linear::~Linear(){};
Linear & Linear::operator= (const Linear & l){
	if(this != &l){
		Standartizor::operator=(l);
		return *this;
	}
	else{
		return *this;
	}
}
double Linear::standartize(double subj) const{
	if(statset){
		return (subj - fromStat.getMin())/(fromStat.getMax() - fromStat.getMin())*(toStat.getMax() - toStat.getMin()) + toStat.getMin();
	}
	else{
		return toStat.getMax() + 1;
	}
}
double Linear::standartizeInv(double subj) const{
	if(statset){
		return (subj - toStat.getMin())/(toStat.getMax() - toStat.getMin())*(fromStat.getMax() - fromStat.getMin()) + fromStat.getMin();
	}
	else{
		return toStat.getMax() + 1;
	}
}
Standartizor* Linear::newDeepCopy(){
	return new Linear(*this);
}
/*Deviation::Deviation(const Statistics & init, const Statistics & to) : Standartizor(init, to){
}
Deviation::Deviation(const Statistics & init) : Standartizor(init){
}
Deviation::Deviation(const Deviation & dev) : Standartizor(dev){
}
Deviation::~Deviation(){}
Deviation & Deviation::operator= (const Deviation & dev){
	if(this != &dev){
		Standartizor::operator=(dev);
		return *this;
	}
	else{
		return *this;
	}
}
double Deviation::standartize(double subj){
	if(statset){
		return (subj - fromStat.getMean())/fromStat.getStddev()*toStat.getStddev() + toStat.getMean();
	}
	else{
		throw exception("statistics not set");
	}
}
double Deviation::standartizeInv(double subj){
	if(statset){
		return (subj - toStat.getMean())/toStat.getStddev()*fromStat.getStddev() + fromStat.getMean();
	}
	else{
		throw exception("statistics not set");
	}
}
Standartizor* Deviation::newDeepCopy(){
	return new Deviation(*this);
}*/
Exponential::Exponential(){
	base = 1;
}
Exponential::Exponential(const Statistics & initFrom, double nbase){
	fromStat = initFrom;
	base = nbase;
	statset = true;
}
Exponential::Exponential(double nbase){
	base = nbase;
}
Exponential::Exponential(const Exponential & ex) : Standartizor(ex){
	base = ex.base;
}
Exponential::~Exponential(){}
Exponential & Exponential::operator= (const Exponential & ex){
	if(&ex != this){
		Standartizor::operator= (ex);
		base = ex.base;
		return *this;
	}
	else{
		return *this;
	}
}
double Exponential::standartize(double subj) const{
	return pow(base, subj);
}
double Exponential::standartizeInv(double subj) const{
	return log(subj)/log(base);
}
Standartizor* Exponential::newDeepCopy(){
	return new Exponential(*this);
}
Logaritmic::Logaritmic(){
	base = 1;
	statset = true;
}
Logaritmic::Logaritmic(const Statistics & initFrom, double nBase){
	base = nBase;
	statset = true;
	fromStat = initFrom;
}
Logaritmic::Logaritmic(double nbase){
	base = nbase;
}
Logaritmic::Logaritmic(const Logaritmic & ex) : Standartizor(ex){
	base = ex.base;
	statset = ex.statset;
}
Logaritmic::~Logaritmic(){}
Logaritmic & Logaritmic::operator= (const Logaritmic & ex){
	if(&ex != this){
		Standartizor::operator= (ex);
		base = ex.base;
		return *this;
	}
	else{
		return *this;
	}
}
double Logaritmic::standartize(double subj) const{
	if(subj > 0){
		return log(subj)/log(base);
	}
	else{
		throw exception("negative argument to logarithm");
	}
}
double Logaritmic::standartizeInv(double subj) const{
	return pow(base, subj);
}
Standartizor* Logaritmic::newDeepCopy(){
	return new Logaritmic(*this);
}
CompositeST::CompositeST(vector<Standartizor*> stVec){
	statset = true;
	Standartizor *temp;
	auto iter = stVec.begin();
	for(; iter < stVec.end(); ++iter){
		temp = (*iter)->newDeepCopy();
		Sprocedure.push_back(temp);
		if(temp->getStatset() == false){
			statset = false;
		}
	}
	if(statset){
		updateSourceStat(NULL);
	}
}
CompositeST::CompositeST(const CompositeST & comp) : Standartizor(comp){
	Standartizor *temp;
	auto it = comp.Sprocedure.begin();
	for(; it < comp.Sprocedure.end(); ++it){
		temp = (*it)->newDeepCopy();
		Sprocedure.push_back(temp);
	}
}
CompositeST::~CompositeST(){
	while(Sprocedure.size() > 0){
		delete Sprocedure.back();
		Sprocedure.pop_back();
	}
}
CompositeST & CompositeST::operator= (const CompositeST & comp){
	if(&comp != this){
		Standartizor::operator= (comp);
		while(Sprocedure.size() > 0){
			delete Sprocedure.back();
			Sprocedure.pop_back();
		}
		Standartizor *temp;
		auto it = comp.Sprocedure.begin();
		for(; it < comp.Sprocedure.end(); ++it){
			temp = (*it)->newDeepCopy();
			Sprocedure.push_back(temp);
		}
		return *this;
	}
	else{
		return *this;
	}
}
void CompositeST::setSourceStat(const Statistics & init){
	if(!statset){
		bool setCheck;
		auto it = Sprocedure.begin();
		for(; it < Sprocedure.end(); ++it){
			setCheck = (*it)->getStatset();
			if(!setCheck){
				(*it)->setSourceStat(init);
			}
		}
		statset = true;
		updateSourceStat(NULL);
	}
	else{
		return;
	}
}
void CompositeST::updateSourceStat(const Standartizor* sPtr){
	if(statset){
		auto it = Sprocedure.begin();
		while(++it != Sprocedure.end()){
 			(*it)->updateSourceStat(*(it - 1));
		}
	}
	return;
}
double CompositeST::standartize(double subj) const{
	if(statset){
		double temp = subj;
		auto it = Sprocedure.begin();
		for(; it < Sprocedure.end(); ++it){
			temp = (*it)->standartize(temp);
		}
		return temp;
	}
	else{
		throw exception("statistics not set");
	}
}
double CompositeST::standartizeInv(double subj) const{
	if(statset){
		double result = subj;
		auto it = Sprocedure.rbegin();
		for(; it < Sprocedure.rend(); ++it){
			result = (*it)->standartizeInv(result);
		}
		return result;
	}
	else{
		throw exception("statistics not set");
	}
}
Standartizor* CompositeST::newDeepCopy(){
	return new CompositeST(*this);
}
