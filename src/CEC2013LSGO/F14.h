#ifndef _F14_H
#define _F14_H

#include "Benchmarks.h"

class F14:public Benchmarks{
 public:
  F14(int overlap_size = 5);
  double compute(double* x) ;
  double compute(double* x, int s_idx);
  ~F14();
};

#endif

