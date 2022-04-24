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
#include <vector>
#include "DEVS_Export.h"

using namespace std;

namespace DEVSpp
{
	class Port;
	class Devs;
	class Coupled;

	DEVSpp_EXP string STR(int v);// return int v as a string
	DEVSpp_EXP string STR(const string& s,int v);//s+::STR(v);

	DEVSpp_EXP string STR(unsigned v); // return int v as a string
	DEVSpp_EXP string STR(const string& s, unsigned v);//s+::STR(v);

	DEVSpp_EXP string STR(double v);// return int v as a string
	DEVSpp_EXP string STR(const string& s, double v);//s+::STR(v);

	//-- split string s using delimiter c by n times if possible
	DEVSpp_EXP vector<string> Split(const string& s, char c);
	//-- return the merged string with s from f to t with delimiter c 
	DEVSpp_EXP string Merge(const vector<string>& s, unsigned f, char c);
	//-- string(pt->Parent->Name+"."+pt->Name);
	DEVSpp_EXP string NameWithParent(DEVSpp::Port* pt);
	
	//  hierarchical name of child from the view of under.
	// if under=NULL, the hierarchical name starts from the root model
	DEVSpp_EXP string HierName(const DEVSpp::Devs* child, const DEVSpp::Coupled* under=NULL) ;
}

