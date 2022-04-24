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
#include <DEVSpp/ModelBase/Transducer.h>

using namespace MB_Basic;

/*virtual*/ void Transducer::init()	
{ 
	SystemTimes.clear();
	while(buffer.empty()==false)
	{
		Job* v = buffer[0];
		buffer.pop_front();
		delete v;
	}
}
/*virtual*/ bool Transducer::delta_x(const PortValue& x) 
{
	Job* pv = dynamic_cast<Job*>(x.value);
	if(pv)
	{
		buffer.push_back(pv); // delete will be done in init()
//cout << x.value->STR() << endl;
		TimeSpan st = Devs::TimeCurrent() - pv->TimeMap["SysIn"]; //<- stamped by Generator
		if(SystemTimes.find(pv->type) == SystemTimes.end())
			SystemTimes.insert(make_pair(pv->type, vector<TimeSpan>() ));
		
		if(st >= 0)
			SystemTimes[pv->type].push_back(st);
		else
			THROW_DEVS_EXCEPTION("Invalid System Time!");
	}else
		cout << "Null received ! " << endl;
		//THROW_DEVS_EXCEPTION("Type casting Failed!");
	return false;
}

/*virtual*/ map<string, double> Transducer::GetPerformance() const
{
	map<string, double> statistics;
	if(CollectStatisticsFlag()) {
		for(map<int, vector<TimeSpan>>::const_iterator it = SystemTimes.begin(); 
			it != SystemTimes.end(); it++)
		{
			int type = it->first; 
			vector<TimeSpan> system_ts = it->second;
			int no = (int)system_ts.size();	
			char buf[100];
			if(no>0) {
				TimeSpan average_st=0;
				for(int i=0; i<no; i++)
					average_st += system_ts[i]; 
				average_st = average_st / (double)no;

				sprintf(buf, "Average System Time of Job%d (hour)", type);
				statistics.insert(make_pair(buf, average_st/3600.0));
				sprintf(buf, "# Job%d produced", type);
				statistics.insert(make_pair(buf, (double)no) );
				sprintf(buf, "Average Throughput (per hour) of Job%d", type);
				statistics.insert(make_pair(buf, (double)no/TimeCurrent()*3600.0) );
			} else { // no < 0
				sprintf(buf, "# Job%d produced", type);
				statistics.insert(make_pair(buf, 0.0) );
				sprintf(buf, "Average Throughput (per hour) of Job%d", type);
				statistics.insert(make_pair(buf, 0.0) );
			}
		}
	}
	return statistics;
}