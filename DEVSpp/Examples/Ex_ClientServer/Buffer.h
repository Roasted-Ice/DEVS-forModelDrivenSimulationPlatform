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
#include "DEVS_Exception.h"
#include "StrUtil.h"

using namespace DEVSpp;

const string IDLE = "Idle";
const string SENDTO = "SendTo";

class Buffer: public Atomic
{
public:
	InputPort* in;
	vector<InputPort*> pull;
	vector<OutputPort*> out;

protected:
	std::deque<tmValue*> m_Clients;
	string m_phase;
	vector<bool> m_OAvail;
	const int m_Osize;
	int send_index;

	void C1(const PortValue& x)
	{
		if(x.port == in){ //receiving a client
			tmValue* client = dynamic_cast<tmValue*>(x.value);
			if(client) {
				m_Clients.push_back(client);
			} else
				THROW_DEVS_EXCEPTION("Dynamic Casting Failed!");
		}
		else // receiving a pull signal 
		{
			for(int i=0; i<m_Osize; i++) {
				if(x.port == pull[i]) {
					m_OAvail[i]= true; //  server_i is available
					break;
				}
			}
		}
	}

	bool Matched()
	{
		if(m_Clients.empty() == false){
			for(int i=0; i < m_Osize; i++){// select server
				if(m_OAvail[i] == true){// server i is available
					m_OAvail[i]=false;//Mark server_i non-available
					send_index = i; // remember i in send_index
					return true;
				}
			}
			return false;
		}else
			return false;
	}
	void C2(PortValue& y)
	{
		if(m_phase == SENDTO){
			y.Set(out[send_index], m_Clients[0]); 
			m_Clients.pop_front();// remove the first client 
		}
	}

public:
	Buffer(const string& name="", int osize=1): Atomic(name), m_Osize(osize)
	{
		CollectStatistics(); // default collecting statistics 
		in = AddIP("in"); //

		for(int i=0; i<m_Osize; i++){
			char tmp[10]; 
			::sprintf(tmp, "out%d", i); out.push_back(AddOP(tmp));
			::sprintf(tmp, "pull%d", i); pull.push_back(AddIP(tmp));
		}

		init();
	}
	virtual ~Buffer() { init(); }

    /*virtual*/ void init()
	{
		m_phase = IDLE;
		m_OAvail.clear(); // clear first
		for(int i=0; i<m_Osize; i++)
			m_OAvail.push_back(true); // add variable

		while(m_Clients.empty() == false)
		{
			tmValue* cl = m_Clients[0]; 
			m_Clients.pop_front();
			delete cl;
		}
	}

	/*virtual*/ Time tau() const 
	{ 
		if(m_phase == IDLE)
			return DBL_MAX;
		else
			return 2.0;
	}

	/*virtual*/ bool delta_x(const PortValue& x) 
	{
		C1(x);
		if(m_phase == IDLE){
			if(Matched()) {
				m_phase = SENDTO;//
				return true; // reschedule as active
			}
		}
		return false;
	}

	/*virtual*/ void delta_y(PortValue& y) 
	{
		C2(y);
		if(Matched())
			m_phase = SENDTO;
		else
			m_phase = IDLE;
	}

	/*virtual*/ string Get_s() const 
	{
		string s = m_phase;
		if(m_phase == SENDTO)
			s+= STR(",send_to: ", send_index);
		
		char tmp[10]; sprintf(tmp, "%d",(int)m_Clients.size());
		s += string(",#J:")+tmp+",AVs:";

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

	/*virtual*/ string Get_Statistics_s() const 
	{
		char tmp[10]; sprintf(tmp, "%d",(int)m_Clients.size());
		return string(tmp); // length Only
	}

	/*virtual*/ map<string, double> Buffer::GetPerformance() const
	{
		map<string, double> statistics;
		if(CollectStatisticsFlag()==true) { 
			TimeSpan E_i=0; // expectation of queueing line length
			for(map<string, double>::const_iterator it = m_statistics.begin();
				it != m_statistics.end(); it++) 
			{
				double probability=it->second/TimeCurrent(); // P(i)

				if(probability < 0.0 || probability > 1.0) {
					THROW_DEVS_EXCEPTION("Invalid Probability!");
				}
				else{
					int i = atoi(it->first.data());
					E_i += probability * i;// E(i)=\Sum_{i} i*P(i)
				}
			}
			string str = "Average Q length: ";
			statistics.insert(make_pair(str, E_i));
		}
		return statistics;
	}
};
