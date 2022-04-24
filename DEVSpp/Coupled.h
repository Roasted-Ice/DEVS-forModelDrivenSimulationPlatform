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
#pragma once
#pragma warning(disable:4251)

#include "Devs.h"
#include "PortValue.h"
#include "DEVS_Export.h"
#include <deque>
#include <list>
#include <map>

namespace DEVSpp {
	
//-- class of DEVS Network 
class DEVSpp_EXP Coupled: public Devs
{
public:
	// constructor
	Coupled(const string& name=""): Devs(name) {}
	// destructor
	virtual ~Coupled();

	//-- modelling related 
	void AddModel(Devs* md); 
	void RemoveModel(Devs* md);
	Devs* GetModel(const string& name) const;
	deque<Devs*>& Children() { return M; }
	const deque<Devs*>& Children() const { return M; }
	void GetLeafChildren(deque<Atomic*>& LD) const; // 11/17/2009

	//-- coupling related
	void AddCP(InputPort* spt, InputPort* dpt); // EIC
	void AddCP(OutputPort* spt, InputPort* dpt); // ITC
	void AddCP(OutputPort* spt, OutputPort* dpt);// EOC

	void RemoveCP(InputPort* spt, InputPort* dpt); // EIC
	void RemoveCP(OutputPort* spt, InputPort* dpt); // ITC
	void RemoveCP(OutputPort* spt, OutputPort* dpt);// EOC

	/* methods for observing status of model */
	/*virtual*/ string Get_q(bool relative=true) const ;
	
	//-- print all couplings in a hierarchical manner
	void PrintCouplings() const; 
	void PrintEICs() const;
	void PrintITCs() const;
	void PrintEOCs() const;

public:
//	void GetInfluencees_x(InputPort* x, deque<pair<Atomic*, InputPort*> >& M_X) const;
//	void GetInfluencees_y(Devs* child, OutputPort* y, deque<pair<Atomic*, InputPort*> >& M_X) const;
	void Update_tL_tN() ;

	/// function called when schedule-time reached
	/*virtual*/ void when_receive_star(bool print_event);
	virtual void when_receive_y(const PortValue& y, bool print_event);
protected:
	deque<Devs*> M;
	//deque<Devs*> Schedule;

	Devs*  First;

	//-- simulation run related 
	/// function called when simulation initialized
	/*virtual*/ void when_receive_init();
	/// function called when collecting statistics needed
	/*virtual*/ void when_receive_cs(); 
	/// function called when an external input "x" comes
	/*virtual*/ void when_receive_x(const PortValue& x,bool print_event);

friend class Atomic;

	DECLARE_CLASS();
};

}// end of namespace of DEVS 
