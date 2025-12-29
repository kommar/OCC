#ifndef OCC_H
#define OCC_H

#include "Error.h"
#include "Evaluation.h"
#include "FloatCommandParam.h"
#include "Log.h"
#include "Optimizer.h"
#include "Problem.h"
#include "RandUtils.h"
#include "RealCoding.h"
#include "RealRandomGeneration.h"
#include "UIntCommandParam.h"
#include "VectorUtils.h"

#include "cmaes/CMAESO.h"

#include <cfloat>
#include <cstdint>
#include <istream>
#include <vector>

using namespace std;


class COCC : public COptimizer<CRealCoding, CRealCoding>
{
public:
	COCC(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed);
	COCC(COCC *pcOther);

	virtual ~COCC();

	virtual COCC *pcCopy() { return new COCC(this); }

	virtual void vInitialize();
	virtual bool bRunIteration(uint32_t iIterationNumber);

	virtual CString sAdditionalSummaryInfo();

private:
	bool b_run_cmaes(CMAESO *pcCMAES, CIndividual<CRealCoding, CRealCoding> *pcLocalBest, uint32_t iIterationNumber, uint16_t iSubEvaluationIndex);

	CMAESO *pc_create_cmaes(uint16_t iSubEvaluationIndex, uint32_t iNumberOfHops);
	void v_clear_cmaeses();

	vector<CMAESO*> v_cmaeses;
	vector<CIndividual<CRealCoding, CRealCoding>*> v_local_bests;

	vector<double> v_improvement_values;
	vector<vector<int>> v_component_indices;
	vector<uint32_t> v_last_improvements;
	vector<double> v_last_iterations;

	vector<int> v_locally_update_counter;
};//class COCC : public COptimizer<CRealCoding, CRealCoding>

#endif//OCC_H