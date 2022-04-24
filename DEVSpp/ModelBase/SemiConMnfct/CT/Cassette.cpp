/************************************************************************
 * DEVS-C++: Open Source of DEVS in C++                                *
 * Copyright (C) 2005-2006  Moon Ho Hwang <moonho.hwang@gmail.com>      * 
 *                                                                      *
 * This library is free software; you can redistribute it and/or        *
 * modify it under the terms of the GNU Lesser General Public           *
 * License as published by the Free Software Foundation; either         *
 * version 2.1 of the License, or (at your option) any later version.   *
 *                                                                      *
 * This library is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU     *
 * Lesser General Public License for more details.                      *
 *                                                                      *
 * You should have received a copy of the GNU Lesser General Public     *
 * License along with this library; if not, write to the Free Software  *
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301,*  
 * USA.                                                                 * 
*************************************************************************/
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Cassette.h>

using namespace CT;

//-- convert to string
/*virtual*/ std::string Cassette::STR() const {
	string str = Job::STR()+","+::STR(capacity)+","; // capacity
	for(int i=0; i<capacity; i++)
	{
		if(i>0) str +=",";
		str+=m_Wafers[i].STR();
	}
	return str;
}

void Cassette::Set_s(const string& str) 
{ 
	Clear();
	vector<string> s = DEVSpp::Split(str, ',');
	vector<string> tmp = DEVSpp::Split(s[0],'=');
	type = ::atoi(tmp[1].data());
	tmp = DEVSpp::Split(s[1],'=');
	id = ::atoi(tmp[1].data());

	capacity = ::atoi(s[2].data());

	for(int i=0; i < capacity; i++) // Be careful i=3
		m_Wafers.push_back(Wafer(s[i+3]));
} 