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
#pragma warning(disable:4251)
#pragma warning(disable:4275)

#include <DEVSpp/ModelBase/SemiConMnfct/CT/Wafer.h>
#include <vector>
#include <DEVSpp/ModelBase/Job.h>
#include <DEVSpp/DEVS_Exception.h>
#include <DEVSpp/StrUtil.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/CT_Export.h>

using namespace std;
using namespace DEVSpp;
using namespace MB_Basic;

namespace CT
{
	class MB_CT_EXP Cassette: public Job
    {
	protected:
		vector<Wafer> m_Wafers;
        int capacity;
	public:
        //-- default constructor
		Cassette(int Type, int no_wafers=0):Job(Type),  capacity(no_wafers) 
        {
			for(int i=0; i<capacity; i++)
				m_Wafers.push_back(Wafer("N"));
        }

		//-- copy constructor
        Cassette(const Cassette& obj) { Copy(obj); }

		void Copy(const Cassette& obj)
		{   
			Job::Copy(obj);
			Clear();
            capacity = obj.capacity;
            for (int i = 0; i < obj.capacity; i++)
				m_Wafers.push_back(obj.m_Wafers[i]);
		}

		//--
		virtual ~Cassette() { }

		void Clear()
		{   capacity = 0;
			m_Wafers.clear();
		}

      	//-- copy me to another instance
		/*virtual*/ Value* Clone() const {return new Cassette(*this);}

		//-- convert to string
		/*virtual*/ std::string STR() const;
		void Set_s(const string& str);

        bool IsEmptyAt(int index) const
        { return (m_Wafers[index].STR() == "0");  }

		const Wafer& GetAt(int index) const
        {
			return m_Wafers[index];
        }

        Wafer& RemoveAt(int index)
        {
			Wafer& wafer = m_Wafers[index];
            m_Wafers[index].Set_s("0"); // 0 means removed
			return wafer;
        }

        void InsertAt(int index, const Wafer& w)
        {
            if (index >= capacity)
                THROW_DEVS_EXCEPTION("Out of Boundary!");
            if (IsEmptyAt(index) == false )
                THROW_DEVS_EXCEPTION("Something in there!");

            m_Wafers[index] = w;
        }
 
		int GetEmptyIndex() const
		{
			for(int i=0; i<(int)m_Wafers.size(); i++){
				if(m_Wafers[i].STR() =="0")
					return i;
			}
			return -1;
		}
    };
}