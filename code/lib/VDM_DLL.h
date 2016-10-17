/***
*  * WHAT
*  *    VDM library basic definitions
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/VDM_DLL.h,v $
*  * VERSION
*  *    $Revision: 1.4 $
*  * DATE
*  *    $Date: 1995/12/29 11:58:00 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * AUTHOR
*  *    $Author: erik $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

/*
***************************************************************************
*  This file contains the definitions used in the implementation
*  of the underlying VDM C++ library functions. 
*  There are defined two basic structures; double linked lists which 
*  are defined in the VDM_DLL class, and arrays which are defined in 
*  the VDM_AR class.
***************************************************************************
*/


#ifndef VDM_DLL_h_
#define VDM_DLL_h_

#include "basic.h"


/*
***************************************************************************
*  VDM_DLL_Node describes a node in the double linked lists
*  defined in VDM_DLL.
***************************************************************************
*/

struct VDM_DLL_Node {
  VDM_DLL_Node* prev_elm_ptr;
  VDM_DLL_Node* next_elm_ptr;
  Generic_p data_elm;
  Generic_p map_content;

  
  VDM_DLL_Node();
  VDM_DLL_Node(cGeneric_p g);
  VDM_DLL_Node(cGeneric_p g, cGeneric_p d);
  ~VDM_DLL_Node();
};



/*
***************************************************************************
*  The VDM_DLL class defines the common data and functions for
*  a double linked list.
***************************************************************************
*/

class VDM_DLL {
 protected:
   int list_length;        
   VDM_DLL_Node* last_node_ptr;
   void InsertBaseNode(cGeneric_p d, cGeneric_p c);
   void InsertNode(VDM_DLL_Node* ptr, cGeneric_p d, cGeneric_p c);
   void DeleteNode(VDM_DLL_Node*&);
   void free_data();

 public:
   VDM_DLL(const VDM_DLL &); 
   VDM_DLL();
   ~VDM_DLL();

   VDM_DLL& operator=(const VDM_DLL & list_parameter);
   int Compare(const VDM_DLL & list) const;
   int report_length_of_list() const;
   Generic_p first_elm(VDM_DLL_Node*&) const;
   Generic_p next_elm(VDM_DLL_Node*&) const;
};



/*
***************************************************************************
*  VDM_Ordered_set defines an ordered set, using double linked list.
*  In metaiv.cc both MapVal and SetVal are implemented using this class.
***************************************************************************
*/

class VDM_Ordered_Set : public VDM_DLL {
 public:
  VDM_Ordered_Set();
  ~VDM_Ordered_Set();

  int seek(cGeneric_p,VDM_DLL_Node*&) const;
  void Remove(cGeneric_p);
  Generic_p Last();
  void Union(const VDM_Ordered_Set & set);
  void InterSect(const VDM_Ordered_Set & set );
  void Difference(const VDM_Ordered_Set &set );
  int SubSet(const VDM_Ordered_Set &set );
  void Insert(cGeneric_p,cGeneric_p,int);
};



/*
***************************************************************************
*  VDM_DLL_List defines dynamic lists double linked lists.
*  In metaiv.cc SequenceVal is implemented using VDM_DLL_Lists
***************************************************************************
*/

class VDM_DLL_List : public VDM_DLL {
 public:
  VDM_DLL_List();
  ~VDM_DLL_List();

  void Append(cGeneric_p);
  void Prepend(cGeneric_p);
  Generic_p Index(int) const;
  Generic_p Hd() const;
  void Tl();
  void RemElem(int i); 
  void Conc(const VDM_DLL_List &);
};



#endif // VDM_DLL_h_
