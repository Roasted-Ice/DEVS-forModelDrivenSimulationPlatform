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

#include "Port.h"
#include "Value.h"
#include <deque>
#include "DEVS_Exception.h"
#include "DEVS_Export.h"
#pragma warning(disable:4275)

namespace DEVSpp {

class DEVSpp_EXP PortValue
{
public:
	Port*  port; //-- either an output prot or an input port 
	Value* value;//-- typecast it to a concrete derived class!

	//------------------------- constructors -------------------------
	PortValue(const Port* p=NULL, Value* v=NULL);
	PortValue(const PortValue& ob);
	//----------------------------------------------------------------
	//---------------------- set or assign operator ------------------
	void Set(const Port* p, Value* v=NULL) ;
	const PortValue& operator=(const PortValue& ob);
	string STR() const ;
	bool operator<(const PortValue& ob) const;
	//----------------------------------------------------------------
};

} // end of namespace of DEVS
