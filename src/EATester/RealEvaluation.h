#ifndef REAL_EVALUATION_H
#define REAL_EVALUATION_H

#define EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER "evaluation_function_number"
#define EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER_MIN 1
#define EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER_MAX 15

#include "Error.h"
#include "Evaluation.h"
#include "RealCoding.h"

#include "../CEC2013LSGO/Header.h"

#include <cstdint>
#include <istream>

using namespace std;


class CRealEvaluation : public CEvaluation<CRealCoding>
{
public:
	CRealEvaluation();
	CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue);
	CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue, double dEqualEpsilon);

	virtual ~CRealEvaluation();

	virtual CError eConfigure(istream *psSettings);

	virtual CRealCoding *pcCreateSampleFenotype();

	double *pdGetMinValues() { return pd_min_values; };
	double *pdGetMaxValues() { return pd_max_values; };

	double dGetEqualEpsilon() { return d_equal_epsilon; };

protected:
	void v_clear_params();

	double d_equal_epsilon;

	double *pd_min_values;
	double *pd_max_values;

private:
	void v_init(double dEqualEpsilon);
};//class CRealEvaluation : public CEvaluation<CRealCoding>


class CRealCEC2013LSGOEvaluation : public CRealEvaluation
{
public:
	CRealCEC2013LSGOEvaluation();

	virtual ~CRealCEC2013LSGOEvaluation();

	virtual CError eConfigure(istream *psSettings);

	virtual double dEvaluateSubEvaluation(CRealCoding *pcFenotype, uint16_t iSubEvaluationIndex);
	virtual void vGetSubEvaluationSizes(vector<uint16_t> *pvSubEvaluationSizes);
	virtual void vGetGenesForSubEvaluation(uint16_t iSubEvaluationIndex, vector<uint16_t> *pvGenesForSubEvaluation);
	virtual void vGetSubEvaluationsForGene(uint16_t iGeneIndex, vector<uint16_t> *pvSubEvaluationsForGene);
	virtual void vGetGenesForGene(uint16_t iGeneIndex, vector<uint16_t> *pvGenesForGene);
	virtual uint16_t iGetNumberOfSubEvaluations() { return (uint16_t)pc_benchmark->getSSize(); };
	virtual uint16_t iGetNumberOfElementsOfSubEvaluation(uint16_t iSubEvaluationIndex) { return (uint16_t)pc_benchmark->getFuncSizes()[iSubEvaluationIndex]; };

protected:
	virtual double d_evaluate(CRealCoding *pcFenotype, uint16_t iShift);

private:
	uint16_t i_sub_ffe;
	Benchmarks *pc_benchmark;
};//class CRealCEC2013LSGOEvaluation : public CRealEvaluation

#endif//REAL_EVALUATION_H