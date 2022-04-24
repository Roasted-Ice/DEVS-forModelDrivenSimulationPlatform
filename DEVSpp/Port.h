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

#include "Named.h"
#include <vector>

namespace DEVSpp {

using namespace std;
	 
class Devs; 

class Port: public Named
{
public:
	Devs* Parent;
	vector<Port*> ToP;   // Successor
	vector<Port*> FromP; // Predecessor
	Port(): Named(""), Parent(NULL){}
	Port(const string& name, Devs* p=NULL): Named(name),Parent(p){}
	bool operator==(const string& name) const { return Name == name; }
};

class InputPort: public Port
{
public:
	InputPort():Port(){}
	InputPort(const string& name, Devs* p=NULL): Port(name,p){}
};

class OutputPort: public Port
{
public:
	OutputPort():Port(){}
	OutputPort(const string& name, Devs* p=NULL): Port(name,p){}
};

} // end of DEVS namespace
