#include "OptimizationCommand.h"

#include "EnumCommandParam.h"
#include "Evaluation.h"
#include "EvaluationUtils.h"
#include "Optimizer.h"
#include "OptimizerUtils.h"
#include "Problem.h"
#include "StringUtils.h"
#include "System.h"

#include <unordered_map>
#include <utility>


COptimizationBasedCommand::COptimizationBasedCommand(CString sName, istream *psSettings, CLog *pcLog, ostream *psSummary)
	: CCommandExecution(sName, psSettings, pcLog, psSummary)
{

}//COptimizationBasedCommand::COptimizationBasedCommand(CString sName, istream *psSettings, CLog *pcLog, ostream *psSummary)

CError COptimizationBasedCommand::eExecute(CCommandParamParser *pcParser, uint32_t iRandomSeed)
{
	CError c_error;

	unordered_map<CString, EProblemType> m_problem_types;
	m_problem_types.insert(pair<const CString, EProblemType>(PROBLEM_ARGUMENT_TYPE_REAL_REAL, PROBLEM_REAL_REAL));

	CEnumCommandParam<EProblemType> p_problem_type(PROBLEM_ARGUMENT_TYPE, &m_problem_types);
	EProblemType e_problem_type = p_problem_type.eGetValue(ps_settings, &c_error);

	if (!c_error)
	{
		switch (e_problem_type)
		{
			case PROBLEM_REAL_REAL:
			{
				c_error = e_optimize_real_real(iRandomSeed);
				break;
			}//PROBLEM_REAL_REAL
			default:
			{
				c_error.vSetError(CError::iERROR_CODE_OPERATOR_NOT_FOUND, "problem");
				break;
			}//default
		}//switch (e_problem_type)
	}//if (!c_error)

	return c_error;
}//CError COptimizationBasedCommand::eExecute(CCommandParamParser *pcParser, uint32_t iRandomSeed)


COptimizationCommand::COptimizationCommand(istream *psSettings, CLog *pcLog, ostream *psSummary)
	: COptimizationBasedCommand(SYSTEM_ARGUMENT_MODE_OPTIMIZATION, psSettings, pcLog, psSummary)
{

}//COptimizationCommand::COptimizationCommand(istream *psSettings, CLog *pcLog, ostream *psSummary)

template <class TGenotype, class TFenotype>
CError COptimizationCommand::e_optimize(uint32_t iRandomSeed)
{
	CError c_error;

	CEvaluation<TFenotype> *pc_evaluation = EvaluationUtils::pcGetEvaluation<TFenotype>(ps_settings, &c_error);

	if (!c_error)
	{
		CTransformation<TGenotype, TFenotype> *pc_transformation = TransformationUtils::pcGetTransformation<TGenotype, TFenotype>(pc_evaluation, ps_settings, &c_error);

		if (!c_error)
		{
			CProblem<TGenotype, TFenotype> *pc_problem = new CProblem<TGenotype, TFenotype>(pc_evaluation, pc_transformation);

			COptimizer<TGenotype, TFenotype> *pc_optimizer = OptimizerUtils::pcGetOptimizer(pc_problem, pc_log, iRandomSeed, ps_settings, &c_error);

			if (!c_error)
			{
				pc_optimizer->vRun();

				CString s_summary;

				
				s_summary.Format
					("%s\t%s\t%.16f\t%s\t%.16f\t%s\t%d\t%s\t%llu\t%s\t%.8lf\t%s\t%llu", 
						OPT_RES_FLAG_FLAGGED, 
						OPT_RES_FLAG_VAL_BEST_RESULT, pc_optimizer->pcGetBestIndividual()->dGetFitnessValue(),
						OPT_RES_FLAG_VAL_OPTIMAL, pc_problem->pcGetEvaluation()->dGetMaxValue(), 
						OPT_RES_FLAG_VAL_PROB_LEN, pc_problem->pcGetEvaluation()->iGetNumberOfElements(),
						OPT_RES_FLAG_VAL_BEST_FFE, pc_optimizer->iGetBestFFE(),
						OPT_RES_FLAG_VAL_BEST_TIME, pc_optimizer->dGetBestTime(), 
						OPT_RES_FLAG_RES_TOTAL_FFE, pc_problem->pcGetEvaluation()->iGetFFE()
					);

				s_summary.AppendFormat("\t%s", pc_optimizer->sAdditionalSummaryInfo());

				if (pc_problem->pcGetEvaluation() != NULL)  s_summary += " " + pc_problem->pcGetEvaluation()->sAdditionalSummaryInfo();

				s_summary.AppendFormat("\t%s\t%s", OPT_RES_FLAG_RES_LOG_FILE, pc_optimizer->sGetLogName());

				(*ps_summary) << s_summary;
				(*ps_summary) << "\t" << OPT_RES_FLAG_VAL_BEST_PHENOTYPE  << "\t" << pc_optimizer->pcGetBestIndividual()->pcGetFenotype();
			}//if (!c_error)

			delete pc_optimizer;
			delete pc_problem;
		}//if (!c_error)
	}//if (!c_error)
	else
	{
		delete pc_evaluation;
	}//else if (!c_error)

	return c_error;
}//CError COptimizationCommand::e_optimize(uint32_t iRandomSeed)