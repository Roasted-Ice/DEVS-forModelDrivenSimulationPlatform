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

#include <DEVSpp/Coupled.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CT_Export.h>

using namespace std;
using namespace DEVSpp;

namespace CT
{
	class MB_CT_EXP ClusterTool: public Coupled
	{
	public:
		vector<InputPort*> IC;
		vector<OutputPort*> OC;
		InputPort *IDB;
		OutputPort *ODB;

	public:

		ClusterTool(const char* name, int ws_id, int cm_size, int pm_size,
			WaferSelectionRule rule);
	};
}
