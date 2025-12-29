#ifndef _F12_H
#define _F12_H

#include "Benchmarks.h"

class F12:public Benchmarks{
 protected:
 public:
  F12();
  double compute(double* x) ;
  double compute(double* x, int s_idx);
  ~F12();
};

#endif
