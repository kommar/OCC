#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#define OPTIMIZER_ARGUMENT_TYPE "optimizer_type"
#define OPTIMIZER_ARGUMENT_TYPE_OCC "occ"


#include "Error.h"
#include "Generation.h"
#include "Individual.h"
#include "Log.h"
#include "Problem.h"
#include "StopCondition.h"

#include <cstdint>
#include <ctime>
#include <functional>
#include <istream>

#include  "util\timer.h"

using namespace std;
using namespace TimeCounters;


enum EOptimizerType
{
	OPTIMIZER_OCC = 0
};//enum EOptimizerType



#define OPT_RES_FLAG_FLAGGED "{FLAGGEDresult}"

#define OPT_RES_FLAG_VAL_BEST_RESULT "{BestVal}"
#define OPT_RES_FLAG_VAL_OPTIMAL "{OptValue}"
#define OPT_RES_FLAG_VAL_PROB_LEN "{ProbLen}"
#define OPT_RES_FLAG_VAL_BEST_FFE "{BestFFE}"
#define OPT_RES_FLAG_VAL_BEST_TIME "{BestTime}"
#define OPT_RES_FLAG_VAL_BEST_UNITATION "{BestUnitation}"
#define OPT_RES_FLAG_VAL_BEST_PHENOTYPE "{BestPhenotype}"

#define OPT_RES_FLAG_RES_RUNTIME "{RunningTime}"
#define OPT_RES_FLAG_RES_TOTAL_FFE "{TotalFFE}"
#define OPT_RES_FLAG_RES_RAND_SEED "{RandSeed}"
#define OPT_RES_FLAG_RES_LOG_FILE "{LogFile}"


template <class TGenotype, class TFenotype>
class COptimizer
{
public:
	COptimizer(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed);
	COptimizer(COptimizer<TGenotype, TFenotype> *pcOther);

	virtual ~COptimizer();

	virtual COptimizer<TGenotype, TFenotype> *pcCopy() = 0;

	virtual CError eConfigure(istream *psSettings);

	virtual void vInitialize();
	virtual bool bRunIteration(uint32_t iIterationNumber) = 0;

	virtual void vReportLinkage() {};
	virtual void vExecuteBeforeEnd() {};
	virtual void vRun();
	virtual CString sAdditionalSummaryInfo() { return(""); };

	void vResetBestIndividual();

	CProblem<TGenotype, TFenotype> *pcGetProblem() { return pc_problem; };

	CStopCondition<TGenotype, TFenotype> *pcGetStopCondition() { return pc_stop_condition; };

	CIndividual<TGenotype, TFenotype> *pcGetBestIndividual() { return pc_best_individual; };
	void vSetBestIndividual(CIndividual<TGenotype, TFenotype> *pcBestIndividual, bool bCopy = true);

	
	double  dGetBestTime() { return d_best_time; };
	uint64_t iGetBestFFE() { return i_best_ffe; };

	CString  sGetLogName();

protected:
	bool b_update_best_individual(uint32_t iIterationNumber, CIndividual<TGenotype, TFenotype> *pcIndividual);
	bool b_update_best_individual(uint32_t iIterationNumber, double dCurrentBestFitnessValue, function<void(TGenotype*)> &&fUpdateBestGenotype);

	virtual CIndividual<TGenotype, TFenotype> *pc_create_individual(TGenotype *pcGenotype)
	{
		return new CIndividual<TGenotype, TFenotype>(pcGenotype, pc_problem->pcGetEvaluation(), pc_problem->pcGetTransformation());
	};

	void v_clear_params();

	CProblem<TGenotype, TFenotype> *pc_problem;
	
	CStopCondition<TGenotype, TFenotype> *pc_stop_condition;

	uint32_t i_gene_pattern_min_length;
	uint32_t i_linkage_frequency;

	CGeneration<TGenotype> *pc_empty_generation;

	CIndividual<TGenotype, TFenotype> *pc_best_individual;

	CTimeCounter  c_optimizer_timer;
	double d_best_time;
	uint64_t i_best_ffe;

	CLog *pc_log;

	uint32_t i_random_seed;

	bool b_own_params;

private:
	void v_update_statistics_of_best(uint32_t iIterationNumber, double dBestTime);
};//class COptimizer

#endif//OPTIMIZER_H
