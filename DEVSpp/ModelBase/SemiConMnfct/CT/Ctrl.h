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
#include <DEVSpp/Atomic.h>
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/StrUtil.h>
#include <DEVSpp/Coupled.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/MoniVar.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CT_Export.h>
#include <DEVSpp/ModelBase/Job.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Cassette.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Config.h>
#include <assert.h>
#include <iostream>
#include <float.h>
#include <algorithm>
#include <deque>

using namespace std;
using namespace DEVSpp;
using namespace MB_Basic;

namespace CT{

class MB_CT_EXP Ctrl: public Atomic
{
public:
	static const string Wait; 
	static const string CmdTransfer;  
	static const string CmdProcess;  
	static const string CmdCM_Out; 
	static const string Ask;

	vector<InputPort*>  ICM_Occu,     // for Cassette Arrival
                        ICM_Free,     // for Cassette DepartFromure
                        IPM_Finished; // for MV_Wafer-Processing Finished

    InputPort*          ITM;  // from transport module 
    vector<OutputPort*> OCPM; // for Cassette Moving Out or MV_Wafer Processing
    OutputPort*         OTM;  // for Control of Transport Module

	InputPort*          IDB; // Inputport from DB;
	OutputPort*         ODB; // Outputport to DB;

	int                ws_id; // work station group id
	string m_phase;
	vector<vector<MV_Wafer*> > m_MV_Wafers; // list of wafer information.

	vector<MV_CM_PM*> m_CM_PM;  // hardware of CM or PM

	//-- command info
	int m_oindex;   // for moving cassett fomr CM[m_oindex], 
	                // or process of PM[m_oindex]
	int m_TMcommand[2][2]; // m_TMcommand: [0][0]: source index, [0][1]: source high(slot) index
					       // m_TMcommand: [1][0]: dest. index,  [1][1]: dest. high(slot) index
	enum TMSTATUS {TO_PICK, TO_PLACE, IDLE}; 
	TMSTATUS m_TMphase;

	WaferSelectionRule m_SelectionRule;
	vector<Time> m_tA; //  arrival time of cassette at CM[i], i=0, 1
	vector<TimeSpan> m_TWRK; // for TWRK at CM[i], i=0, 1	

	deque<pair<int, Signal> > m_UnaskedJobs; // key: 0 or 1 for CM[i], Signal: 
	deque<pair<int, Signal> > m_AskedJobs; // key: 0 or 1 for CM[i], Signal: 
	deque<pair<int, Signal>> m_AnsweredJobs; // key: 0 or 1 for CM[i], Signal: 

	bool UseGlobalIndex() const { return m_SelectionRule == GLOBAL; }

	Time GetLocalArrivalTime(int cassette_index) const { return m_tA[cassette_index]; }

	Time GetGlobalArrivalTime(int cassette_index) const { 
		deque<pair<int, Signal>>::const_iterator it = m_AnsweredJobs.begin();
		for(; it != m_AnsweredJobs.end(); it++)
			if((*it).first == cassette_index){
				double at= (*it).second.at;
				return at;
			}

		assert(0);
		return 0.0;
	}

	double GetGlobalEvaluation(int cassette_index) const { 
		deque<pair<int, Signal>>::const_iterator it = m_AnsweredJobs.begin();
		for(; it != m_AnsweredJobs.end(); it++)
			if((*it).first == cassette_index){
				double v= (*it).second.value;
				return v;
			}

		assert(0);
		return 0.0;
	}

	TimeSpan GetLocalNextProcessTime(int cassette_index, int wafer_index) const
	{
		return m_MV_Wafers[cassette_index][wafer_index]->m_UnfinishedProcesses[0].second;
	}

	TimeSpan GetLocalTotalWork(int cassette_index) const { return m_TWRK[cassette_index]; }
	
	TimeSpan GetLocalTotalWorkRemaining(int cassette_index) const
	{
		TimeSpan twr = 0;
		for(int i=0; i < (int) m_MV_Wafers[cassette_index].size(); i++)
			twr += m_MV_Wafers[cassette_index][i]->GetLocalRemainingProcessTime();
		return twr;
	}

	void MoveFromUnaskedToAsked()
	{ 
		pair<int, Signal> v = m_UnaskedJobs[0];
		m_UnaskedJobs.pop_front();
		m_AskedJobs.push_back(v);
	}

	void MoveFromAskedToAnsweredJobs(Signal sig)
	{
		deque<pair<int, Signal>>::iterator it = m_AskedJobs.begin();
		for(; it != m_AskedJobs.end(); it++) {
			pair<int, Signal> v = *it;
			if( v.second.job_id == sig.job_id)
				break;
		}
		assert(it != m_AskedJobs.end());
		pair<int, Signal> v = *it;
		v.second = sig; // important to copy value!
		m_AnsweredJobs.push_back(v);
		m_AskedJobs.erase(it);
	}

	void RemoveFromAnsweredJobs(int CMindex)
	{
		deque<pair<int, Signal>>::iterator it = m_AnsweredJobs.begin();
		for(; it != m_AnsweredJobs.end(); it++)
			if((*it).first == CMindex)
				break;
		assert(it != m_AnsweredJobs.end());
		m_AnsweredJobs.erase(it);
	}

	/* cm_size: cassette module size, pm_size: process module size */
	Ctrl(const string& name, int wsg_id, int cm_size, int pm_size, WaferSelectionRule rule): 
		Atomic(name), ws_id(wsg_id), m_SelectionRule(rule)			
	{	
		ITM = AddIP("ITM");  
		OTM = AddOP("OTM");  
		IDB = AddIP("IDB");  
		ODB = AddOP("ODB");  
//cout << "cm_size=" << cm_size << ", pm_size=" << pm_size << endl;
		InitCMsPMs(cm_size, pm_size);
		
		for(int i=0; i < cm_size; i++)
			ICM_Free.push_back(AddIP("ICM_Free"+STR(i)));
		for(int i=0; i < cm_size; i++)
			ICM_Occu.push_back(AddIP("ICM_Occu"+STR(i)));
		for(int i=0; i <cm_size+pm_size; i++)
			OCPM.push_back(AddOP("OCPM"+STR(i)));
		for(int i=0; i < pm_size; i++)
			IPM_Finished.push_back(AddIP("IPM_Finished"+STR(i)));

		for(int i=0; i < cm_size; i++)
		{
			m_tA.push_back(-1.0); // non arrival
			m_TWRK.push_back(0); // no need to work
		}

		init(); 
	}
	int GetCMsize() const { return (int) m_MV_Wafers.size(); }

	void DeleteWafers(int i)
	{
		for(int j=0; j < (int) m_MV_Wafers[i].size(); j++)
			delete m_MV_Wafers[i][j];
		m_MV_Wafers[i].clear(); // clear wafers
	}

    /*virtual*/ void init()
	{
		m_phase = Wait;
		m_oindex = 0;
        m_TMphase = IDLE;
		m_TMcommand[0][0]=m_TMcommand[0][1]=m_TMcommand[1][0]=m_TMcommand[1][1]=0;

		//-- delete all wafers  
		for(int i=0; i< GetCMsize(); i++)
			DeleteWafers(i);

		//-- initalize all CM_PMs
		for (int i = 0; i < (int)m_CM_PM.size(); i++)
        {
            if (i < GetCMsize() ) // == |CM| 
				m_CM_PM[i]->init(MV_CM_PM::F); // imake CM finished
            else
				m_CM_PM[i]->init(MV_CM_PM::N); // make PM new
        }

		for(int i=0; i < GetCMsize(); i++)
		{
			m_tA[i] = -1.0; // non arrival
			m_TWRK[i] = 0; // no need to work
		}

		m_UnaskedJobs.clear();  // clear all memory
		m_AskedJobs.clear();  // clear all memory
		m_AnsweredJobs.clear();  // clear all memory
	}

	/*virtual*/ TimeSpan tau() const 
	{ 
		if (m_phase == CmdTransfer || m_phase == CmdProcess ||
            m_phase == CmdCM_Out || m_phase == Ask)
			return 0;
        else
            return DBL_MAX; 
	}

	/*virtual*/ bool delta_x(const PortValue& x) 
	{
		UpdateMVs(x);
        if (m_phase == Wait)
        {
            CheckCommand();
            return true; //x_RescheduleMe();
        }
		return false;
	}

	/*virtual*/ void delta_y(PortValue& y) 
	{
		static TMcmd tmcmd;
		static bValue<double> bv(0);
		if (m_phase == CmdTransfer){
			tmcmd.c[0][0] = m_TMcommand[0][0]; tmcmd.c[0][1] = m_TMcommand[0][1];
			tmcmd.c[1][0] = m_TMcommand[1][0]; tmcmd.c[1][1] = m_TMcommand[1][1];
			y.Set(OTM, &tmcmd);
		}else if (m_phase == CmdProcess) {
			int wi = m_CM_PM[m_oindex]->m_Assigned[0];
			int wj = m_CM_PM[m_oindex]->m_Assigned[1];
            bv.v = m_MV_Wafers[wi][wj]->GetLocalNextProcessTime();
            y.Set(OCPM[m_oindex], &bv); 
		}
		else if (m_phase == CmdCM_Out) {
			bv.v = 1;
            y.Set(OCPM[m_oindex], &bv);
		}else if(m_phase == Ask)
		{
			static Signal osig;
			osig = m_UnaskedJobs[0].second;
			y.Set(ODB, &osig);
		}
		 
		CheckCommand();
	}


	/*virtual*/ string Get_s() const;
//	/*virtual*/ void Set_s(const string& str);
private:

    /// Allocate two variables for Wafers and CM_PM;
    void InitCMsPMs(int cm_size, int pm_size)
    {
        for (int i = 0; i < cm_size; i++)
            m_MV_Wafers.push_back(vector<MV_Wafer*>());

        for (int i = 0; i < cm_size+pm_size; i++)
        {
            MV_CM_PM* p ;
            if (i < cm_size)
            {
				p = new MV_CM_PM(MV_CM_PM::F);  // processing done alway
                p->m_PossibleProcesses.insert(make_pair(0, 0));     // 0: storing
            }
            else
            {
                p = new MV_CM_PM(MV_CM_PM::N); // processing not done. new
                int j = i - cm_size;
                if (j == 0)
                    p->m_PossibleProcesses.insert(make_pair(1, 1)); // 1: eaching for 1
				else if (j == 1)
                    p->m_PossibleProcesses.insert(make_pair(11, 11)); // 11: eaching for 11
                else if (j == 2)
                    p->m_PossibleProcesses.insert(make_pair(2, 2)); // 2: cleaning
                else // (j==3)
                    p->m_PossibleProcesses.insert(make_pair(3, 3)); // 3: drying
            }
            m_CM_PM.push_back(p);
        }// for cm_size+pm_size
    }


	bool IsFrom(const PortValue& x, const vector<InputPort*>& IP, int& index)
    {
        for (int i = 0; i < (int)IP.size(); i++)
        {
            if (x.port == IP[i])
            {
                index = i;
                return true;
            }
        }
        return false;
    }

	//------------------------------- Update State Variables --------------------------
    void MakeCassetteGone(int cassette_index)
    { 
		CT::DepartFrom(	m_MV_Wafers, 
						m_CM_PM[cassette_index]->m_Assigned[0], 
						m_CM_PM[cassette_index]->m_Assigned[1],
						m_CM_PM, cassette_index);
		DeleteWafers(cassette_index);
    } // make it empty 

public:

	/// Make a new cassette at CM i
	/// </summary>
	/// <param name="cassette_index"> means the order of teh CM module</param>
	static void MakeCassetteNew(int cassette_index, int lot, int ws_id,
							    vector<vector<MV_Wafer*> >& Wafers,
								vector<Time>& ArrivalTimes,
								vector<TimeSpan>& TotalProcessTimes,
								const vector<MV_CM_PM*>& CM_PM)
	{
		if (Wafers[cassette_index].size() > 0)
			THROW_DEVS_EXCEPTION("Non-empty Cassette");
		int n_slot = Config::GetNoSlots(lot);
		for (int i = 0; i < n_slot; i++)
		{
			MV_Wafer& wafer = *new MV_Wafer();
			vector<pair<int, double> > processInfo = Config::GetLotProcess(lot, ws_id);
			TotalProcessTimes[cassette_index] = 0.0;
			for(int j=0; j<(int)processInfo.size(); j++) {
				wafer.m_UnfinishedProcesses.push_back(processInfo[j]);
				TotalProcessTimes[cassette_index] += processInfo[j].second;
			}
			wafer.m_UnfinishedProcesses.push_back(make_pair(-1, 0)); // -1 end of processes required
			wafer.m_Source = wafer.m_Dest = cassette_index;
			Wafers[cassette_index].push_back(&wafer);

		}
		ArrivalTimes[cassette_index] = Devs::TimeCurrent();
	}

private:
	void UpdateMVAfterTMPick()
    {
		if (m_TMphase == TO_PICK) {
			if(m_TMcommand[0][0] >= GetCMsize() ) // Pick from PM 
				CT::DepartFrom( m_MV_Wafers,
								m_CM_PM[m_TMcommand[0][0]]->m_Assigned[0], 
								m_CM_PM[m_TMcommand[0][0]]->m_Assigned[1], 
								m_CM_PM, m_TMcommand[0][0] );
			m_TMphase = TO_PLACE; // make it ToPlace
		}
		else
			THROW_DEVS_EXCEPTION("Invalid TM Working!");
    }

	void UpdateMVAfterTMPlace()
    {
		static int i=0;
		if (m_TMphase == TO_PLACE) {
//cout << "i=" << i++;
//cout << ", m_TMcommand[1][0]="<<m_TMcommand[1][0] << ",  m_CM_PM[m_TMcommand[1][0]]->m_phase=" ;
//cout << m_CM_PM[m_TMcommand[1][0]]->m_phase << ", m_Assigned=" << m_CM_PM[m_TMcommand[1][0]]->m_Assigned << endl;
			CT::ArriveAt(	m_MV_Wafers,
							m_CM_PM[m_TMcommand[1][0]]->m_Assigned[0], 
							m_CM_PM[m_TMcommand[1][0]]->m_Assigned[1], 
							m_CM_PM,m_TMcommand[1][0]);
			if (m_TMcommand[1][0] >= GetCMsize()) // Place to PM 
				m_CM_PM[m_TMcommand[1][0]]->m_phase = MV_CM_PM::N; // new one comes in
			m_TMphase = IDLE; // make it idle
		}
		else
			THROW_DEVS_EXCEPTION("Invalid TM Working!");
    }

    /// Update a set of monitoring variables
    void UpdateMVs(const PortValue& x)
    {
		int index;
        if (x.port == ITM)// from Transport Module
        {
			bValue<int>* iv = dynamic_cast<bValue<int>*>(x.value);
            if (iv && iv->v == 1) // end of ShrinkAfterPick
                UpdateMVAfterTMPick();
            else if (iv && iv->v == 0) // end of ShrinkAfterPlace 
                UpdateMVAfterTMPlace();
            else
                THROW_DEVS_EXCEPTION("Unknow Value!");
        }
		else if(x.port == IDB)
		{
			Signal* sig = dynamic_cast<Signal*>(x.value);
			if(sig)
				MoveFromAskedToAnsweredJobs(*sig);
		}
		else if (IsFrom(x, ICM_Free, index)) {// from ICM_Free
            MakeCassetteGone(index);
			if(UseGlobalIndex() && m_AnsweredJobs.size()>0)
				RemoveFromAnsweredJobs(index);
		}
		else if (IsFrom(x, ICM_Occu, index)) {// from ICM_Occu
            //bValue<int>* iv = dynamic_cast<bValue<int>*>(x.value);
			Cassette* pC = dynamic_cast<Cassette*>(x.value);
			MakeCassetteNew(index, pC->type, ws_id, m_MV_Wafers, m_tA, m_TWRK, m_CM_PM);
			if(UseGlobalIndex()) { //Only for Global Index case
				Signal sig(pC->type, pC->id, Parent->Name.data(), TimeCurrent());
				m_UnaskedJobs.push_back(make_pair(index, sig)); // 
			}
		}
		else if (IsFrom(x, IPM_Finished, index))
        {   
			index = index + GetCMsize(); // 
			m_CM_PM[index]->m_phase = MV_CM_PM::F; // working done
			int i=m_CM_PM[index]->m_Assigned[0];
			int j=m_CM_PM[index]->m_Assigned[1];
			if(i > -1 && j > -1 && (int)m_MV_Wafers[i][j]->m_UnfinishedProcesses.size() > 0)
				m_MV_Wafers[i][j]->m_UnfinishedProcesses.erase(
				m_MV_Wafers[i][j]->m_UnfinishedProcesses.begin() ); // delete the first
        }
    }
	//------------------------------------------------------------------------------------------------

	//------------------ Determine Control State according to Command --------------------------------
    /// Check if all processes of a wafer is finished so it should return to a CM.
    bool CheckMV_WaferReturnToCM()
    {
        for (int i = 0; i < GetCMsize(); i++)
        {
			int n_slot = (int)m_MV_Wafers[i].size();
            for (int j = 0; j < n_slot; j++)
            {
                if (m_MV_Wafers[i][j]->m_UnfinishedProcesses[0].first == -1 && // end of process
                    m_MV_Wafers[i][j]->m_Dest != i) // not assigned to go home
                {   // it sould go back CM[i][j] because it came from it.
                    // source and destination
					assert(m_MV_Wafers[i][j]->m_Source != -1);
                    m_TMcommand[0][0] = m_MV_Wafers[i][j]->m_Source; // source inidex
                    m_TMcommand[0][1] = 0; // process hight is 0
                    m_TMcommand[1][0] = i; // home index
                    m_TMcommand[1][1] = j; // home hight
					CT::ReserveToGoTo(m_MV_Wafers, i, j, m_CM_PM, i);
                 //   printf("Tras:ToCM, (%d,%d) to (%d,%d)",m_TMcommand[0][0], m_TMcommand[0][1], 
														   //m_TMcommand[1][0], m_TMcommand[1][1]);
                    return true;
                }
            }
        }
        return false;
    }

	struct PossiblePair
	{
		int w_i, w_j; // wafer index of cassette
		int cm_pm_k; //	index of CM and PM modules
		double value; // evaluation value
		double at; // arrival time for tie breaking

		PossiblePair(int i,int j, int k, double v, double t):w_i(i), w_j(j), cm_pm_k(k), value(v), at(t){}
		const PossiblePair& operator=(const PossiblePair& ob)
		{
			w_i = ob.w_i; w_j = ob.w_j; cm_pm_k = ob.cm_pm_k; value = ob.value; at = ob.at;
			return *this;
		}
	};

	//----- functor for sorting by next schedule time ---------
	struct lt_PP
	{
	  bool operator()(PossiblePair p1, PossiblePair p2) const
	  {
		  return p1.value < p2.value || (p1.value == p2.value && p1.at < p2.at);
	  }
	};

	bool SelectOne(vector<PossiblePair>& PP)
	{
		if(PP.empty()) 
			return false;
		else { 
			sort(PP.begin(), PP.end(), lt_PP());
			if(m_SelectionRule == LMWKR){ // most => make reverse
				reverse(PP.begin(), PP.end());
			}

			PossiblePair pp = PP[0];
			int i=pp.w_i, j = pp.w_j, k = pp.cm_pm_k;
			//------------ Assing Transfer Command --------------------------//
			assert(m_MV_Wafers[i][j]->m_Source != -1);
			m_TMcommand[0][0] = m_MV_Wafers[i][j]->m_Source; // current posi of source
			if (m_TMcommand[0][0] < GetCMsize()) // if at cassette module
				m_TMcommand[0][1] = j; // at the slot of a cassette
			else // if at a PM module
				m_TMcommand[0][1] = 0; // process hight is 0
			m_TMcommand[1][0] = k;
			m_TMcommand[1][1] = 0;

			CT::ReserveToGoTo(m_MV_Wafers, i, j, m_CM_PM, k);
			//printf("Tras:ToPM, (%d,%d) to (%d,%d)", m_TMcommand[0][0], m_TMcommand[0][1], 
									//                m_TMcommand[1][0], m_TMcommand[1][1]);
			//---------------------------------------------------------------------------
			return true;
		}
	}

    bool CheckMV_WaferMoveToPM()
    {
		if( UseGlobalIndex() && m_UnaskedJobs.size()>0 || m_AskedJobs.size() > 0)
			return false;

		vector<PossiblePair> PP;
        for (int i = 0; i < GetCMsize() ; i++)
        {
			int n_slot = (int)m_MV_Wafers[i].size();
            for (int j = 0; j < n_slot; j++)
            {
                if (m_MV_Wafers[i][j]->m_Source != m_MV_Wafers[i][j]->m_Dest) // moving now
                    continue;
				if (m_CM_PM[m_MV_Wafers[i][j]->m_Dest]->m_phase != MV_CM_PM::F) // waiting next process
                    continue;
                if ((int)m_MV_Wafers[i][j]->m_UnfinishedProcesses.size() > 0)
                {
                    for (int k = 0; k < (int)m_CM_PM.size(); k++)
                    {

                        if( m_CM_PM[k]->IsPossible(m_MV_Wafers[i][j]->m_UnfinishedProcesses[0].first) &&
                            m_CM_PM[k]->IsAvailable() )
							if(m_SelectionRule == LCM0F)
							{
								//------------ Assing Transfer Command --------------------------//
								assert(m_MV_Wafers[i][j]->m_Source != -1);
								m_TMcommand[0][0] = m_MV_Wafers[i][j]->m_Source;
								if (m_TMcommand[0][0] < GetCMsize())
									m_TMcommand[0][1] = j; // at the slot of a cassette
								else // at a PM module
									m_TMcommand[0][1] = 0; // process hight is 0
								m_TMcommand[1][0] = k;
								m_TMcommand[1][1] = 0;

								CT::ReserveToGoTo(m_MV_Wafers, i, j, m_CM_PM, k);
								//printf("Tras:ToPM, (%d,%d) to (%d,%d)", m_TMcommand[0][0], m_TMcommand[0][1], 
														//                m_TMcommand[1][0], m_TMcommand[1][1]);
								//---------------------------------------------------------------------------
								return true;
							}
							else if(m_SelectionRule == LFAFS)
								PP.push_back(PossiblePair(i, j, k, GetLocalArrivalTime(i), GetLocalArrivalTime(i)));
							else if(m_SelectionRule == LTWRK) //
								PP.push_back(PossiblePair(i, j, k, GetLocalTotalWork(i), GetLocalArrivalTime(i)));
							else if(m_SelectionRule == LSPT) //
								PP.push_back(PossiblePair(i, j, k, GetLocalNextProcessTime(i,j), GetLocalArrivalTime(i)));
							else if(m_SelectionRule == LLWKR || m_SelectionRule == LMWKR) // GetLocalTotalWorkRemaining
								PP.push_back(PossiblePair(i, j, k, GetLocalTotalWorkRemaining(i), GetLocalArrivalTime(i)));
							else if(m_SelectionRule == GLOBAL) {
								double v = GetGlobalEvaluation(i)+GetLocalTotalWorkRemaining(i);
								double at = GetGlobalArrivalTime(i);
								PP.push_back(PossiblePair(i, j, k, v, at));
							} 
							//	PP.push_back(PossiblePair(*this, i,j,k));
                    }
                }
            }
        }
		if(m_SelectionRule == LCM0F)
			return false; // nothing selected
		else 
			return SelectOne(PP);
    }
    /// Check if Transportation Command should be generated
    /// <returns>If true, m_phase := PHASE.CmdTransfer; </returns>
    bool CheckTransportableMV_Wafer()
    {
        if (m_TMphase != IDLE) return false;
        else if (CheckMV_WaferReturnToCM() || CheckMV_WaferMoveToPM())
        {
            m_TMphase = TO_PICK;
            return true;
        }
        else 
			return false;
    }
    /// Check if a new wafer is waiting for next process on a PM.
    /// <returns>if true, m_phase := PHASE.CmdProcess</returns>
    bool CheckNewMV_WaferOccupied()
    {
        for (int i = GetCMsize(); i < (int)m_CM_PM.size(); i++)
        {
			int wi = m_CM_PM[i]->m_Assigned[0];
			int wj = m_CM_PM[i]->m_Assigned[1];
            if ( wi != -1 && wj != -1 &&
                m_MV_Wafers[wi][wj]->m_Source == i &&
				m_CM_PM[i]->m_phase == MV_CM_PM::N)
            {
				m_CM_PM[i]->m_phase = MV_CM_PM::W;
                m_oindex = i;
                return true;
            }
        }
        return false;
    }

    /// Check if a cassette is fulled with all finished wafers.
    /// <returns>if true, m_phase := PHASE.CmdCM_Out</returns>
    bool CheckCassetteFinished()
    {
        for (int i = 0; i < GetCMsize(); i++)
        {
			int n_slot = (int)m_MV_Wafers[i].size();
            if (n_slot == 0)
                continue;
             
            bool found = true;
			
            for (int j = 0; j < n_slot; j++)
            {
                if (m_MV_Wafers[i][j]->IsArrived() ==false ||
                    m_MV_Wafers[i][j]->m_Dest != i || 
                    m_MV_Wafers[i][j]->m_UnfinishedProcesses[0].first != -1)
                {
                    found = false;
                    break;
                }
            }
            if (found)
            {
                m_oindex = i; // CM[i] is going to move out !
                for (int j = 0; j < n_slot; j++) // switch the last 
                    m_MV_Wafers[i][j]->m_UnfinishedProcesses[0].first = INT_MIN;
                return true;
            }
        }
        return false;
    }

    /// Check we should generate a command.
    void CheckCommand()
    {
		if (m_phase == Ask)
			MoveFromUnaskedToAsked();
		if (m_UnaskedJobs.size() > 0 )
			m_phase = Ask;
        else if (CheckTransportableMV_Wafer())// idle -> check command to TM
            m_phase = CmdTransfer;
        else if (CheckNewMV_WaferOccupied())
            m_phase = CmdProcess;
        else if (CheckCassetteFinished())
            m_phase = CmdCM_Out;
        else
            m_phase = Wait;
    }
	//------------------------------------------------------------------------------------------------
}; 
}