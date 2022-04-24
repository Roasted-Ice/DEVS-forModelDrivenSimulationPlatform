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

#if !defined(MB_BASIC_EXPORT)
#    pragma message( "===== MB_Basic.lib automatically linked! =====" ) 
#	 pragma comment( lib, "MB_Basic.lib" ) 
#endif 

#if defined(WIN32) && defined(DLL)
#    if defined(MB_BASIC_EXPORT)
#	     define MB_BASIC_EXP _declspec(dllexport)
#	 else //(MB_BASIC_EXPORT)
#	     define MB_BASIC_EXP _declspec(dllimport)
#	 endif // (MB_BASIC_EXPORT)
#else // !(WIN32 && DLL)
#	define MB_BASIC_EXP
#endif// (WIN32 && DLL)
 
