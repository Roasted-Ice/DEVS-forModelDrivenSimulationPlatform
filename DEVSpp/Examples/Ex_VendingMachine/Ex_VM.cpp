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
#include <math.h>
#include <assert.h>

using namespace std;
using namespace DEVSpp;

const string IDLE="Idle";
const string WAIT="Wait";
const string O_PEPSI="O_Pepsi";
const string O_COCA="O_Coke";

class VM: public Atomic
{
public:
	InputPort * idollar, *pepsi_btn, *coke_btn;
	OutputPort * odollar, *pepsi, *coke;

	VM(const string& name=""): Atomic(name)
	{ 
		idollar = AddIP("dollar"); 
		pepsi_btn = AddIP("pepsi_btn"); 
		coke_btn = AddIP("coke_btn");

		odollar = AddOP("dollar"); 
		pepsi = AddOP("pepsi"); 
		coke = AddOP("coke");
		init();
	}
    /*virtual*/ void init()
	{
		m_phase = IDLE;
	}
	/*virtual*/ Time tau() const 
	{ 
		if(m_phase == WAIT)
			return 15;
		else if(m_phase == O_PEPSI)
			return 2;
		else if(m_phase == O_COCA)
			return 2;
		else
			return DBL_MAX;
	}
	/*virtual*/ bool delta_x(const PortValue& x) 
	{
		if(m_phase == IDLE && x.port == idollar){
			m_phase = WAIT;
			return true;
		} else if(m_phase == WAIT && x.port == pepsi_btn) {
			m_phase = O_PEPSI;
			return true;
		} else if(m_phase == WAIT && x.port == coke_btn) {
			m_phase = O_COCA;
			return true;
		}else
			return false;
	}

	/*virtual*/ void delta_y(PortValue& y) 
	{
		if(m_phase == WAIT)
			y.Set(odollar);
		else if(m_phase == O_PEPSI)
			y.Set(pepsi);
		else if(m_phase == O_COCA)
			y.Set(coke);
		m_phase = IDLE;
	}
	/*virtual*/ string Get_s() const 
	{
		return m_phase; 
	}

protected:
	string m_phase;
};


//-- call back function to inject an event to simulator
PortValue InjectMsg(Devs& md)
{
	VM& vm = (VM&)md;
	string input;
	cout << "[d]ollar [p]epsi_botton [c]oce_botton > " ;
	cin >> input;
	if(input == "d")
		return PortValue(vm.idollar);
	else if(input == "p")
		return PortValue(vm.pepsi_btn);
	else if(input == "c")
		return PortValue(vm.coke_btn);
	else {
		cout <<"Invalid input! Try again! \n";
		return PortValue();
	}
}

void main( void )
{
	VM* vm = new VM("VM") ; //-- simulation model
	vm->CollectStatistics();
	SRTEngine simEngine(*vm, 10000, InjectMsg); // see above function 
	simEngine.RunConsoleMenu();
	delete vm;
}

