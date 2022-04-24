/************************************************************************
 * ODEVS-C++: Open Source of DEVS in C++                                *
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
#include <deque>
#include <vector>
#include <DEVSpp/ModelBase/Buffer.h>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/SF_Export.h>
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
#include <DEVSpp/ModelBase/Job.h>

using namespace DEVSpp;

namespace SF
{
 
class SF_EXP Buffer: public MB_Basic::Buffer
{
protected: 
	deque<MB_Basic::Job*> m_UnaskedJobs; // V1
	deque<MB_Basic::Job*> m_AskedJobs; // V2
    deque<MB_Basic::Signal> m_PriorityQ; 

public:
	InputPort* IDB;
	OutputPort* ODB, *SysOut;

	// InputPort IDB;	OutputPort ODB;	OutputPort SysOut;
	Buffer(const string& name="", int osize=1): MB_Basic::Buffer(name, osize)
	{ 
		IDB = AddIP("IDB");
		ODB = AddOP("ODB"); 
		SysOut = AddOP("SysOut"); 
		init(); 
	}
	virtual ~Buffer() { }

	/*virtual*/ void init();
	/*virtual*/ bool delta_x(const PortValue& x) ;
	/*virtual*/ void delta_y(PortValue& y) ;
	///*virtual*/ PortValue lambda() const;
	/*virtual*/ Time tau() const ;
	void UpdateVariable(const DEVSpp::PortValue& x);
};

}