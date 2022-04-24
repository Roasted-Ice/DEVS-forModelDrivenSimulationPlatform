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
using namespace std;

#define DEVS_RT_TYPE unsigned int*

#define DECLARE_CLASS() \
public:/*no private for Linux */ \
	static  unsigned int m_sV; \
public: \
	virtual string VClassName() const ; \
	virtual unsigned int VVersion()   const ; \
	static void* create_object(); \
	static string ClassName() ; \
	static DEVS_RT_TYPE Register(); \
	static bool m_bRegistered; \
	virtual bool IsKindOf(DEVS_RT_TYPE pver) const; \
	virtual bool IsSameClass(DEVS_RT_TYPE pver) const; \
	virtual bool IsKindOf(string pver) const; \
	virtual bool IsSameClass(string pver) const; \

//-------------------------------------------------------------------