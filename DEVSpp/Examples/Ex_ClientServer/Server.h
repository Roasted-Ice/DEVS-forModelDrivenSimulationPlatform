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
#include "Atomic.h"
#include <deque>
#include <vector>

using namespace DEVSpp;

const string BUSY = "Busy";
const string COLLIDED = "Collided";

class Server: public Atomic
{
protected:
	string m_phase;
	tmValue*   m_pClient;
public:
	InputPort* in;
	OutputPort* out;

public:
	Server(const string& name=""): Atomic(name), 
		m_pClient(NULL)
	{
		in = AddIP("in"); out = AddOP("out");
		init();
	}
	virtual ~Server() { init(); }

    /*virtual*/ void init()
	{
		m_phase = IDLE;
		if(m_pClient)
			delete m_pClient;
		m_pClient=NULL;
	}

	/*virtual*/ bool delta_x(const PortValue& x) 
	{
		if(x.port == in){ //receiving a client
			if(m_phase == IDLE){
				m_pClient = dynamic_cast<tmValue*>(x.value);
				assert(m_pClient);
				m_phase = BUSY;
				return true;
			}else if( m_phase == BUSY)
			{
				m_phase = COLLIDED;
				return true;
			} // already collided,
		} 
		return false;
	}
	/*virtual*/ void delta_y(PortValue& y) 
	{
		y.Set(out, m_pClient); 
		m_pClient = NULL; // don't delete that. sink will delete that.
		m_phase = IDLE;
	}

	/*virtual*/ Time tau() const 
	{ 
		if(m_phase == IDLE)
			return DBL_MAX;
		else if(m_phase == BUSY) {
			assert(m_pClient);
			return 10;
		}
		else
			return DBL_MAX;
	}

	/*virtual*/ string Get_s() const 
	{ return m_phase; }
};
