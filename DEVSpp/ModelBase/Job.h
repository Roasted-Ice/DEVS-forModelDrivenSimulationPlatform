/*************************************************************************
 * DEVS++: DEVS Open Source in C++                                       *
 * Copyright (C) 2005-2007  Moon Ho Hwang <moonho.hwang@gmail.com>       *
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
#include <DEVSpp/PortValue.h>
#include <DEVSpp/StrUtil.h>

using namespace DEVSpp;

namespace MB_Basic
{
	class Job: public tmValue
	{
	public:
		int type;
		int id;

		Job(int Type=0, int Id = 0):tmValue(), type(Type), id(Id) {}
		Job(const Job& ob) { Copy(ob); } 
		void Copy(const Job& ob){ tmValue::Copy(ob); type = ob.type; id = ob.id; } 
		/*virtual*/ Value* Clone() const {return new Job(*this);}
		/*virtual*/ std::string STR() const 
		{
			string str="job_type="+::STR(type)+", job_id="+::STR(id);
			return str;
		}
	};
		
	//-- Signal having, Job, Resource, and Evaluation
	class Signal: public Value
	{
	public:
		int job_type; // associated job type
		int job_id; // associated job id
		string resource; // plant name (signal source, destination)
		enum Action {COMPLETE, EVALUATION, ROUTE};
		Action action;

		double value;  // evaluating value
		double at;     // arrival time

		//-- constructor 
		Signal(int type, int id, const char* res, double AT/*, Action a*/): job_type(type), job_id(id), 
			resource(res), at(AT), value(0.0), action(EVALUATION)
		{
		}

		Signal(const Signal& ob): job_type(ob.job_type), job_id(ob.job_id),
			resource(ob.resource), value(ob.value), action(ob.action), at(ob.at) {}

		const Signal& operator=(const Signal& ob) 
		{
			job_type = ob.job_type;
			job_id = ob.job_id;
			resource = ob.resource;
			value = ob.value;
			action = ob.action;
			at = ob.at;
			return *this;
		}

		Signal(): job_type(0), job_id(0), resource(""), value(0.0), action(EVALUATION), at(0.0) {}
	};
}
