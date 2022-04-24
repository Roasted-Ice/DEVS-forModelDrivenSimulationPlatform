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
#include <iostream>
#include <stdio.h>
#pragma warning(disable:4996)

#define THROW_DEVS_EXCEPTION(err_msg) \
{ \
char __buffer[100]; \
sprintf(__buffer,"%s :: %s line %d",err_msg,__FILE__,__LINE__); \
DEVSpp::Exception __err(__buffer); \
throw __err; \
}

using namespace std;

namespace DEVSpp {
 
class Exception
{
public:
	//--- constructors and destructor ------------------------ 
	Exception(const Exception& src):err_str(src.err_str){}
	Exception(string err_str):err_str(err_str){}
	virtual ~Exception(){}
	//--------------------------------------------------------
	//---- assign operator -----------------------------------
	const Exception& operator=(const Exception& src)
	{   err_str = src.err_str;
		return *this;
	}

	//--- Get a description of the error condition --------------
	string getErrorMsg() { return err_str; }

	//---- output to stream  
	friend ostream& operator<<(ostream& strm, Exception& err)
	{   strm << err.err_str;
		return strm;
	}

	string err_str;
};

} // end of namespace of DEVS
 