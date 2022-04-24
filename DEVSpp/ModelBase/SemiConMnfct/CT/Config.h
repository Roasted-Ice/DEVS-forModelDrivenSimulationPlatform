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
#pragma once
#pragma warning(disable:4251)
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CT_Export.h>
#include <map>
#include <vector>

using namespace std;

namespace CT
{
	class MB_CT_EXP Config
    {
	protected:
		/// key: lot type, value: no of wafers
		static map<int, int> lot2wafers;
		/// key: pair<int, int>: firs: lot type, second: work station id
		/// value: sequence of pairs whose key is process id and value is process time.
		static map<pair<int, int>, vector<pair<int, double>> > lot2processes; 

	public:
        static void SetNoSlots(int lot_type, int nsl) { lot2wafers[lot_type] = nsl; }
        static int GetNoSlots(int lot_type) { return lot2wafers[lot_type]; }

        static void SetLotProcessTime(int lot_type, int ws_id, vector<pair<int, double>> pt) 
										{ lot2processes[make_pair<int,int>(lot_type, ws_id)] = pt; }
        static double GetLotProcessTime(int lot_type, int ws_id) 
		{ 
			/*assert( lot2processes.find(make_pair<int,int>(lot_type, ws_id)) !=
				    lot2processes.end()); */
			vector<pair<int, double>> prc = lot2processes[make_pair<int,int>(lot_type, ws_id)];
			double pt = 0;
			for(int i=0; i<(int)prc.size();i++)
				pt += prc[i].second;
			return pt*GetNoSlots(lot_type);
		}
        static vector<pair<int, double>> GetLotProcess(int lot_type, int ws_id) 
										{ return lot2processes[make_pair<int,int>(lot_type, ws_id)]; }
    };

	enum WaferSelectionRule 
	{
		LCM0F, // CM0 first if waiting one there 
		LFAFS, // (First Arrival First Served) the wafter whose lot arrives first (static)
		LTWRK, // (Total Work) the least total processing time (static)
		LSPT,  // (Shortes Processing Time) the shortest imminent operation time of wafer (dynamic)
		LLWKR, // (Least Work Remaining) the least amount time of total processing remaining of wafer (dynamic)
		LMWKR, // (MORE Work Remaining) the least amount time of total processing remaining of wafer (dynamic)
		GLOBAL 
	};
}