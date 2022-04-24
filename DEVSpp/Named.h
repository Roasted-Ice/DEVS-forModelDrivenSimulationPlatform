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

namespace DEVSpp {

using namespace std;
	
///--------A class having a name ------------------
class Named
{
public:
	Named(const string& name):Name(name){}
	string Name;
};//-----------------------------------------------
	 
/* A comparable structure.  
   This will be used for sorting models by their names */
struct lt_Name
{
  bool operator()(const Named* s1, const Named* s2) const
  {
    return (s1->Name < s2->Name) ;
  }
};//-------------------------------------------------------
 
} // DEVS namespace