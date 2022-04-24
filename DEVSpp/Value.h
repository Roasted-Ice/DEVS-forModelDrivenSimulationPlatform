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
#include <string>
#include <strstream>
#include <map>
#include "DEVS_Export.h"
#pragma warning(disable:4996)
#pragma warning(disable:4275)
#pragma warning(disable:4251)

using namespace std;

namespace DEVSpp {

/*-- abstract class of Value from which all the message value
     should be derived --*/
class Value
{
protected:
	Value(){}
public:
	//-- copy me to another instance
	virtual Value* Clone() const {return NULL;}
	//-- convert to string
	virtual string STR() const {return string(); }
};

//-- basic value for bool, char, int, long, float, double 
//-- such as bValue<bool>, bValue<char>, bValue<int> and so on.
template<class V>
class bValue: public Value
{
public:
	//-- value field
	V v;

	//-- constructor
	bValue(V _v):v(_v){}

	//-- copy me to another instance
	/*virtual*/ Value* Clone() const {return new bValue(v);}

	//-- convert value to string 
	/*virtual*/ string STR() const 
	{ 
		strstream sstrem; sstrem << v << '\0';	sstrem.freeze();
		char* tmp = sstrem.str(); // get the momory allocated.
		string str(tmp); // copy to string
		delete tmp; // delete freezing memory
		return str;
	}
};

//-- class with map<string, double> which is used "name","time_stamp" for measuring performance
class DEVSpp_EXP tmValue: public Value
{
public:
	map<string, double> TimeMap;
	tmValue(){}
	tmValue(const tmValue& ob) { Copy(ob); }
	void Copy(const tmValue& ob) ;
	/*virtual*/ Value* Clone() const {return new tmValue(*this);}
	/*virtual*/ string STR() const ;
};

} // DEVSpp namespace