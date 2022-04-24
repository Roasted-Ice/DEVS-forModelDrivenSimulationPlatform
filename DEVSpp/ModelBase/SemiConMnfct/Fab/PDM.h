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

#include <vector>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/SF_Export.h>

using namespace std;

namespace SF
{
	class MV_Process
	{
	public:
		int lot_id; // lot id
		int ws_id ; // workstation id
		double mean_pt; // mean_pt of processing time
		double STD_pt; // standard deviation of processing time
		MV_Process(int LID, int WID, double Mean, double std): lot_id(LID),
				ws_id(WID),  mean_pt(Mean), STD_pt(std){}
	};

	//-- class of Process Definition Devs
	class SF_EXP PDM
	{
		vector<MV_Process> m_PS; // Process Sequence
	public:
		PDM(){}
		//-- Add
		void Add(MV_Process ob) { m_PS.push_back(ob); }
		//-- Get size of Process Definition 
		int GetSize() const { return (int) m_PS.size(); }
		//-- clear all m_PS
		void Clear() { m_PS.clear(); }
		//-- Get Process Definitions
		vector<MV_Process> GetPS() const { return m_PS; }
		//-- 
		MV_Process Get(int i) const { return m_PS[i]; }
		//-- 
		bool GetProcess(int pro_index, MV_Process& mvp) const;
		 
		bool GetMeanProcessTime(int pro_index, double& mpt) const;
		 
		bool GetRemaingProcesses(int pro_index, vector<MV_Process>& rp) const;
		 
		bool GetRemainingMeanProcessTime(int pro_index, double& rmpt) const;
	};
}
