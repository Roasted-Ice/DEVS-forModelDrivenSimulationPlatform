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
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/Router.h>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/SF_Export.h>
#include <DEVSpp/ModelBase/Job.h>

using namespace DEVSpp;
 
namespace SF
{
 
class SF_EXP Router: public Atomic
{
protected: 
	deque<MB_Basic::Job*> m_UnaskedJobs; // V1
	deque<MB_Basic::Job*> m_AskedJobs; // V2
	deque<MB_Basic::Signal> m_Answer;
	string m_phase;

public:
	InputPort *in, *IDB;
	OutputPort *ODB;
	OutputPort *SysOut;

	Router(const string& name=""): Atomic(name)
	{ 
		in = AddIP("in");  IDB = AddIP("IDB"); 
		ODB = AddOP("ODB"); SysOut = AddOP("SysOut");
		init(); 
	}
	virtual ~Router() { }
	/*virtual*/ void init();
	/*virtual*/ bool delta_x(const PortValue& x);
	/*virtual*/ void delta_y(PortValue& y);
	/*virtual*/ Time tau() const;
};

}