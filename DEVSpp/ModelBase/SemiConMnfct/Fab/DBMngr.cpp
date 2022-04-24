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
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/DBMngr.h>
#include <assert.h>

using namespace SF;

const string DBMngr::Idle="Idle"; 
const string DBMngr::Answer="Answer";  

/*virtual*/ void DBMngr::init()
{
	m_phase = Idle;

	//-- delete all Lots 
	while(m_MV_Lots.empty() ==false) {
		pair<int, MV_Lot*> lot = *m_MV_Lots.begin();
		m_MV_Lots.erase(m_MV_Lots.begin());
		delete lot.second;
	}
	m_IQueryBuffer.clear();
}

MV_Lot* DBMngr::FindMV_Lot(int job_id) const 
{
	map<int, MV_Lot*>::const_iterator it = m_MV_Lots.find(job_id);
	if(it == m_MV_Lots.end())
		return NULL;
	else 
		return it->second;
	
}

void DBMngr::RemoveMV_Lot(int job_id) 
{
	map<int, MV_Lot*>::iterator it = m_MV_Lots.find(job_id);
	if(it != m_MV_Lots.end())
		m_MV_Lots.erase(it);

}


bool DBMngr::MakeComplete(MV_Lot& lot)
{
	if(m_MapOfPDM.find(lot.type) == m_MapOfPDM.end())
		assert(0);
	else if(lot.m_CP_Index < m_MapOfPDM[lot.type].GetSize()-1)
		lot.m_CP_Index ++ ; // all finished up
	else if(lot.m_CP_Index == m_MapOfPDM[lot.type].GetSize()-1)
		lot.m_CP_Index = -1;
	else
		assert(0);
	return true;
}

bool DBMngr::OutputPrepared()
{
	if(m_IQueryBuffer.empty() == false) {
		Signal sig = m_IQueryBuffer[0];
		MV_Lot* pLot = FindMV_Lot(sig.job_id);
		if( pLot == NULL ) { //-- new one
			pLot = new MV_Lot(sig.job_id, sig.job_type, TimeCurrent());
			//m_MV_Lots.push_back(pLot);
			m_MV_Lots.insert(make_pair(pLot->id, pLot));
		}
		if(m_MapOfPDM.find(pLot->type) == m_MapOfPDM.end()) { // no PDM found
			assert(0);
			return false;
		} 
		else {
			m_Output = sig; // copy to m_Output 
			if( pLot->AllFinishedLot() )  
				m_Output.value = -1;
			else if(sig.action == MB_Basic::Signal::EVALUATION) {
				if(m_priority == GFAFS)
					m_Output.value = pLot->m_tA;
				else{ // GLWKR
					bool result = 
						m_MapOfPDM[pLot->type].GetRemainingMeanProcessTime(pLot->m_CP_Index, m_Output.value);
					assert(result);
				}
			}
			else // ROUTE
			{
				assert(m_MapOfPDM.find(pLot->type) != m_MapOfPDM.end());
				MV_Process pdm = m_MapOfPDM[pLot->type].Get(pLot->m_CP_Index);
				m_Output.value = pdm.ws_id; // next workstation id;
			}
		} 
		return true;
	}
	return false; // m_IQueryBuffer is asked questions, 
}

string DBMngr::Get_s() const 
{
	string str = m_phase;
	return str;
}

/*virtual*/ bool DBMngr::delta_x(const PortValue& x) 
{
	if(x.port == Query) {
		Signal* pSig = dynamic_cast<Signal*>(x.value);
		if(pSig)
			m_IQueryBuffer.push_back(*pSig);
		else
			THROW_DEVS_EXCEPTION("Invalid Message at DBMngr::delta_ext!");
	}
	else if(x.port == Completed)
	{
		Job* job = dynamic_cast<Job*>(x.value);
		if(job){
			MV_Lot* pLot = FindMV_Lot(job->id);
			double wkr;
			m_MapOfPDM[pLot->type].GetRemainingMeanProcessTime(pLot->m_CP_Index, wkr);
//cout << "job: type=" << job->type << ", id=" << job->id << ", cp=" 
//     << pLot->m_CP_Index << "/" << m_MapOfPDM[job->type].GetSize() << ", WKR=" << wkr
//	 << " is completed at " << TimeCurrent() << "!\n";
			assert(pLot);
			MakeComplete(*pLot); // make increased if possible.
			if(pLot->AllFinishedLot())
				cout << "job: type=" << job->type << ", id=" << job->id << ", WKR=" << wkr
					 << "'s all procedures are completed at " << TimeCurrent() << "!\n";
		} // pull signal 
	}
	else
		THROW_DEVS_EXCEPTION("Invalid Message at DBMngr::delta_ext!");
	
	if (m_phase == Idle && OutputPrepared()) {
		m_phase = Answer; 
        return true; // x_RescheduleMe();
	}
	return false;
}

/*virtual*/ void DBMngr::delta_y(PortValue& y) 
{
	static Signal outValue;
	outValue = m_Output;
	//OutputPort& op = (*OP.find(m_Output.resource)).second;
	OutputPort* op = GetOP(m_Output.resource);
	y.Set(op, &outValue);

	do {
		MV_Lot* pLot = FindMV_Lot(m_IQueryBuffer[0].job_id);
		m_IQueryBuffer.pop_front(); // remove the first input signal
		if( pLot->AllFinishedLot() ) {
			cout << "job_id="<<pLot->id<< " is removed at " << TimeCurrent() << "!\n";
			RemoveMV_Lot(pLot->id); // remove from m_MV_Lots
		}
	} while(m_IQueryBuffer.size() > 0 && OutputPrepared()==false);

	if(m_IQueryBuffer.empty()) // no more input signal to be replyed
		m_phase = Idle; // no more response needed
	else // broken by OutputPrepared() == true
		m_phase = Answer;
}