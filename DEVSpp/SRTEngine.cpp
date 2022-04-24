/*************************************************************************
 * DEVS++: DEVS Open Source in C++                                       *
 * Copyright (C) 2005-2009  Moon Ho Hwang <moonho.hwang@gmail.com>       *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU Lesser General Public            *
 * License as published by the Free Software Foundation; either          *
 * version 2.1 of the License, or (at your option) any later version.    *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 * Lesser General Public License for more details.                       *
 *                                                                       *
 * You should have received a copy of the GNU Lesser General Public      *
 * License along with this library; if not, write to the Free Software   *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, *
 * USA.                                                                  *
 *************************************************************************/
#pragma warning(disable:4503)
#include "SRTEngine.h"
#include "DEVS_Exception.h"
#include "ClassAdmin.h"
#include <iostream>
#include <math.h>
#include "Coupled.h"
#include "Atomic.h"
#include <stdio.h>
#include <time.h>
#include "StrUtil.h"

#ifdef SLEEP 
#undef SLEEP
#endif 
 
#if defined(WIN32)
#include <windows.h> // <afx.h> 
#define CLEAR_SCREEN() system("cls")
#define SLEEP(n) Sleep(n)
#else
#include <unistd.h>
#define SLEEP(n) sleep(n)
#endif

#define ENDL "\n"

DWORD day_start = 0;
ofstream   OFile;
const char gfname[] = "devspp_log.txt";

using namespace DEVSpp;

#if defined(WIN32)
	HANDLE    m_hThread(0L);
//friend UINT RootCoordiThread(LPVOID lpParam);
#else
	pthread_t m_hThread(0L);
//friend void* RootCoordiThread(void* lpParam);
#endif

#define MILLISECS_PER_DAY 24*60*60*1000

double t_975[]={
	12.706, 4.303, 3.182, 2.776, 2.571,
	 2.447, 2.365, 2.306, 2.262, 2.228,
	 2.201, 2.179, 2.160, 2.145, 2.131,
	 2.120, 2.110, 2.101, 2.093, 2.086,
	 2.080, 2.074, 2.069, 2.064, 2.060,
	 2.056, 2.052, 2.048, 2.045, 2.042
};

//--------- "<<" will be used for logging transitions  ----
const Logger& Logger::operator<<(const string& v) const
{
	cout << v;
	if(SRTEngine::GetLogOn() && SRTEngine::GetLogTransition()){
		if(OFile.is_open() == false)
			OFile.open(gfname, ios_base::app);
		OFile << v;
		OFile.close();
	}
	return *this;
} 

const Logger& Logger::operator<<(const double& v) const
{
	cout << v;
	if(SRTEngine::GetLogOn() && SRTEngine::GetLogTransition()){
		if(OFile.is_open() == false)
			OFile.open(gfname, ios_base::app);
		OFile << v;
		OFile.close();
	}
	return *this;
}
//--------------------------------------------------------------

void Logger::PrintString(const string& v, bool flag)
{
	cout << v;
	if(SRTEngine::GetLogOn() && flag){
		if(OFile.is_open() == false)
			OFile.open(gfname, ios_base::app);
		OFile << v;
		OFile.close();
	}
}

void Logger::PrintDouble(double v, bool flag)
{
	cout << v;
	if(SRTEngine::GetLogOn() && flag){
		if(OFile.is_open() == false)
			OFile.open(gfname, ios_base::app);
		OFile << v;
		OFile.close();
	}
}

Logger SRTEngine::CLog;

Time GStartingRunTime;

unsigned long DEVSpp::milliseconds()
{
    SYSTEMTIME t_now;
	GetSystemTime(&t_now);
	return  t_now.wMilliseconds
			+ 1000*(60*(60*t_now.wHour + t_now.wMinute)+t_now.wSecond)+
			+ (t_now.wDay-day_start)*MILLISECS_PER_DAY;
}

/**
The ThreadProc function acts as the starting point for executing activity
threads.
*/
#if defined(WIN32)
DWORD WINAPI ThreadProc(LPVOID activity)
#else
void* ThreadProc(void* lpParam)
#endif
{
	SRTEngine* a = (SRTEngine*)activity;
	a->Loop();
	return 0;
}

unsigned SRTEngine::m_bLog = (1 | 2); // on, performance

SRTEngine::SRTEngine(Devs& model, Time ending_t, CallBack cbf): 
m_model(model), m_ending_time(ending_t), call_back_method(cbf)//, m_bigmouth(bigmouth)  
{
	m_StepMode = GO_THROUGH;  
	m_time_scale = 1; //v default: real time
    InitControlVariables();
	
	SYSTEMTIME t_now;
	GetSystemTime(&t_now);
	day_start = t_now.wDay;
	m_psm = SRTEngine::P_relative;  
	b_acs = true; // -- animation one 
	m_animation_interval = 0.25;

	OFile.open(gfname);
	char dateStr [9];
	char timeStr [9];
	char buff[100];
	string str="DEVS++: C++ Open Source of DEVS Implementation, (C) 2005~2009, http://odevspp.sourceforge.net\n";
	_strdate( dateStr);	sprintf(buff, "The current date is %s \n", dateStr);str += buff;
	_strtime( timeStr );sprintf(buff, "The current time is %s \n", timeStr);str += buff;
 
	CLog.PrintString(str, true);

#if defined(WIN32)
	DWORD dwThreadId;
	m_hThread = ::CreateThread(NULL, 0, 
					(LPTHREAD_START_ROUTINE)ThreadProc, 
					(void*)this, 0, &dwThreadId);
#else
	if(::pthread_create(&m_hThread, NULL,
			ThreadProc, (void*)this) != 0) 
	m_hThread = 0;// it might by necessary ... but...
#endif
}

SRTEngine::~SRTEngine() 
{ 
	KillLoop(); 
	OFile.close();
}


void SRTEngine::StampClockTime()
{
	m_PreviousClockTime = milliseconds(); // remember the starting clock time
}
 
void SRTEngine::SetTimeScale(double ts)
{
	if(fabs(ts)<1.E-6 || ts > 1.E+6)
		m_time_scale = DBL_MAX;
	else 
        m_time_scale = ts;
}

void SRTEngine::Reset()
{
    Pause();  
	Devs::t_Current = 0.0;
	m_model.when_receive_init();  
}

void SRTEngine::Pause() 
{
	m_bPauseComp = false;
	m_bPauseReq = true;
	while(!m_bPauseComp)
		SLEEP(10);
}

void SRTEngine::KillLoop()
{
	m_bExitReq = true; // making it true 
	while(!m_bExitComp)
		SLEEP(10);
}

/// Inject a portvalue into the simulator's input bag
void SRTEngine::Inject(PortValue x)
{
    if (m_bPauseComp == true)
        StampClockTime(); // stemp the invoking time
	m_ExtX.push_front(x); // add x to m_ExtX buffer
	m_bPauseReq = false; // awacken 
}
/// Initialize control Variables
void SRTEngine::InitControlVariables()
{
	Devs::t_Current = 0.0;
	m_model.when_receive_init();  
	m_ExtX.clear();
	m_bPauseReq = true;  
	m_bPauseComp = true; 
	m_bExitReq = false;  
	m_bExitComp = false;  
}

/// Set Runing Flag on called by Outside 
void SRTEngine::Step()
{
    if (m_bPauseReq == false && m_StepMode == STEP_BY_ONE_SCHEDULE)
        return;

    m_StepMode = STEP_BY_ONE_SCHEDULE;
    StampClockTime();
    m_bPauseReq = false;
}

/// Starter for a simulation run
void SRTEngine::Run(unsigned n)
{	 
	m_no_sim_run = n; // no simulation run
	if(m_bPauseReq == true){ 
		m_StepMode = GO_THROUGH; 
		StampClockTime();
		GStartingRunTime = milliseconds(); // 
		m_bPauseReq = false;  
	}
}

/// Continue to step, run, mrun
void SRTEngine::Continue()
{	 
	if(m_bPauseComp){ 
		StampClockTime();
		m_bPauseReq = false;  
	}
}

void SRTEngine::MultiRun(unsigned n)
{
	m_current_run = 0;

	char buff[100];
	if(m_ending_time == DBL_MAX)
		sprintf(buff, "Simulation Ending Time: INFINITY \nNo. of Repetition: %d \n", n);
	else
		sprintf(buff, "Simulation Ending Time: %.3lf \nNo. of Repetition: %d \n", m_ending_time, n);
	CLog.PrintString(string(buff), true);
	Run(n);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void SRTEngine::ScheduleExecution()
{
	if (m_model.TimeCurrent() >= m_ending_time){
		m_model.t_Current = m_ending_time;// adjust to the ending time
		cout <<  "The current time has reached to the simulation ending time.\n";
		cout << m_menu;
		m_bPauseReq = true; 
	}
	else if (m_model.TimeNext() == DBL_MAX)
	{
		cout << "All models are passive.\n";
		cout << m_menu;
		m_bPauseReq = true; 
	}
	else {
		try{
			if(b_acs)
				CLEAR_SCREEN();
			else if(m_psm != P_NONE)
				CLog << ENDL;
			if(m_psm != P_NONE)
				CLog << m_model.Get_q((m_psm==P_relative)) 
					 << ENDL " --({" ;
			m_model.when_receive_star((m_psm != P_NONE)); 
			if(m_psm != P_NONE){
				CLog << "},t_c=" << m_model.TimeCurrent()<<")--> " << ENDL
					 << m_model.Get_q((m_psm==P_relative)) << ENDL << ENDL;
			}
		}
		catch(Exception& e)
		{
			m_bExitReq = true;
			cout << e.err_str << ENDL;
			throw e;
		}
		if ( m_StepMode == STEP_BY_ONE_SCHEDULE) {// stop looping
			m_bPauseReq = true;
			cout << m_menu;
		}
	}
}
//------------------- (9) -----------------------
void SRTEngine::AnimationExecution()
{
	if (m_model.TimeCurrent() >= m_ending_time){
		m_model.t_Current = m_ending_time;// adjust to the ending time
		CLEAR_SCREEN();
		cout << m_model.Get_q((m_psm==P_relative)) << " at " << m_model.TimeCurrent() << ENDL;
		cout << "The current time has reached to the simulation ending time." << ENDL;
		cout << m_menu;
		m_bPauseReq = true;
	}
	else {
		try{
			CLEAR_SCREEN();
			cout << m_model.Get_q((m_psm==P_relative)) << " at " << m_model.TimeCurrent() << ENDL
				 << m_menu;
		}
		catch(Exception& e)
		{
			m_bExitReq = true;
			cout << e.err_str << ENDL;
			throw e;
		}
	}
}
//++++++++++++++++++++++++++++ (6) ++++++++++++++++++++++++++++++
void SRTEngine::TransmitExtEvent()
{
	/*if (m_model.TimeCurrent() >= m_ending_time){
		cout << "The current time has reached to the simulation ending time." << ENDL;
		m_bPauseReq = true; 
	}
	else*/ 
	{
		//Devs::t_Current = m_model.TimeNext(); // adjust to the schedule
		while(m_ExtX.empty() == false)
		{
			PortValue pv = m_ExtX.front(); m_ExtX.pop_front();
			try{
				if(b_acs)
					CLEAR_SCREEN();
				else if(m_psm != P_NONE)
					CLog << ENDL;
				if(m_psm != P_NONE)
					CLog << m_model.Get_q((m_psm==P_relative)) 
					<< ENDL << " --({?" << pv.STR() ;
				m_model.when_receive_x(pv,(m_psm != P_NONE));
				if(m_psm != P_NONE){
					CLog << "}, t_c=" << m_model.TimeCurrent()<<")--> " << ENDL
						 << m_model.Get_q((m_psm==P_relative)) << ENDL << ENDL;
				}
			}
			catch(Exception& e)
			{
				m_bExitReq = true;
				cout << e.err_str << ENDL;
				throw e;
			}
		}
		if (m_model.TimeNext() == DBL_MAX)
		{
			cout << "All models are passive." << ENDL;
			cout << m_menu;
			m_bPauseReq = true; 
		}
		else if (m_StepMode == STEP_BY_ONE_SCHEDULE || m_model.TimeCurrent() >= m_ending_time) {  // stop looping
			m_bPauseReq = true; 
			cout << m_menu;
		}
	}
}

void CollectPerformanceOfaRun(const Devs& model, map<string, map<string, vector<double> > >& ms)
{
	deque<Atomic*> LD;
	if(model.IsKindOf(DEVS_RT(Coupled)))
		((Coupled&)model).GetLeafChildren(LD);
	else if(model.IsKindOf(DEVS_RT(Atomic)))
		LD.push_back((Atomic*) &model);

	for(int i=0; i < (int)LD.size(); i++)
	{
		if(LD[i]->CollectStatisticsFlag()) { 
			string hname = HierName(LD[i]);
			map<string, double> statistics = LD[i]->GetPerformance();
			for(map<string, double>::iterator it = statistics.begin(); it!=statistics.end(); it++)
			{
				string item = it->first;
				double value = it->second;
				if(ms.find(hname) == ms.end())// hname is NOT registered yet!
					ms.insert(make_pair(hname, map<string, vector<double> >()));
				if(ms[hname].find(item) == ms[hname].end()) // item is NOT registered yet!
					ms[hname].insert(make_pair(item, vector<double>()));
				ms[hname][item].push_back(value);
			}
		}
	}
}

void PrintPerformanceOfTotalRuns(const map<string, map<string, vector<double> > >& ms)
{
	SRTEngine::CLog.PrintString("============= Total Performance Indexes ========= \n", SRTEngine::GetLogPerformance());
	for(map<string, map<string, vector<double> > >::const_iterator i = ms.begin(); i != ms.end(); i++)
	{
		string hname = i->first;
		SRTEngine::CLog.PrintString(hname+ENDL, SRTEngine::GetLogPerformance());
		map<string, vector<double> > statistics = i->second;

		for(map<string, vector<double> >::iterator it = statistics.begin(); it!=statistics.end(); it++)
		{
			string item = it->first;
			vector<double> values = it->second;
			double mean = 0;
			int n = (int)values.size();
			for(int j=0; j<n; j++) 
				mean += values[j];
			mean = mean/n;
			double variance = 0;
			for(int k=0; k<n; k++) 
				variance += (values[k]-mean)*(values[k]-mean);

			double standard_deviation = 0;
			if(n>1)
				standard_deviation = sqrt(variance/(n-1)/n);

			SRTEngine::CLog.PrintString(item+": ", SRTEngine::GetLogPerformance());
			SRTEngine::CLog.PrintDouble(mean, SRTEngine::GetLogPerformance());
			if(n>1 && n<30) {
				int df = n-1; // degree of freedome = n-1, array index = df-1
				SRTEngine::CLog.PrintString(", 95% CI: [", SRTEngine::GetLogPerformance());
				SRTEngine::CLog.PrintDouble(mean-standard_deviation*t_975[df-1], SRTEngine::GetLogPerformance());
				SRTEngine::CLog.PrintString(", ", SRTEngine::GetLogPerformance());
				SRTEngine::CLog.PrintDouble(mean+standard_deviation*t_975[df-1], SRTEngine::GetLogPerformance());
				SRTEngine::CLog.PrintString("]", SRTEngine::GetLogPerformance());
			}
			SRTEngine::CLog.PrintString(string(ENDL), SRTEngine::GetLogPerformance());
		}
		SRTEngine::CLog.PrintString(string(ENDL), SRTEngine::GetLogPerformance());
	}
}

void SRTEngine::PrintPerformanceOfaRun() const
{
	deque<Atomic*> LD;
	if(m_model.IsKindOf(DEVS_RT(Coupled)))
		((Coupled&)m_model).GetLeafChildren(LD);
	else if(m_model.IsKindOf(DEVS_RT(Atomic)))
		LD.push_back((Atomic*) &m_model);

	char buf[1000];
	sprintf(buf, "============= Performance Indexes of Sim. Run %d ========= \n", m_current_run);
	CLog.PrintString(buf, GetLogPerformance());
	sprintf(buf, "CPU Run Time: %f sec. \n", (milliseconds()-GStartingRunTime)/1000.0);
	CLog.PrintString(buf, GetLogPerformance());

	for(int i=0; i < (int)LD.size(); i++)
	{
		if(LD[i]->CollectStatisticsFlag()) { 
			CLog.PrintString(HierName(LD[i])+ENDL, GetLogPerformance());
			map<string, double> statistics = LD[i]->GetPerformance();
			for(map<string,double>::iterator it = statistics.begin(); it!=statistics.end(); it++)
			{
				CLog.PrintString(it->first+": ", GetLogPerformance());
				CLog.PrintDouble(it->second, GetLogPerformance());
				CLog.PrintString(string(ENDL), GetLogPerformance());
			}
			CLog.PrintString(string(ENDL), GetLogPerformance());
		}
	}
}

void SRTEngine::PrintTotalState() const
//const Devs& model, SRTEngine::PrintStateMode m_psm)
{
	string str = m_model.Get_q(m_psm==SRTEngine::P_relative);
	str += " at "; 
	char tmp[10];
	sprintf(tmp, "%.3lf", Devs::TimeCurrent());
	str += string(tmp) + ENDL + ENDL;
	CLog.PrintString(str, GetLogTotalState());
}

void SRTEngine::Loop()
{
	unsigned long PrevAniClock = milliseconds();
	while (!m_bExitReq)
	{
		if (!m_bPauseReq)
		{
			if (m_ExtX.size() > 0) // external input 
				TransmitExtEvent(); //-- (6)
			else {
				if(m_time_scale == DBL_MAX){ // pure virtual simulation, that is, discrete event simulation
					Time nt = min(m_model.TimeNext(), m_ending_time);
					if(nt < DBL_MAX) {
						Devs::t_Current = nt;
						ScheduleExecution(); 
					}
					else{
						cout << "All models are passive.\n";
						cout << m_menu;
						m_bPauseReq = true; 
					}
				}
				else {
					unsigned long nowClock = milliseconds();
					double delta = (((double)(nowClock - m_PreviousClockTime)) / 1.0E+3) * m_time_scale;
					
					if(delta > 1E-05)  
					{
						Devs::t_Current += delta;
						/*if(m_StepMode == STEP_BY_ONE_SCHEDULE && m_model.TimeNext() == DBL_MAX) 
							m_bPauseReq = false;
						else */ //  for passive model & waiting user input 
						Time nt = min(m_model.TimeNext(), m_ending_time);
						double diff = nt - Devs::t_Current;
						//if( diff < -1E-03) // too late
						//	cout << "VIOLATING real-time constraint of 1E-03 by " << diff << "!" << ENDL;
						if( diff < 1E-03) { // schedule time reached!
							Devs::t_Current = nt; // adjust to tN
							ScheduleExecution();  
						}
						m_PreviousClockTime = nowClock; // update previous clock time
					} 
					{
						double ani_delta = (((double)(nowClock - PrevAniClock)) / 1.0E+3);
						if(!m_bPauseReq && b_acs && ani_delta > m_animation_interval){
							AnimationExecution(); // add collect statis inside here later on !!!!!
							PrevAniClock = nowClock;
						}else
							continue;
					}
				}
			}
			//if(m_bigmouth) cout << "Time : " << Devs::t_Current << ENDL;
		}
		if(m_bPauseReq == true )//  if(m_bPauseComp == false)
		{
			m_model.when_receive_cs();
			if( m_current_run < m_no_sim_run && DEVSpp::IsEqual(m_model.TimeCurrent(), m_ending_time, 1.E-5))
			{ // need to repeat
				++m_current_run;
				PrintPerformanceOfaRun(); // increase m_current_run by one 
				CollectPerformanceOfaRun(m_model, m_multi_statistics);
				PrintTotalState();
				CLog.PrintString(string("----------------------------------------")+ENDL, true);
				if(m_current_run < m_no_sim_run)
				{
					Devs::t_Current = 0.0;
					m_model.when_receive_init();  
					Run(m_no_sim_run); // inside m_bPauseReq = false;
				}
				else // m_current_run == m_no_sim_run
				{ // COLLECT BATCH STATISTICS !
					m_bPauseComp = true;
					PrintPerformanceOfTotalRuns(m_multi_statistics); // increase m_current_run by one 
					CLog.PrintString(string("========== Simulation Run Completed! ==========")+ENDL, true);
				}
			}else
				m_bPauseComp = true;
			if(m_bPauseComp == true)
				SLEEP(100); // slow down for checking restart
		}
		else 
		{
			if(m_time_scale < DBL_MAX) //  not fastest run
				SLEEP(10); // sleep 0.01 sec
			 // otherwise, don't sleep, keep going
		}
	}	
//-- simulation exception 
	m_bExitComp = true;
}

void Print_dtmode(SRTEngine::PrintStateMode m_psm)
{
	cout << "current dt_mode: "; 
	if( m_psm==SRTEngine::P_NONE) cout << "none" << endl;
	else if(m_psm==SRTEngine::P_relative) 
		cout <<"relative";
	else cout << "absolute";
	
	if(m_psm!=SRTEngine::P_NONE) {
		if(m_psm==SRTEngine::P_relative)
			cout << " [(state, t_s, t_e), " ;
		else cout << " [(state, t_l, t_n), " ;
	}
	cout << "\n" ;
}

void Print_animode(bool b_acs, double m_animation_interval)
{
	cout << "current animation mode: ";
	if(b_acs) cout << " on and interval= " << m_animation_interval;
	else cout << "off";
	cout << "\n";
}

void Print_log_settings()
{
	cout<< "current log setting: " 
		<< (SRTEngine::GetLogOn()? "on, ": "off, ") 
		<< (SRTEngine::GetLogPerformance()? "p": "0")
		<< (SRTEngine::GetLogTotalState()? "q":"0") 
		<< (SRTEngine::GetLogTransition()? "t":"0");
	cout << "\n";
}

void SRTEngine::PrintSettings() const
{
	cout << "scale factor: ";
	if( GetTimeScale()==DBL_MAX) cout << "inf" << endl;
	else cout << GetTimeScale() << endl;

	cout << ((m_StepMode==STEP_BY_ONE_SCHEDULE) ? "step-by-step mode" 
												: "run-through mode") << endl;
	cout << "current time: " << Devs::TimeCurrent() << endl;
	cout << "simulation ending time: " << GetEndingTime() << endl;
	Print_dtmode(m_psm);
	Print_animode(b_acs, m_animation_interval);
	Print_log_settings();
}
void SRTEngine::PrintCouplings() const
{
	if(m_model.IsKindOf(DEVS_RT(Coupled)))
		((Coupled&)m_model).PrintCouplings();
	else
		cout << "Devs is not coupled DEVS!" << ENDL;
}

void SRTEngine::SetLogOn(bool flag) 
{ 
	if(flag != GetLogOn() ) m_bLog ^=  1;
	//if(flag)
	//	OFile.open(gfname);
	//else
	//	OFile.close();
}

void SRTEngine::RunConsoleMenu()
{
	m_menu="\nscale, step, run, mrun, [p]ause, pause_at, [c]ontinue, reset, rerun, [i]nject, \ndtmode, animode, print, cls, log, [e]xit \n>";

	string input;
	while(1)
	{
		cout << m_menu;
		cin >> input;
		
		if(input=="scale")
		{	double value;
			cout << "current scale value: ";
			if(GetTimeScale()==DBL_MAX) 
				cout << "inf" ;
			else 
				cout << GetTimeScale();
			cout << ";  new value > "; 
			cin >> value; 
			SetTimeScale(value);
		}
		else if(input == "run")
			MultiRun(1);
		else if(input == "mrun")
		{   cout << "No. of repetition: "; unsigned nr;
			cin >> nr;
			MultiRun(nr);
		}
		else if(input == "step") Step();
		else if(input == "pause" || input == "p") Pause();
		else if(input == "continue" || input == "c") Continue();
		else if(input == "pause_at"){ Time at_time;
			cout << "ending time > "; cin >> at_time;  
			if( Devs::TimeCurrent() <= at_time )
				SetEndingTime(at_time); 
			else 
				cout << "Ending time should be greater than or equal to " <<  Devs::TimeCurrent() << ENDL;
		}
		else if(input=="reset") Reset();
		else if(input=="rerun") Rerun();
		else if (input == "inject" || input =="i" ){
			if(call_back_method==NULL)
				cout << "The call back function is not defined!" << ENDL;
			else {
				PortValue pv = call_back_method(m_model);
				Inject(pv); // PortValue(InputPort("in"), new Client(60)));
			}
		}
		else if (input == "dtmode"){
			Print_dtmode(m_psm);
			cout << "options: none rel abs nc >";
			cin >> input;  
			if(input == "none")  
				Set_dtmode(SRTEngine::P_NONE);
			else if(input == "rel")
				Set_dtmode(SRTEngine::P_relative);			
			else if(input == "abs")
				Set_dtmode(SRTEngine::P_absolute);			
			else if(input == "nc") 
				;
			else
				cout << "Invalid input!" << ENDL;

		}else if(input == "animode")
		{
			Print_animode(b_acs, m_animation_interval);
			cout << "options: none ani nc >";
			cin >> input;  
			if (input == "none")
				SetAnimationFlag(false); // for animation;
			else if(input == "nc") 
				;
			else { double ai=::atof(input.data());
				if(ai-1E-2 > 0) {
					SetAnimationFlag(true);
					SetAnimationInterval(ai);
				}else
					cout << "Invalid input!" << ENDL;
			}
		}
		else if(input == "print") {
			cout << "options: q cpl s p > ";
			cin >> input;  
			if(input=="q")
				PrintTotalState();
			else if(input == "cpl") {
				PrintCouplings();
			}else if(input == "s")
				PrintSettings();
			else if(input == "p")
				PrintPerformanceOfaRun();
			else
				cout << "Invalid input!" << ENDL;
		}else if(input == "log")
		{
			Print_log_settings();
			cout << "options: on off {+,-}{pqt} nc > "; 
			cin >> input;
			if(input=="on") SetLogOn(true);
			else if(input=="off") SetLogOn(false);
			else if(input=="nc") 
				continue;
			else if(input.find('+') != -1 ){
				if(input.find('p') != -1 )
					SetLogPerformance(true);
				if(input.find('q') != -1 )
					SetLogTotalState(true);
				if(input.find('t') != -1)
					SetLogTransition(true);
				Print_log_settings();
			}
			else if(input.find('-')!= -1){
				if(input.find('p') != -1 )
					SetLogPerformance(false);
				if(input.find('q') != -1 )
					SetLogTotalState(false);
				if(input.find('t') != -1)
					SetLogTransition(false);
				Print_log_settings();
			}
			else
				cout << "Invalid input! " <<endl;
		}
		else if(input == "cls")
			CLEAR_SCREEN();
		else if (input =="e" || input == "exit")
			break;
	}
	KillLoop();
}