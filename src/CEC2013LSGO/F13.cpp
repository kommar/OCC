#include "F13.h"
#include <stdio.h>

F13::F13(int overlap_size):Benchmarks(){
  m_havenextGaussian=0;
  Ovector = NULL;
  minX = -100;
  maxX = 100;
  ID = 13;
  s_size = 20;
  //dimension = 905;        // because of overlapping
  overlap = overlap_size;
  dimension = 1000 - 19 * overlap;
  anotherz = new double[dimension];

  if(Ovector==NULL)
    {
	  int id = ID;

	  if (overlap != 5)
	  {
		  ID *= 10;

		  if (overlap >= 10)
		  {
			  ID *= 10;
		  }

		  ID += overlap;
	  }

      Ovector = readOvector();
      Pvector = readPermVector();
	  ID = id;
      r25 = readR(25);
      r50 = readR(50);
      r100 = readR(100);
      s = readS(s_size);
      w = readW(s_size);

	  variablesForFunc = vector<vector<int>>(s_size);
	  funcForVariables = vector<vector<int>>(dimension);
	  variablesForVariables = vector<unordered_set<int>>(dimension);

	  int c = 0;

	  for (int i = 0; i < s_size; i++)
	  {
		  for (int j = c - i * overlap; j < c + s[i] - i * overlap; ++j)
		  {
			  variablesForFunc[i].push_back(Pvector[j]);
			  funcForVariables[Pvector[j]].push_back(i);
		  }

		  funcSizes.push_back(s[i]);

		  for (int j = 0; j < s[i] - 1; j++)
		  {
			  for (int k = j + 1; k < s[i]; k++)
			  {
				  variablesForVariables[variablesForFunc[i][j]].insert(variablesForFunc[i][k]);
				  variablesForVariables[variablesForFunc[i][k]].insert(variablesForFunc[i][j]);
			  }
		  }

		  c = c + s[i];
	  }
    }
}

F13::~F13(){
  delete[] Ovector;
  delete[] Pvector;
  delete[] anotherz;
  for (int i = 0; i < 25; ++i)
    {
      delete[] r25[i];
    }
  for (int i = 0; i < 50; ++i)
    {
      delete[] r50[i];
    }
  for (int i = 0; i < 100; ++i)
    {
      delete[] r100[i];
    }
  delete[] r25;
  delete[] r50;
  delete[] r100;
  delete[] s;
  delete[] w;

}

double F13::compute(double*x){
  int i;
  double result=0.0;
        
  if (Ovector == NULL)
  {
	  Ovector = readOvector();
	  Pvector = readPermVector();
	  r25 = readR(25);
	  r50 = readR(50);
	  r100 = readR(100);
	  s = readS(s_size);
	  w = readW(s_size);

	  variablesForFunc = vector<vector<int>>(s_size);
	  funcForVariables = vector<vector<int>>(dimension);
	  variablesForVariables = vector<unordered_set<int>>(dimension);

	  int c = 0;

	  for (int i = 0; i < s_size; i++)
	  {
		  for (int j = c - i * overlap; j < c + s[i] - i * overlap; ++j)
		  {
			  variablesForFunc[i].push_back(Pvector[j]);
			  funcForVariables[Pvector[j]].push_back(i);
		  }

		  funcSizes.push_back(s[i]);

		  for (int j = 0; j < s[i] - 1; j++)
		  {
			  for (int k = j + 1; k < s[i]; k++)
			  {
				  variablesForVariables[variablesForFunc[i][j]].insert(variablesForFunc[i][k]);
				  variablesForVariables[variablesForFunc[i][k]].insert(variablesForFunc[i][j]);
			  }
		  }

		  c = c + s[i];
	  }
  }
  
  for(i=0;i<dimension;i++)
    {
      anotherz[i]=x[i]-Ovector[i];
    }
  // cout<<"o"<<endl;

  // s_size non-separable part with rotation
  int c = 0;
  for (i = 0; i < s_size; i++)
    {
      // cout<<"c="<<c<<", i="<<i<<endl;
      anotherz1 = rotateVectorConform(i, c);
      // cout<<"done rot"<<endl;
      result += w[i] * schwefel(anotherz1, s[i]);
      delete []anotherz1;
      // cout<<result<<endl;
    }
  
  return(result);
}

double F13::compute(double* x, int s_idx) {
	int i;
	double result = 0.0;

	if (Ovector == NULL)
	{
		Ovector = readOvector();
		Pvector = readPermVector();
		r25 = readR(25);
		r50 = readR(50);
		r100 = readR(100);
		s = readS(s_size);
		w = readW(s_size);

		variablesForFunc = vector<vector<int>>(s_size);
		funcForVariables = vector<vector<int>>(dimension);
		variablesForVariables = vector<unordered_set<int>>(dimension);

		int c = 0;

		for (int i = 0; i < s_size; i++)
		{
			for (int j = c - i * overlap; j < c + s[i] - i * overlap; ++j)
			{
				variablesForFunc[i].push_back(Pvector[j]);
				funcForVariables[Pvector[j]].push_back(i);
			}

			funcSizes.push_back(s[i]);

			for (int j = 0; j < s[i] - 1; j++)
			{
				for (int k = j + 1; k < s[i]; k++)
				{
					variablesForVariables[variablesForFunc[i][j]].insert(variablesForFunc[i][k]);
					variablesForVariables[variablesForFunc[i][k]].insert(variablesForFunc[i][j]);
				}
			}

			c = c + s[i];
		}
	}

	for (i = 0; i < dimension; i++)
	{
		anotherz[i] = x[i] - Ovector[i];
	}
	// cout<<"o"<<endl;

	int c = 0;

	for (i = 0; i < s_idx; i++)
	{
		c = c + s[i];
	}

	i = s_idx;

	// cout<<"c="<<c<<", i="<<i<<endl;
	anotherz1 = rotateVectorConform(i, c);
	// cout<<"done rot"<<endl;
	result += w[i] * schwefel(anotherz1, s[i]);
	delete[]anotherz1;
	// cout<<result<<endl;

	return(result);
}
