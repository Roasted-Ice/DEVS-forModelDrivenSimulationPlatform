/************************************************************************
 * DEVS++: DEVS Open Source in C++                                      *
 * Copyright (C) 2005-2009  Moon Ho Hwang <moonho.hwang@gmail.com>      * 
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
#include "Atomic.h"
#include "SRTEngine.h"
#include <iostream>

using namespace std;
using namespace DEVSpp;

class SimplestTimer: public Atomic
{
public:
	OutputPort* op; //

	SimplestTimer(const string& name=""): Atomic(name) 
	{ op = AddOP("op"); }
    /*virtual*/ void init(){ }
	/*virtual*/ Time tau() const { 
		return 3.3;
	}
	/*virtual*/ bool delta_x(const PortValue& x) {return false;}
	/*virtual*/ void delta_y(PortValue& y) 
	{
		y.Set(op);
	}
	
	/*virtual*/ string Get_s() const { 
		return string("Working"); 
	}
};

void main( void )
{ 
	SimplestTimer* STimer = new SimplestTimer("STimer") ; //-- simulation model
	//STimer->CollectStatistics(true);
	SRTEngine simEngine(*STimer); // plug-in simulation model and ending time
    
	//simEngine.SetAnimationFlag(false);
	//simEngine.SetTimeScale(DBL_MAX); //1000);//DBL_MAX);
	//simEngine.Set_dtmode(SRTEngine::P_NONE);	
	
	simEngine.RunConsoleMenu();
	delete STimer;
}

