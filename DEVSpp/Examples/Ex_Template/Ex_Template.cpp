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
#include "Atomic.h" //--- (1)
#include "Coupled.h"
#include "SRTEngine.h"
#include "RNG.h"
#include <iostream>
#include <math.h>

using namespace std;
using namespace DEVSpp; //--- (2)

const string WAIT = "Wait"; 
const string SEND = "Send";

//---- definition of atomic DEVS for Player --- (3)
class Player: public Atomic
{
public:
	OutputPort* send;  //-- associated ports --- (4)
	InputPort* receive;
protected:  //-- associated internal state variables ----(5)
	string    m_phase;  
	bool      m_width_ball; 
public:
    Player(const string& name="", bool with_ball=false): Atomic(name), 
		m_phase(WAIT), m_width_ball(with_ball)
	{	
		send = AddOP("send");       //--- add ports --- (6)
		receive = AddIP("receive"); 
	}
	//---- four characteristic functions ------- (7)
	/*virtual*/ void init()
	{ 
		if(m_width_ball)
			m_phase = SEND;
		else
			m_phase = WAIT;
	}
	
	/*virtual*/ TimeSpan tau() const 
	{
		static rv urv;

		if(m_phase == SEND)
			return urv.uniform(0.1, 1.2); //---- (8)
		else
			return DBL_MAX;
	}
	
	/*virtual*/ bool delta_x(const PortValue& x)
	{
		if(x.port == receive)
		{
			if(m_phase == WAIT) {
				m_phase = SEND;
				return true;
			}
		}
		return false;
	}
  	/*virtual*/ void delta_y(PortValue& y)  
	{
		if(m_phase == SEND) {
			y.Set(send);
		    m_phase = WAIT;
		}
	}
    //------ end of four characteristic functions -------
	//----- showing the current status ------(9)
	/*virtual*/ string Get_s() const
	{
		return m_phase;
	}
}; 


Coupled* MakePingPongGame(const string& name)
{
	Coupled* PingPong =  new Coupled(name);// ----(10)
	Player* A = new Player("A", true); 	//--- (11)
	Player* B = new Player("B", false); 	
	
	A->CollectStatistics(true); //-- (12)
	B->CollectStatistics(true);

	PingPong->AddModel(A); //-- (13)
	PingPong->AddModel(B);

	//-- Internal Coupling --------  (14)
	PingPong->AddCP(A->send, B->receive);
	PingPong->AddCP(B->send, A->receive);

	PingPong->PrintCouplings(); //---- (15)
	return PingPong;
}

void main(void)
{
	Coupled* PingPong = MakePingPongGame("PingPong"); 
	SRTEngine simEngine(*PingPong);//-- (16)
	simEngine.RunConsoleMenu(); //-- (17)
	delete PingPong; 
}

