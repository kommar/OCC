#include "RealEvaluation.h"

#include "CommandParam.h"
#include "EnumCommandParam.h"
#include "FloatVectorCommandParam.h"
#include "StringUtils.h"
#include "RandUtils.h"
#include "UIntCommandParam.h"
#include "UIntVectorCommandParam.h"

#include <atlstr.h>
#include <cfloat>
#include <cmath>
#include <unordered_map>


CRealEvaluation::CRealEvaluation()
{
	v_init(DBL_MIN);
}//CRealEvaluation::CRealEvaluation()

CRealEvaluation::CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue)
	: CEvaluation<CRealCoding>(iNumberOfElements, dMaxValue)
{
	v_init(DBL_MIN);
}//CRealEvaluation::CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue)

CRealEvaluation::CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue, double dEqualEpsilon)
	: CEvaluation<CRealCoding>(iNumberOfElements, dMaxValue)
{
	v_init(dEqualEpsilon);
}//CRealEvaluation::CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue, double dEqualEpsilon)

CRealEvaluation::~CRealEvaluation()
{
	v_clear_params();
}//CRealEvaluation::~CRealEvaluation()

CError CRealEvaluation::eConfigure(istream *psSettings)
{
	v_clear_params();

	return CEvaluation<CRealCoding>::eConfigure(psSettings);
}//CError CRealEvaluation::eConfigure(istream *psSettings)

CRealCoding * CRealEvaluation::pcCreateSampleFenotype()
{
	return new CRealCoding(i_number_of_elements, nullptr, pd_min_values, pd_max_values, d_equal_epsilon);
}//CRealCoding * CRealEvaluation::pcCreateSampleFenotype()

void CRealEvaluation::v_init(double dEqualEpsilon)
{
	d_equal_epsilon = dEqualEpsilon;

	pd_min_values = nullptr;
	pd_max_values = nullptr;
}//void CRealEvaluation::v_init(double dEqualEpsilon)

void CRealEvaluation::v_clear_params()
{
	delete pd_min_values;
	delete pd_max_values;

	v_init(d_equal_epsilon);
}//void CRealEvaluation::v_clear_params()


CRealCEC2013LSGOEvaluation::CRealCEC2013LSGOEvaluation()
{
	i_sub_ffe = 0;
	pc_benchmark = nullptr;
}//CRealCEC2013LSGOEvaluation::CRealCEC2013LSGOEvaluation()

CRealCEC2013LSGOEvaluation::~CRealCEC2013LSGOEvaluation()
{
	delete pc_benchmark;
}//CRealCEC2013LSGOEvaluation::~CRealCEC2013LSGOEvaluation()

CError CRealCEC2013LSGOEvaluation::eConfigure(istream *psSettings)
{
	delete pc_benchmark;
	pc_benchmark = nullptr;

	CError c_error = CRealEvaluation::eConfigure(psSettings);

	if (!c_error)
	{
		CUIntCommandParam p_function_number(EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER,
			EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER_MIN, EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER_MAX * 100);

		uint32_t i_function_number = (uint32_t)p_function_number.iGetValue(psSettings, &c_error);

		if (!c_error)
		{
			pc_benchmark = generateFuncObj((int)i_function_number);
		}//if (!c_error)
	}//if (!c_error)

	if (!c_error)
	{
		i_number_of_elements = (uint16_t)pc_benchmark->getDimension();
		d_max_value = 0;

		pd_min_values = new double[i_number_of_elements];
		pd_max_values = new double[i_number_of_elements];

		for (uint16_t i = 0; i < i_number_of_elements; i++)
		{
			*(pd_min_values + i) = (double)pc_benchmark->getMinX();
			*(pd_max_values + i) = (double)pc_benchmark->getMaxX();
		}//for (uint16_t i = 0; i < i_number_of_elements; i++)
	}//if (!c_error)

	return c_error;
}//CError CRealCEC2013LSGOEvaluation::eConfigure(istream *psSettings)

double CRealCEC2013LSGOEvaluation::dEvaluateSubEvaluation(CRealCoding *pcFenotype, uint16_t iSubEvaluationIndex)
{
	double d_result = -pc_benchmark->compute(pcFenotype->pdGetValues(), iSubEvaluationIndex);

	i_sub_ffe++;

	if (i_sub_ffe == (uint16_t)pc_benchmark->getSSize())
	{
		i_ffe++;
		i_sub_ffe = 0;
	}//if (i_sub_ffe == (uint16_t)pc_benchmark->getSSize())

	return d_result;
}//double CRealCEC2013LSGOEvaluation::dEvaluateSubEvaluation(CRealCoding *pcFenotype, uint16_t iSubEvaluationIndex)

void CRealCEC2013LSGOEvaluation::vGetSubEvaluationSizes(vector<uint16_t> *pvSubEvaluationSizes)
{
	pvSubEvaluationSizes->clear();
	pvSubEvaluationSizes->insert(pvSubEvaluationSizes->end(), pc_benchmark->getFuncSizes().begin(), pc_benchmark->getFuncSizes().end());
}//void CRealCEC2013LSGOEvaluation::vGetSubEvaluationSizes(vector<int> *pvSubEvaluationSizes)

void CRealCEC2013LSGOEvaluation::vGetGenesForSubEvaluation(uint16_t iSubEvaluationIndex, vector<uint16_t> *pvGenesForSubEvaluation)
{
	pvGenesForSubEvaluation->clear();
	pvGenesForSubEvaluation->insert(pvGenesForSubEvaluation->end(), pc_benchmark->getVariablesForFunc(iSubEvaluationIndex).begin(), pc_benchmark->getVariablesForFunc(iSubEvaluationIndex).end());
}//void CRealCEC2013LSGOEvaluation::vGetGenesForSubEvaluation(uint16_t iSubEvaluationIndex, vector<uint16_t> *pvGenesForSubEvaluation)

void CRealCEC2013LSGOEvaluation::vGetSubEvaluationsForGene(uint16_t iGeneIndex, vector<uint16_t> *pvSubEvaluationsForGene)
{
	pvSubEvaluationsForGene->clear();
	pvSubEvaluationsForGene->insert(pvSubEvaluationsForGene->end(), pc_benchmark->getFuncsForVariable(iGeneIndex).begin(), pc_benchmark->getFuncsForVariable(iGeneIndex).end());
}//void CRealCEC2013LSGOEvaluation::vGetSubEvaluationsForGene(uint16_t iSubEvaluationIndex, vector<uint16_t> *pvSubEvaluationsForGene)

void CRealCEC2013LSGOEvaluation::vGetGenesForGene(uint16_t iGeneIndex, vector<uint16_t> *pvGenesForGene)
{
	pvGenesForGene->clear();
	pvGenesForGene->insert(pvGenesForGene->end(), pc_benchmark->getVariablesForVariable(iGeneIndex).begin(), pc_benchmark->getVariablesForVariable(iGeneIndex).end());
}//void CRealCEC2013LSGOEvaluation::vGetGenesForGene(uint16_t iGeneIndex, vector<uint16_t> *pvGenesForGene)

double CRealCEC2013LSGOEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)
{
	return -pc_benchmark->compute(pcFenotype->pdGetValues() + iShift);
}//double CRealCEC2013LSGOEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)