/*************************************************************************
 * DEVS++: DEVS Open Source in C++                                       *
 * Copyright (C) 2005-2009  Moon Ho Hwang <moonho.hwang@gmail.com>       *
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
#pragma warning(disable:4786)
#pragma warning(disable:4996)
#pragma warning(disable:4835)

#include "ClassAdmin.h"
#include <assert.h>
#include <stdio.h>

#define _USE_HIER_PARENTS_SET_

#if defined(WIN32) && defined(_DEBUG)
#include <windows.h> // <afx.h>
//#define new DEBUG_NEW
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
#endif

using namespace DEVSpp;
using namespace std;

Class_Tree Class_Tree::Instance;

Class_Tree::Class_Tree() : m_root(0, "ROOT") 
{
    //Instance = this;
}

int G_NewCount = 0;
int G_DeleteCount = 0;

void* Class_Node::CreateObject() 
{ 
	if(static_create_function)
		return (*static_create_function)();
	else 
		return NULL;
}

void Class_Node::AppendChildNode(Class_Node* child_node)// 2002.07.19
{ 
    m_children.push_back(child_node); 
    child_node->m_parents.push_back(this); 

    //------ hierarchical child node of child_node(including child_node) -----
    list<Class_Node*> HierChildren;
    child_node->GetHierChildren(HierChildren);
    HierChildren.push_back(child_node);

    //------ hierarchical parent of this(including this) -----
    list<Class_Node*> HierParents;
    this->GetHierParents(HierParents);
    HierParents.push_back(this);
    //------ assign 
    list<Class_Node*>::iterator itc = HierChildren.begin();
    for(; itc != HierChildren.end(); itc++)
    {
        Class_Node* child = *itc;
        list<Class_Node*>::iterator itp = HierParents.begin();
        for(; itp != HierParents.end(); itp++)
        {
            Class_Node* parent = *itp;
            child->m_HierParents.insert(parent);
        }
    }
}

void Class_Node::GetHierChildren(list<Class_Node*>& HierChildren) const
{
    list<Class_Node*>::const_iterator cit = m_children.begin();
    for(; cit != m_children.end(); cit++)
    {
        HierChildren.push_back(*cit);
        (*cit)->GetHierChildren(HierChildren);
    }
}

void Class_Node::GetHierParents(list<Class_Node*>& HierParents) const
{
    list<Class_Node*>::const_iterator cit = m_parents.begin();
    for(; cit != m_parents.end(); cit++)
    {
        HierParents.push_back(*cit);
        (*cit)->GetHierParents(HierParents);
    }
}

Class_Node::~Class_Node() 
{ 
}

//--- 1998.9.2
Class_Node* Class_Node::FindChildNode(DEVS_RT_TYPE pver) const
{ 
    /* if same as me, return me. */
    if( m_pVersion == pver ) return (Class_Node*) this;
    list<Class_Node*>::const_iterator i = m_children.begin();
    for (; i != m_children.end(); ++i) {
	    Class_Node* cn = (*i)->FindChildNode(pver);
	    if( cn ) return cn; // if we find a child, return it.
    }
    return NULL; // otherwise return NULL
}

bool   Class_Node::IsMyHierParent(const Class_Node& HParent) const
{
#   if defined(_USE_HIER_PARENTS_SET_) 
    return (&HParent == this || m_HierParents.find((Class_Node*)&HParent)
           !=m_HierParents.end())? true : false;
#   else 
	    if( &HParent == this) 
            return true;
    list<Class_Node*>::const_iterator i;
    for (i = m_parents.begin(); i != m_parents.end(); ++i) {
        bool cn = (*i)->IsMyHierParent(HParent);
        if( cn ) return cn; // If we find a parent, return true.
    }
    return false; // otherwise, return false.
#   endif
}

void Class_Node::GetHierName(const string& PrevLevel, list<string>& strlist) const
{ 
    string CurrLevel = m_className + "." + PrevLevel;
    if(m_parents.empty()) strlist.push_back(CurrLevel);

    list<Class_Node*>::const_iterator i;
    for(i=m_parents.begin(); i != m_parents.end(); ++i) 
        (*i)->GetHierName(CurrLevel, strlist);
}

void Class_Node::PrintAllChildren() const 
{
    list<string> strlist;

    GetHierName("", strlist);
    list<string>::iterator i;
    for(i=strlist.begin(); i != strlist.end(); ++i)
        cout << (*i) <<"\n";
	    
    list<Class_Node*>::const_iterator j;
    for(j=m_children.begin(); j != m_children.end(); ++j)
        (*j)->PrintAllChildren();

}

void Class_Node::PrintAllChildren(ostream& strm) const
{
    static int length = 0;

    if(m_children.empty()) {
        length += m_className.length();
        strm << m_className << "\n"; 
        length -= m_className.length();
    }else {
        list<Class_Node*>::const_iterator j;

        for(j=m_children.begin(); j != m_children.end(); ) {
            length += m_className.length()+1;
            strm << m_className << ".";
            (*j)->PrintAllChildren(strm);
            length -= (m_className.length()+1);
            j++; 
            if(j != m_children.end()) {
                int i=0;
                while(i++<length)
	                strm << ".";
            }
        }
    }
}

Class_Tree::~Class_Tree()
{
    map<DEVS_RT_TYPE, Class_Node*>::iterator it =  m_map.begin();
    for(;it != m_map.end(); it++)
    {
        Class_Node* cd = (*it).second;
        string str = cd->m_className + " deleted!\n";
 //       TRACE((char*) str.data());
        delete cd; // (*it).second;
        G_DeleteCount ++;
    }
    m_map.clear();
    m_strmap.clear();
//
//#ifdef _DEBUG
//    TRACE("NewCount %d, DeleteCount %d", G_NewCount, G_DeleteCount);
//#endif 
    assert(G_NewCount == G_DeleteCount);
}

//-- 정상의 조건에서 찾는 cd_mem에 해당하는 Class_Node를 리턴한다. 
//-- 이것을 받아 후속 함수에서 m_pRObject에 assigne하기 바란다. 
Class_Node* 
Class_Tree::AppendClassToParentClass(	DEVS_RT_TYPE cd_vrs,
								    string cd_name,
									DEVS_RT_TYPE pr_vrs /* NULL */ ) { 
    Class_Node* pr_node ;
    if(cd_name=="") return NULL;

    // if the parent is not define, let the parent be root
    if(pr_vrs == NULL)
      pr_node = &m_root;
    else {
      pr_node = FindNode(pr_vrs);
      if(!pr_node) return NULL; // we cannot find the parent
    }

    Class_Node* child_node = FindNode(cd_vrs);

    if(child_node) {// the parent has been already registered.
	    if(pr_node->FindChildNode(cd_vrs)) return NULL; //false; // 이미 들어가 있다.
	    else pr_node->AppendChildNode(child_node);
    }else if(!FindNode(cd_name) ) {
		child_node = new Class_Node(cd_vrs, cd_name); 
		G_NewCount ++;
		assert(child_node);
		pr_node->AppendChildNode(child_node);

        m_map[cd_vrs] = child_node;
        m_strmap[cd_name] = child_node;
    } else {
        string tmp = cd_name + " Class Name Collision";
//        TRACE(tmp.data());
        assert(0);
    }
    return child_node;
}

bool  Class_Tree::A_IsKindOf_B(unsigned int * A, unsigned int * B) const {
    Class_Node* cd_node = FindNode(A);
    Class_Node* pr_node = FindNode(B);
    if( !cd_node || !pr_node ) return false;
    return cd_node->IsMyHierParent(*pr_node);
}

bool  Class_Tree::A_IsKindOf_B(string A, string B) const {
    Class_Node* cd_node = FindNode(A);
    Class_Node* pr_node = FindNode(B);
    if( !cd_node || !pr_node ) return false;
    return cd_node->IsMyHierParent(*pr_node);
}
list<string> Class_Tree::GetAllInstancibleKindsOf(const string& ClassName) const
{
	list<string> tmp;
	 Str2ClassNodePtr::const_iterator it = m_strmap.begin();
	for(; it != m_strmap.end(); it++) {
		if(A_IsKindOf_B((*it).first, ClassName)) {
			if((*it).second->static_create_function != NULL) {
				tmp.push_back((*it).first);
			}
		}
	}
	return tmp;
}

Class_Node* Class_Tree::FindNode(DEVS_RT_TYPE pVersion) const 
{ 
    Ptr2ClassNodePtr::const_iterator i = m_map.find(pVersion); 
    if(i == m_map.end()) return NULL;
    else return (*i).second ;
}

Class_Node* Class_Tree::FindNode(string name) const
{ 
    Str2ClassNodePtr::const_iterator i = m_strmap.find(name); 
    if(i == m_strmap.end()) return NULL;
    else return (*i).second ; 
}

//=========================================================================
//Class_Tree& GetGClassTree() { return G_Class_Tree; }

void DEVSpp::PrintAllClassTree() { Class_Tree::Instance.PrintAll(); }
void DEVSpp::PrintAllClassTree(ostream& strm) { Class_Tree::Instance.PrintAll(strm); }


void* DEVSpp::CreateObjectByClassName(const string& class_name) 
{
	Class_Node* nd =Class_Tree::Instance.FindNode(class_name);
	if(nd) 
		return nd->CreateObject();
	else{ 				
		char err_msg[256];
		sprintf(err_msg,"Cannot Create Object!\nClass Name : %s", class_name.c_str()); 
		throw err_msg;		
	}
}

bool DEVSpp::IsAKindOfB(const string& A, const string& B)
{
    Class_Node* cd_node = Class_Tree::Instance.FindNode(A);
    Class_Node* pr_node = Class_Tree::Instance.FindNode(B);
    if( !cd_node || !pr_node ) return false;
    return cd_node->IsMyHierParent(*pr_node);
}

list<string> DEVSpp::GetAllInstancibleKindsOf(const string& ClassName)
{
	return  Class_Tree::Instance.GetAllInstancibleKindsOf(ClassName);
}