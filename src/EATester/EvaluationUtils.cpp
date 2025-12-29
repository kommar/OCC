#include "EvaluationUtils.h"

#include "EnumCommandParam.h"
#include "RealCoding.h"
#include "RealEvaluation.h"
#include "StringUtils.h"

#include <atlstr.h>
#include <unordered_map>
#include <utility>

template <class TFenotype>
CEvaluation<TFenotype> * EvaluationUtils::pcGetEvaluation(istream *psSettings, CError *pcError)
{
	CEvaluation<TFenotype> *pc_evaluation = nullptr;

	size_t i_fenotype_type_hash_code = typeid(TFenotype).hash_code();

	unordered_map<CString, EEvaluationType> m_evaluation_types;

	if (i_fenotype_type_hash_code == typeid(CRealCoding).hash_code())
	{
		m_evaluation_types.insert(pair<const CString, EEvaluationType>(EVALUATION_ARGUMENT_TYPE_REAL_CEC2013_LSGO, EVALUATION_REAL_CEC2013_LSGO));
	}//else if (i_fenotype_type_hash_code == typeid(CRealCoding).hash_code())

	CEnumCommandParam<EEvaluationType> p_type(EVALUATION_ARGUMENT_TYPE, &m_evaluation_types);
	EEvaluationType e_type = p_type.eGetValue(psSettings, pcError);

	if (!*pcError)
	{
		switch (e_type)
		{
			case EVALUATION_REAL_CEC2013_LSGO:
			{
				pc_evaluation = (CEvaluation<TFenotype>*)new CRealCEC2013LSGOEvaluation();
				break;
			}//case EVALUATION_REAL_CEC2013_LSGO
			default:
			{
				pcError->vSetError(CError::iERROR_CODE_OPERATOR_NOT_FOUND, "evaluation");
				break;
			}//default
		}//switch (pcParams->eGetType())

		if (!*pcError)
		{
			*pcError = pc_evaluation->eConfigure(psSettings);
		}//if (!*pcError)
	}//if (!*pcError)

	return pc_evaluation;
}//CEvaluation<TFenotype> * EvaluationUtils::pcGetEvaluation(istream *psSettings, CError *pcError)


template CEvaluation<CRealCoding> * EvaluationUtils::pcGetEvaluation(istream*, CError*);