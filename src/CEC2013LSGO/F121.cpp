#include "F12.h"
#include <stdio.h>

F12::F12():Benchmarks(){
  Ovector = NULL;
  minX = -100;
  maxX = 100;
  ID = 12;
  anotherz = new double[dimension];
  s_size = dimension - 1;

  variablesForFunc = vector<vector<int>>(s_size);
  funcForVariables = vector<vector<int>>(dimension);
  variablesForVariables = vector<unordered_set<int>>(dimension);

  for (int i = 0; i < dimension; i++)
  {
	  if (i < dimension - 1)
	  {
		  funcForVariables[i].push_back(i);
		  variablesForVariables[i].insert(i + 1);
	  }

	  if (i > 0)
	  {
		  funcForVariables[i].push_back(i - 1);
		  variablesForVariables[i].insert(i - 1);
	  }
  }

  for (int i = 0; i < s_size; i++)
  {
	  variablesForFunc[i].push_back(i);
	  variablesForFunc[i].push_back(i + 1);

	  funcSizes.push_back(2);
  }
}

F12::~F12(){
  delete[] Ovector;
  delete[] anotherz;
}

double F12::compute(double*x){
  int i;
  double result=0.0;

  if(Ovector==NULL)
    {
      Ovector = readOvector();
    }

  for(i=0;i<dimension;i++)
    {
      anotherz[i]=x[i]-Ovector[i];
    }

  result=rosenbrock(anotherz, dimension);
  return(result);
}

double F12::compute(double* x, int s_idx) {
	int i;
	double result = 0.0;

	if (Ovector == NULL)
	{
		Ovector = readOvector();
	}

	for (i = 0; i < dimension; i++)
	{
		anotherz[i] = x[i] - Ovector[i];
	}

	result = rosenbrock_(anotherz, 2, s_idx);
	return(result);
}

