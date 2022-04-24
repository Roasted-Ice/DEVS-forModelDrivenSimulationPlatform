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
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CM.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/PM.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/TM.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Config.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Ctrl.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/ClusterTool.h>

using namespace std;
using namespace DEVSpp;
using namespace CT;
using namespace MB_Basic;

ClusterTool::ClusterTool(const char* name, int ws_id, int cm_size, int pm_size,
						 WaferSelectionRule rule): Coupled(name)
{
	IDB = AddIP("IDB"); // input from DB
	ODB = AddOP("ODB"); // output to DB

	Ctrl* ctrl = new Ctrl("ctrl", ws_id, cm_size, pm_size, rule); 

	AddModel(ctrl); // Ctrl

	for(int i=0; i < cm_size; i++)// CM 
	{
		IC.push_back(AddIP("IC"+::STR(i)));
		OC.push_back(AddOP("OC"+::STR(i)));
		CM* cm = new CM("cm"+::STR(i));
		AddModel(cm);
//		cm->CollectStatistics();
	} 
	for(int i=0; i < pm_size; i++)// PM 
	{
		PM* pm;
		if(i==0||i==1)
			pm = new PM("pm"+::STR(i), "E");
		else if(i == 2)
			pm = new PM("pm"+::STR(i), "C");
		else
			pm = new PM("pm"+::STR(i), "D");

		AddModel(pm);
		pm->CollectStatistics();
	}  
	TM* tm = new TM("tm", cm_size+pm_size);
	AddModel(tm); // TM

	//------------------------ Couplings ------------------------------
	for(int i=0; i< cm_size; i++) // CMs
	{
		CM* cm = (CM*)GetModel(STR("cm",i));
        AddCP(IC[i], cm->IC);
        AddCP(cm->OHS, ctrl->ICM_Occu[i]);
        AddCP(cm->OC, OC[i]);
        AddCP(cm->OC, ctrl->ICM_Free[i]);
        AddCP(ctrl->OCPM[i], cm->IHS);

        AddCP(cm->OW, tm->IW[i]);
        AddCP(tm->OS[i], cm->IS);
        AddCP(tm->OW[i], cm->IW);
	}

	for(int i=0; i< pm_size; i++) // PMs
	{
		PM* pm = (PM*)GetModel(STR("pm",i));
        AddCP(ctrl->OCPM[cm_size+i], pm->IHS);
        AddCP(pm->OHS, ctrl->IPM_Finished[i]);

        AddCP(pm->OW, tm->IW[cm_size+i]);
        AddCP(tm->OS[cm_size+i], pm->IS);
        AddCP(tm->OW[cm_size+i], pm->IW);
	}

	AddCP(ctrl->OTM, tm->IHS); // ctrl->tm
    AddCP(tm->OHS, ctrl->ITM); // tm->ctrl
	AddCP(ctrl->ODB, ODB);
	AddCP(IDB, ctrl->IDB);
}
