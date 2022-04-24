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
#include <map>
#include <deque>
#include <DEVSpp/ModelBase/Job.h>
#include <DEVSpp/RNG.h>
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/ModelBase/MB_Basic_Export.h>
#include <math.h>
#include <float.h>

using namespace DEVSpp;
using namespace std;
using namespace MB_Basic;

class MB_BASIC_EXP Transducer: public Atomic
{
public:
	InputPort* in;  

protected:
	map<int, vector<TimeSpan> > SystemTimes;
	deque<Job*> buffer;

public:
	Transducer(const string& name=""): Atomic(name)
	{ m_cs = true; // default collecting statistics 
		in = AddIP("in");
	}
	virtual ~Transducer() { init(); }
    
	/*virtual*/ void init()	;
	/*virtual*/ Time tau() const { return DBL_MAX; }
	/*virtual*/ bool delta_x(const PortValue& x) ;
	/*virtual*/ void delta_y(PortValue& y) {}

	/*virtual*/ string Get_s() const { return "Work"; }
	// return a string showing performance data
	/*virtual*/ map<string, double> GetPerformance() const;
};
