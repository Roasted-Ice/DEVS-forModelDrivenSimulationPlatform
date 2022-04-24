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
#include <DEVSpp/ModelBase/Job.h>
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/ModelBase/MB_Basic_Export.h>

using namespace DEVSpp;

namespace MB_Basic
{
const string IDLE = "Idle";
const string SENDTO = "SendTo";

class MB_BASIC_EXP Buffer: public Atomic
{
public:
	vector<OutputPort*> out;  
	InputPort* in;
	vector<InputPort*> pull;

protected:
	std::deque<Job*> m_Clients;
	string m_phase;
	vector<bool> m_OAvail;
	const int m_Osize;
	int send_index;

	void C1(const PortValue& x);
	void C2(PortValue& y);

	bool Matched();

public:
	Buffer(const string& name="", int osize=1);
	virtual ~Buffer() { init(); }

    /*virtual*/ void init();
	/*virtual*/ bool delta_x(const PortValue& x) ;
	/*virtual*/ void delta_y(PortValue& y) ;
	///*virtual*/ PortValue lambda() const;
	/*virtual*/ Time tau() const ;

	/*virtual*/ string Get_s() const ;
	/*virtual*/ string Get_Statistics_s() const ;
	/*virtual*/ map<string, double> GetPerformance() const;
};

}