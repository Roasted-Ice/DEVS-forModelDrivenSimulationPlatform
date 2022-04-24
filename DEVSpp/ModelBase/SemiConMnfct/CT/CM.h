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
#pragma warning(disable:4355)

#include <vector>
#include <math.h>
#include <DEVSpp/Atomic.h>
#include <DEVSpp/StrUtil.h>
#include <DEVSpp/ClassAdmin.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Cassette.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Wafer.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Config.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CT_Export.h>

using namespace std;
using namespace DEVSpp;

namespace CT{

class MB_CT_EXP CM: public Atomic
{
DECLARE_CLASS();
	static const string SendCM;
	static const string Empty;
	static const string S_Occupied;
	static const string Occupied;
	static const string SendWafer;
	static const string Error_Empty_Slot_Picking;
	static const string Error_Occupied_Slot_Feeding;
	static const string Error_Empty_Cassette;
public:
	
	InputPort	 *IC, // for Cassette
                 *IHS,// for from hier controller
                 *IW, // for Wafer
                 *IS; // for robot_pull_Signal
	OutputPort   *OC, // for Cassette
                 *OHS,// for to hier controller
                 *OW; // for Wafer

	string		 m_phase;
	int			 m_slot_i;   // output slot
	Cassette*    m_pCassette; // Cassette*   

	CM(const string& name=""): Atomic(name), m_pCassette(NULL) 
	{ 
		IC= AddIP("IC"); // for Cassette
		IHS = AddIP("IHS");// for from hier controller
		IW = AddIP("IW"); // for Wafer
		IS = AddIP("IS"); // for robot_pull_Signal
		OC = AddOP("OC"); // for Cassette
		OHS = AddOP("OHS");// for to hier controller
		OW = AddOP("OW"); // for Wafer
		init(); 
	}

    /*virtual*/ void init()
	{
		m_phase = SendCM;
		m_slot_i = 0;
		if(m_pCassette)
			delete m_pCassette;
		m_pCassette = NULL; 
	}
	/*virtual*/ TimeSpan tau() const ;
	/*virtual*/ bool delta_x(const PortValue& x) ;
	/*virtual*/ void delta_y(PortValue& y) ;
	/*virtual*/ string Get_s() const 
	{ 
		string str = m_phase + "," + ::STR(m_slot_i) +",";
		if(m_phase == Empty)
			str += "NO_CS";
		else {
			str += "CS,";
			if(m_pCassette)
				str +=m_pCassette->STR();
			else
				str += "NO_CASSETTE";
		}
		return str;
	}
	/*virtual*/ void Set_s(const string& str) 
	{ 
		vector<string> s = DEVSpp::Split(str,',');//phase, slot_i, 0 or 1, .....
		m_phase = s[0];
		m_slot_i=atoi(s[1].data());
		if(s[2]=="NO_CS") {
			if(m_pCassette)
				delete m_pCassette;
			m_pCassette = NULL;
		}
		else 
		{
			string ca_str = DEVSpp::Merge(s, 3, ',');
//cout << "str: " << str << ", ca_str: " << ca_str << endl;
			if(m_pCassette == NULL)
				m_pCassette = new Cassette(Config::GetNoSlots(0));
			m_pCassette->Set_s(ca_str);
//cout << "Cassette: " << m_pCassette->STR() << endl;
		}
	}   
};

}