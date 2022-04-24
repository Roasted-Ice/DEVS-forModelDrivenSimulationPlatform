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

#include <DEVSpp/ModelBase/SemiConMnfct/Fab/Buffer.h>
#include <assert.h>
#include <float.h>
#include <algorithm>
 
namespace SF {
 
const string Ask = "Ask";

class lt_Signal // signal and arrival time
{
public:
	bool operator()(const MB_Basic::Signal& p1, const MB_Basic::Signal& p2) const
	{
		return	p1.value < p2.value || 
				(p1.value == p2.value && p1.at < p2.at);
	}
};

deque<MB_Basic::Job*>::iterator FindJob_it(deque<MB_Basic::Job*>& Jobs, int job_id)
{
	deque<MB_Basic::Job*>::iterator it = Jobs.begin();
	for(; it != Jobs.end(); it++)
		if(job_id == (*it)->id)
			return it;

	return it;
}

void Buffer::init()
{
	MB_Basic::Buffer::init();
	m_UnaskedJobs.clear();
	m_AskedJobs.clear(); 
	m_PriorityQ.clear();
}


MB_Basic::Job* Find_job(deque<MB_Basic::Job*>& Clients, int job_id)
{
	MB_Basic::Job* job = NULL;
	for(int i=0; i < (int) Clients.size(); i++)
	{
		if(Clients[i]->id == job_id)
		{
			job = (MB_Basic::Job*) Clients[i];
			break;
		}
	}
	return job;
}

deque<MB_Basic::Job*>::iterator FindJob(deque<MB_Basic::Job*>& Jobs, int job_id)
{
	deque<MB_Basic::Job*>::iterator it = Jobs.begin();
	for(; it != Jobs.end(); it++)
		if(job_id == (*it)->id)
			return it;

	return it;
}

void Buffer::UpdateVariable(const DEVSpp::PortValue& x)
{
	if (x.port == in) { //receiving a client
		MB_Basic::Job* client = dynamic_cast<MB_Basic::Job*>(x.value);
		if(client) { // if NULL, it can be pull signal 
			m_Clients.push_back(client);
			m_UnaskedJobs.push_back(client);
		}
	}
	else if (x.port == IDB)
	{
		MB_Basic::Signal* sig = dynamic_cast<MB_Basic::Signal*>(x.value);
		assert(sig);
		deque<MB_Basic::Job*>::iterator it = FindJob(m_AskedJobs, sig->job_id);
		if( it != m_AskedJobs.end())
			m_AskedJobs.erase(it);
		m_PriorityQ.push_back(*sig);
		sort(m_PriorityQ.begin(), m_PriorityQ.end(), lt_Signal());
	}
	else // receiving a pull signal 
	{
		for(int i=0; i < m_Osize; i++){  
			if(x.port == pull[i]) {
				m_OAvail[i]= true; //  server_i is available
				break;
			}
		}
	}
}

bool Buffer::delta_x(const PortValue& x) 
{
	UpdateVariable(x);
	if( m_phase == MB_Basic::IDLE)
	{
		if( m_UnaskedJobs.size() > 0){
			m_phase = Ask;
			return true; // x_RescheduleMe();
		} 
		else if(m_PriorityQ.size()>0) {
			if (m_AskedJobs.empty() && Matched() )
				return true; //x_RescheduleMe(); // reschedule as active
		}
	}
	return false;// continue
}

void Buffer::delta_y(PortValue& y) 
{
	if(m_phase == Ask) { // move m_UnanskedJobs[0] to m_AskedJobs
		static MB_Basic::Signal signal;
		signal.job_id = m_UnaskedJobs[0]->id;
		signal.job_type = m_UnaskedJobs[0]->type;
		signal.action = MB_Basic::Signal::EVALUATION;
		signal.resource = Name;
		//signal.at = TimeCurrent();

		y.Set(ODB, &signal);

		m_AskedJobs.push_back(m_UnaskedJobs[0]);
		m_UnaskedJobs.pop_front(); 
	}
	else { 
		// SendTOi: remove the job of m_PriorityQ[0] from m_Clients
		MB_Basic::Job* job = SF::Find_job(m_Clients, m_PriorityQ[0].job_id);
//cout << Name << " selects type=" <<m_PriorityQ[0].job_type 
//     << ", id=" << m_PriorityQ[0].job_id << ", value=" << m_PriorityQ[0].value <<endl;
		if(m_phase == MB_Basic::SENDTO) {
			OutputPort* op= GetOP(STR("out", send_index));
			y.Set(op, job); // m_Clients[0]); // return dollar
		}

		deque<MB_Basic::Job*>::iterator it = m_Clients.begin();
		for(; it != m_Clients.end(); it++)
			if((*it)->id == m_PriorityQ[0].job_id)
				break;
		assert(it != m_Clients.end());
		m_Clients.erase(it); // changing state in delta_int() not in lambda()
		m_PriorityQ.pop_front();
	}

	if( m_UnaskedJobs.size() > 0){
			m_phase = Ask;
			return; //x_RescheduleMe();
	} 
	else if(m_PriorityQ.size()>0) {
		if (m_AskedJobs.empty() && Matched() )
			return; //x_RescheduleMe(); // reschedule as active
	}
	m_phase = MB_Basic::IDLE;
}

//
//PortValue Buffer::lambda() const
//{ 
//	if(m_phase == Ask)
//	{
//		static MB_Basic::Signal signal;
//		signal.job_id = m_UnaskedJobs[0]->id;
//		signal.job_type = m_UnaskedJobs[0]->type;
//		signal.action = MB_Basic::Signal::EVALUATION;
//		signal.resource = Name;
//		//signal.at = TimeCurrent();
//
//		return PortValue(ODB, &signal);
//	}
//	else {
//		//-- find the Job of the first priority Signal
//		MB_Basic::Job* job = FindJob(m_Clients, m_PriorityQ[0].job_id);
////cout << Name << " selects type=" <<m_PriorityQ[0].job_type 
////     << ", id=" << m_PriorityQ[0].job_id << ", value=" << m_PriorityQ[0].value <<endl;
//		for(int i=0; i < m_Osize; i++){
//			 if(m_phase == SENDTO[i]) 
//				 return PortValue(out[i], job); // return dollar
//		}
//		return PortValue(); // slient event
//	}
//}

/*virtual*/ Time Buffer::tau() const 
{ 
	if(m_phase == MB_Basic::IDLE)
		return DBL_MAX;
	else if(m_phase == Ask)
		return 1; // sec
	else // SendTo
		return 60; // 1 min.
}
 
}
 