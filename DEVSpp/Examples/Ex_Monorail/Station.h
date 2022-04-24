/************************************************************************
 * DEVS++: DEVS Open Source in C++                                      *
 * Copyright (C) 2005-2009  Moon Ho Hwang <moonho.hwang@gmail.com>      * 
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
#include <vector>
#include <iostream>
#include <math.h>
#include <float.h>
#include "Atomic.h"

const string EMPTY="E";
const string LOADING="L";
const string SENDING="S";
const string WAITING="W";
const string COLLIDED="C";

using namespace std;
using namespace DEVSpp;

#define REMEMBERING  // for testing the effect of using nso

class Station: public Atomic
{
public:
	string m_phase;
	bool   init_occupied;
	bool   nso;//next_state_occpied
	const TimeSpan loading_t;

	InputPort* ipull, *ivehicle;
	OutputPort* ovehicle;

	Station(const string& name, bool occupied, TimeSpan lt): 
		Atomic(name), init_occupied(occupied), loading_t(lt), nso(true)
		{ 
			ipull = AddIP("pull"); ivehicle = AddIP("vehicle");
			ovehicle = AddOP("vehicle");
			init(); 
		}
    /*virtual*/ void init()
	{
		if(init_occupied == true)
			m_phase = SENDING;
		else
			m_phase = EMPTY;
		//cout << Name << ":" << Get_s()<<endl;
	}	
	/*virtual*/ TimeSpan tau() const 
	{ 
		if (m_phase == SENDING)
			return 10;
		else if (m_phase == LOADING)
			return loading_t;
		else 
			return DBL_MAX;
	}
	/*virtual*/ bool delta_x(const PortValue& x) 
	{
		if( x.port == ipull) {
			nso = false;
			if( m_phase == WAITING){
#ifdef REMEMBERING
				nso = true;
#endif
				m_phase = SENDING;
				return true;
			}
		}
		else if(x.port == ivehicle) {
			if(m_phase == EMPTY)
				m_phase = LOADING;
			else // rest cases lead to Colided! 
				m_phase = COLLIDED;
			return true;
		}
		return false; 
	}
	/*virtual*/ void delta_y(PortValue& y) {
		if(m_phase == LOADING){
			if(nso == true)
				m_phase = WAITING;
			else {
#ifdef REMEMBERING
				nso = true;
#endif
				m_phase = SENDING;
			}
		}  else if(m_phase == SENDING) {
			y.Set(ovehicle);
			m_phase = EMPTY;
		}  
	} 
	/*virtual*/ string Get_s() const 
	{
		string str = "phase="+m_phase +",nso="; 
		if(nso) str +="true";
		else    str +="false";
		return str;
	}
};