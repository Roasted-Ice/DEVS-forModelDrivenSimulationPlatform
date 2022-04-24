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
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/Buffer.h>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/DBMngr.h>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/PDM.h>
#include <DEVSpp/ModelBase/WIPRegulator.h>
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/Router.h>

using namespace std;
using namespace DEVSpp;
using namespace MB_Basic;
 

void MakeLotCTConfiguration(int ws_id)
{
	//----------------- Lot: 0 -------------------------
    CT::Config::SetNoSlots(0, 15);  //15
	vector<pair<int, double> > processInfo0;
	processInfo0.push_back(make_pair(1, 60)); // etching (min)
	processInfo0.push_back(make_pair(2, 40)); // cleaning (min)
	processInfo0.push_back(make_pair(3, 40)); // drying (min)
	CT::Config::SetLotProcessTime(0, ws_id, processInfo0); // 2100/60 = 35 (win)
    //----------------------------------------------------
	//----------------- Lot: 1 -------------------------
    CT::Config::SetNoSlots(1, 25);  
	vector<pair<int, double> > processInfo1;
	processInfo1.push_back(make_pair(11, 40)); // etching (min)
	processInfo1.push_back(make_pair(2, 30)); // etching (min)
	processInfo1.push_back(make_pair(3, 30)); // drying (min)
	CT::Config::SetLotProcessTime(1, ws_id, processInfo1);// 2500 /60 =41.7 
	//--------------------------------------------------
}

//
//int W[][13]={{1,2,-1}, // 12 * 35.0 = 420.0
//             {1,2,-1}};        //  8 * 41.7 = 333.6 (win)

int NoW= 24; //2 //4;

//int W[][13]={{1,2,3,4,1,2,3,4,1,2,3,4,-1}, // 12 * 35.0 = 420.0
//             {1,2,3,4,1,2,3,4,-1}};        //  8 * 41.7 = 333.6 (win)
//

int W[][173]=
{{1,2,13,14,23,15,20,22,23,22,17,13,14,15,
23,16,24,23,22,17,1,8,4,22,22,1,2,8,13,14,
18,23,15,16,23,18,22,1,1,13,14,23,15,16,24,
23,22,17,1,2,8,9,21,22,1,4,22,22,1,2,13,14,
23,15,16,24,24,23,22,17,24,1,2,7,1,3,22,13,
15,23,22,22,22,17,13,14,18,23,15,16,20,23,1,
17,1,1,3,13,14,16,24,23,22,17,9,21,1,3,13,
14,15,23,15,16,24,23,22,17,1,3,13,14,23,15,
16,23,15,16,24,23,22,17,1,3,10,22,12,6,22,6,
1,1,4,10,19,23,1,10,13,14,16,21,12,13,14,18,
23,15,15,15,16,19,23,22,17,11,13,14,15,21,23,5,-1},
{1,2,13,14,23,15,20,22,23,22,17,13,14,15,
23,16,24,23,22,17,1,8,4,22,22,1,2,8,13,14,
18,23,15,16,23,18,22,1,1,13,14,23,15,16,24,
23,22,17,1,2,8,9,21,22,1,4,22,22,1,2,13,14,
23,15,16,24,24,23,22,17,24,1,2,7,1,3,22,13,
15,23,22,22,22,17,13,14,18,23,15,16,20,23,1,
17,1,1,3,13,14,16,24,23,22,17,9,21,1,3,13,
14,15,23,15,16,24,23,22,17,1,3,13,14,23,15,
16,23,15,16,24,23,22,17,1,3,10,22,12,6,22,6,
1,1,4,10,19,23,1,10,13,14,16,21,12,13,14,18,
23,15,15,15,16,19,23,22,17,11,13,14,15,21,23,5,-1}};


/*
int* MakeWorsktateSequences(int lot_id)
{
	W.push_back(1);
	W.push_back(2);
	W.push_back(1);
	W.push_back(2);

	return W;
}
*/

void MakePDMs(SF::DBMngr* dbm)
{
	SF::PDM pdm;
	for(int loti = 0; loti < 2; loti++) { 
		//vector<int> WS = MakeWorsktateSequences(loti);
		pdm.Clear();
		
		for(int i = 0; W[loti][i] != -1; i++) {
			double meant = CT::Config::GetLotProcessTime(loti, W[loti][i]);
			pdm.Add(SF::MV_Process(loti, W[loti][i], meant, 1));
		}
		dbm->m_MapOfPDM.insert(make_pair(loti, pdm)); // (lot_type, PDM)
	}
}


SF::Buffer* MakeBuffer(const char* name, int no_o, Coupled* SF, SF::DBMngr* dbm)
{
	SF::Buffer* bf = new SF::Buffer(name, no_o);
	bf->CollectStatistics();
	SF->AddModel(bf);
	
	SF->AddCP(bf->ODB, dbm->Query);//IC:Buffer1->DBMngr.Query
	SF->AddCP(dbm->AddOP(bf->Name), bf->IDB);//IC:DBMngr->Buffer1
	return bf;
}

SF::Router* MakeRouter(const char* name, Coupled* SF, SF::DBMngr* dbm)
					   //,const map<int, SF::Buffer*>& OPortToBuffer)
{
	SF::Router* rt = new SF::Router(name);
	SF->AddModel(rt);
	SF->AddCP(rt->ODB, dbm->Query);//IC:RT1->DBMngr.Query
	SF->AddCP(dbm->AddOP(rt->Name), rt->IDB);//IC: DBMngr->RT1
	return rt;
}

CT::ClusterTool* MakeCT(const char* name, int ws_id, int cm, int pm, Coupled* SF, 
						SF::Buffer* bf, SF::Router* rt, SF::DBMngr* dbm)
{
	CT::ClusterTool* ct = new CT::ClusterTool(name, ws_id, cm, pm, CT::GLOBAL); // ); //CT::LLWKR);
	SF->AddModel(ct);
	for(int i=0; i < 2; i++){
		SF->AddCP(bf->out[i], ct->IC[i]); // IC: Buffer1->CT1 
		SF->AddCP(ct->OC[i], bf->pull[i]); // IC: CT1->Buffer1.pull
		SF->AddCP(ct->OC[i], rt->in); // IC: CT1->RT1.in
		SF->AddCP(ct->OC[i], dbm->Completed); // IC: CT1->DBMngr.Completed
		if(i==0) {
			SF->AddCP(ct->ODB, dbm->Query);//IC: RT1->DBMngr.Query
			SF->AddCP(dbm->AddOP(ct->Name),ct->IDB);//IC: DBMngr->RT1
		}
	}
	return ct;
}

void MakeRoutePaths(Coupled* SF, vector<SF::Router*>& RTs, vector<SF::Buffer*>& BFs)
{
	assert(((int) RTs.size()) == ((int) BFs.size()));
	for(int i=0; i< (int)RTs.size(); i++) 
		for(int j=0; j<(int)BFs.size(); j++) 
			SF->AddCP(RTs[i]->AddOP(STR(j+1)), BFs[j]->in);  // IC
}

WIPRegulator* MakeWIPRegulator(const char* name, int wip, Coupled* SF, 
							   const vector<SF::Router*>& RTs)
{
	WIPRegulator* rg = new WIPRegulator(name,wip);
	SF->AddModel(rg);
	
	SF->AddCP(SF->GetIP("in"), rg->in); // EIC
	SF->AddCP(rg->pull, SF->GetOP("pull")); // EOC

	for(int i=0; i<(int)RTs.size(); i++) {
		SF::Router* rt = RTs[i];
		SF->AddCP(rt->SysOut, rg->out); // ITC
		SF->AddCP(rt->SysOut, SF->GetOP("out")); // EOC
	}
	return rg;
}


Coupled* MakeSemiFab(const string& name)
{
	Coupled* SF = new Coupled(name);
	SF::DBMngr* dbm = dbm = new SF::DBMngr("DBMngr");
	dbm->m_priority = SF::DBMngr::GLWKR; //GFAFS; //GLWKR;
	SF->AddModel(dbm);
	SF->AddIP("in");
	SF->AddOP("pull");
	SF->AddOP("out");

	vector<SF::Buffer*> BFs;
	for(int i=1; i<=NoW;i++) {
		char tmp[10]; sprintf(tmp, "Buffer%d", i);
		BFs.push_back(MakeBuffer(tmp, 2, SF, dbm));
	}
		
	vector<SF::Router*> RTs;
	for(int i=1; i<=NoW;i++) {
		char tmp[10]; sprintf(tmp, "Router%d", i);
		RTs.push_back(MakeRouter(tmp, SF, dbm));
	}

	for(int i=1; i<=NoW; i++)
		MakeLotCTConfiguration(i);

	vector<CT::ClusterTool*> CTs;
	for(int i=1; i<=NoW;i++) {
		char tmp[10]; sprintf(tmp, "CT%d", i);
		MakeCT(tmp, i, 2, 4, SF, BFs[i-1], RTs[i-1], dbm);
	}
	
	MakeRoutePaths(SF, RTs, BFs);
	MakeWIPRegulator("WIPRegulator", NoW*4, SF, RTs);

	SF->AddCP(SF->GetIP("in"), BFs[0]->in); // EIC

	MakePDMs(dbm);
	return SF;
}
 

Coupled* MakeEF(const string& name)
{
	Coupled* EF = new Coupled(name);
	EF->AddOP("oc0"); EF->AddIP("is0"); EF->AddIP("ic");
	Generator* gn0 = new Generator("Gen0",true, 10*60);//10 min.
	//gn0->ClientMap.push_back(make_pair(1.0, new CT::Cassette(0, CT::Config::GetNoSlots(0))));
	
	gn0->ClientMap.push_back(make_pair(0.5, new CT::Cassette(0, CT::Config::GetNoSlots(0))));
	gn0->ClientMap.push_back(make_pair(0.5, new CT::Cassette(1, CT::Config::GetNoSlots(1))));
	
	//gn0->CollectStatistics();
	EF->AddModel(gn0);
	EF->AddCP(gn0->out, EF->GetOP("oc0") ); // EOC
	EF->AddCP(EF->GetIP("is0"), gn0->is); // EIC

	Transducer* tr = new Transducer("Trans");
	EF->AddModel(tr);
	EF->AddCP(EF->GetIP("ic"), tr->in); // EIC
	return EF;
}

Coupled* MakeSystem(const string& name)
{
	Coupled* System = new Coupled("CT_EF");
	Coupled* SF = MakeSemiFab("SF");
	Coupled* EF = MakeEF("EF");
	System->AddModel(SF);
	System->AddModel(EF);
	System->AddCP(EF->GetOP("oc0"), SF->GetIP("in"));
	System->AddCP(SF->GetOP("pull"), EF->GetIP("is0"));
	System->AddCP(SF->GetOP("out"), EF->GetIP("ic"));
	//System->Flatten();
	return System;
}


int main( void )
{
	Coupled* CT = MakeSystem("CTS");
	SRTEngine simEngine(*CT,31536000); 
	//    86,400=  1 day
	//   604,800=  7 days
	// 2,592,000= 30 days  
	// 5,184,000= 60 days
	//31,536,000=365 days
	simEngine.SetTimeScale(DBL_MAX); 
	simEngine.SetAnimationFlag(false);
	simEngine.Set_dtmode(DEVSpp::SRTEngine::P_NONE);
	simEngine.SetLogTransition(true);
	simEngine.RunConsoleMenu();
	
	delete CT;
	return 0;
}

