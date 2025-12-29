//https://github.com/Flyki/Large-Scale-Overlapping-Optimization/blob/master/CMAESO.h

#pragma once

#include "../Evaluation.h"
#include "../RealCoding.h"
#include "../RandUtils.h"

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <chrono>
#include <list>
#include "cmaes_interface.h"
#include <sstream>
#include <cstring>
#include <fstream>
#include <numeric>

extern "C" {
#include "boundary_transformation.h"
}

extern "C" {
	void cmaes_boundary_transformation_init(cmaes_boundary_transformation_t*,
		double const* lower_bounds, double const* upper_bounds, unsigned long len_of_bounds);
	void cmaes_boundary_transformation_exit(cmaes_boundary_transformation_t*);
	void cmaes_boundary_transformation(cmaes_boundary_transformation_t*,
		double const* x, double* y, unsigned long len);
	void cmaes_boundary_transformation_inverse(cmaes_boundary_transformation_t* t,
		double const* y, double* x, unsigned long len);
	void cmaes_boundary_transformation_shift_into_feasible_preimage(cmaes_boundary_transformation_t* t,
		double const* x, double* x_shifted, unsigned long len);
}

using namespace std;

class CMAESO
{
public:
	CMAESO(CEvaluation<CRealCoding>*, int, vector<int>, int, int seed);
	CMAESO(CEvaluation<CRealCoding>*, int, vector<int>, int, int seed, CRealCoding*, vector<int>);
	~CMAESO();
	void writeParameters(int D);
	void writeParameters(int D, double* xmean);
	void writeParameters(int D, double* xmean, double* stdev);
	int calculateFitness(double*, int, long int&);
	void generateSubsolution();
	void updateTheDistribution();
	bool isFeasible(double*);
	void resume(vector<int>, int);
	bool updateContextVector(double* gbest, double& gbestf, int DIM);
	void restart(double* gbest, bool calcSigma, bool useStddev);

	int nsvar() { return novar - sharedDims.size(); }
	vector<int>& getSharedVariables() { return sharedDims; }

	cmaes_t evo;
	cmaes_boundary_transformation_t boundaries;
	double* lowerBound;
	double* upperBound;
	int id;
	double* fitness;
	double* fitnessForBest;
	CEvaluation<CRealCoding>* fp;
	int swarmsize;
	int novar;
	double* const* swarm;
	vector<int> dims;
	vector<int> sharedDims;
	int curBest;
	string parameterfilename;
	string restartfilename;
	int times;
	CRealCoding *evaluationIndividual;
};

