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
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Config.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/TMcmd.h>
#include <DEVSpp/ModelBase/SemiConMnfct/CT/Ctrl.h>
#include <assert.h>

using namespace CT;

const string Ctrl::Ask="Ask"; 
const string Ctrl::Wait="Wait"; 
const string Ctrl::CmdTransfer="CmdTransfer";  
const string Ctrl::CmdProcess="CmdProcess";  
const string Ctrl::CmdCM_Out="CmdCM_Out"; 

///////////////////////////////////////////////////////////////////////////////////////////////////////////
string GetIndex_s(string phase, int index)
{
	if(phase == "CmdProcess" || phase == "CmdProcess")
		return "index="+::STR(index);
	else 
		return "index="+::STR(0);
}
//
//void SetIndex_s(string str, int& index)
//{
//	vector<string> s2 = DEVSpp::Split(str,'=');  
//	assert(s2[0]=="index");	index = atoi(s2[1].data());
//}

string GetTMWorkTransfer(string phase, Ctrl::TMSTATUS TMWork, const int t[2][2])
{
	if(phase == Ctrl::CmdTransfer)
	{
		return "TMwork=TO_PICK,trans="+::STR(t[0][0])+":"+::STR(t[0][1])+":"+
				  ::STR(t[1][0])+":"+::STR(t[1][1]);
	}
	else
	{
		if(TMWork==Ctrl::TO_PICK)
			return "TMwork=TO_PICK,trans="+::STR(t[0][0])+":"+::STR(t[1][0]);
		else if(TMWork==Ctrl::TO_PLACE)
			return "TMwork=TO_PLACE,trans="+::STR(t[1][0]);
		else
			return "TMwork=IDLE";
	}
}
//
//void SetTMWorkTransfer(string str, string phase, Ctrl::TMSTATUS& tmwork, 
//					   int& t00, int& t01, int& t10, int& t11)
//{
//	vector<string> s12 = DEVSpp::Split(str, ',');
//
//	vector<string> s2 = DEVSpp::Split(s12[0],'=');				 
//	assert(s2[0]=="TMwork"); 
//	if(s2[1] == "TO_PICK") tmwork=Ctrl::TO_PICK;
//	else if(s2[1] == "TO_PLACE") tmwork = Ctrl::TO_PLACE;
//	else if(s2[1] == "IDLE") tmwork = Ctrl::IDLE;
//	else 
//		assert(0);
//	if(tmwork != Ctrl::IDLE)
//	{
//		s2 = DEVSpp::Split(s12[1],'=');	
//		assert(s2[0]=="trans");
//
//		s2 = DEVSpp::Split(s2[1],':');	
//		if(phase == Ctrl::CmdTransfer)
//		{
//			t00=atoi(s2[0].data());  
//			t01=atoi(s2[1].data()); // src: cm_pm index & slot index
//			t10=atoi(s2[2].data());  
//			t11=atoi(s2[3].data()); // dest: cm_pm index & slot index
//		}
//		else if(tmwork==Ctrl::TO_PICK)
//		{
//			t00=atoi(s2[0].data());  // src: cm_pm index
//			t10=atoi(s2[1].data());  // dest: cm_pm index
//		}else if(tmwork == Ctrl::TO_PLACE)
//			t10=atoi(s2[0].data()); // dest: cm_pm index
//	}
//}


string GetWafers(const vector<vector<MV_Wafer*> >& m_MV_Wafers,
				 const vector<MV_CM_PM*>& m_CM_PM)
{
	string str = "NoOfCassettes="+::STR((int)m_MV_Wafers.size())+",";  
	for(int i=0; i < (int) m_MV_Wafers.size(); i++){
		str += "CM:"+::STR(i)+
			   ",NoOfWafers="+::STR((int)m_MV_Wafers[i].size());
		if((int)m_MV_Wafers[i].size()>0) str += ",";
		for(int j=0; j < (int)m_MV_Wafers[i].size(); j++) {
			if(j>0) str += ",";
			str += m_MV_Wafers[i][j]->STR(m_CM_PM);
		}
		str += ",";
	}
	return str;
}
//
//void SetWafers(string str,
//			   vector<vector<MV_Wafer*> >& m_MV_Wafers,
//				vector<Time>& ArrivalTimes,
//				vector<TimeSpan>& TotalProcessTimes,
//			   const vector<MV_CM_PM*>& m_CM_PM)
//{
//	vector<string> s2 = DEVSpp::Split(str,',');	
//	vector<string> tmp = DEVSpp::Split(s2[0], '=');
//	assert(tmp[0]=="NoOfCassettes");
//	assert((int)m_MV_Wafers.size() == atoi(tmp[1].data())); 
//	int k=1;
//	for(int i=0; i < (int)m_MV_Wafers.size(); i++) { 
//		tmp = DEVSpp::Split(s2[k++], ':');//s2[1]
//		assert(tmp[0]=="CM"); assert(i == atoi(tmp[1].data()));
//		tmp = DEVSpp::Split(s2[k++], '=');//s2[2]
//		assert(tmp[0]=="NoOfWafers");
//		int no_wafers = atoi(tmp[1].data());
//		if(no_wafers>0)
//			Ctrl::MakeCassetteNew(i, 0, m_MV_Wafers,ArrivalTimes, TotalProcessTimes, m_CM_PM);
//		for(int j=0; j < no_wafers; j++)
//			m_MV_Wafers[i][j]->Set(s2[k++], m_CM_PM);
//	}
//}

string GetCM_PMs(const vector<MV_CM_PM*>& m_CM_PM,
				 const vector<vector<MV_Wafer*> >& m_MV_Wafers)
{
	string str = "NoOfCM_PMs="+::STR((int)m_CM_PM.size())+",";
	//-- all CM and PM monitoring variables ---
	for(int i=0; i < (int) m_CM_PM.size(); i++) {
		if(i>0) str += ",";
		str += m_CM_PM[i]->STR(m_MV_Wafers);
	}
	return str;
}
//
//void SetCM_PMs(string str, vector<MV_CM_PM*>& m_CM_PM,
//			   const vector<vector<MV_Wafer*> >& m_MV_Wafers)
//{
//	vector<string> s2 = DEVSpp::Split(str,',');	
//	vector<string> tmp = DEVSpp::Split(s2[0], '=');
//	assert(tmp[0]=="NoOfCM_PMs");
//	assert((int)m_CM_PM.size() == atoi(tmp[1].data())); 
//	for(int k=0; k < (int)m_CM_PM.size(); k++) {
//		m_CM_PM[k]->Set(s2[k+1], m_MV_Wafers);
////cout << "k=" << k << ",m_CM_PM[k]->m_Assigned="<< m_CM_PM[k]->m_Assigned << endl;
//	}
//}
//
//void Ctrl::Set_s(const string& str) 
//{ 
////cout << str << endl;
//	vector<string> semicomma_s = DEVSpp::Split(str,';');   //not , but ;
//	m_phase = semicomma_s[0];			
//	SetIndex_s(semicomma_s[1], m_oindex);
//	SetTMWorkTransfer(semicomma_s[2], m_phase, m_TMphase, 
//		m_TMcommand[0][0], m_TMcommand[0][1], m_TMcommand[1][0], m_TMcommand[1][1]);
//	SetWafers(semicomma_s[3],m_MV_Wafers, m_tA, m_TWRK, m_CM_PM);		
//	SetCM_PMs(semicomma_s[4],m_CM_PM, m_MV_Wafers);	
//} 

string Ctrl::Get_s() const 
{
	string str = m_phase+";"+GetIndex_s(m_phase, m_oindex)+";"
				+GetTMWorkTransfer(m_phase, m_TMphase, m_TMcommand)+";";

	str += GetWafers(m_MV_Wafers,m_CM_PM)+";";
	str += GetCM_PMs(m_CM_PM,m_MV_Wafers); 
	if(m_TMphase == Ctrl::TO_PLACE && m_CM_PM[m_TMcommand[1][0]]->m_Assigned == NULL)
	{
		cout << str<<endl;
		cout << "Got ya!" << endl;
		string in;
		cin >> in;
	}
	return str;
}
