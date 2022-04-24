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

#include "Value.h"
using namespace DEVSpp;

void tmValue::Copy(const tmValue& ob)
{
	TimeMap.clear();
	for(map<string, double>::const_iterator i = ob.TimeMap.begin(); 
		i != ob.TimeMap.end(); i++)
		TimeMap.insert(*i);
}

/*virtual*/ string tmValue::STR() const 
{ 	char tmp[100];		
	string str;
	for(map<string, double>::const_iterator i = TimeMap.begin(); 
		i != TimeMap.end(); i++){
		if(i != TimeMap.begin()) str +=",";
		sprintf(tmp, "%s:%.3lf", i->first.data(), i->second);
		str += tmp;
	}
	return str;
}