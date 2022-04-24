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
#pragma once

#pragma warning(disable:4786)
#pragma warning(disable:4251)

#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <map>
#include <set>
#include "DEVS_Export.h"
#include "DeclareClass.h"

using namespace std;

namespace DEVSpp{

class DEVSpp_EXP Class_Node
{
///  friend class
friend class Class_Tree;
friend DEVSpp_EXP void* CreateObjectByClassName(const string& class_name) ;

public:
    /// memory variable for version 
    DEVS_RT_TYPE m_pVersion;

protected:
    /// class name
    /*const*/ string m_className; 
    /// parent nodes  
    list<Class_Node*> m_parents;

    /// children nodes 
    list<Class_Node*> m_children;

    struct ltClass_Node
    {
        bool operator()(Class_Node* s1, Class_Node* s2) const
	        { return (s1->m_pVersion < s2->m_pVersion); }
    };

    /// for fast-searching mechanism: set (2002.07.19) 
    set<Class_Node*, ltClass_Node>  m_HierParents;

    void* (* static_create_function)();

    void* CreateObject();

    // called by the destructor
    // void RemoveMeFromParensts(Class_Node* trigger) ;
    ///
    void GetHierChildren(list<Class_Node*>& HierChildren) const;
    void GetHierParents(list<Class_Node*>& HierParents) const;

     
public:
    /// constructor with cstr class name
    Class_Node(DEVS_RT_TYPE pver, const string& cstr) :
	    m_pVersion(pver), m_className(cstr), static_create_function(0) {}

    void SetCreateObjectPointer(void* (* pfunc)()) { static_create_function = pfunc;}

    /// Test HParent is my ancestor or not
    bool IsMyHierParent(const Class_Node& HParent) const ;

    /// destructor in which it's all children are deleted, remove me from my parents
    ~Class_Node() ;
protected: 

    /// find class node using DEVS_RT_TYPE class_rt
    Class_Node* FindChildNode(DEVS_RT_TYPE class_rt) const ; 

    /// Append child_node to me, make child_node have me as parents);
    void AppendChildNode(Class_Node* child_node) ; // 2002.07.19

    /// append hierarchical name to lst upto CurrLevel
    void GetHierName(const string& CurrLevel, list<string>& lst) const;

    /// Print all children to cout 
    void PrintAllChildren() const ;
    /// Print all children to strm 
    void PrintAllChildren(ostream& strm) const;
};//-------------------------------------------------------------------


typedef map<DEVS_RT_TYPE, Class_Node*> Ptr2ClassNodePtr;
typedef map<string,   Class_Node*> Str2ClassNodePtr;

//-------------------- A class managin the class of Class Node ------------------
///
class DEVSpp_EXP Class_Tree
{
	Class_Node        m_root; /* dummy root Class Node */
	Ptr2ClassNodePtr  m_map;  /* registered classes */
	Str2ClassNodePtr  m_strmap; 
public:    
    static Class_Tree Instance;
	/// constructor
	Class_Tree();
	/// destructor
	~Class_Tree() ;
	
	/*  Find Class_Node* whose {version, class_name} is equal to {pVersion, name}, respectively: 
	    return NULL if not found */
	Class_Node* FindNode(DEVS_RT_TYPE pVersion) const;
	Class_Node* FindNode(string name) const ;

	/* create cd_vrs ClassNode whose name is cd_name and append it to pr_vrs ClassNode (if exist) 
	   pr_vrs=NULL indicate the ROOT Node. The return-value is the newly-created cd_vrs node. */
	Class_Node* AppendClassToParentClass(	DEVS_RT_TYPE cd_vrs,
										string cd_name,
										DEVS_RT_TYPE pr_vrs=NULL);

	/// Print all children from the root to cout !
	void PrintAll() const { m_root.PrintAllChildren(); }

	/// Print all children from root to strm !
	void PrintAll(ostream& strm) const { m_root.PrintAllChildren(strm); }

	/// Get All instancible and kinds of B registered in 
	list<string> GetAllInstancibleKindsOf(const string& B) const;

	/// Test whether the A  Class is kind of the B Class or not
	bool  A_IsKindOf_B(DEVS_RT_TYPE A, DEVS_RT_TYPE B) const ;
	
	/// Test whether the A-named Class is kind of the B-named Class or not
	bool  A_IsKindOf_B(string A, string B) const ;

	///  Test whether the A-name Class is same as the B-named Class or not
	bool  A_IsSameOf_B(string A, string B) const { return FindNode(A)==FindNode(B) ; }
	 
	///  Test whether the A-name Class is same as the B-named Class or not
	bool  A_IsSameOf_B(DEVS_RT_TYPE A, DEVS_RT_TYPE B) const 
						{ return A==B; } // if they have same meory addresses
};//-------------------------------------------------------------------


//-------------- IMPLEMENT_COMMON(cls,cls_str, vers) --------------
#define IMPLEMENT_COMMON(cls,cls_str, vers) \
string cls::VClassName() const { return cls_str; } \
string cls::ClassName() { return cls_str; } \
unsigned int cls::m_sV = vers; \
bool cls::m_bRegistered = false; \
unsigned int cls::VVersion()   const {return m_sV; } \
bool cls::IsKindOf(DEVS_RT_TYPE pver) const \
{\
   if(m_bRegistered == false) cls::Register();\
   return Class_Tree::Instance.A_IsKindOf_B(&m_sV, pver);  \
}\
bool cls::IsKindOf(string classname) const \
{\
   if(m_bRegistered == false) cls::Register();\
   return Class_Tree::Instance.A_IsKindOf_B(cls_str, classname);  \
}\
bool cls::IsSameClass(DEVS_RT_TYPE pver) const \
{\
   if(m_bRegistered == false) cls::Register();\
   return Class_Tree::Instance.A_IsSameOf_B(&m_sV, pver);  \
}\
bool cls::IsSameClass(string classname) const \
{\
   if(m_bRegistered == false) cls::Register();\
   return Class_Tree::Instance.A_IsSameOf_B(cls_str, classname);  \
}\
//--------------------------------------------------------------------

#define IMPLEMENT_ABSTRACT(cls,cls_str, vers) \
    IMPLEMENT_COMMON(cls,cls_str, vers) \
    void* cls::create_object() { return NULL; } \


#define IMPLEMENT_CONCRETE(cls,cls_str, vers) \
    IMPLEMENT_COMMON(cls,cls_str, vers) \
    void* cls::create_object() { return new cls; } \


//------------------------ _IMPLMENT_CLASS0()------------------------
#define CLASS0_REGISTER(cls, cls_str, vers) \
 DEVS_RT_TYPE cls::Register() \
{ \
    if(!m_bRegistered ){ \
        m_bRegistered = true; \
		Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str); \
		Class_Tree::Instance.FindNode(&m_sV)->SetCreateObjectPointer(cls::create_object); \
	} \
	return &m_sV;\
}

#define IMPLEMENT_CLASS0(cls, cls_str, vers) \
    IMPLEMENT_CONCRETE(cls,cls_str, vers) \
    CLASS0_REGISTER(cls, cls_str, vers)

#define IMPLEMENT_ABSTRACT_CLASS0(cls, cls_str, vers) \
    IMPLEMENT_ABSTRACT(cls,cls_str, vers) \
    CLASS0_REGISTER(cls, cls_str, vers)
//----------------------------------------------------------------------

//----------------------- _ IMPLEMENT_CLASS1() -----------------------
#define CLASS1_REGISTER(cls, cls_str, vers, prn_cls) \
DEVS_RT_TYPE cls::Register() \
{\
    /* if parent is not register, register it. */\
    if(!prn_cls::m_bRegistered) \
        prn_cls::Register(); \
    /* if flag is not true (i.e. cls is not registered), register cls and make the flag true. */ \
    if(!m_bRegistered ){ \
        m_bRegistered = true; \
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls::m_sV));\
        Class_Tree::Instance.FindNode(&m_sV)->SetCreateObjectPointer(cls::create_object); \
    } \
    return &m_sV;\
}

#define IMPLEMENT_CLASS1(cls, cls_str, vers, prn_cls) \
    IMPLEMENT_CONCRETE(cls,cls_str, vers) \
    CLASS1_REGISTER(cls, cls_str, vers, prn_cls)

#define IMPLEMENT_ABSTRACT_CLASS1(cls, cls_str, vers, prn_cls) \
    IMPLEMENT_ABSTRACT(cls,cls_str, vers) \
    CLASS1_REGISTER(cls, cls_str, vers, prn_cls)
//-------------------------------------------------------------------

//----------------------- IMPLEMENT_CLASS2() -----------------------
#define CLASS2_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2) \
DEVS_RT_TYPE cls::Register() \
{\
    /* if some of parents are not register, register them. */\
    if(!prn_cls1::m_bRegistered) \
        prn_cls1::Register(); \
    if(!prn_cls2::m_bRegistered) \
        prn_cls2::Register(); \
    /* if flag is not true (i.e. cls is not registered), register cls and make the flag true. */ \
    if(!m_bRegistered ){ \
        m_bRegistered = true; \
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls1::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls2::m_sV));\
        Class_Tree::Instance.FindNode(&m_sV)->SetCreateObjectPointer(cls::create_object); \
    } \
    return &m_sV;\
}

#define IMPLEMENT_CLASS2(cls, cls_str, vers, prn_cls1, prn_cls2) \
    IMPLEMENT_CONCRETE(cls,cls_str, vers) \
    CLASS2_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2)

#define IMPLEMENT_ABSTRACT_CLASS2(cls, cls_str, vers, prn_cls1, prn_cls2) \
    IMPLEMENT_ABSTRACT(cls,cls_str, vers) \
    CLASS2_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2)
//-------------------------------------------------------------------

//----------------------- IMPLEMENT_CLASS3() -----------------------
#define CLASS3_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3) \
DEVS_RT_TYPE cls::Register() \
{\
    /* if some of parents are not register, register them. */\
    if(!prn_cls1::m_bRegistered) \
        prn_cls1::Register(); \
    if(!prn_cls2::m_bRegistered) \
        prn_cls2::Register(); \
    if(!prn_cls3::m_bRegistered) \
        prn_cls3::Register(); \
    /* if flag is not true (i.e. cls is not registered), register cls and make the flag true. */ \
    if(!m_bRegistered ){ \
        m_bRegistered = true; \
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls1::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls2::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls3::m_sV));\
        Class_Tree::Instance.FindNode(&m_sV)->SetCreateObjectPointer(cls::create_object); \
    } \
    return &m_sV;\
}\

#define IMPLEMENT_CLASS3(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3) \
    IMPLEMENT_CONCRETE(cls,cls_str, vers) \
    CLASS3_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3)


#define IMPLEMENT_ABSTRACT_CLASS3(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3) \
    IMPLEMENT_ABSTRACT(cls,cls_str, vers) \
    CLASS3_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3)
//-------------------------------------------------------------------

//----------------------- IMPLEMENT_CLASS4() -----------------------
#define CLASS4_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4) \
DEVS_RT_TYPE cls::Register() \
{\
    /* if some of parents are not register, register them. */\
    if(!prn_cls1::m_bRegistered) \
        prn_cls1::Register(); \
    if(!prn_cls2::m_bRegistered) \
        prn_cls2::Register(); \
    if(!prn_cls3::m_bRegistered) \
        prn_cls3::Register(); \
    if(!prn_cls4::m_bRegistered) \
        prn_cls4::Register(); \
    /* if flag is not true (i.e. cls is not registered), register cls and make the flag true. */ \
    if(!m_bRegistered ){ \
        m_bRegistered = true; \
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls1::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls2::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls3::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls4::m_sV));\
        Class_Tree::Instance.FindNode(&m_sV)->SetCreateObjectPointer(cls::create_object); \
    } \
    return &m_sV;\
}

#define IMPLEMENT_CLASS4(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4) \
    IMPLEMENT_CONCRETE(cls,cls_str, vers) \
    CLASS4_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4)

#define IMPLEMENT_ABSTRACT_CLASS4(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4) \
    IMPLEMENT_ABSTRACT(cls,cls_str, vers) \
    CLASS4_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4)
//-------------------------------------------------------------------

//----------------------- IMPLEMENT_CLASS5() -----------------------
#define CLASS5_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5) \
 DEVS_RT_TYPE cls::Register() \
{\
    /* if some of parents are not register, register them. */\
    if(!prn_cls1::m_bRegistered) \
	    prn_cls1::Register(); \
    if(!prn_cls2::m_bRegistered) \
	    prn_cls2::Register(); \
    if(!prn_cls3::m_bRegistered) \
	    prn_cls3::Register(); \
    if(!prn_cls4::m_bRegistered) \
	    prn_cls4::Register(); \
    if(!prn_cls5::m_bRegistered) \
	    prn_cls5::Register(); \
    /* if flag is not true (i.e. cls is not registered), register cls and make the flag true. */ \
    if(!m_bRegistered ){ \
        m_bRegistered = true; \
	    Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls1::m_sV));\
	    Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls2::m_sV));\
	    Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls3::m_sV));\
	    Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls4::m_sV));\
	    Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls5::m_sV));\
	    Class_Tree::Instance.FindNode(&m_sV)->SetCreateObjectPointer(cls::create_object); \
    } \
    return &m_sV;\
}

#define IMPLEMENT_CLASS5(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5) \
    IMPLEMENT_CONCRETE(cls,cls_str, vers) \
    CLASS5_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5) 


#define IMPLEMENT_ABSTRACT_CLASS5(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5) \
    IMPLEMENT_ABSTRACT(cls,cls_str, vers) \
    CLASS5_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5) 

//-------------------------------------------------------------------

//----------------------- IMPLEMENT_CLASS6() -----------------------
#define CLASS6_REGISTER(cls, cls_str, vers, \
	prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5, prn_cls6) \
DEVS_RT_TYPE cls::Register() \
{\
    /* if some of parents are not register, register them. */\
    if(!prn_cls1::m_bRegistered) \
        prn_cls1::Register(); \
    if(!prn_cls2::m_bRegistered) \
        prn_cls2::Register(); \
    if(!prn_cls3::m_bRegistered) \
        prn_cls3::Register(); \
    if(!prn_cls4::m_bRegistered) \
        prn_cls4::Register(); \
    if(!prn_cls5::m_bRegistered) \
        prn_cls5::Register(); \
    if(!prn_cls6::m_bRegistered) \
        prn_cls6::Register(); \
    /* if flag is not true (i.e. cls is not registered), register cls and make the flag true. */ \
    if(!m_bRegistered ){ \
        m_bRegistered = true; \
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls1::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls2::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls3::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls4::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls5::m_sV));\
        Class_Tree::Instance.AppendClassToParentClass(&m_sV, cls_str, &(prn_cls6::m_sV));\
        Class_Tree::Instance.FindNode(&m_sV)->SetCreateObjectPointer(cls::create_object); \
    } \
    return &m_sV;\
}

#define IMPLEMENT_CLASS6(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5, prn_cls6) \
    IMPLEMENT_CONCRETE(cls,cls_str, vers) \
    CLASS6_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5, prn_cls6) \


#define IMPLEMENT_ABSTRACT_CLASS6(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5, prn_cls6) \
    IMPLEMENT_ABSTRACT(cls,cls_str, vers) \
    CLASS6_REGISTER(cls, cls_str, vers, prn_cls1, prn_cls2, prn_cls3, prn_cls4, prn_cls5, prn_cls6) \

//-------------------------------------------------------------------
//---------------- xyREGISTER(cls) && DEVS_RT(cls) -------------------------
#define REGISTER(cls) cls::Register() 
#define DEVS_RT(cls) cls::Register() 
//----------------------------------------------------------------------------


DEVSpp_EXP Class_Tree& GetGClassTree() ;
DEVSpp_EXP void  PrintAllClassTree() ;
DEVSpp_EXP void  PrintAllClassTree(ostream& strm) ;
DEVSpp_EXP void* CreateObjectByClassName(const string& class_name) ;
DEVSpp_EXP bool	 IsAKindOfB(const string& A, const string& B);
DEVSpp_EXP list<string> GetAllInstancibleKindsOf(const string& ClassName);

} // end of namespace of DEVS 
