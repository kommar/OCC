//https://github.com/Flyki/Large-Scale-Overlapping-Optimization/blob/master/CMAESO.cpp

#include "CMAESO.h"

CMAESO::CMAESO(CEvaluation<CRealCoding>* fp, int id, vector<int> dims, int swarmsize, int seed) {
	this->fp = fp;
	this->id = id;
	this->novar = dims.size();
	this->evaluationIndividual = fp->pcCreateSampleFenotype();
	this->evaluationIndividual->vSetEmptyValues();
	stringstream ss;
	string idStr, theFuncStr;
	ss << id;
	ss >> idStr;
	ss.clear();
	ss >> theFuncStr;
	parameterfilename = theFuncStr + "_" + idStr + "_cmaes_initials.par";
	restartfilename = theFuncStr + "_" + idStr + "_resume.par";
	writeParameters(novar);
	fitness = cmaes_init(&evo, novar, NULL, NULL, 0, swarmsize, parameterfilename.c_str());
	this->swarmsize = (int)cmaes_Get(&evo, "lambda");
	this->dims = dims;
	this->lowerBound = new double[2];
	this->lowerBound[0] = evaluationIndividual->pdGetMinValues()[0];
	this->lowerBound[1] = evaluationIndividual->pdGetMinValues()[0];
	this->upperBound = new double[2];
	this->upperBound[0] = evaluationIndividual->pdGetMaxValues()[0];
	this->upperBound[1] = evaluationIndividual->pdGetMaxValues()[0];
	cmaes_boundary_transformation_init(&boundaries, lowerBound, upperBound, 2);
	this->times = 2;
	fitnessForBest = new double[swarmsize];
}

CMAESO::CMAESO(CEvaluation<CRealCoding>* fp, int id, vector<int> dims, int swarmsize, int seed, CRealCoding* initSolution, vector<int> sharedDims) {
	this->fp = fp;
	this->id = id;
	this->novar = dims.size();
	this->evaluationIndividual = fp->pcCreateSampleFenotype();
	this->evaluationIndividual->vSetEmptyValues();
	stringstream ss;
	string idStr, theFuncStr;
	ss << id;
	ss >> idStr;
	ss.clear();
	ss >> theFuncStr;
	parameterfilename = theFuncStr + "_" + idStr + "_cmaes_initials.par";
	restartfilename = theFuncStr + "_" + idStr + "_resume.par";

	vector<double> initX(dims.size());

	for (size_t i = 0; i < initX.size(); i++)
	{
		initX[i] = initSolution->pdGetValues()[dims[i]];
	}

	this->times = 1;
	writeParameters(novar, initX.data());

	fitness = cmaes_init(&evo, novar, initX.data(), NULL, seed, swarmsize, parameterfilename.c_str());
	this->swarmsize = (int)cmaes_Get(&evo, "lambda");
	this->dims = dims;
	this->lowerBound = new double[2];
	this->lowerBound[0] = evaluationIndividual->pdGetMinValues()[0];
	this->lowerBound[1] = evaluationIndividual->pdGetMinValues()[0];
	this->upperBound = new double[2];
	this->upperBound[0] = evaluationIndividual->pdGetMaxValues()[0];
	this->upperBound[1] = evaluationIndividual->pdGetMaxValues()[0];
	cmaes_boundary_transformation_init(&boundaries, lowerBound, upperBound, 2);
	this->times = 2;
	
	this->sharedDims = sharedDims;

	fitnessForBest = new double[swarmsize];
}

void CMAESO::writeParameters(int D) {
	ofstream parfile(parameterfilename.c_str());
	parfile << "N " << D << endl;
	parfile << "initialX 1:" << endl;
	parfile << " " << ((double)evaluationIndividual->pdGetMaxValues()[0] + evaluationIndividual->pdGetMinValues()[0]) / 2.0 << endl;
	parfile << "initialStandardDeviations 1:" << endl;
	parfile << " " << ((double)evaluationIndividual->pdGetMaxValues()[0] - evaluationIndividual->pdGetMinValues()[0]) / 3.0 << endl;
	parfile << "stopMaxFunEvals 1e299 " << endl;
	parfile << "stopMaxIter 1e299 " << endl;
	parfile << "stopTolFun 1e-100" << endl;
	parfile << "seed 0" << endl;
	parfile.close();
}

void CMAESO::writeParameters(int D, double* xmean) {
	ofstream parfile(parameterfilename.c_str());
	parfile << "N " << D << endl;
	parfile << "initialX " << D << ":" << endl;
	for (int i = 0; i < D; i++) {
		parfile << " " << xmean[i];
	}
	parfile << endl;
	parfile << "initialStandardDeviations 1:" << endl;
	parfile << " " << ((double)evaluationIndividual->pdGetMaxValues()[0] - evaluationIndividual->pdGetMinValues()[0]) / (3.0 * times) << endl;
	parfile << "stopMaxFunEvals 1e299 " << endl;
	parfile << "stopMaxIter 1e299 " << endl;
	parfile << "stopTolFun 1e-100" << endl;
	parfile << "seed 0" << endl;
	parfile.close();
}

void CMAESO::writeParameters(int D, double* xmean, double* stdev) {
	ofstream parfile(parameterfilename.c_str());
	parfile << "N " << D << endl;
	parfile << "initialX " << D << ":" << endl;
	for (int i = 0; i < D; i++) {
		parfile << " " << xmean[i];
	}
	parfile << endl;
	parfile << "initialStandardDeviations " << D << ":" << endl;
	for (int i = 0; i < D; i++) {
		parfile << " " << stdev[i];
	}
	parfile << endl;
	parfile << "stopMaxFunEvals 1e299 " << endl;
	parfile << "stopMaxIter 1e299 " << endl;
	parfile << "stopTolFun 1e-100" << endl;
	parfile << "seed 0" << endl;
	parfile.close();
}

void CMAESO::resume(vector<int> dims, int swarmsize) {
	cmaes_exit(&evo);
	this->dims = dims;
	this->novar = dims.size();
	writeParameters(dims.size());
	fitness = cmaes_init(&evo, dims.size(), NULL, NULL, 0, swarmsize, parameterfilename.c_str());
	this->swarmsize = (int)cmaes_Get(&evo, "lambda");
	char* resName = new char[restartfilename.length() + 1];
	strcpy_s(resName, restartfilename.length() + 1, restartfilename.c_str());
	cmaes_resume_distribution(&evo, resName);
	delete[] resName;
}

void CMAESO::restart(double* gbest, bool calcSigma, bool useStddev) {
	double minDistance = -DBL_MAX;
	double* xm = new double[novar];
	for (int i = 0; i < novar; i++) {
		xm[i] = gbest[dims[i]];
		double distance = abs(evo.rgxmean[i] - xm[i]);
		if (distance > minDistance) 
		{
			minDistance = distance;
		}
	}
	if (!useStddev)
	{
		cmaes_exit(&evo);
		writeParameters(novar, xm);
		fitness = cmaes_init(&evo, novar, xm, NULL, 0, swarmsize, parameterfilename.c_str());
		delete[] xm;
		times++;
	}
	else
	{
		double* stdev = new double[novar];

		for (int i = 0; i < novar; i++)
		{
			stdev[i] = evo.sigma * sqrt(evo.C[i][i]);
		}

		cmaes_exit(&evo);
		writeParameters(novar, xm, stdev);
		fitness = cmaes_init(&evo, novar, xm, stdev, 0, swarmsize, parameterfilename.c_str());

		delete[] stdev;
	}
}

int CMAESO::calculateFitness(double* contextVector, int allDim, long int& fes) {
	for (int i = 0; i < allDim; i++) {
		evaluationIndividual->pdGetValues()[i] = contextVector[i];
	}

	vector<uint16_t> real_shared_variables;

	double* x_in_bounds = cmaes_NewDouble(novar);
	for (int i = 0; i < swarmsize; i++) {
		cmaes_boundary_transformation(&boundaries, swarm[i], x_in_bounds, novar);
		for (int j = 0; j < novar; j++) {
			evaluationIndividual->pdGetValues()[dims[j]] = x_in_bounds[j];
		}

		fitness[i] = -fp->dEvaluate(evaluationIndividual);

		fes++;
	}
	curBest = 0;
	for (int i = 0; i < swarmsize; i++) {
		if (fitness[i] < fitness[curBest]) {
			curBest = i;
		}
	}
	free(x_in_bounds);

	return curBest;
}

bool CMAESO::isFeasible(double* xx) {
	bool flag = true;
	for (int i = 0; i < novar; i++) {
		if (xx[i] > evaluationIndividual->pdGetMaxValues()[0] || xx[i] < evaluationIndividual->pdGetMinValues()[0]) {
			flag = false;
			break;
		}
	}
	return flag;
}

void CMAESO::generateSubsolution() {
	double* x_in_bounds = cmaes_NewDouble(novar);
	swarm = cmaes_SamplePopulation(&evo);
	for (int i = 0; i < swarmsize; i++) {
		cmaes_boundary_transformation(&boundaries, swarm[i], x_in_bounds, novar);
		while (isFeasible(x_in_bounds) == false) {
			swarm = cmaes_ReSampleSingle(&evo, i);
			cmaes_boundary_transformation(&boundaries, swarm[i], x_in_bounds, novar);
		}
	}
	free(x_in_bounds);
}

void CMAESO::updateTheDistribution() {
	cmaes_UpdateDistribution(&evo, fitness);
}

bool CMAESO::updateContextVector(double* gbest, double& gbestf, int DIM) {
	if (fitness[curBest] < gbestf) {
		double* x_in_bounds = cmaes_NewDouble(novar);
		cmaes_boundary_transformation(&boundaries, swarm[curBest], x_in_bounds, novar);
		double* gbest_copy = cmaes_NewDouble(DIM);

		for (int i = 0; i < novar; i++) {
			gbest_copy[dims[i]] = gbest[dims[i]];
			gbest[dims[i]] = x_in_bounds[i];
		}

		gbestf = fitness[curBest];

		free(x_in_bounds);
		free(gbest_copy);

		return true;

	}

	return false;
}

CMAESO::~CMAESO() {
	cmaes_exit(&evo);
	cmaes_boundary_transformation_exit(&boundaries);
	delete[] lowerBound;
	delete[] upperBound;
	delete evaluationIndividual;
	delete[] fitnessForBest;
}