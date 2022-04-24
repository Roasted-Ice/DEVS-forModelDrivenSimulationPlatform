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
#include <DEVSpp/ModelBase/SemiConMnfct/CT/PM.h>
#include <DEVSpp/DEVS_Exception.h>
#include <float.h>

using namespace CT;

IMPLEMENT_ABSTRACT_CLASS1(PM, "CT.PM", 20061216, Atomic);

const string PM::Empty="Empty"; 
const string PM::SensingF="SensingF";  
const string PM::OccupiedN="OccupiedN";  
const string PM::Work="Work"; 
const string PM::OccupiedF="OccupiedF";  
const string PM::SendWafer="SendWafer"; 
const string PM::Error_Empty_Picking="Error_Empty_Picking"; 
const string PM::Error_Occupied_Feeding="Error_Occupied_Feeding";  
const string PM::Error_COMMAND="Error_COMMAND"; 

/*virtual*/ TimeSpan PM::tau() const 
{ 
	if(m_phase == SendWafer)
		return 0; 
	else if (m_phase == Work)
        return m_proc_time;  
	else if(m_phase == OccupiedF && m_ready_to_pick)
		return 0;
	else
		return DBL_MAX; 
}

/*virtual*/ bool PM::delta_x(const PortValue& x) 
{
	if (m_phase == Error_COMMAND ||
		m_phase == Error_Empty_Picking ||
		m_phase == Error_Occupied_Feeding)
		return false;

    if (x.port == IW) 
    {
        if (m_phase == Empty)
        {
            m_phase = OccupiedN;
            WaferFeed* wf = dynamic_cast<WaferFeed*>(x.value);
			m_wafer = wf->wafer;
        }
        else
            m_phase = Error_Occupied_Feeding;
    }
    else  if (x.port == IHS) 
    {
        if (m_phase == OccupiedN)
        {
			bValue<double>* bi = dynamic_cast<bValue<double>*>(x.value);
			if(bi) { 
				m_proc_time = bi->v;
				m_phase = Work; 
			}else
				m_phase = Error_COMMAND;
        }else
            m_phase = Error_COMMAND;
    }
    else if (x.port == IS) // request sending a wafer
    {
		if (m_phase == OccupiedN)
			 m_phase = Error_Empty_Picking;
		else if(m_phase == OccupiedF){
            m_phase = SendWafer;
			m_ready_to_pick = false;
		} else if (m_phase == Empty) // no wafer at slot[0]
            m_phase = Error_Empty_Picking;
		else if (m_phase == Work) {
			m_ready_to_pick = true;
            //m_phase = Error_COMMAND;
		}
    }
    return true;
}

/*virtual*/ void PM::delta_y(PortValue& y) 
{
	static Wafer ow("0");
	if (m_phase == Work){
		static bValue<int> v(2);
		y.Set(OHS, &v);
		m_phase = OccupiedF;
		m_wafer.Set_s(m_proc_str); // change wafer status
	}
	else if (m_phase == SendWafer){ 
		ow = m_wafer;
		y.Set(OW, &ow);
		m_wafer.Set_s("0");
        m_phase = Empty;
	} 
	else if(m_phase == OccupiedF)
	{
		m_phase = SendWafer;
		m_ready_to_pick = false;
	}else
		THROW_DEVS_EXCEPTION("Invalide Phase!");
}
