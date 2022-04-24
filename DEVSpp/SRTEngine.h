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
#pragma once
#pragma warning(disable:4251)
#pragma warning(disable:4786)

#include "PortValue.h"
#include "Devs.h"
#include "DEVS_Export.h"

#include <list>
#include <float.h>

/*-------------------  Functionalities: -------------------------------
   (0) A simulation engine which has its own thread. 
   (1) Its simulation run can perform when "Run" function called.
   (2) The simuation speed is controllable according to "m_time_scale" 
       variable.
   (3) Pause: stop a simulation run.
       Reset: Pause and initialize the current time.
	   Rerun: reset and run.
   (4) Considering the schedule of simulation models
       (4.1) pure virtual simulation
	   (4.2) soft real-time simulation
   (5) Step-by-Schedule mode (as well as run-through mode)
   (6) Handling the external input event coming from outside of 
       the simulation environment
   (7) simulation ending time
   (9) animation of analytical continuous change
   (10) printing menu 
   (11) Callback function for Inject handling
-----------------------------------------------------------------------*/
namespace DEVSpp {


class Logger
{
public:
	const Logger& operator<<(const string& v) const;
	const Logger& operator<<(const double& v) const;
	void PrintString(const string& v, bool flag);
	void PrintDouble(double v, bool flag);
};
 
class DEVSpp_EXP SRTEngine
{  
	list<PortValue> m_ExtX;  

	void TransmitExtEvent();

	enum STEP_MODE { GO_THROUGH, STEP_BY_ONE_SCHEDULE };

	STEP_MODE m_StepMode;

	/// execution function of simulation model at schedule time
	void ScheduleExecution(); 

	/// hightest simulator  
	Devs& m_model;

	/// Previous Clock Time
    unsigned long m_PreviousClockTime; //-- 
  
	/// Flag of Pause requested
	bool m_bPauseReq;  

	/// Flag of Pause compled
	bool m_bPauseComp;  

	/* Time Scale:  
	 	scale=0.5      => speed=RT/2.0; (2 times slower)
	 	scale=1        => speed=RT; (fast as real time if we can)
	 	scale=2.0      => speed=2*RT; (two times faster if we can)
	 	scale=INFINITE => speed=INFT; (pure discrete event simulation)
	*/
    double m_time_scale;

    /// Flag of Exit requested
 	bool m_bExitReq;

	/// Flag of Exit completed
	bool m_bExitComp;

	////
	bool m_bigmouth;

	Time m_ending_time; //

	string m_menu;

	TimeSpan m_animation_interval;

	/// first bit: not log
	/// second bit: transition 
	/// third bit: final state
	/// fourth bit: performance of one run
	/// fifth bit: performance of all batch
	static unsigned m_bLog; 

	//-- number of simulation run when multiple run (mrun);
	unsigned m_no_sim_run;
	unsigned m_current_run;
public:
	static Logger CLog;
	enum PrintStateMode {P_NONE, P_relative, P_absolute};
	typedef PortValue (*CallBack)(Devs& md);
private:
	PrintStateMode m_psm; // print state mode

	bool b_acs; // flag for analytical continuous simulation

	void AnimationExecution();

	CallBack call_back_method;
public:
	//Devs& modl: DEVS Devs to be simulated;
	//Time ending_t: simulation termination time;
	//CallBack cbf: function pointer whose form is PortValue (*CallBack)() which
    //              is supposed to return an input event to be used for injection;
	SRTEngine(Devs& modl, Time ending_t = DBL_MAX, CallBack cbf=NULL);

	/// Destructor
    virtual ~SRTEngine() ;
	
	/// Inject a portvalue into the simulator's input bag
    void Inject(PortValue x);

	/// Initialize control Variables
	void InitControlVariables();

	/// Set Runing Flag on called by Outside 
    void Step();

	//-- n multi run
	void MultiRun(unsigned n);

	/// pause the simulation run. Called by a non-simulation thread.
    void Pause() ;

	/// Continue to run (or step, or mrun). Called by a non-simulation thread.
    void Continue() ;

	/// Reinitialize Devs
	void Reset();

	/// Reset models: Reset & Run() 
    void Rerun() { Reset(); MultiRun(1); }
	 
	/// Simulation Loop which is supposed to be called by a simulation "THREAD"
    void Loop( );
	void KillLoop();

	/// Get the time scale 
    double GetTimeScale() const { return m_time_scale; }
	/// Set the time scale
    void SetTimeScale(double ts);

	Time GetEndingTime() const { return m_ending_time; }
	void SetEndingTime(Time et) { m_ending_time = et; }

	void Set_dtmode(PrintStateMode flag) { m_psm = flag; } 
	void Get_dtmode(PrintStateMode& flag) const { flag = m_psm; }

	void SetAnimationFlag(bool flag) { b_acs = flag; } 
	bool GetAnimationFlag() const { return b_acs; }

	void SetAnimationInterval(TimeSpan ai) { m_animation_interval = ai; }
	TimeSpan GetAnimationInterval() const { return m_animation_interval; }

	void RunConsoleMenu();

	void PrintTotalState() const;
	void PrintCouplings() const;
	void PrintSettings() const;
	void PrintPerformanceOfaRun() const;

	static void SetLogOn(bool flag=true) ;
	static void SetLogPerformance(bool flag=true) { if(flag !=GetLogPerformance()) m_bLog ^= 2; } 
	static void SetLogTotalState(bool flag=true) { if(flag !=GetLogTotalState()) m_bLog ^= 4; } 
	static void SetLogTransition(bool flag=true) { if(flag !=GetLogTransition()) m_bLog ^= 8;} 
	static bool GetLogOn() { return (m_bLog & 1)!=0; }	
	static bool GetLogPerformance() { return (m_bLog & 2)!=0; }
	static bool GetLogTotalState() { return (m_bLog & 4)!=0; }
	static bool GetLogTransition() { return (m_bLog & 8)!=0; }

	//-- key: name of atomic model, 
	//-- value: (key: item, value: vector of values)
	map<string, map<string, vector<double> > > m_multi_statistics;
private: 
	/// Remember the clcok time when getting start of a run.
	void StampClockTime(); 
	/// Starter for a simulation run
	void Run(unsigned n);
};
 
	//-- return current time as milliseconds
	unsigned long milliseconds();

}// end of namespace of DEVS