/************************************************************************
 * DEVS-C++: Open Source of DEVS in C++                                *
 * Copyright (C) 2005-2006  Moon Ho Hwang <moonho.hwang@gmail.com>      * 
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
#pragma once

#include <DEVSpp/PortValue.h>
#include <DEVSpp/StrUtil.h>

using namespace std;
using namespace DEVSpp;

namespace CT
{
	class TMcmd: public Value
    {
	public:     
        ///  
        /// 2x2 command
        /// cmd[0,0]: source CM+PM index;      cmd[0,1]: source slot index;
        /// cmd[1,0]: destination CM+PM index; cmd[1,1]: destination slot index;
        ///  
        int c[2][2];
        
        TMcmd()
        {	
			for(int i=0; i < 2; i++)
				for(int j=0; j < 2; j++)
					c[i][j] = 0;
		}

        /*virtual*/ string STR() const 
        {
			return "pick:("+::STR(c[0][0])+","+::STR(c[0][1])+"),"+
				   "place:("+::STR(c[1][0])+","+::STR(c[1][1])+")";
        }
    };	   


}