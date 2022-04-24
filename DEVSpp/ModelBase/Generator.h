/*************************************************************************
 * DEVS++: DEVS Open Source in C++                                       *
 * Copyright (C) 2005-2007  Moon Ho Hwang <moonho.hwang@gmail.com>       *
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
#include <DEVSpp/Atomic.h>
#include <DEVSpp/RNG.h>
#include <DEVSpp/ModelBase/Job.h>
#include <DEVSpp/ModelBase/MB_Basic_Export.h>
#include <vector>
#include <assert.h>

using namespace std;
using namespace DEVSpp;

namespace MB_Basic{

	//-- global variable for tracking generating type
//	int gen_type;
	
	//-- generator class creating Job type objects
	//-- according to probability of ClientMap.
	//-- inter-generating time can be modified at ta() function
	class MB_BASIC_EXP Generator: public Atomic
	{
		static const string Work;
		static const string Idle;
	public:
		OutputPort*	out;  
		InputPort*	is;

		// pairs of (probability, Client*) 
		vector<pair<double, Job*> >ClientMap;
		unsigned         m_no_gen; // total generated number

	protected:
		string			 m_phase;
		bool			 m_use_wait_signal;
		TimeSpan         mean_inter_gen_time;
	public:
		Generator(const string& name="", bool wait_signal=false, double m=1.0): 
		   Atomic(name),m_use_wait_signal(wait_signal), mean_inter_gen_time(m), m_no_gen(0) 
		{
			is = AddIP("is"); out = AddOP("out");
		}
		
		virtual ~Generator() { init(); }
		
		/*virtual*/ void init()	;
		/*virtual*/ Time tau() const ;
		/*virtual*/ bool delta_x(const PortValue& x);
		/*virtual*/ void delta_y(PortValue& y) ;

		/*virtual*/ string Get_s() const { return m_phase; }

		// return a string showing performance data
		/*virtual*/ map<string, double> GetPerformance() const
		{
			return m_statistics;
		}
	};


} // end of namespace
