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
#include <DEVSpp/SRTEngine.h>
#include <DEVSpp/Coupled.h>
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Config.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/ClusterTool.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Cassette.h>
#include <DEVSpp/ModelBase/Generator.h>
#include <DEVSpp/ModelBase/Transducer.h>

using namespace std;
using namespace DEVSpp;
using namespace CT;
using namespace MB_Basic;
 
//const string MB_Basic::Generator<Cassette>::Work = "Work";
//const string MB_Basic::Generator<Cassette>::Idle = "Idle";

class ExpFrame: public Coupled
{
public:
	OutputPort *oc0, *oc1;
	InputPort  *is0, *is1, *ic;

	ExpFrame(const string& name):Coupled(name){
		oc0 = AddOP("oc0"); oc1 = AddOP("oc1"); 
		is0 = AddIP("is0"); is1 = AddIP("is1"); ic = AddIP("ic"); 
	}
};

ExpFrame* MakeEF(const string& name)
{
	ExpFrame* EF = new ExpFrame(name);
	 
	Generator* gn0 = new Generator("Gen0",true, 60); // 1 min
	gn0->ClientMap.push_back(make_pair(0.5, new Cassette(0, Config::GetNoSlots(0))));
	gn0->ClientMap.push_back(make_pair(0.5, new Cassette(1, Config::GetNoSlots(1))));
	gn0->CollectStatistics();
	EF->AddModel(gn0);
	EF->AddCP(gn0->out, EF->oc0); // EOC
	EF->AddCP(EF->is0, gn0->is); // EIC

	Generator* gn1 = new Generator("Gen1", true, 60); // 1 min
	gn1->ClientMap.push_back(make_pair(0.5, new Cassette(0, Config::GetNoSlots(0))));
	gn1->ClientMap.push_back(make_pair(0.5, new Cassette(1, Config::GetNoSlots(1))));
	gn1->CollectStatistics();
	EF->AddModel(gn1);
	EF->AddCP(gn1->out, EF->oc1); // EOC
	EF->AddCP(EF->is1, gn1->is); // EIC

	Transducer* tr = new Transducer("Trans");
	EF->AddModel(tr);
	EF->AddCP(EF->ic, tr->in); // EIC
	return EF;
}

void MakeLotCTConfiguration()
{
	int ws_id = 1; // only one
	//----------------- Lot: 0 -------------------------
    Config::SetNoSlots(0, 15);
	vector<pair<int, double> > processInfo0;
	processInfo0.push_back(make_pair(1, 60)); // etching (min)
	processInfo0.push_back(make_pair(2, 40)); // cleaning (min)
	processInfo0.push_back(make_pair(3, 40)); // drying (min)
	Config::SetLotProcessTime(0, ws_id, processInfo0);
    //--------------------------------------------------
	//----------------- Lot: 1 -------------------------
    Config::SetNoSlots(1, 25);
	vector<pair<int, double> > processInfo1;
	processInfo1.push_back(make_pair(11, 40)); // etching (min)
	processInfo1.push_back(make_pair(2, 30)); // etching (min)
	processInfo1.push_back(make_pair(3, 30)); // drying (min)
	Config::SetLotProcessTime(1, ws_id, processInfo1);
	//--------------------------------------------------
}

Coupled* MakeCTSystem(const string& name)
{
	Coupled* System = new Coupled("CT_EF");
	MakeLotCTConfiguration();
	//ctrl->m_SelectionRule = Ctrl::LCM0F;
	//ctrl->m_SelectionRule = Ctrl::LFAFS;
	//ctrl->m_SelectionRule = Ctrl::LSPT;
	//ctrl->m_SelectionRule = Ctrl::LLWKR;
	//ctrl->m_SelectionRule = Ctrl::LMWKR;
	//ctrl->m_SelectionRule = Ctrl::GLOBAL;

	ClusterTool* CT = new ClusterTool("CT", 1, 2, 4, LLWKR);
	ExpFrame* EF = MakeEF("EF");
	System->AddModel(CT);
	System->AddModel(EF);
	//---- ICs -------------------------
	System->AddCP(EF->oc0, CT->IC[0]); 
	System->AddCP(EF->oc1, CT->IC[1]); 
	System->AddCP(CT->OC[0], EF->is0);
	System->AddCP(CT->OC[0], EF->ic);
	System->AddCP(CT->OC[1], EF->is1);
	System->AddCP(CT->OC[1], EF->ic);
	return System;
}

 
int main( void )
{
	Coupled* CT = MakeCTSystem("CTS");//    86,400=  1 day
	SRTEngine simEngine(*CT,604800);  //   604,800=  7 days
	                                  // 2,592,000= 30 days  
	                                  //31,536,000=365 days
	simEngine.SetTimeScale(DBL_MAX); 
	simEngine.SetAnimationFlag(false);
	simEngine.Set_dtmode(DEVSpp::SRTEngine::P_NONE);
	simEngine.SetLogTransition(true);
	simEngine.RunConsoleMenu();
	
	delete CT;
	return 0;
}

