/*************************************************************************
 * DEVS++: DEVS Open Source in C++                                       *
 * Copyright (C) 2005-2009  Moon Ho Hwang <moonho.hwang@gmail.com>       *
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
#pragma warning(disable:4786)
#include "Coupled.h"
#include "Atomic.h"
#include "DEVS_Exception.h"
#include "ClassAdmin.h"
#include <assert.h>
#include <algorithm>
#include <float.h>
#include "StrUtil.h"

using namespace DEVSpp;
 
IMPLEMENT_CLASS1(Coupled, "Coupled", 20090806, Devs);

void Coupled::AddModel(Devs* md) 
{ 
	if(find(M.begin(), M.end(), md)==M.end()){
		M.push_back(md); 
		md->Parent = this;
		//Schedule.push_back(md); // -- for dynamic structuring
		
		if(md->TimeNext()==DBL_MAX)
			
		Update_tL_tN();
	}
}


void Coupled::RemoveModel(Devs* md) 
{ 
	deque<Devs*>::iterator it = find(M.begin(), M.end(), md);
	if(it!=M.end()){
		M.erase(it); 
		//deque<Devs*>::iterator jt = find(Schedule.begin(), Schedule.end(), md);
		//if(jt != Schedule.end())
		//	Schedule.erase(jt); // -- for dynamic structuring
		Update_tL_tN();
	}
}

Coupled::~Coupled()
{
	while(M.empty()==false)
	{
		Devs* md = M.front();
		M.pop_front();
		delete md;
	}
}

Devs* Coupled::GetModel(const string& name) const
{
	for(deque<Devs*>::size_type i = 0; i < M.size(); i++)
		if(M[i]->Name == name)
			return M[i];

	return NULL;
}


/* A comparable structure.  
   This will be used for sorting models by their next-event schedules */
struct lt_MtN
{
  bool operator()(const Devs* s1, const Devs* s2) const
  {
    return (s1->TimeNext() < s2->TimeNext()) ;
  }
};//-------------------------------------------------------

void Coupled::Update_tL_tN()
{
	t_Last = TimeCurrent();
	t_Next = DBL_MAX;
	for(int i=0; i<(int)M.size(); i++) // Schedule.size(); i++)
	{
		if(M[i]->TimeNext() < t_Next) {
			t_Next = M[i]->TimeNext();
			First = M[i];
		}
	}

	// Using Sort !
	//sort(Schedule.begin(), Schedule.end(), lt_MtN());
	//if(Schedule.empty() == false){
	//	t_Next = Schedule.front()->TimeNext();
	//	for(deque<Devs*>::size_type i=0; i<M.size(); i++)
	//	{
	//		Time tl = M[i]->TimeLast();
	//		if( t_Last < tl) t_Last = tl; // t_Last=max(...);
	//	}
	//}
}

//-- external input coupling
void Coupled::AddCP(InputPort* spt, InputPort* dpt)
{
	if(spt == NULL || dpt == NULL || 
		!(spt->Parent == this && dpt->Parent != this))
	{
		cout << "Assert Failure in External Input Coupling!\n";
		assert (0);
	}
	spt->ToP.push_back(dpt);
	dpt->FromP.push_back(spt);
}

void Coupled::RemoveCP(InputPort* spt, InputPort* dpt){ // EIC
	if(spt == NULL || dpt == NULL || 
		!(spt->Parent == this && dpt->Parent != this))
	{
		cout << "Assert Failure in External Input Coupling!\n";
		assert (0);
	}
	spt->ToP.erase(std::find(spt->ToP.begin(), spt->ToP.end(), dpt));
	dpt->FromP.erase(std::find(dpt->FromP.begin(), dpt->FromP.end(), spt));
}


//-- internal coupling
void Coupled::AddCP(OutputPort* spt, InputPort* dpt)
{
	if(spt == NULL || dpt == NULL || 
		!(spt->Parent != this && dpt->Parent != this))
	{
		cout << "Assert Failure in Internal Coupling!\n";
		assert (0);
	}
	spt->ToP.push_back(dpt);
	dpt->FromP.push_back(spt);
}

void Coupled::RemoveCP(OutputPort* spt, InputPort* dpt){ // IT
if(spt == NULL || dpt == NULL || 
		!(spt->Parent != this && dpt->Parent != this))
	{
		cout << "Assert Failure in Internal Coupling!\n";
		assert (0);
	}
	spt->ToP.erase(std::find(spt->ToP.begin(), spt->ToP.end(), dpt));
	dpt->FromP.erase(std::find(dpt->FromP.begin(), dpt->FromP.end(), spt));
}
//-- external output coupling
void Coupled::AddCP(OutputPort* spt, OutputPort* dpt)
{
	if(spt == NULL || dpt == NULL ||
		!(spt->Parent != this && dpt->Parent == this))
	{
		cout << "Assert Failure in External Output Coupling!\n";
		assert (0);
	}
	spt->ToP.push_back(dpt);
	dpt->FromP.push_back(spt);
}

void Coupled::RemoveCP(OutputPort* spt, OutputPort* dpt){// EOC
	if(spt == NULL || dpt == NULL ||
		!(spt->Parent != this && dpt->Parent == this))
	{
		cout << "Assert Failure in External Output Coupling!\n";
		assert (0);
	}
	spt->ToP.erase(std::find(spt->ToP.begin(), spt->ToP.end(), dpt));
	dpt->FromP.erase(std::find(dpt->FromP.begin(), dpt->FromP.end(), spt));
}

void MakeIndentation(const string& name, string& str)
{
	str += "\n";
	for(int i=0; i<(int)name.size(); i++)
		str += " ";
}

string Coupled::Get_q(bool relative) const
{
	string str="("+Name+":";
	for(deque<Devs*>::size_type i = 0; i < M.size(); i++)
	{
		if(i>0) str+=","; 
		MakeIndentation(Name, str);
		str+=M[i]->Get_q(relative);
	}
	str+=")\n";
	return str;
}

string HierName(const Coupled* p)
{
	string str;
	if(p->Parent)
		str = HierName(p->Parent)+".";
	str += p->Name;
	return str;
}

void Coupled::PrintEICs() const
{
	cout << "\n -- External Input Coupling (EIC) -- \n"; 
	int n=0;
	for(set<InputPort*, lt_Name>::const_iterator it = m_IPs.begin();
		it != m_IPs.end(); it++)
	{
		for(int i=0; i<(int)(*it)->ToP.size(); i++) {
			cout << (*it)->Name << " --> " 
				 << NameWithParent((*it)->ToP[i]) << endl;
			n++;
		}
	}
	cout << " ------ # of EICs: " << n << "-----" << endl;
}

void Coupled::PrintITCs() const
{
	cout << "\n -- Internal Coupling (ITC) -- \n"; 
	int n=0;
	for(int c = 0; c < (int) M.size(); c++ ) {
		for(set<OutputPort*, lt_Name>::const_iterator it = M[c]->m_OPs.begin();
			it != M[c]->m_OPs.end(); it++)
		{
			for(int i=0; i<(int)(*it)->ToP.size(); i++) {
				if( (*it)->ToP[i]->Parent != (Devs*) this) { 
					cout << NameWithParent(*it) << " --> " 
						 << NameWithParent((*it)->ToP[i])<< endl;
					n++;
				}
			}
		}
	}
	cout << " ------ # of ITCs: " << n << "-----" << endl;
}

void Coupled::PrintEOCs() const
{
	cout << "\n -- External Output Coupling (EOC) -- \n"; 
	int n=0;
	for(int c = 0; c < (int) M.size(); c++ ) {
		for(set<OutputPort*, lt_Name>::const_iterator it = M[c]->m_OPs.begin();
			it != M[c]->m_OPs.end(); it++)
		{
			for(int i=0; i<(int)(*it)->ToP.size(); i++) {
				if( (*it)->ToP[i]->Parent == (Devs*) this) { 
					cout << NameWithParent((*it)) << " --> " 
						 << (*it)->ToP[i]->Name << endl;
					n++;
				}
			}
		}
	}
	cout << " ------ # of EOCs: " << n << "-----" << endl;
}

void Coupled::PrintCouplings() const
{
	cout << "Inside of " << HierName(this) << endl;
	PrintEICs();
	PrintITCs();
	PrintEOCs();

	/* for hierarchical printing of couplings
	for(deque<Devs*>::size_type i = 0; i < M.size(); i++){
		if(M[i]->IsKindOf(DEVS_RT(Coupled)))
			((Coupled*)M[i])->PrintCouplings();
	}*/
} 

void Coupled::GetLeafChildren(deque<Atomic*>& LD) const { // 11/17/2009
	for(deque<Devs*>::size_type i = 0; i < M.size(); i++)
	{
		if(M[i]->IsKindOf(DEVS_RT(Coupled)))
			((Coupled*)M[i])->GetLeafChildren(LD);
		else if(M[i]->IsKindOf(DEVS_RT(Atomic)))
			LD.push_back((Atomic*)M[i]);
	}
}
//----------------------------------------------------------------------------------
void Coupled::when_receive_init()
{
	//Schedule.clear();
	for(deque<Devs*>::size_type i = 0; i < M.size(); i++){
		M[i]->when_receive_init();
		//Schedule.push_back(M[i]);
	}

	Update_tL_tN();
}

void Coupled::when_receive_star(bool print_event)
{
	if(M.empty() == false ) {
		if(IsEqual(t_Next, t_Current, 1E-5))
		{
			//Devs* First = Schedule.front();
			First->when_receive_star(print_event);
			Update_tL_tN();
		}
		else
			THROW_DEVS_EXCEPTION("Schedule Violation in Coupled!");
	}
}

void Coupled::when_receive_cs()
{
	if(t_Last <= t_Current && t_Current <= t_Next) {
		for(deque<Devs*>::size_type i = 0; i < M.size(); i++)
			M[i]->when_receive_cs();
	}else
		THROW_DEVS_EXCEPTION("Schedule Violation in Coupled!");
}

void Coupled::when_receive_x(const PortValue& x,bool print_event)
{
	if(t_Last <= t_Current && t_Current <= t_Next) {
		for(int i=0; i<(int)x.port->ToP.size(); i++)
		{
			Port* dest_pt= x.port->ToP[i];
			PortValue pv(dest_pt, x.value);
			dest_pt->Parent->when_receive_x(pv,print_event);
		}
		Update_tL_tN();
	}else
		THROW_DEVS_EXCEPTION("Schedule Violation in Coupled!");
}

 
void Coupled::when_receive_y(const PortValue& y,bool print_event)
{
	if(y.port==NULL)
		return;

	for(int i=0; i<(int)y.port->ToP.size(); i++)
	{//-- transmit y to influencees  
		Port* dest_pt = y.port->ToP[i];
		PortValue pv(dest_pt, y.value);
		if(dest_pt->Parent == this ) // external output coupling
		{
			if(Parent)
				Parent->when_receive_y(pv, print_event);
		}
		else// internal coupling
		{
			dest_pt->Parent->when_receive_x(pv,print_event);
			// Update_tL_tN(); // redundant, don't call it
		}
	}
}

/*
void Coupled::GetInfluencees_x(InputPort x, deque<pair<Atomic*, InputPort> >& M_X) const
{
	DevsPort m_p((Devs*)this, x);

	for(CoupleMap::const_iterator it = C.lower_bound(m_p); it!= C.upper_bound(m_p); it++)
	{
		if(it->second.model->IsKindOf(DEVS_RT(Atomic))){
			M_X.push_back(pair<Atomic*, InputPort>((Atomic*)it->second.model, InputPort(it->second.port)));
		}else if(it->second.model->IsKindOf(DEVS_RT(Coupled)))
			((Coupled*)it->second.model)->GetInfluencees_x(it->second.port, M_X);
		else
			THROW_DEVS_EXCEPTION("Invalid Devs!");
	}
}

void Coupled::GetInfluencees_y(Devs* child, OutputPort y, deque<pair<Atomic*, InputPort> >& M_X) const
{
	DevsPort m_p(child, y);

	for(CoupleMap::const_iterator it = C.lower_bound(m_p); it!= C.upper_bound(m_p); it++)
	{
		if(it->second.model->IsKindOf(DEVS_RT(Atomic))){ // internal coupling
			M_X.push_back(pair<Atomic*, InputPort>((Atomic*)it->second.model, InputPort(it->second.port)));
		}else if(it->second.model->IsKindOf(DEVS_RT(Coupled)))
		{
			if((const Coupled*)it->second.model == this){ // external output coupling
				if(Parent)
					Parent->GetInfluencees_y((Devs*)this, it->second.port, M_X);
			}
			else // internal coupling 
				((Coupled*)it->second.model)->GetInfluencees_x(it->second.port, M_X);
		}
		else
			THROW_DEVS_EXCEPTION("Invalid Devs!");
	}
}
*/
 