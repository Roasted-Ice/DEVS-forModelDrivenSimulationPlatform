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
#pragma warning(disable:4996)

#include "PortValue.h"
using namespace DEVSpp;

PortValue::PortValue(const Port* p, Value* v)
{ 
	Set(p, v); 
}

PortValue::PortValue(const PortValue& ob)
{ 
	Set(ob.port, ob.value); 
}

void PortValue::Set(const Port* p, Value* v) 
{ 
	port = (Port*)p; value = v;
}

const PortValue& PortValue::operator=(const PortValue& ob)
{
	Set(ob.port, ob.value); 
	return *this; 
}

string PortValue::STR() const 
{
	string str;
	if(port)
		str = port->Name;
	if(value)
		str += ":"+value->STR();
	return str;
}

bool PortValue::operator<(const PortValue& ob) const
{
	return (port < ob.port && value < ob.value);
	//return (STR() < ob.STR());
}
//----------------------------------------------------------------