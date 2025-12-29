#ifndef EVALUATION_H
#define EVALUATION_H

#define EVALUATION_ARGUMENT_TYPE "evaluation_type"
#define EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO "real_cec2013_lsgo"

#include "Error.h"

#include <cstdint>
#include <istream>
#include <vector>
#include <direct.h>

using namespace std;

enum EEvaluationType
{
	EVALUATION_REAL_CEC2013_LSGO = 0
};//enum EEvaluationType


template <class TFenotype>
class CEvaluation
{
public:
	CEvaluation();
	CEvaluation(uint16_t iNumberOfElements, double dMaxValue);

	virtual ~CEvaluation();

	virtual CError eConfigure(istream *psSettings) { return CError(iERROR_PARENT_CEVALUATION); };

	double dEvaluate(TFenotype *pcFenotype);

	virtual TFenotype *pcCreateSampleFenotype() = 0;

	virtual bool bIsMaxValue(double dValue) { return dValue == d_max_value; };
	virtual bool bOptimalFound() { return(false); }

	uint16_t iGetNumberOfElements() { return i_number_of_elements; };

	double dGetMaxValue() { return d_max_value; };

	uint64_t iGetFFE() { return i_ffe; };

	void vSetFFE(uint64_t iFFE) { i_ffe = iFFE; };
	void vSetZeroFFE() { vSetFFE(0); };

	void vIncreaseFFE() { i_ffe++; };
	void vDecreaseFFE() { i_ffe--; };


	virtual CString  sAdditionalSummaryInfo() { return(""); }

	virtual double dEvaluateSubEvaluation(TFenotype *pcFenotype, uint16_t iSubEvaluationIndex) { return dEvaluate(pcFenotype); };
	virtual void vGetSubEvaluationSizes(vector<uint16_t> *pvSubEvaluationSizes) {};
	virtual void vGetGenesForSubEvaluation(uint16_t iSubEvaluationIndex, vector<uint16_t> *pvGenesForSubEvaluation) {};
	virtual void vGetSubEvaluationsForGene(uint16_t iGeneIndex, vector<uint16_t> *pvSubEvaluationsForGene) {};
	virtual void vGetGenesForGene(uint16_t iGeneIndex, vector<uint16_t> *pvGenesForGene) {};
	virtual uint16_t iGetNumberOfSubEvaluations() { return 1; };
	virtual uint16_t iGetNumberOfElementsOfSubEvaluation(uint16_t iSubEvaluationIndex) { return iGetNumberOfElements(); };

protected:
	static uint32_t iERROR_PARENT_CEVALUATION;

	virtual double d_evaluate(TFenotype *pcFenotype, uint16_t iShift) = 0;
	virtual double d_evaluate_double(TFenotype *pcFenotype, uint16_t iShift) { return(0); };
	virtual  void  v_get_true_gene_dependencies(vector<vector<int>>  *pvDependentGenes) { return; };

	void v_init(uint16_t iNumberOfElements, double dMaxValue);

	uint16_t i_number_of_elements;

	double d_max_value;

	uint64_t i_ffe;
};//class CEvaluation


#endif//EVALUATION_H