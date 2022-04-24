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

#include <DEVSpp/ModelBase/Generator.h>
#include <float.h>

using namespace MB_Basic;

const string Generator::Work = "Work";
const string Generator::Idle = "Idle";

/*virtual*/ void Generator::init()	
{
	if(m_use_wait_signal)
		m_phase = Idle;
	else
		m_phase = Work;

	for(int i=0; i<(int)ClientMap.size();i++) {
		//char buf[10]; sprintf(buf,"%d",i);
		m_statistics.insert(make_pair(STR(i), 0));
	}
	m_no_gen = 0;
}
		
/*virtual*/ Time Generator::tau() const 
{ 
	static rv erv;
	if(m_phase == Idle)
		return DBL_MAX;
	else {
		TimeSpan t = erv.exp(mean_inter_gen_time);//erv.triangular(0.5, 1.0, 1.5); 
		return t;
	}
}

/*virtual*/ bool Generator::delta_x(const PortValue& x)
{
	if(x.port == is && m_use_wait_signal && m_phase == Idle)
	{
		m_phase = Work;
		return true;;
	}
	return false;
}

/*virtual*/ void Generator::delta_y(PortValue& y) {
	static rv urg;
	double r = urg.uniform(0.0,1.0);
	
	int i=0;
	double c=0.0; // cumulative proability value
	for(; i<(int)ClientMap.size(); i++)
	{
		if(r <= c+ClientMap[i].first) // making cdf from pmf
			break; // if find the range, break out.
		else
			c += ClientMap[i].first;
	}

	Job* pClient = NULL;
	int gen_type = -1;
	if(i<(int)ClientMap.size()){ // found case
		pClient = (Job*) ClientMap[i].second->Clone();
		pClient->id = m_no_gen;
		pClient->TimeMap.insert(make_pair("SysIn", Devs::TimeCurrent()));
		gen_type = pClient->type;
//cout << pClient->STR() << endl;
	}
	assert(pClient);
	y.Set(out, pClient); 

	if(m_use_wait_signal)
		m_phase = Idle;

	if(gen_type != -1) { // something generated
		m_no_gen ++;
		//-- tracking the generating jobs with its type
		//char buf[10]; sprintf(buf,"%d",gen_type);
		m_statistics[STR(gen_type)] = m_statistics[STR(gen_type)]+1; 
	}
}