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

#include <DEVSpp/ModelBase/Buffer.h>
#include <assert.h>
#include <float.h>

using namespace MB_Basic;


void Buffer::C1(const PortValue& x)
{
	if(x.port == in){ //receiving a client
		Job* client = dynamic_cast<Job*>(x.value);
		assert(client);
		m_Clients.push_back(client);
	}
	else if(x.port->Name.find("pull") != std::string::npos) 
	{
		for(int i=0; i<m_Osize; i++) {
			if(x.port == pull[i]) {
				m_OAvail[i]= true; //  server_i is available
				break;
			}
		}
	}
}

bool Buffer::Matched()
{
	if(m_Clients.empty() == false){
		for(int i=0; i < m_Osize; i++){// select server
			if(m_OAvail[i] == true){// server i is available
				m_OAvail[i]=false;// mark server_i non-available fron now on.
				send_index = i;
				m_phase = SENDTO; // we're going to send a client server_i
				return true;
			}
		}
		return false;
	}else
		return false;
}


Buffer::Buffer(const string& name, int osize): Atomic(name), m_Osize(osize)
{
	in = AddIP("in");

	m_cs = true; // default collecting statistics 

	for(int i=0; i<m_Osize; i++){
		out.push_back(AddOP(STR("out",i)));
		pull.push_back(AddIP(STR("pull",1)));
	}

	init();
}

/*virtual*/ void Buffer::init()
{
	m_phase = IDLE;
	m_OAvail.clear(); // clear first
	for(int i=0; i<m_Osize; i++)
		m_OAvail.push_back(false); //true); // add variable

	while(m_Clients.empty() == false)
	{
		Job* cl = m_Clients[0]; 
		m_Clients.pop_front();
		delete cl;
	}
}

/*virtual*/ bool Buffer::delta_x(const PortValue& x) 
{
	C1(x);
	if(m_phase == IDLE && Matched())
		return true; // reschedule as active
	else
		return false;
}

/*virtual*/ void Buffer::C2(PortValue& y) 
{
	if(m_phase == SENDTO) 
		y.Set(out[send_index], m_Clients[0]); // return dollar

	m_Clients.pop_front(); // changing state in delta_int() not in lambda()
}

/*virtual*/ void Buffer::delta_y(PortValue& y) 
{
	C2(y);
	if(Matched())
		return;
	else
		m_phase = IDLE;
}
 

/*virtual*/ Time Buffer::tau() const 
{ 
	if(m_phase == IDLE)
		return DBL_MAX;
	else
		return 2.0;
}

/*virtual*/ string Buffer::Get_s() const 
{
	string s = m_phase;
	if(m_phase == SENDTO)
		s+= STR("send_to: ", send_index);
	
	//char tmp[10]; sprintf_s(tmp, "%d",(int)m_Clients.size());
	s += string(",#J:")+STR((int)m_Clients.size()) +",AVs:";

	for(int i=0; i<m_Osize; i++)
	{
		if(i>0) s+=",";
		if(m_OAvail[i] == true)
			s += "1";
		else
			s += "0";
	}
	return s; // phase * #J: length * AVs: xxx
}

/*virtual*/ string Buffer::Get_Statistics_s() const 
{
	return STR((int)m_Clients.size()); // length Only
}

/*virtual*/ map<string, double> Buffer::GetPerformance() const
{
	map<string, double> statistics;
	if(m_cs) { 
		TimeSpan qln=0;
		for(map<string, double>::const_iterator it = m_statistics.begin();
			it != m_statistics.end(); it++) 
		{
			double probability = it->second / TimeCurrent();

			if(probability < 0.0 || probability > 1.0) {
				THROW_DEVS_EXCEPTION("Invalid Probability!");
			}
			else{
				int i = atoi(it->first.data());
				qln += probability * i;
			}
		}
 
		statistics.insert(make_pair("Average Q length: ", qln));
	}
	return statistics;
}