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
#include <map>
#include "DEVS_Export.h"

using namespace std;

namespace DEVSpp {

/// abstract class of atomic DEVS models
class DEVSpp_EXP Atomic: public Devs
{
protected:
	Atomic(const string& name): Devs(name), m_cs(false) {}
public:
	//-------------- 4 characteristic functions  ----------------
	virtual void init() = 0;
	virtual bool delta_x(const PortValue& x) = 0;
	virtual void delta_y(PortValue& ys) = 0;
	virtual TimeSpan tau() const = 0;
	
	//---------------- string related ---------------
	virtual string Get_s() const { return string();}
	/* return string of (s,ta(s),t_r) if remaining==true
	   return string of (s,ta(s),t_e) otherwise */
	/*virtual*/ string Get_q(bool relative=true) const ;
	
	//-- virtual destructor 
	virtual ~Atomic() {}
	
	//---------------- for collecting statistics ---------------
	void CollectStatistics(bool flag = true) { m_cs = flag; }
	bool CollectStatisticsFlag() const { return m_cs; }
	// return a string showing performance data
	virtual map<string, double> GetPerformance() const;
	//-- string for key of statistics
	virtual string Get_Statistics_s() const { return Get_s(); }
    //-----------------------------------------------------------
	
protected:
	//-- make an output event. It will used in the overridden delta_y
	void Make_y(PortValue& y, OutputPort* op, Value* v=NULL);

	/*-- functions called by simulation algorithm --*/ 
	/*virtual*/ void when_receive_init(); 
	/*virtual*/ void when_receive_star(bool print_event); 
	/*virtual*/ void when_receive_phi(); 
	/*virtual*/ void when_receive_cs(); 
	/*virtual*/ void when_receive_x(const PortValue& x,bool print_event);   

	bool   m_cs; // collecting statistics flag in terms of cumulating times 
	Time   t_Lcs; // last time of collecting statistics;  
	map<string, double> m_statistics; // (string, value)

	DECLARE_CLASS();

friend class SRTEngine;
};
  
}// end of namespace of DEVSPP 
