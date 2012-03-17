#ifndef _STAT_H_
#define _STAT_H_
#include<vector>
#include<iostream>
using namespace std;

class Statistics{
private :
	double max, min, stddev, mean;
public :
	Statistics(){
		max = 2; min = 1; stddev = 0.5; mean = 1.5;
	};
	Statistics(double nmin, double nmax, double nmean, double nstddev){
		max = nmax; min = nmin; mean = nmean; stddev = nstddev;
	};
	Statistics(Statistics & s){
		max = s.getMax();
		min = s.getMin();
		stddev = s.getStddev();
		mean = s.getMean();
	};
	~Statistics(){};
	double getMax() const{
		return max;
	};
	static Statistics createStatistics(double nmin, double nmax, double nmean, double nstddev){
		Statistics nStat(nmin, nmax, nmean, nstddev);
		return nStat;
	};
	const Statistics & operator= (const Statistics & s){
		if(&s != this){
			min = s.getMin();
			max = s.getMax();
			stddev = s.getStddev();
			mean = s.getMean();
			return *this;
		}
		else{
			return *this;
		}
	};
	double getMin() const{
		return min;
	};
	double getStddev() const{
		return stddev;
	};
	double getMean() const{
		return mean;
	};
	void printStat(){
		cout << "max = " << max << endl;
		cout << "min = " << min << endl;
		cout << "mean = " << mean << endl;
		cout << "dev = " << stddev << endl;
	}
};

#endif