/************************************************************************
 * DEVS++: DEVS Open Source in C++                                      *
 * Copyright (C) 2005-2009  Moon Ho Hwang <moonho.hwang@gmail.com>      * 
 *                                                                      *
 * This library is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU Lesser General Public           *
 * License as published by the Free Software Foundation; either         *
 * version 2.1 of the License, or (at your option) any later version.   *
 *                                                                      *
 * This library is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     *
 * Lesser General Public License for more details.                      *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,*  
 * USA.                                                                 * 
*************************************************************************/
#pragma once
#include "Atomic.h"
#include <vector>
#include <deque>
#include "RNG.h"
#include "DEVS_Exception.h"

using namespace DEVSpp;

class Transducer: public Atomic
{
protected:
	InputPort* in;
	deque<tmValue*> Collector;

public:
	Transducer(const string& name=""): Atomic(name) 
	{ 
		CollectStatistics(true); // default collecting statistics 
		in = AddIP("in"); 
	}
	virtual ~Transducer() { init(); }
    /*virtual*/ void init()	{ 
		while(Collector.size()>0) // delete all pv in Collector
		{
			tmValue* pv = Collector[0];
			Collector.pop_front();
			delete pv;
		}
	}
	/*virtual*/ Time tau() const { return DBL_MAX; }
	/*virtual*/ bool delta_x(const PortValue& x) 
	{
		tmValue* pv = dynamic_cast<tmValue*>(x.value);
		if(pv)
		{
			//-- (event, time) stamping
			pv->TimeMap.insert(make_pair("SysOut", Devs::TimeCurrent())); 
			Collector.push_back(pv); // delete contents later in int();
		}else
			THROW_DEVS_EXCEPTION("Type casting Failed!");
		return false;
	}
	/*virtual*/ void delta_y(PortValue& y) {}

	/*virtual*/ string Get_s() const { return "Wait"; }
	// return a string showing performance data
	/*virtual*/ map<string, double> GetPerformance() const
	{
		map<string, double> statistics;
		if(CollectStatisticsFlag()) {
			string str = "Throughput";
			statistics.insert(make_pair(str, 
					Collector.size()/TimeCurrent()));

			TimeSpan average_st=0;
			for(int i=0; i<(int)Collector.size(); i++){
				tmValue* pv = Collector[i];
				TimeSpan system_t = pv->TimeMap["SysOut"] - pv->TimeMap["SysIn"];
				average_st += system_t;
			}
			average_st = average_st / (double)Collector.size();
			str = "Average System Time";
			statistics.insert(make_pair(str, average_st));

		}
		return statistics;
	}
};
