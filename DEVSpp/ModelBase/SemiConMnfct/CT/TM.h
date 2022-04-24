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
#include <DEVSpp/ModelBase/SemiConMnfct/CT/TMcmd.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Config.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CT_Export.h>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using namespace DEVSpp;

namespace CT{

class MB_CT_EXP TM: public Atomic
{
DECLARE_CLASS();

static const string Idle;
static const string MoveToPick;
static const string WaitReceive;
static const string ShrinkAfterPick;
static const string MoveToPlace;
static const string ShrinkAfterPlace;
static const string Error;
//const string StrechToPick, 
//const string StrechToPlace, 

public:
	InputPort	       *IHS;// for from hier controller
    vector<InputPort*>  IW; // for Wafer

	OutputPort         *OHS;// for to hier controller
	vector<OutputPort*> OS; // for pull signal
    vector<OutputPort*> OW; // for Wafer

	string	 m_phase;
	int		 m_pick_index[2], m_place_index[2], m_curr_index[2];
    TimeSpan m_travel_t;    // 50 millisec for 1 degree
    Wafer	 m_wafer;
	const int m_no_cm_pm;

	TM(const string& name, int no_cm_pm): Atomic(name),
		m_no_cm_pm(no_cm_pm), m_wafer("0")
	{ 
		IHS = AddIP("IHS");// for from hier controller
		OHS = AddOP("OHS");// for to hier controller

		for(int i=0; i < no_cm_pm; i++)
			IW.push_back(AddIP(STR("IW",i)));// for Wafer
		for(int i=0; i < no_cm_pm; i++)
			OW.push_back(AddOP(STR("OW",i)));// for pull signal
		for(int i=0; i < no_cm_pm; i++)
			OS.push_back(AddOP(STR("OS",i)));// for Wafer
		
		init(); 
	}

    /*virtual*/ void init()
	{
		m_phase = Idle;
		m_wafer.Set_s("0");
		m_curr_index[0] = m_curr_index[1] = 0;
		m_pick_index[0] = m_pick_index[1] = 0;
		m_place_index[0] = m_place_index[1] = 0;
	}
	/*virtual*/ TimeSpan tau() const ;
	/*virtual*/ bool delta_x(const PortValue& x) ;
	/*virtual*/ void delta_y(PortValue& y) ;

    bool IsInputIW(const Port* ip);
    double CalcMovingRadian(int curr_index, int dest_index);
    double CalcMovingTIme(int curr_index, int dest_index);
    void UpdateCurrentPosition(int dest_index[]);

	//vector<TMcmd*> GetAllTMcmds() const;
	/*
	string	m_phase;
	string  m_proc_str;
	int		m_pick_index[2], m_place_index[2], m_curr_index[2];
    int		m_travel_t;    // 50 millisec for 1 degree
    Wafer	m_wafer;
	*/
	/*virtual*/ string Get_s() const; 

	/*virtual*/ void Set_s(const string& str) ;// for verification
};

}