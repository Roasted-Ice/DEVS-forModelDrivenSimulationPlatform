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
#include <deque>
#include <vector>
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/ModelBase/Job.h>
#include <DEVSpp/ModelBase/MB_Basic_Export.h>

using namespace DEVSpp;

namespace MB_Basic
{
//-- if reach upper bound, idle
//-- if not trigger when receiving in,
class MB_BASIC_EXP WIPRegulator: public Atomic
{
public:
	InputPort* in, *out;
	OutputPort *pull;

	//unsigned   m_triggering_WIP;
	//unsigned   m_request_WIP;
	unsigned   m_upper_bound;
	unsigned   m_current_WIP;
	string     m_phase;
protected:

public:
	WIPRegulator(const string& name="", unsigned upper_bound=1);
	virtual ~WIPRegulator() { init(); }

    /*virtual*/ void init();
	/*virtual*/ Time tau() const ;
	/*virtual*/ bool delta_x(const PortValue& x) ;
	/*virtual*/ void delta_y(PortValue& y) ;

	/*virtual*/ string Get_s() const ;
	/*virtual*/ string Get_Statistics_s() const ;
	/*virtual*/ map<string, double> GetPerformance() const;
};

}