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
#include <vector>
#include <map>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CT_Export.h>

using namespace std;

namespace CT
{
	class MV_CM_PM;

    /// The monitoring Variable for a Wafer
    class MV_Wafer
    {
        /// a sequence of required processes that is not unfinished
	public:
		vector<pair<int, double>> m_UnfinishedProcesses;  

		/// index of vector<MV_CM_PM> where the wafer is on; -1 is not assigned
        int m_Source;
        /// index of vector<MV_CM_PM> where the wafer will be sent to; -1 is not assigned
        int m_Dest;

		/// Default Constructor
		MV_Wafer():m_Dest(-1), m_Source(-1) {}
		/// Check whther a wafer arrived or not.
        bool IsArrived() const { return (m_Source != -1 && m_Source == m_Dest); }

		string STR(const vector<MV_CM_PM*>& m_CM_PM) const ;
		void Set(string str, const vector<MV_CM_PM*>& m_CM_PM) ;

		//----
		double GetLocalNextProcessTime() const {
			return m_UnfinishedProcesses[0].second;
		}

		//---- 
		double GetLocalRemainingProcessTime() const {
			double rpt = 0;
			for(int i=0;i<(int)m_UnfinishedProcesses.size(); i++)
				rpt += m_UnfinishedProcesses[i].second;
			return rpt;
		}
    };

    /// <summary>
    /// The monitoring Variable for a cassette module (CM) and a processor module (PM)
    /// </summary>
    class MV_CM_PM
    {
	public:
		/// i,j index of vector<vector<VM_Wafer*> > which is assigned wafer,
		/// m_Assigned[0] = -1 if not assigned 
        int m_Assigned[2];

		enum PHASE { N/*New*/, W/*Working*/, F/*Finished*/};

		/// If m_Assigned is not NULL, 
        /// one of {New, Working, Finished} is possible 
		PHASE m_phase;

		/// all possible processes performed 
        map<int, int> m_PossibleProcesses;
        
        MV_CM_PM(const PHASE& phase) { init(phase); }

        /// Check if a CM_PM can provide the processes proc.
        /// return true if a set of prossible processes contains the proc as a key; false, otherwise;</returns>
        bool IsPossible(int proc) const { 
			return (m_PossibleProcesses.find(proc) != m_PossibleProcesses.end()) ; 
		}

		/// Check if a CM_PM can be assigned now.
        /// return true if nothing assigned now; false otherwise.</returns>
        bool IsAvailable() const { return (m_Assigned[0] == -1); }

        void init(const PHASE& phase)
        {
            m_phase = phase;
            m_Assigned[0] = m_Assigned[1] = -1;
        }

				
		string STR(const vector<vector<MV_Wafer*> >& m_MV_Wafers) const ;
		void Set(string str, const vector<vector<MV_Wafer*> >& m_MV_Wafers) ;
    };

	/// Reserve a wafer to go to a cmpm
    void ReserveToGoTo(vector<vector<MV_Wafer*> >& Wafers, int wi, int wj, 
		               vector<MV_CM_PM*>& CM_PM, int cpi);

	/// A wafer arrives AT a cmpm
    void ArriveAt(vector<vector<MV_Wafer*> >& Wafers, int wi, int wj, 
		               vector<MV_CM_PM*>& CM_PM, int cp);
     
    /// A wafer departs FROM a cmpm
    void DepartFrom(vector<vector<MV_Wafer*> >& Wafers, int wi, int wj, 
		               vector<MV_CM_PM*>& CM_PM, int cp);
}