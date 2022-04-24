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
#include <deque>
#include <DEVSpp/ModelBase/Job.h>

using namespace DEVSpp;

namespace MB_Basic
{
	//const string IDLE = "Idle";
	const string BUSY = "Busy";
	const string COLLIDED = "Collided";

	class Server: public Atomic
	{
	public:
		OutputPort* out;  
		InputPort* in;
		
	protected:
		string  m_phase;
		Job*	m_pJob;
	public:
		Server(const string& name=""): Atomic(name),m_pJob(NULL) 
		{ in = AddIP("in"); out = AddOP("out"); init(); }

		virtual ~Server() { init(); }
		/*virtual*/ void init();
		/*virtual*/ Time tau() const ;
		/*virtual*/ bool delta_x(const PortValue& x) ;
		/*virtual*/ void delta_y(deque<PortValue>& ys) ;
		/*virtual*/ string Get_s() const 
		{
			return m_phase; 
		}
	};
}
