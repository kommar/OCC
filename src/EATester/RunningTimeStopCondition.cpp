#include "RunningTimeStopCondition.h"

#include "RealCoding.h"
#include "UIntCommandParam.h"

template <class TGenotype, class TFenotype>
CRunningTimeStopCondition<TGenotype, TFenotype>::CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation)
	: CStopCondition<TGenotype, TFenotype>(pcEvaluation)
{

}//CRunningTimeStopCondition<TGenotype, TFenotype>::CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation)

template <class TGenotype, class TFenotype>
CRunningTimeStopCondition<TGenotype, TFenotype>::CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation, int  iRunningTime)
	: CStopCondition<TGenotype, TFenotype>(pcEvaluation)
{
	i_running_time_sec = iRunningTime;
}//CRunningTimeStopCondition<TGenotype, TFenotype>::CRunningTimeStopCondition(CEvaluation<TFenotype> *pcEvaluation, time_t tRunningTime)

template <class TGenotype, class TFenotype>
CError CRunningTimeStopCondition<TGenotype, TFenotype>::eConfigure(istream *psSettings)
{
	CError c_error = CStopCondition<TGenotype, TFenotype>::eConfigure(psSettings);

	if (!c_error)
	{
		CUIntCommandParam p_running_time(STOP_CONDITION_ARGUMENT_TIME);
		i_running_time_sec = (time_t)p_running_time.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	return c_error;
}//CError CRunningTimeStopCondition<TGenotype, TFenotype>::eConfigure(istream *psSettings)

template <class TGenotype, class TFenotype>
bool CRunningTimeStopCondition<TGenotype, TFenotype>::bStop(CTimeCounter  *pcTimeCounter, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual)
{
	bool b_stop = CStopCondition::bStop(pcTimeCounter, iIterationNumber, iFFE, pcBestIndividual);

	if (!b_stop)
	{
		if (i_running_time_sec < pcTimeCounter->dGetTimePassed())
			b_stop = true;
		else
			b_stop = false;
	}//if (!b_stop)

	return b_stop;
}//bool CRunningTimeStopCondition<TGenotype, TFenotype>::bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual)

template class CRunningTimeStopCondition<CRealCoding, CRealCoding>;