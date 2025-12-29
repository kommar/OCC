#ifndef _F13_H
#define _F13_H

#include "Benchmarks.h"

class F13:public Benchmarks{
protected:
public:
	F13(int overlap_size = 5);
	double compute(double* x) ;
	double compute(double* x, int s_idx);
	~F13();
};

#endif

