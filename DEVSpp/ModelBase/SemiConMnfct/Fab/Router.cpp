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
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/Router.h>
#include <assert.h>
#include <float.h>
#include <algorithm>
 
namespace SF {
 
const string Ask = "Ask";
const string Send = "Send";
const string Idle = "Idle";

//deque<MB_Basic::Job*>::iterator FindJob_it(deque<MB_Basic::Job*>& Jobs, int job_id);
MB_Basic::Job* Find_job(deque<MB_Basic::Job*>& Clients, int job_id);

void Router::init()
{
	m_phase = Idle;
	m_UnaskedJobs.clear();
	m_AskedJobs.clear(); 
	m_Answer.clear();
}

bool Router::delta_x(const PortValue& x) 
{
	if (x.port == in) { //receiving a client
		MB_Basic::Job* client = dynamic_cast<MB_Basic::Job*>(x.value);
		if(client) { // if NULL, it can be pull signal 
//cout << Name << " receives " << client->STR() << " at " << TimeCurrent() << "!\n";
			m_UnaskedJobs.push_back(client);
		}
	}
	else if (x.port == IDB)
	{
		MB_Basic::Signal* sig = dynamic_cast<MB_Basic::Signal*>(x.value);
		assert(sig);
		m_Answer.push_back(*sig);
	}

	if( m_phase == Idle)
	{
		if( m_UnaskedJobs.size() > 0){ // asking
			m_phase = Ask;
			return true; //x_RescheduleMe();
		} 
		else if (m_Answer.size() >0 ){ // sending
			m_phase = Send;
			return true; //x_RescheduleMe();  
		}
	}
	return false; 
}

void Router::delta_y(PortValue& y) 
{
	if(m_phase == Ask) { // move m_UnanskedJobs[0] to m_AskedJobs
		static MB_Basic::Signal signal;
		signal.job_id = m_UnaskedJobs[0]->id;
		signal.job_type = m_UnaskedJobs[0]->type;
		signal.action = MB_Basic::Signal::ROUTE;
		signal.resource = Name; // my name
		y.Set(ODB, &signal);

		m_AskedJobs.push_back(m_UnaskedJobs[0]);
		m_UnaskedJobs.pop_front(); 
	}
	else { // Send
		MB_Basic::Job* job = Find_job(m_AskedJobs, m_Answer[0].job_id);
		int dest = (int) m_Answer[0].value;
		if(dest < 0)
			y.Set(SysOut, job);
		else {
			OutputPort* op = GetOP(STR(dest));
			y.Set(op, job);
		}

		deque<MB_Basic::Job*>::iterator it = m_AskedJobs.begin();
		for(; it != m_AskedJobs.end(); it++)
			if((*it)->id == m_Answer[0].job_id)
				break;
		assert(it != m_AskedJobs.end());
		m_AskedJobs.erase(it);// changing state in delta_int() not in lambda()
		m_Answer.pop_front();
	}

	if( m_UnaskedJobs.size() > 0) // asking
			m_phase = Ask;
	else if (m_Answer.size() >0 ) // sending
			m_phase = Send;
	else
		m_phase = Idle;
}
//
//PortValue Router::lambda() const
//{ 
//	if(m_phase == Ask)
//	{
//		static MB_Basic::Signal signal;
//		signal.job_id = m_UnaskedJobs[0]->id;
//		signal.job_type = m_UnaskedJobs[0]->type;
//		signal.action = MB_Basic::Signal::ROUTE;
//		signal.resource = Name; // my name
//		//signal.at = TimeCurrent();
//		return PortValue(ODB, &signal);
//	}
//	else //if(m_phase == Send)
//	{
//		MB_Basic::Job* job = FindJob(m_AskedJobs, m_Answer[0].job_id);
//		int dest = (int) m_Answer[0].value;
//		if(dest < 0)
//			return PortValue(SysOut, job);
//		else {
//			assert(out.find(dest) != out.end());
//			const OutputPort& op = (const OutputPort&) (*out.find(dest)).second;
//			return PortValue((OutputPort&)op, job);
//		}
//	}
//}

/*virtual*/ Time Router::tau() const 
{ 
	if(m_phase == Idle)
		return DBL_MAX;
	else if(m_phase == Ask)
		return 1; // sec
	else // SendToi
		return 0; // no dealy
}
 
}
 