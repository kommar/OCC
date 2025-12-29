#include "OptimizerUtils.h"

#include "OCC.h"
#include "EnumCommandParam.h"
#include "RealCoding.h"
#include "StringUtils.h"

#include <atlstr.h>
#include <unordered_map>
#include <utility>


template <class TGenotype, class TFenotype>
COptimizer<TGenotype, TFenotype> * OptimizerUtils::pcGetOptimizer(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed, istream *psSettings, CError *pcError, bool bIsObligatory)
{
	COptimizer<TGenotype, TFenotype> *pc_optimizer = nullptr;

	size_t i_genotype_type_hash_code = typeid(TGenotype).hash_code();
	size_t i_fenotype_type_hash_code = typeid(TFenotype).hash_code();

	unordered_map<CString, EOptimizerType> m_optimizer_types;

	if (i_genotype_type_hash_code == typeid(CRealCoding).hash_code() && i_fenotype_type_hash_code == typeid(CRealCoding).hash_code())
	{
		m_optimizer_types.insert(pair<const CString, EOptimizerType>(OPTIMIZER_ARGUMENT_TYPE_OCC, OPTIMIZER_OCC));
	}//if (i_genotype_type_hash_code == typeid(CRealCoding).hash_code() && i_fenotype_type_hash_code == typeid(CRealCoding).hash_code())

	CEnumCommandParam<EOptimizerType> p_type(OPTIMIZER_ARGUMENT_TYPE, &m_optimizer_types, bIsObligatory);
	EOptimizerType e_type = p_type.eGetValue(psSettings, pcError);

	if (!*pcError && p_type.bHasValue())
	{
		switch (e_type)
		{
			case OPTIMIZER_OCC:
			{
				pc_optimizer = (COptimizer<TGenotype, TFenotype>*)new COCC((CProblem<CRealCoding, CRealCoding>*)pcProblem, pcLog, iRandomSeed);
				break;
			}//case OPTIMIZER_OCC
			default:
			{
				pcError->vSetError(CError::iERROR_CODE_OPERATOR_NOT_FOUND, "optimizer");
				break;
			}//default
		}//switch (e_type)
	}//if (!*pcError && p_type.bHasValue())

	if (!*pcError)
	{
		*pcError = pc_optimizer->eConfigure(psSettings);
	}//if (!*pcError)

	return pc_optimizer;
}//COptimizer<TGenotype, TFenotype> * OptimizerUtils::pcGetOptimizer(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed, istream *psSettings, COptimizerParams<TGenotype, TFenotype> **ppcParams, CError *pcError, bool bIsObligatory)


template COptimizer<CRealCoding, CRealCoding> * OptimizerUtils::pcGetOptimizer(CProblem<CRealCoding, CRealCoding>*, CLog*, uint32_t, istream*, CError*, bool);