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
#include <vector>
#include <map>
 
using namespace std;

namespace SF
{
    /// The monitoring Variable for a Wafer
    class MV_Lot
    {
	public:
		//-- id for lot (bar code value)
		unsigned id;
		//-- id for lot (bar code value)
		unsigned type;
		//-- arrival time at system
		double  m_tA; 
		//-- 0 <= < associated PDM.size(); -1, end of process, should be out.
		int  m_CP_Index;  
		//-- if true, CurrentProcess is started (but not finished)
		//-- otherwise, it is waiting to be processed 
		bool m_bCP_Started; 

		/// Default Constructor
		MV_Lot(unsigned ID, unsigned TYPE, double ta):
			id(ID), type(TYPE), m_tA(ta), m_CP_Index(0), m_bCP_Started(false) {}

		string MV_Lot::STR() const;

		bool operator<(const MV_Lot& ob) const
		{
			return id < ob.id;
		}

		bool AllFinishedLot() const { return m_CP_Index == -1; }
    };
}