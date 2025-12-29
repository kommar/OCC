#ifndef RUNNING_TIME_STOP_CONDITION_H
#define RUNNING_TIME_STOP_CONDITION_H

#include "Evaluation.h"
#include "StopCondition.h"

#include <ctime>
#include <istream>

using namespace std;

template <class TGenotype, class TFenotype>
class CRunningTimeStopCondition : public CStopCondition<TGenotype, TFenotype>
{
public:
	CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation);
	CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation, int  iRunningTime);

	virtual CError eConfigure(istream *psSettings);

	virtual bool bStop(CTimeCounter  *pcTimeCounter, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual);

	virtual double dGetNow(CTimeCounter  *pcTimeCounter, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual) { return (pcTimeCounter->dGetTimePassed()); }
	virtual double dGetMax() { return (double)i_running_time_sec; }

private:
	int  i_running_time_sec;
};//class CRunningTimeStopCondition

#endif//RUNNING_TIME_STOP_CONDITION_H