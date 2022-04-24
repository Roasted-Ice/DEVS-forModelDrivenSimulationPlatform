/************************************************************************
 * DEVS-C++: Open Source of DEVS in C++                                *
 * Copyright (C) 2005-2006  Moon Ho Hwang <moonho.hwang@gmail.com>      * 
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

#include <DEVSpp/Atomic.h>
#include <DEVSpp/StrUtil.h>
#include <DEVSpp/ClassAdmin.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Wafer.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CT_Export.h>

using namespace std;
using namespace DEVSpp;

namespace CT{

class MB_CT_EXP PM: public Atomic
{
DECLARE_CLASS();
public:
	static const string Empty;
	static const string SensingF;
	static const string OccupiedN;
	static const string Work;
	static const string OccupiedF;
	static const string SendWafer; 
	static const string Error_Empty_Picking;
	static const string Error_Occupied_Feeding;
	static const string Error_COMMAND; 

public:
	InputPort	 *IHS,// for from hier controller
                 *IW, // for Wafer
                 *IS; // for robot_pull_Signal
	OutputPort   *OHS,// for to hier controller
                 *OW; // for Wafer


	string		 m_phase;
	string       m_proc_str;
	TimeSpan	 m_proc_time; // processing time
	Wafer        m_wafer; // Wafer
	bool         m_ready_to_pick;

	PM(const string& name, const string& ps): 
		Atomic(name), m_proc_str(ps), m_wafer("0")  
	{ 
		IHS = AddIP("IHS"); // for from hier controller
		IW = AddIP("IW"); // for Wafer
		IS= AddIP("IS"); // for robot_pull_Signal
		OHS = AddOP("OHS"); // for to hier controller
		OW = AddOP("OW");// for Wafer
		init(); }

    /*virtual*/ void init()
	{
		m_phase = Empty;
		m_wafer.Set_s("0");
		m_ready_to_pick = false;
	}
	/*virtual*/ TimeSpan tau() const;
	/*virtual*/ bool delta_x(const PortValue& x) ;
	/*virtual*/ void delta_y(PortValue& y);
	/*virtual*/ string Get_s() const 
	{
		string str = m_phase+",";
		if(m_phase == Empty)
			str += "0";
		else 
			str += m_wafer.STR();
		return str;
	} 

	/*virtual*/ void Set_s(const string& str) 
	{ 
		vector<string> s = DEVSpp::Split(str,',');//phase, slot_i, 0 or 1, .....
		m_phase = s[0];
		m_wafer.Set_s(s[1]); 
	} 


};

}