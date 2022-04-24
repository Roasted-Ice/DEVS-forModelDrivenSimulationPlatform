/************************************************************************
 * DEVS-C++: Open Source of DEVS in C++                                 *
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
#pragma once

#include <DEVSpp/PortValue.h>
#include <DEVSpp/StrUtil.h>

using namespace std;
using namespace DEVSpp;

namespace CT
{
	class Wafer: public Value
	{
	protected:
		string status;
	public:
		Wafer(const string& str): status(str) {}
		Wafer(const Wafer& ob) { status = ob.status; }
		/*virtual*/ Value* Clone() const { return new Wafer(*this); }
		//-- convert to string
		/*virtual*/ std::string STR() const { return status; }
		void Set_s(const string & s) { status = s; }
		const Wafer& operator=(const Wafer& obj) 
		{ status = obj.status; return *this; }
	};

	class WaferFeed: public Value
	{
	public:
		int    slot_i;
		Wafer  wafer;
		WaferFeed(int si, const Wafer& job): slot_i(si), wafer(job){}

		WaferFeed(const WaferFeed& ob): slot_i(ob.slot_i),  wafer(ob.wafer){ }

		//-- copy me to another instance
		/*virtual*/ Value* Clone() const { return new WaferFeed(*this); }
		
		//-- convert to string
		/*virtual*/ std::string STR() const 
		{ 
			return DEVSpp::STR(slot_i)+","+wafer.STR();
		}
	};
}