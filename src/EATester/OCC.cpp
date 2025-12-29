#include "OCC.h"

COCC::COCC(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)
	: COptimizer<CRealCoding, CRealCoding>(pcProblem, pcLog, iRandomSeed)
{

}//COCC::COCC(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)

COCC::COCC(COCC *pcOther)
	: COptimizer<CRealCoding, CRealCoding>(pcOther)
{

}//COCC::COCC(COCCCMAES *pcOther)

COCC::~COCC()
{
	v_clear_cmaeses();
}//COCC::~COCC()

void COCC::vInitialize()
{
	v_clear_cmaeses();

	CRealCoding *pc_sample = pc_problem->pcGetEvaluation()->pcCreateSampleFenotype();

	CRealRandomGeneration c_generation(pc_sample);

	CIndividual<CRealCoding, CRealCoding> *pc_random = new CIndividual<CRealCoding, CRealCoding>(c_generation.pcGenerate(), pc_problem);

	pc_random->vEvaluate();

	b_update_best_individual(0, pc_random);

	v_component_indices.resize(pc_problem->pcGetEvaluation()->iGetNumberOfElements(), vector<int>());

	for (uint16_t i = 0; i < pc_problem->pcGetEvaluation()->iGetNumberOfSubEvaluations(); i++)
	{
		v_cmaeses.push_back(pc_create_cmaes(i, 0));
		v_local_bests.push_back(new CIndividual<CRealCoding, CRealCoding>(pc_random));
	}//for (uint16_t i = 0; i < pc_problem->pcGetEvaluation()->iGetNumberOfSubEvaluations(); i++)

	v_improvement_values.resize(v_cmaeses.size(), 0);
	v_last_improvements.resize(v_cmaeses.size(), 0);
	v_last_iterations.resize(v_cmaeses.size(), 1);
	v_locally_update_counter.resize(v_cmaeses.size(), 0);

	delete pc_sample;
	delete pc_random;
}//void COCC::vInitialize()

bool COCC::bRunIteration(uint32_t iIterationNumber)
{
	bool b_updated = false;

	vector<uint16_t> v_order;
	v_order.resize(v_cmaeses.size());
	iota(v_order.begin(), v_order.end(), 0);

	random_shuffle(v_order.begin(), v_order.end());

	uint32_t i_counter = 0;

	vector<uint16_t> v_improvements;

	vector<double> v_last_improvement_values(v_order.size());

	vector<uint16_t> v_ranking(v_cmaeses.size());

	iota(v_ranking.begin(), v_ranking.end(), 0);

	sort(v_ranking.begin(), v_ranking.end(), [&](uint16_t i0, uint16_t i1)
	{
		return v_improvement_values[i0] > v_improvement_values[i1];
	});

	vector<int> v_iterations_to_add(v_order.size(), 0);

	for (size_t i = 0; i < v_order.size() && !pc_stop_condition->bStop(&c_optimizer_timer, iIterationNumber, pc_problem->pcGetEvaluation()->iGetFFE(), pc_best_individual); i++)
	{
		double d_best_before = -pc_best_individual->dGetFitnessValue();

		uint32_t i_iterations = 0;

		uint32_t i_last_impr_temp = v_last_improvements[v_order[i]];

		bool b_local_updated = false;

		int i_iterations_to_do = min(max((int)std::floor(v_last_iterations[v_order[i]]), 0), (int)v_cmaeses[v_order[i]]->dims.size());

		while (i_iterations < i_iterations_to_do)
		{
			if (b_run_cmaes(v_cmaeses[v_order[i]], v_local_bests[v_order[i]], iIterationNumber, v_order[i]))
			{
				if (!b_local_updated)
				{
					i_counter++;
					b_updated = true;
					v_improvements.push_back(v_order[i]);
					v_last_improvements[v_order[i]] = iIterationNumber;

					b_local_updated = true;
				}//if (!b_local_updated)

				v_locally_update_counter[v_order[i]]++;
			}//if (b_run_cmaes(v_cmaeses[v_order[i]], v_local_bests[v_order[i]], iIterationNumber, v_order[i]))

			i_iterations++;
		}//while (i_iterations < i_iterations_to_do)

		if (b_local_updated)
		{
			vector<int> &v_neighbours = v_cmaeses[v_order[i]]->getSharedVariables();

			v_last_iterations[v_order[i]] = max(0, v_last_iterations[v_order[i]] - 2);
			v_iterations_to_add[v_order[i]] = 0;

			for (size_t j = 0; j < v_neighbours.size(); j++)
			{
				v_iterations_to_add[v_neighbours[j]] += 1;
			}//for (size_t j = 0; j < v_neighbours.size(); j++)
		}//if (b_local_updated)
	}//for (size_t i = 0; i < v_order.size() && !pc_stop_condition->bStop(&c_optimizer_timer, iIterationNumber, pc_problem->pcGetEvaluation()->iGetFFE(), pc_best_individual); i++)

	vector<int> contrindexRank;
	for (int i = 0; i < (int)v_cmaeses.size(); i++) {
		contrindexRank.push_back(i);
	}
	for (int i = 0; i < (int)v_cmaeses.size() - 1; i++) {
		for (int j = 0; j < (int)v_cmaeses.size() - 1 - i; j++) {
			if (v_improvement_values[contrindexRank[j]] < v_improvement_values[contrindexRank[j + 1]]) {
				int temp = contrindexRank[j];
				contrindexRank[j] = contrindexRank[j + 1];
				contrindexRank[j + 1] = temp;
			}
		}
	}
	vector<int> awardlist;
	double biggestContri = v_improvement_values[contrindexRank[0]];
	for (int i = 0; i < (int)v_cmaeses.size(); i++) {
		if (biggestContri / v_improvement_values[contrindexRank[i]] < 2.0) {
			awardlist.push_back(contrindexRank[i]);
		}
		else {
			break;
		}
	}

	if (awardlist.size() == v_cmaeses.size()) {
		awardlist.clear();
	}

	random_shuffle(awardlist.begin(), awardlist.end());

	for (auto const e : awardlist)
	{
		if (b_run_cmaes(v_cmaeses[e], v_local_bests[e], iIterationNumber, e))
		{
			vector<int> &v_neighbours = v_cmaeses[e]->getSharedVariables();

			b_updated = true;
			v_last_improvements[e] = iIterationNumber;

			v_last_iterations[e] = max(0, v_last_iterations[e] - 2);
			v_iterations_to_add[e] = 0;

			for (size_t j = 0; j < v_neighbours.size(); j++)
			{
				v_iterations_to_add[v_neighbours[j]] += 1;
			}//for (size_t j = 0; j < v_neighbours.size(); j++)

			v_locally_update_counter[e]++;
		}//if (b_run_cmaes(v_cmaeses[e], v_local_bests[e], iIterationNumber, e))
	}//for (auto const e : awardlist)

	for (size_t i = 0; i < v_cmaeses.size(); i++)
	{
		v_last_iterations[i] += v_iterations_to_add[i];
	}//for (size_t i = 0; i < v_cmaeses.size(); i++)

	sort(v_improvements.begin(), v_improvements.end());

	CString s_improvements_log_message;

	s_improvements_log_message.Append("[");

	for (size_t i = 0; i < v_improvements.size(); i++)
	{
		if (i > 0)
		{
			s_improvements_log_message.Append(", ");
		}

		s_improvements_log_message.AppendFormat("%d", v_improvements[i]);
	}

	s_improvements_log_message.Append("]");

	CString s_log_message;

	s_log_message.Format
	(
		"iteration: %u; best fitness: %f; ffe: %u; time: %.2lf; updated: %.2lf; impr: %s",
		iIterationNumber,
		pc_best_individual->dGetFitnessValue(),
		pc_problem->pcGetEvaluation()->iGetFFE(),
		c_optimizer_timer.dGetTimePassed(),
		(double)i_counter / v_cmaeses.size(),
		s_improvements_log_message
	);//s_log_message.Format

	pc_log->vPrintLine(s_log_message, true);

	CString s_last_improvements_log;

	for (size_t i = 0; i < v_last_improvements.size(); i++)
	{
		s_last_improvements_log.AppendFormat("[%d]: %d (%f); ", i, v_last_improvements[i], v_last_iterations[i]);
	}//for (size_t i = 0; i < v_last_improvements.size(); i++)

	pc_log->vPrintLine(s_last_improvements_log, false);

	return b_updated;
}//bool COCC::bRunIteration(uint32_t iIterationNumber)

CString COCC::sAdditionalSummaryInfo()
{
	CString s_additional_summary;

	for (size_t i = 0; i < v_locally_update_counter.size(); i++)
	{
		s_additional_summary.AppendFormat("%d\t", v_locally_update_counter[i]);
	}//for (size_t i = 0; i < v_locally_update_counter.size(); i++)

	return s_additional_summary;
}//CString COCC::sAdditionalSummaryInfo()

bool COCC::b_run_cmaes(CMAESO *pcCMAES, CIndividual<CRealCoding, CRealCoding> *pcLocalBest, uint32_t iIterationNumber, uint16_t iSubEvaluationIndex)
{
	long i_used_ffe = 0;

	bool updated = false;

	CIndividual<CRealCoding, CRealCoding> c_individual(pc_best_individual);

	double d_fitness_value = -c_individual.dGetFitnessValue();
	pcCMAES->generateSubsolution();
	pcCMAES->calculateFitness(c_individual.pcGetGenotype()->pdGetValues(), (int)c_individual.pcGetGenotype()->iGetNumberOfDimensions(), i_used_ffe);

	updated = pcCMAES->updateContextVector(c_individual.pcGetGenotype()->pdGetValues(), d_fitness_value, (int)c_individual.pcGetGenotype()->iGetNumberOfDimensions());
	pcCMAES->updateTheDistribution();

	if (-d_fitness_value > c_individual.dGetFitnessValue())
	{
		c_individual.vSetFitnessValue(-d_fitness_value);
	}//if (-d_fitness_value > c_individual.dGetFitnessValue())

	double d_best_before = -pc_best_individual->dGetFitnessValue();

	updated = b_update_best_individual(iIterationNumber, &c_individual);

	double d_best_after = -pc_best_individual->dGetFitnessValue();

	v_improvement_values[iSubEvaluationIndex] = 0.5 * v_improvement_values[iSubEvaluationIndex] + 0.5 * (d_best_before - d_best_after);

	return updated;
}//bool COCC::b_run_cmaes(CMAESO *pcCMAES, CIndividual<CRealCoding, CRealCoding> *pcLocalBest, uint32_t iIterationNumber, uint16_t iSubEvaluationIndex)

CMAESO * COCC::pc_create_cmaes(uint16_t iSubEvaluationIndex, uint32_t iNumberOfHops)
{
	vector<uint16_t> v_variables;
	vector<uint16_t> v_sub_evaluations;

	vector<uint16_t> v_buf;

	pc_problem->pcGetEvaluation()->vGetGenesForSubEvaluation(iSubEvaluationIndex, &v_buf);

	vector<uint16_t> v_buf_buf;

	unordered_set<uint16_t> s_variables(v_buf.begin(), v_buf.end());

	vector<int> v_shared_variables;

	s_variables.clear();

	for (size_t i = 0; i < v_buf.size(); i++)
	{
		pc_problem->pcGetEvaluation()->vGetSubEvaluationsForGene(v_buf[i], &v_buf_buf);

		s_variables.insert(v_buf[i]);

		if (v_buf_buf.size() > 1)
		{
			v_shared_variables.push_back(v_buf[i]);
		}//if (v_buf_buf.size() > 1)
	}//for (size_t i = 0; i < v_buf.size(); i++)

	unordered_set<uint16_t> s_sub_evaluations;

	for (unordered_set<uint16_t>::iterator it = s_variables.begin(); it != s_variables.end(); it++)
	{
		v_buf.clear();

		pc_problem->pcGetEvaluation()->vGetSubEvaluationsForGene(*it, &v_buf);

		s_sub_evaluations.insert(v_buf.begin(), v_buf.end());
	}//for (unordered_set<uint16_t>::iterator it = s_variables.begin(); it != s_variables.end(); it++)

	bool b_steady_state = false;

	for (uint16_t i = 0; i < iNumberOfHops && !b_steady_state; i++)
	{
		b_steady_state = s_variables.size() == pc_problem->pcGetEvaluation()->iGetNumberOfElements();

		if (!b_steady_state)
		{
			for (unordered_set<uint16_t>::iterator it = s_variables.begin(); it != s_variables.end(); it++)
			{
				v_buf.clear();

				pc_problem->pcGetEvaluation()->vGetSubEvaluationsForGene(*it, &v_buf);

				s_sub_evaluations.insert(v_buf.begin(), v_buf.end());
			}//for (unordered_set<uint16_t>::iterator it = s_variables.begin(); it != s_variables.end(); it++)

			for (unordered_set<uint16_t>::iterator it = s_sub_evaluations.begin(); it != s_sub_evaluations.end(); it++)
			{
				v_buf.clear();

				pc_problem->pcGetEvaluation()->vGetGenesForSubEvaluation(*it, &v_buf);

				for (size_t j = 0; j < v_buf.size(); j++)
				{
					pc_problem->pcGetEvaluation()->vGetSubEvaluationsForGene(v_buf[j], &v_buf_buf);

					if (v_buf_buf.size() == 1 || i + 1 < iNumberOfHops)
					{
						if (!s_variables.count(v_buf[j]))
						{
							v_shared_variables.push_back(v_buf[j]);
						}//if (!s_variables.count(v_buf[j]))

						s_variables.insert(v_buf[j]);						
					}//if (v_buf_buf.size() == 1 || i + 1 < iNumberOfHops)
				}//if (v_buf_buf.size() == 1 || i + 1 < iNumberOfHops)
			}//for (unordered_set<uint16_t>::iterator it = s_sub_evaluations.begin(); it != s_sub_evaluations.end(); it++)
		}//if (!b_steady_state)
	}//for (uint16_t i = 0; i < iNumberOfHops && !b_steady_state; i++)

	s_sub_evaluations.erase(iSubEvaluationIndex);

	v_variables.insert(v_variables.end(), s_variables.begin(), s_variables.end());
	v_sub_evaluations.insert(v_sub_evaluations.end(), s_sub_evaluations.begin(), s_sub_evaluations.end());

	for (size_t i = 0; i < v_variables.size(); i++)
	{
		v_component_indices[v_variables[i]].push_back(iSubEvaluationIndex);
	}//for (size_t i = 0; i < v_variables.size(); i++)

	if (v_variables.empty())
	{
		return nullptr;
	}//if (v_variables.empty())

	int i_seed = (int)RandUtils::iRandNumber(0, INT32_MAX);
	int i_lambda = 4 + 3 * log((double)v_variables.size());

	return new CMAESO(pc_problem->pcGetEvaluation(), iSubEvaluationIndex, vector<int>(v_variables.begin(), v_variables.end()), i_lambda, i_seed, pc_best_individual->pcGetGenotype(), vector<int>(v_sub_evaluations.begin(), v_sub_evaluations.end()));
}//CMAESO * COCC::pc_create_cmaes(uint16_t iSubEvaluationIndex, uint32_t iNumberOfHops)

void COCC::v_clear_cmaeses()
{
	VectorUtils::vDeleteElementsAndClear(&v_cmaeses);
	VectorUtils::vDeleteElementsAndClear(&v_local_bests);
}//void COCC::v_clear_cmaeses()