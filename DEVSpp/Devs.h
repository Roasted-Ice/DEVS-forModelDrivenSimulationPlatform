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
#pragma warning(disable:4275)
#pragma warning(disable:4786)

#include <fstream>
#include <map>
#include <set>
#include <iostream>
#include "PortValue.h"
#include "DEVS_Export.h"
#include "DeclareClass.h"

using namespace std;

namespace DEVSpp {

typedef double Time;
typedef double TimeSpan ;
 
class Coupled; // see Coupled.h for details.

/// base class of DEVS models
class DEVSpp_EXP Devs: public Named
{
public:	
    Coupled*  Parent; // parent pointer
    set<InputPort*, lt_Name> m_IPs; // set of input ports
    set<OutputPort*, lt_Name> m_OPs; // set of output ports

	//-- Input Port related functions --
	InputPort* AddIP(const string& ipn);
	InputPort* GetIP(const string& ipn) const;
	InputPort* RemoveIP(const string& ipn);
	void PrintAllIPs() const;

	//-- Output Port related functions --
	OutputPort* AddOP(const string& opn);
	OutputPort* GetOP(const string& opn) const;
	OutputPort* RemoveOP(const string& opn);
	void PrintAllOPs() const;

	//------ time related functions ---------
	/// get last-schedule-update time
	Time TimeLast() const { return t_Last; }
	/// get next-schedule time
	Time TimeNext() const { return t_Next; }
	/// get lifespan
	Time TimeLifespan() const;// { return t_Next-t_Last; }
	/// get current time;
	static Time TimeCurrent() { return t_Current; }
	/// get elapsed time since last schedule
	TimeSpan TimeElapsed() const { return t_Current - t_Last; }
	/// get reamining time to next schedule
	TimeSpan TimeRemaining() const; //{ return t_Next - t_Current; }
	/// virtual destructor 
	virtual ~Devs();
	
	/* return string of (s, t_s, t_e) if relative==true
	   return string of (s, t_l, t_n) otherwise */
	virtual string Get_q(bool relative=true) const {return string(); }

protected:
	// protected constructor => abstracted class
	Devs(const string& name);

	//--- functions for abstract simulation algorithm ---
	/// called when simulation initialized
	virtual void when_receive_init()=0;
	/// called when schedule-time reached,(print_event for debugging)
	virtual void when_receive_star(bool print_event)=0; 
	/// called when collecting statistics needed
	virtual void when_receive_cs()=0;  
	/// called when an external input "x" comes 
	virtual void when_receive_x(const PortValue& x, bool print_event)=0; 
	//----------------------------------------------------

	static fstream m_sf_log;

	//------------ discrete updating times ---------------
	Time t_Last; // last-schedule-update time
	Time t_Next; // next-schedule time
	//----------------------------------------------------
friend class Coupled;
friend class Atomic;
	//--------------- continuously times -----------------
	static Time t_Current; // current time
	
	//----------------------------------------------------
friend class SRTEngine;

	DECLARE_CLASS();
};


//
//DEVSpp_EXP ostream& operator<<(ostream& strm, const Devs& md) const ;
//DEVSpp_EXP istream& operator>>(istream& strm, const Devs& md); 

//-- check two double values a and b are equal in terms of tolerance tol
DEVSpp_EXP 
bool IsEqual(double a, double b, double tol=1E-3);

//-- check if a is equal to infinity
DEVSpp_EXP bool IsInfinity(double a, double tol=1E-3) ;

} // end of namespace of DEVS 
