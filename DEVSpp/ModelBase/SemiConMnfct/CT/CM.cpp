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
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CM.h>
#include <DEVSpp/DEVS_Exception.h>
#include <float.h>


using namespace CT;

IMPLEMENT_ABSTRACT_CLASS1(CM, "CT.CM", 20061216, Atomic);

const string CM::SendCM="SendCM";
const string CM::Empty="Empty";
const string CM::S_Occupied="S_Occupied";
const string CM::Occupied="Occupied";
const string CM::SendWafer="SendWafer";
const string CM::Error_Empty_Slot_Picking="Error_Empty_Slot_Picking";
const string CM::Error_Occupied_Slot_Feeding="Error_Occupied_Slot_Feeding";
const string CM::Error_Empty_Cassette="Error_Empty_Cassette";

/*virtual*/ bool CM::delta_x(const PortValue& x) 
{
	if (m_phase == Empty)
	{
        if (x.port  == IC)
        {
			Cassette* pCassette = dynamic_cast<Cassette*>(x.value);
			if(pCassette) {
				m_phase = S_Occupied;
				m_pCassette = pCassette;
				return true;
			}
        }
        else if (x.port == IHS || x.port == IS || x.port == IW )
        {
            m_phase = Error_Empty_Cassette;
			return true;
        }
    }
    else if (m_phase == Occupied)
    {
		if (x.port == IHS){
            m_phase = SendCM;
			return true;
		}
        else if (x.port == IS) // request sending a wafer
        {
			bValue<int>* in=dynamic_cast<bValue<int>*>(x.value);
			if(in) {
				m_slot_i = in->v;
				m_phase = SendWafer;
				return true;
			}
        }
        else if (x.port == IW)// receive a wafer 
        {
			WaferFeed* wf = dynamic_cast<WaferFeed*>(x.value);
            int i = wf->slot_i;
            if (m_pCassette->IsEmptyAt(i))
                m_pCassette->InsertAt(i, wf->wafer);
            else
                m_phase = Error_Occupied_Slot_Feeding;
			return true;
        }
    } 
	return false; // continue
} 

/*virtual*/ void CM::delta_y(PortValue& y) 
{
	static Wafer ow("0");

	if( m_phase == S_Occupied){
		y.Set(OHS, m_pCassette);  
		m_phase = Occupied;
	}
	else if (m_phase == SendCM)
	{
		y.Set(OC, m_pCassette);
		m_phase = Empty;
		m_pCassette = NULL; // ->Clear();
	}
	else if (m_phase == SendWafer)
	{
		m_phase = Occupied;
		if (m_pCassette->IsEmptyAt(m_slot_i))
			m_phase = Error_Empty_Slot_Picking;
		else {
			ow = m_pCassette->GetAt(m_slot_i);
			y.Set(OW, (Value*)&ow);
			m_pCassette->RemoveAt(m_slot_i);
		}
	}
	else
		THROW_DEVS_EXCEPTION("Invalide Phase!");
}
 
	/*virtual*/ TimeSpan CM::tau() const 
	{ 
		if(m_phase == S_Occupied || m_phase == SendCM || m_phase == SendWafer)
			return 0;//iInterval(0); 
		else  
			return DBL_MAX; //iInterval(INT_MAX); 
	}
