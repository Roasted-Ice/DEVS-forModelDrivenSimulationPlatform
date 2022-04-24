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
#include "Devs.h"
#include "ClassAdmin.h"
#include <float.h>
#include <math.h>
#include "Coupled.h"
#include <assert.h>
#include "StrUtil.h"

using namespace DEVSpp;

IMPLEMENT_ABSTRACT_CLASS0(Devs, "Devs", 20060806);

Time Devs::t_Current = 0.0; 

Devs::Devs(const string& name): Named(name), Parent(NULL), t_Last(0.0), t_Next(DBL_MAX)
{
}

Devs::~Devs()
{
	while(m_IPs.size()>0)
	{
		InputPort* ip = *m_IPs.begin();
		m_IPs.erase(m_IPs.begin());
		delete ip;
	}
	while(m_OPs.size()>0)
	{
		OutputPort* op = *m_OPs.begin();
		m_OPs.erase(m_OPs.begin());
		delete op;
	}
}

InputPort* Devs::AddIP(const string& name)
{
	InputPort* ip = new InputPort(name, this);
	m_IPs.insert(ip);
	return ip;
}

InputPort* Devs::GetIP(const string& name) const
{
	set<InputPort*, lt_Name>::const_iterator it
		= m_IPs.find(&InputPort(name));
	if(it == m_IPs.end()) {
		cout << Name << " has no " << name << " input port! \n";
		PrintAllIPs();
		return NULL;
	}
	else 
		return *it;
}

InputPort* Devs::RemoveIP(const string& name)
{
	set<InputPort*, lt_Name>::iterator it
		= m_IPs.find(&InputPort(name));
	if(it == m_IPs.end()) return NULL;
	else {
		InputPort* tmp = *it;
		m_IPs.erase(it);
		return tmp;
	}
}
 
void Devs::PrintAllIPs() const
{
	cout << Name << "'s IPs (" << m_OPs.size() << ")\n";
	for(set<InputPort*, lt_Name>::const_iterator it = m_IPs.begin();
		it != m_IPs.end(); it++)
		cout << (*it)->Name << endl;
}
 
OutputPort* Devs::AddOP(const string& name)
{
	OutputPort* ip = new OutputPort(name, this);
	m_OPs.insert(ip);
	return ip;
}

OutputPort* Devs::GetOP(const string& name) const
{
	set<OutputPort*, lt_Name>::const_iterator it
		= m_OPs.find(&OutputPort(name));
	if(it == m_OPs.end()) {
		cout << Name << " has no " << name << " output port! \n";
		PrintAllOPs();
		return NULL;
	}
	else return *it;
}


OutputPort* Devs::RemoveOP(const string& name)
{
	set<OutputPort*, lt_Name>::iterator it
		= m_OPs.find(&OutputPort(name));
	if(it == m_OPs.end()) return NULL;
	else {
		OutputPort* tmp = *it;
		m_OPs.erase(it);
		return tmp;
	}
}


void Devs::PrintAllOPs() const
{
	cout << Name << "'s OPs (" << m_OPs.size() << ")\n";
	for(set<OutputPort*, lt_Name>::const_iterator it = m_OPs.begin();
		it != m_OPs.end(); it++)
		cout << (*it)->Name << endl;

}

bool DEVSpp::IsEqual(double a, double b, double tol)
{
	return fabs(a-b)<tol;
}

bool DEVSpp::IsInfinity(double a, double tol)
{
	return IsEqual(a, DBL_MAX, tol);
}


Time Devs::TimeLifespan() const
{ 
	if(IsInfinity(t_Next))
		return DBL_MAX;
	else
		return t_Next-t_Last; 
}

Time Devs::TimeRemaining() const
{ 
	if(IsInfinity(t_Next))
		return DBL_MAX;
	else
		return t_Next-t_Current; 
}
