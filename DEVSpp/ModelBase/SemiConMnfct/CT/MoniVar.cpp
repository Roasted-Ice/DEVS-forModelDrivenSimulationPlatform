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
#include <DEVSpp/ModelBase/SemiConMnfct/CT/MoniVar.h>
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/StrUtil.h>
#include <assert.h>

using namespace CT;
using namespace DEVSpp;

string MV_Wafer::STR(const vector<MV_CM_PM*>& m_CM_PM) const 
{ 
	// list of wafer information.
	string str = ::STR(m_Source)+":"+::STR(m_Dest);
	str +=":Unfinished";
	for(int i=0; i< (int)m_UnfinishedProcesses.size(); i++)
		str +=":"+::STR(m_UnfinishedProcesses[i].first)
		     +":"+::STR(m_UnfinishedProcesses[i].second);

	return str;
}

/*
void MV_Wafer::Set(string str, const vector<MV_CM_PM*>& m_CM_PM) 
{
	vector<string> tmp = DEVSpp::Split(str, ':');
	m_Source = atoi(tmp[0].data());
	m_Dest = atoi(tmp[1].data());

	assert(tmp[2]=="Unfinished");
	for(int i=3; i<(int)tmp.size(); i++)
	{
		int pro = atoi(tmp[i].data());
		double pro_t = atof(tmp[++i].data());
		m_UnfinishedProcesses.push_back(make_pair(pro,pro_t));
	}
}
*/

string MV_CM_PM::STR(const vector<vector<MV_Wafer*> >& m_MV_Wafers) const
{
	string str;
	if(m_phase == MV_CM_PM::N) str = "N:";
	else if(m_phase == MV_CM_PM::W)	str = "W:";
	else str = "F:";

	str+=::STR(m_Assigned[0])+":"+::STR(m_Assigned[1]);
	return str;
}

/*
void MV_CM_PM::Set(string str, const vector<vector<MV_Wafer*> >& m_MV_Wafers)
{
	vector<string> tmp = DEVSpp::Split(str, ':');
	if(tmp[0] == "N") m_phase = MV_CM_PM::N;
	else if(tmp[0] == "W") m_phase = MV_CM_PM::W;
	else if(tmp[0] == "F") m_phase = MV_CM_PM::F;
	else assert(0);

	m_Assigned[0] = atoi(tmp[1].data());
	m_Assigned[1] = atoi(tmp[2].data());
}
*/

//-------------------------------------------------------------------------
void CT::ReserveToGoTo(vector<vector<MV_Wafer*> >& Wafers, int wi, int wj, 
		               vector<MV_CM_PM*>& CM_PM, int cp)
{
  /*  wafer->m_Dest = cmpm;
    cmpm->m_Assigned = wafer;*/
	Wafers[wi][wj]->m_Dest = cp;
	CM_PM[cp]->m_Assigned[0]=wi;
	CM_PM[cp]->m_Assigned[1]=wj;
}
 
/// A wafer arrives AT a cmpm
void CT::ArriveAt(/*MV_Wafer* wafer, MV_CM_PM* cmpm*/
				  vector<vector<MV_Wafer*> >& Wafers, int wi, int wj, 
		               vector<MV_CM_PM*>& CM_PM, int cp)
{
    //if (wafer->m_Dest != cmpm)
    //    THROW_DEVS_EXCEPTION("Invalid Arrival!");
    //wafer->m_Source = cmpm; // source becomes destination.
	if(Wafers[wi][wj]->m_Dest != cp)
		THROW_DEVS_EXCEPTION("Invalid Arrival!");
    Wafers[wi][wj]->m_Source = cp; // source becomes destination.
}

/// A wafer departs FROM a cmpm
void CT::DepartFrom(/*MV_Wafer* wafer, MV_CM_PM* cmpm*/
					vector<vector<MV_Wafer*> >& Wafers, int wi, int wj, 
		               vector<MV_CM_PM*>& CM_PM, int cp)
{
	//if(wafer != NULL) {
	//	if (wafer->m_Source != cmpm)
	//		THROW_DEVS_EXCEPTION("Invalid Departure!");
	//	wafer->m_Source = NULL; // source becomes free.
	//}
 //   if (cmpm->m_Assigned != wafer)
 //       THROW_DEVS_EXCEPTION("Invalid Departure!");

 //   cmpm->m_Assigned = NULL; // get free

	if(wi != -1 && wj != -1 && Wafers.empty()== false && Wafers[wi].empty() == false) {
		if (Wafers[wi][wj]->m_Source != cp)
			THROW_DEVS_EXCEPTION("Invalid Departure!");
		Wafers[wi][wj]->m_Source = -1; // source becomes free.
	}
    if (CM_PM[cp]->m_Assigned[0] != wi || CM_PM[cp]->m_Assigned[1] != wj)
        THROW_DEVS_EXCEPTION("Invalid Departure!");

    CM_PM[cp]->m_Assigned[0] = -1;  CM_PM[cp]->m_Assigned[1] = -1; // get free
}