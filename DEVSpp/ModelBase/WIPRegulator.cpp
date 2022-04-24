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
#include <DEVSpp/ModelBase/WIPRegulator.h>
#include <assert.h>
#include <float.h>

using namespace MB_Basic;
 
const string Trigger="Trigger";
const string Idle = "Idle";

WIPRegulator::WIPRegulator(const string& name, unsigned upper_bound): 
	Atomic(name), m_upper_bound(upper_bound)
{
	in = AddIP("in");
	out = AddIP("out");
	pull = AddOP("pull");

	m_cs = true; // default collecting statistics 
	init();
}

/*virtual*/ void WIPRegulator::init()
{
	m_current_WIP = 0;
	m_phase = Trigger;
}

/*virtual*/ bool WIPRegulator::delta_x(const PortValue& x) 
{
	if(x.port == in)
		m_current_WIP++;
	else if(x.port == out)
		m_current_WIP--;
cout << "WIP: " << m_current_WIP << endl;
	if(m_phase == Idle && m_current_WIP < m_upper_bound) {
		m_phase = Trigger;
		return true;
	}
	return false;
}
/*virtual*/ void WIPRegulator::delta_y(PortValue& y) 
{
	static bValue<unsigned> request(1);
	y.Set(pull, &request); 
	m_phase = Idle;
} 

/*virtual*/ Time WIPRegulator::tau() const 
{ 
	if(m_phase == Idle)
		return DBL_MAX;
	else
		return 0.0;
}

/*virtual*/ string WIPRegulator::Get_s() const 
{
	string s = m_phase+", # of WIP:"+STR(m_current_WIP)+"\n";
	return s; // phase * #J: length * AVs: xxx
}

/*virtual*/ string WIPRegulator::Get_Statistics_s() const 
{
	return STR(m_current_WIP); // length Only
}

/*virtual*/ map<string, double> WIPRegulator::GetPerformance() const
{
	map<string, double> statistics;
	if(m_cs) { 
		TimeSpan qln=0;
		
		for(map<string, double>::const_iterator it = m_statistics.begin();
			it != m_statistics.end(); it++) 
			qln += it->second;

		if(IsEqual(qln, TimeCurrent(), 0.01) == false)
			THROW_DEVS_EXCEPTION("Invalid Statistics!");

		qln = 0;

		for(map<string, double>::const_iterator it = m_statistics.begin();
			it != m_statistics.end(); it++) 
		{
			double probability = it->second / TimeCurrent();

			if(probability < 0.0 || probability > 1.0) {
				THROW_DEVS_EXCEPTION("Invalid Probability!");
			}
			else{
				string tmp = it->first;
				double i = atoi(tmp.data());
cout << "WIP: " << i << ", P(WIP): " << probability;
				qln += probability * i;
			}
		}
 
		statistics.insert(make_pair("Average # of WIP: ", qln));
	}
	return statistics;
}