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
#include <DEVSpp/ModelBase/SemiConMnfct/CT/TM.h>
#include <DEVSpp/DEVS_Exception.h>
#include <float.h>

using namespace CT;

IMPLEMENT_ABSTRACT_CLASS1(TM, "CT.TM", 20061216, Atomic);

const string TM::Idle="Idle";
const string TM::MoveToPick="MoveToPick";
//const string StrechToPick, 
const string TM::WaitReceive="WaitReceive";
const string TM::ShrinkAfterPick="ShrinkAfterPick";
const string TM::MoveToPlace="MoveToPlace";
// const string  StrechToPlace, 
const string TM::ShrinkAfterPlace="ShrinkAfterPlace";
const string TM::Error="Error";

bool TM::IsInputIW(const Port* ip)
{
	for(int i=0; i<m_no_cm_pm; i++){
        if(ip == IW[i])
            return true;
    }
    return false;
}

	
double TM::CalcMovingRadian(int curr_index, int dest_index)
{
    if (curr_index < dest_index)
    {
        int CW_delta = (int)(dest_index - curr_index);
        int CCW_delta = (int)(curr_index + m_no_cm_pm - dest_index);
        if (CW_delta < CCW_delta)
            return -CW_delta * 60 * M_PI / 180.0;
        else
            return CCW_delta * 60 * M_PI / 180.0;
    }
    else if (dest_index < curr_index)
    {
        int CCW_delta = (int)(curr_index - dest_index);
        int CW_delta = (int)(dest_index + m_no_cm_pm - curr_index);

        if (CW_delta < CCW_delta)
            return -CW_delta * 60 * M_PI / 180.0;
        else
            return CCW_delta * 60 * M_PI / 180.0;
    }
    else
        return 0.0;
}

double TM::CalcMovingTIme(int curr_index, int dest_index)
{
	/* Previous Codes
    double travel_radian = CalcMovingRadian(curr_index, dest_index);
    // 2*M_PI (360 degree) movement requires 24 (000 milli) sec.
    // 2*M_PI : 24000 = travel_radian : x
    // x = 24000 * travel_radian / (2*M_PI)
    return fabs(travel_radian) / (2 * M_PI) * 24;//24000;
	*/
	double delta_index = curr_index-dest_index;
	return fabs(delta_index)*2.0; // 2 sec/index
}

void TM::UpdateCurrentPosition(int dest_index[])
{
    for (int i = 0; i < 2; i++)
        m_curr_index[i] = dest_index[i];
}

/*virtual*/ TimeSpan TM::tau() const 
{ 
	if (m_phase == MoveToPick)
        return (m_travel_t + 2.); // /60.;
    else if (m_phase == ShrinkAfterPick || m_phase == ShrinkAfterPlace)
        return 2.; // /60.;
    else if (m_phase == MoveToPlace)
        return (m_travel_t+2.); // /60.;
    else 
        return DBL_MAX;
}


/*virtual*/ bool TM::delta_x(const PortValue& x) 
{
	if (m_phase == Error)
		return false;

    if (x.port  == IHS)
    {
        if (m_phase == Idle)
        {
            m_phase = MoveToPick; 
            TMcmd* tpcmd = dynamic_cast<TMcmd*>(x.value);
			if(tpcmd) { 
				m_pick_index[0] = tpcmd->c[0][0];
				m_pick_index[1] = tpcmd->c[0][1];
				m_place_index[0] = tpcmd->c[1][0];
				m_place_index[1] = tpcmd->c[1][1];
				m_travel_t = CalcMovingTIme(m_curr_index[0], m_pick_index[0]);
			}else
				m_phase = Error;
        }
        else
            m_phase = Error;
    }
    else if (IsInputIW(x.port)) 
    {
		if (m_phase == WaitReceive){
            m_phase = ShrinkAfterPick;
			Wafer* wf = dynamic_cast<Wafer*>(x.value);
			if(wf)
				m_wafer = *wf;
			else
				m_phase = Error;
		}
		else
            m_phase = Error;
    }
    return true; // x_RescheduleMe();
}
 

/*virtual*/ void TM::delta_y(PortValue& y)
{
	static bValue<int> bv(0);
 	static WaferFeed  wf(0, m_wafer);

	if (m_phase == MoveToPick)
    {
		bv.v = m_pick_index[1];
		y.Set(OS[m_pick_index[0]], &bv);
        m_phase = WaitReceive;
        UpdateCurrentPosition(m_pick_index);
    }
    else if (m_phase == ShrinkAfterPick)
    {
		bv.v = 1;
        y.Set(OHS, &bv); 
        m_phase = MoveToPlace;
        m_travel_t = CalcMovingTIme(m_curr_index[0], m_place_index[0]); 
    }
    else if (m_phase == MoveToPlace)
    {
		wf.slot_i = m_place_index[1];
 		wf.wafer = m_wafer;
		y.Set(OW[m_place_index[0]], &wf);
        m_wafer.Set_s("0"); // make null
        UpdateCurrentPosition(m_place_index);
        m_phase = ShrinkAfterPlace;
    }
    else if (m_phase == ShrinkAfterPlace)
    {
		bv.v = 0;
		y.Set(OHS, &bv);
        m_phase = Idle;
    }
    else if (m_phase == Idle)
        ;// skip
    else
        throw new Exception("Invalide Phase!");
}

/*virtual*/ string TM::Get_s() const 
{
	string str = m_phase+";c="+::STR(m_curr_index[0]) + "," +::STR(m_curr_index[1]); 
	if( m_phase !=Idle) {
		 str +=";s="+::STR(m_pick_index[0]) + "," +::STR(m_pick_index[1]) 
			 + ";d="+::STR(m_place_index[0]) + "," +::STR(m_place_index[1])+
			 +";travel_t="+::STR(m_travel_t) + 
			 ";wafer="+ m_wafer.STR();  
	}
	return str;
}


/*virtual*/ void TM::Set_s(const string& str) // for verification
{ 
	vector<string> s = DEVSpp::Split(str,';');//phase, slot_i, 0 or 1, .....
	m_phase = s[0];
	{
		vector<string> tmp=DEVSpp::Split(s[1], '=');
		vector<string> cs=DEVSpp::Split(tmp[1], ',');
		m_curr_index[0]=atoi(cs[0].data()); 
		m_curr_index[1]=atoi(cs[1].data());
	}
	if(m_phase != Idle) {
		{
			vector<string> tmp=DEVSpp::Split(s[2], '=');
			vector<string> cs=DEVSpp::Split(tmp[1], ',');
			m_pick_index[0]=atoi(cs[0].data()); 
			m_pick_index[1]=atoi(cs[1].data());
		}
		{
			vector<string> tmp=DEVSpp::Split(s[3], '=');
			vector<string> cs=DEVSpp::Split(tmp[1], ',');
			m_place_index[0]=atoi(cs[0].data()); 
			m_place_index[1]=atoi(cs[1].data());
		}
		{
			vector<string> tmp=DEVSpp::Split(s[4], '=');
			vector<string> cs=DEVSpp::Split(tmp[1], ',');
			m_travel_t = atoi(cs[0].data()); 
		}
		{
			vector<string> tmp=DEVSpp::Split(s[5], '=');
			vector<string> cs=DEVSpp::Split(tmp[1], ',');
			m_wafer.Set_s(cs[0].data());
		}
	}
} 