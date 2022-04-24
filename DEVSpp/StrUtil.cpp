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

#include "StrUtil.h"
#include <float.h>
#include <assert.h>

using namespace std;

string DEVSpp::STR(int v)
{
	if (v == INT_MAX)
		return "inf";
	else if(v == -INT_MAX) 
		return "-inf";
	else
	{
		char tmp[100];
		sprintf(tmp, "%d", v);
		return tmp;
	}
}
 
string DEVSpp::STR(unsigned v)
{ 
	return STR((int) v); 
}

string DEVSpp::STR(const string& s, int v)
{
	return s+DEVSpp::STR(v);
}

string DEVSpp::STR(const string& s, unsigned v)
{
	return s+DEVSpp::STR(v);
}

string DEVSpp::STR(const string& s, double v)
{
	return s+DEVSpp::STR(v);
}

string DEVSpp::STR(double v)
{
	if (v == DBL_MAX)
		return "inf";
	else if(v == -DBL_MAX) 
		return "-inf";
	else
	{
		char tmp[100];
		sprintf(tmp, "%.3lf", v);
		return tmp;
	}
}

vector<string> DEVSpp::Split(const string& str, char c)
{
	vector<string> tmp;
	char buff[10000];
	string::size_type from = 0;
	bool termi = false;
	while(!termi){
		string::size_type to = str.find(c, from);
		if(to > str.size()) // no ,
		{
			if(from < str.size()){
				to = str.size();
				termi = true;
			}
		}
		str.copy(buff, to, from); //Does not append a null-terminator
		buff[to-from]='\0'; // null terminating
		tmp.push_back(string(buff));
		if(to+1 < str.size())
			from = to+1;
		else
			break;
	} 
	return tmp;
}

//-- return the merged string from f to end with delimiter c 
string DEVSpp::Merge(const vector<string>& s, unsigned f, char c)
{
	assert(f <= s.size());
	string tmp;
	for(vector<string>::size_type i = f; i< s.size(); i++)
	{
		if(i > f)
			tmp += c;
		tmp += s[i];
	}
	return tmp;
}

#include "Port.h"
#include "Devs.h"
#include "Coupled.h"

string DEVSpp::NameWithParent(DEVSpp::Port* pt)
{
	return string(pt->Parent->Name+"."+pt->Name);
}

string DEVSpp::HierName(const Devs* child, const Coupled* under)
{
	if(child->Parent == under)
		return child->Name;
	else
		return DEVSpp::HierName(child->Parent, under)+"."+child->Name;
}
