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
#include <DEVSpp/ModelBase/SemiConMnfct/Fab/MV_Lot.h>
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/StrUtil.h>
#include <assert.h>

using namespace SF;
using namespace DEVSpp;

string MV_Lot::STR() const 
{ 
	// list of wafer information.
	string str = ":Unfinished";
	/*for(map<int,double>::const_iterator it  = m_ProcessDefinition.begin(); 
		it != m_ProcessDefinition.end(); it++)
		str +=":"+::STR(it.fis)
		     +":"+::STR(m_ProcessDefinition[i]);*/

	return str;
}
   