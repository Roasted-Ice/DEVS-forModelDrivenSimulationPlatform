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
#include "Coupled.h"
#include "Buffer.h"
#include "Server.h"
#include "Generator.h"
#include "Transducer.h"

using namespace std;
using namespace DEVSpp;

Coupled* MakeClientServer(const string& name, int nservers)
{
	Coupled* CS = new Coupled(name);

	Buffer* bf = new Buffer("BF", nservers);
	bf->CollectStatistics(true);

	CS->AddModel(bf);
	CS->AddCP(CS->AddIP("in"), bf->in); // EIC

	for(int i=0; i < nservers; i++) {
		Server* srv = new Server(STR("SV",i) );
		srv->CollectStatistics(true);
		CS->AddModel(srv);
		
		CS->AddCP(bf->out[i], srv->in ); // IC
		CS->AddCP(srv->out, CS->AddOP(STR("out",i) )); // EOC
		CS->AddCP(srv->out, bf->pull[i]); // IC
	}
	return CS;
}

Coupled * MakeExperimentalFrame(const string& name)
{
	Coupled* EF = new Coupled(name);
	EF->AddOP("out");
	EF->AddIP("in");

	Generator* gn = new Generator("Gen");
	EF->AddModel(gn);
	EF->AddCP(gn->GetOP("out"), EF->GetOP("out") ); // EOC
	
	Transducer* tr = new Transducer("Trans");
	EF->AddModel(tr);
	EF->AddCP(EF->GetIP("in"), tr->GetIP("in") ); // EIC
	
	return EF;
}

Coupled* MakeTotalClientServerSystem(int n)
{
	Coupled* System = new Coupled("CSsystem");
	Coupled* CS = MakeClientServer("CS", n); //-- simulation model
	Coupled* EF = MakeExperimentalFrame("EF");
	System->AddModel(CS);
	System->AddModel(EF);
	System->AddCP(EF->GetOP("out"), CS->GetIP("in"));
	for(int i=0; i<n; i++) {
		char str[10]; sprintf(str, "out%d",i);
		System->AddCP(CS->GetOP(str), EF->GetIP("in"));
	}
	return System;
}

void main( void )
{
	Coupled* Sys = MakeTotalClientServerSystem(5) ; //-- simulation model
	Sys->PrintCouplings();

	SRTEngine simEngine(*Sys, 10000); //  
	simEngine.SetAnimationFlag(false);
	simEngine.SetTimeScale(DBL_MAX); // 
	simEngine.Set_dtmode(SRTEngine::P_NONE);
	simEngine.RunConsoleMenu();
	delete Sys;
}

