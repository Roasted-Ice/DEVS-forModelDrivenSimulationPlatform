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

#include "Atomic.h"
#include <float.h>
#include <stdlib.h>
#include "Coupled.h"
#include "DEVS_Exception.h"
#include "ClassAdmin.h"
#include "SRTEngine.h"
#include <assert.h>
#include "StrUtil.h"

using namespace DEVSpp;

IMPLEMENT_ABSTRACT_CLASS1(Atomic, "Atomic", 20090806, Devs);

string Atomic::Get_q(bool relative) const
{
	string str = "("+Name+":"+Get_s();
	if(relative)
	{
		str +=", t_s=";
		char tmp[100];
		if( TimeNext() == DBL_MAX)
			str += "inf";
		else 
		{
			TimeSpan t_s = TimeLifespan();
			sprintf(tmp, "%.3lf", t_s);
			str += tmp;
		}
		sprintf(tmp, ", t_e=%.3lf)", TimeElapsed()); 
		str += tmp;
	}
	else
	{
		TimeSpan t_l = TimeLast();
		char tmp[100];		
		sprintf(tmp, ", t_l=%.3lf", t_l);
		str += tmp;

		if( TimeNext() == DBL_MAX)
			str += ", t_n=inf)";
		else 
		{
			TimeSpan t_n = TimeNext();
			sprintf(tmp, ", t_n=%.3lf)", t_n);
			str += tmp;
		}
	}
	return str;
}

void Atomic::when_receive_init()  
{ 	
	t_Last = TimeCurrent();
	t_Lcs = TimeCurrent();
	m_statistics.clear(); // newly added.

	//-- make new schedule !
	init();
	t_Next = TimeCurrent()+tau();
}


void Atomic::when_receive_star(bool print_event)
{
	when_receive_cs();
	if(IsEqual(t_Next, t_Current, 1E-5)) {
		PortValue y;
		delta_y(y);
		
		t_Last = TimeCurrent();
		double ta = tau();
		if(ta == DBL_MAX)
			t_Next = DBL_MAX;
		else if(ta < 0.0) {
			THROW_DEVS_EXCEPTION("Minus Schedule Violation!");
		}
		else
			t_Next = t_Current+ta;

		if(print_event)
			SRTEngine::CLog << "!" << HierName(this) << "." << y.STR();
		if(Parent)
			Parent->when_receive_y(y,print_event);
	}else
		THROW_DEVS_EXCEPTION("Schedule Violation!");
}

void Atomic::when_receive_x(const PortValue& x, bool print_event)
{
	when_receive_cs();
	if(t_Last <= t_Current && t_Current <= t_Next) {
		if(print_event)
			SRTEngine::CLog << ",?" << HierName(this) << "." << x.STR();

		bool b = delta_x(x); 
		if(b == true) {// reschedule is needed.
			t_Last = TimeCurrent();
			double ta = tau();
			if(ta == DBL_MAX)
				t_Next = DBL_MAX;
			else if(ta < 0.0) {
				THROW_DEVS_EXCEPTION("Minus Schedule Violation!");
			} else
				t_Next = t_Current+ta;
		}
	}else
		THROW_DEVS_EXCEPTION("Schedule Violation!");
}

void Atomic::when_receive_cs()
{
	Time dT = TimeCurrent() - t_Lcs;// dT: accumulating time span
	if(CollectStatisticsFlag() == true) //
	{
		string state_str = Get_Statistics_s();
		if(m_statistics.find(state_str) == m_statistics.end())
            m_statistics.insert(make_pair(state_str, 0.0));// new entry
        m_statistics[state_str] += dT;//add dT to staying time
	}
	t_Lcs = TimeCurrent(); // update t_Lcs as the current time.
}

//-- make an output event
void Atomic::Make_y(PortValue& y, OutputPort* op, Value* v)
{
	assert(op) ;
	y.Set(op, v);
}
 
// return a string showing performance data
/*virtual*/ map<string, double> Atomic::GetPerformance() const
{
	map<string, double> statistics;
	if(CollectStatisticsFlag()==true) {
		for(map<string, double>::const_iterator it = m_statistics.begin();
			it != m_statistics.end(); it++) 
		{
			double probability = it->second / TimeCurrent();

			if(probability < 0.0 || probability > 1.0) {
				THROW_DEVS_EXCEPTION("Invalid Probability!");
			}
			else
				statistics[it->first] = probability;
		}
	}
	return statistics;
}