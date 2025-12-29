#include "System.h"

#include "CommandParamParser.h"
#include "OptimizationCommand.h"
#include "RandUtils.h"
#include "StringCommandParam.h"
#include "UIntCommandParam.h"
#include "FloatCommandParam.h"

#include <ctime>




uint32_t CSystem::iERROR_CODE_SYSTEM_IN_FILE_NOT_FOUND = CError::iADD_ERROR("iERROR_CODE_SYSTEM_IN_FILE_NOT_FOUND");
uint32_t CSystem::iERROR_CODE_SYSTEM_SETTNGS_FILE_NOT_FOUND = CError::iADD_ERROR("iERROR_CODE_SYSTEM_SETTNGS_FILE_NOT_FOUND");

CSystem::CSystem(int iArgCount, char **ppcArgValues, CError *pcError)
{
	pf_log = nullptr;

	*pcError = e_read_arguments_from_file(iArgCount, ppcArgValues);

	if (!*pcError)
	{
		f_result_summary.open(s_result_summary_file);

		if (f_result_summary.good())
		{
			v_register_commands();
		}//if (f_result_summary.good())
		else
		{
			pcError->vSetError(CError::iERROR_CODE_SYSTEM_WRONG_FILE_PATH, s_result_summary_file);
		}//else if (f_result_summary.good())
	}//if (!*pcError)
}//CSystem::CSystem(int iArgCount, char **ppcArgValues, CError *pcError)

CSystem::~CSystem()
{
	if  (pf_log != nullptr)  delete  pf_log;

	m_arguments.clear();

	for (unordered_map<CString, CCommandExecution*>::iterator i_it = m_commands.begin(); i_it != m_commands.end(); i_it++)
	{
		delete i_it->second;
	}//for (unordered_map<CString, CCommandExecution*>::iterator i_it = m_commands.begin(); i_it != m_commands.end(); i_it++)

	m_commands.clear();

	f_result_summary.close();

	f_settings.close();
}//CSystem::~CSystem()

CError CSystem::eRun()
{
	CError c_error;
	
	CTimeCounter  c_experiment_timer;
	c_experiment_timer.vSetStartNow();
	//time_t t_start = time(nullptr);

	CCommandParamParser c_command_param_parser(&m_arguments);
	
	CStringCommandParam c_dest_file_param(SYSTEM_ARGUMENT_DESTINATION_FILE);
	CStringCommandParam c_mode_param(SYSTEM_ARGUMENT_MODE);
	CUIntCommandParam c_random_seed_param(SYSTEM_ARGUMENT_RANDOM_SEED, false);
	CFloatCommandParam c_log_system_param(SYSTEM_ARGUMENT_LOG_SYSTEM_OFFSET, false);

	
	CString  s_dest_file_param = c_dest_file_param.sGetValue(&f_settings, &c_error);

	if (!c_error)
	{
		c_error = e_create_log_file(s_dest_file_param, &s_dest_file_param);

		if (!c_error)
		{
			int  i_log_system_off;
			i_log_system_off = (int) ::round(c_log_system_param.fGetValue(&f_settings, &c_error));
			if (c_log_system_param.bHasValue() == false)  i_log_system_off = 0;
			c_log.vLogSystemToShow(i_log_system_off);
			
			CString s_mode = c_mode_param.sGetValue(&f_settings, &c_error);

			if (!c_error)
			{
				i_random_seed = c_random_seed_param.iGetValue(&f_settings, &c_error);

				if (!c_error)
				{
					if (c_random_seed_param.bHasValue())
					{
						i_random_seed = c_random_seed_param.iGetValue();
						RandUtils::vInit(i_random_seed);
					}//if (c_random_seed_param.bHasValue())
					else
					{
						i_random_seed = RandUtils::iInit();
					}//else if (c_random_seed_param.bHasValue())

					FILE  *pf_file;
					CString  s_buf;
					s_buf.Format("z_seed_%d.txt", i_random_seed);
					pf_file = fopen(s_buf, "w+");
					fprintf(pf_file, "%d", i_random_seed);
					fclose(pf_file);


					if (m_commands.count(s_mode) > 0)
					{
						c_error = m_commands.at(s_mode)->eExecute(&c_command_param_parser, i_random_seed);

						if (!c_error)
						{
							//time_t t_stop = time(nullptr);
							//uint32_t i_running_time = (uint32_t)(t_stop - t_start);
							double  d_running_time;
							d_running_time = c_experiment_timer.dGetTimePassed();

							CString s_summary;
							s_summary.Format("\t%s\t%lf\t%s\t%u", OPT_RES_FLAG_RES_RUNTIME, d_running_time, OPT_RES_FLAG_RES_RAND_SEED, i_random_seed);

							f_result_summary << s_summary << endl;
						}//if (!c_error)
					}//if (m_commands.count(s_mode) > 0)
					else
					{
						c_error.vSetError(CError::iERROR_CODE_SYSTEM_MODE_NOT_FOUND, s_mode);
					}//else if (m_commands.count(s_mode) > 0)

					::remove(s_buf);
					//pf_file = fopen(s_buf, "w+");
					//fclose(pf_file);
				}//if (!c_error)
			}//if (!c_error)


			c_error = c_log.eSave(pf_log, i_log_system_off);
			if  (c_log.iGetLineNumber(LOG_SYSTEM_PARETO_FRONT) > 0)
				c_error = c_log.eSave(s_dest_file_param + "pf.txt", LOG_SYSTEM_PARETO_FRONT);
			
			delete  pf_log;
			pf_log = nullptr;

			//c_log.eSave(s_log_file, i_log_system_off);
		}//if (!c_error)
	}//if (!c_error)

	return c_error;
}//CError CSystem::eRun()




void CSystem::v_register_commands()
{
	v_register_command(new COptimizationCommand(&f_settings, &c_log, &f_result_summary));
}//void CSystem::v_register_commands()




CError CSystem::e_create_log_file(CString  sFileName, CString  *psFinalFilename)
{ 
	CError c_err;

	int  i_file_counter;
	CString  s_effective_file_name;
	i_file_counter = 0;
	s_effective_file_name.Format("%s_%.3d.txt", sFileName, i_file_counter);
		
	while (::PathFileExists(s_effective_file_name) == true)
	{
		i_file_counter++;
		s_effective_file_name.Format("%s_%.3d.txt", sFileName, i_file_counter);
	}//while  (System::IO::File::Exists(s_file_name)  ==  true)

	pf_log = new ofstream(s_effective_file_name);

	*psFinalFilename = s_effective_file_name;

	c_log.vSetLogFile(s_effective_file_name);

	return(c_err);
}//CError CSystem::e_create_log_file(CString  sFileName)


CError CSystem::e_read_arguments_from_file(int iArgCount, char **ppcArgValues)
{
	CError c_err;

	CString  s_argument(*(ppcArgValues+1));

	if  (s_argument  !=  "")
    {
		CString  s_file_name, s_buf;
		FILE  *pf_in_file;

		s_result_summary_file.Format("%s_out.txt", s_argument);
		s_log_file.Format("%s_log.txt", s_argument);
		s_file_name.Format("%s_entry.txt", s_argument);


		pf_in_file = fopen(s_file_name, "r");
		if (pf_in_file == NULL)
		{
			c_err.vSetError(iERROR_CODE_SYSTEM_IN_FILE_NOT_FOUND, s_file_name);
			return(c_err);
		}//if  (pf_in_file  ==  NULL)


		CString  s_in_data;
		s_in_data = Tools::sReadLine(pf_in_file);
		fclose(pf_in_file);

		e_load_settings(s_in_data);
    }//if  (s_argument  !=  "")

	return(c_err);
}//CError CSystem::e_read_arguments_from_file(int iArgCount, char **ppcArgValues)



CError CSystem::e_load_settings(CString sSettingsFile)
{
	CError c_err;

	f_settings.open(sSettingsFile);

	if (!f_settings.good())
	{
		c_err.vSetError(iERROR_CODE_SYSTEM_SETTNGS_FILE_NOT_FOUND, sSettingsFile);
	}//if (!f_settings.good())

	return(c_err);
}//CError CSystem::e_load_settings(CString sSettingsFile)