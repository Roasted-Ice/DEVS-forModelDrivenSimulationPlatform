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
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/StrUtil.h>
#include <assert.h>
#include <iostream>
#include <float.h>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/SF_Export.h>
#include <algorithm>
#include <deque>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/MV_Lot.h>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/PDM.h>
#include <DEVSpp/ModelBase/Job.h>

using namespace std;
using namespace DEVSpp;
using namespace MB_Basic;

namespace SF{


class SF_EXP DBMngr: public Atomic
{
public:
	InputPort  *Query, *Completed; // Input Port from Plant 
	
	//-- key: lot type
	//-- value: associated process definition model
	map<int, PDM> m_MapOfPDM;
	
	enum Priority {GFAFS, GLWKR};
	Priority m_priority;

private:
	static const string Idle; 
	static const string Answer;  
	string m_phase;

	//-- a set of lots Work-In-Process in Fab
	map<int, MV_Lot*> m_MV_Lots; //key: lot_id, 
	//-- buffer containining query message
	deque<Signal> m_IQueryBuffer;// Input Buffer
	//-- Output Message to Plant
	Signal        m_Output; // output

	MV_Lot* FindMV_Lot(int job_id) const;
	bool MakeComplete(MV_Lot& lot);
	void RemoveMV_Lot(int job_id);

public:
	DBMngr(const string& name): Atomic(name),m_priority(GFAFS)
	{	
		Query = AddIP("Query"); Completed = AddIP("Completed");
		init(); 
	}
    /*virtual*/ void init();
	/*virtual*/ TimeSpan tau() const 
	{ 
		if (m_phase == Answer)
			return 0;
        else // m_phase == Idle
            return DBL_MAX; 
	}
	/*virtual*/ bool delta_x(const PortValue& x) ;
	/*virtual*/ void delta_y(PortValue& y) ;
	/*virtual*/ string Get_s() const;
private:
	bool OutputPrepared();
};

}