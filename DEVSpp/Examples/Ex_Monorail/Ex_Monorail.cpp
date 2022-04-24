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
#include "Station.h"
#include "SRTEngine.h"
#include "Coupled.h"

Coupled* MakeMonorail(const char* name)
{
	Coupled* monorail = new Coupled(name);
	//-- Add Station 0 to 3 ----
	Station* ST0 = new Station("ST0", false, 10); 
	ST0->CollectStatistics();

	Station* ST1 = new Station("ST1", true, 30); 
	ST1->CollectStatistics();

	Station* ST2 = new Station("ST2", false,10); 
	ST2->CollectStatistics();

	Station* ST3 = new Station("ST3", true,10); 
	ST3->CollectStatistics();

	monorail->AddModel(ST0); 
	monorail->AddModel(ST1); 
	monorail->AddModel(ST2); 
	monorail->AddModel(ST3); 
    //---------------------------------------------
	//-------- Add internal couplings ------------
	monorail->AddCP(ST0->ovehicle, ST1->ivehicle); 
    monorail->AddCP(ST1->ovehicle, ST0->ipull); 

	monorail->AddCP(ST1->ovehicle, ST2->ivehicle); 
    monorail->AddCP(ST2->ovehicle, ST1->ipull); 

	monorail->AddCP(ST2->ovehicle, ST3->ivehicle); 
    monorail->AddCP(ST3->ovehicle, ST2->ipull); 

	monorail->AddCP(ST3->ovehicle, ST0->ivehicle); 
    monorail->AddCP(ST0->ovehicle, ST3->ipull); 
	//---------------------------------------------
	return monorail;
} 
 
void main( void )
{
	Coupled* monorail = MakeMonorail("mr");
	SRTEngine simEngine(*monorail, 1000); 
	//simEngine.SetAnimationFlag(false);
	simEngine.SetTimeScale(DBL_MAX); //1000);//DBL_MAX);
	//simEngine.Set_dtmode(SRTEngine::P_NONE);
	simEngine.RunConsoleMenu(); 
	delete monorail;
}

