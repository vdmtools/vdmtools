/***
*  * WHAT
*  *    VDM library basic implementations
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/lib/VDM_DLL.cc,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2001/06/12 15:04:34 $
*  * STATUS
*  *    $State: Exp $
*  * PROJECT
*  *    Afrodite - ESPRIT III programme, project no. 6500.
*  * AUTHOR
*  *    $Author: paulm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

/*
***************************************************************************
*   
***************************************************************************
*/



#include "VDM_DLL.h"
#include "m4lib_errmsg.h"



/*
***************************************************************************
*  VDM_DLL_Node 
***************************************************************************
*/

VDM_DLL_Node::VDM_DLL_Node() {
  data_elm = NULL;
  map_content = NULL;
  next_elm_ptr = NULL;
  prev_elm_ptr = NULL;
}

VDM_DLL_Node::VDM_DLL_Node(cGeneric_p d_e) {
  data_elm = new Generic(*d_e);
  map_content = NULL;
  next_elm_ptr = NULL;
  prev_elm_ptr = NULL;
}

VDM_DLL_Node::VDM_DLL_Node(cGeneric_p d_e, cGeneric_p m_c) {
  data_elm = new Generic(*d_e);
  map_content = new Generic(*m_c);
  next_elm_ptr = NULL;
  prev_elm_ptr = NULL;
}

VDM_DLL_Node::~VDM_DLL_Node() {
  delete data_elm;
  delete map_content;
}




/**************************************************************************
*
*  VDM_DLL
*
*  All VDM_DLL member functions both works for lists which uses the
*  map_content pointer (MapVal) and those which does not.
**************************************************************************/




void VDM_DLL::InsertBaseNode(cGeneric_p d_e, cGeneric_p m_c) {
  if (m_c == NULL)
    last_node_ptr = new VDM_DLL_Node(d_e);
  else
    last_node_ptr = new VDM_DLL_Node(d_e,m_c);
  last_node_ptr->next_elm_ptr = last_node_ptr;
  last_node_ptr->prev_elm_ptr = last_node_ptr;
  list_length = 1;
}


/**************************************************************************
*  VDM_DLL::InsertNode inserts a node in the double linked list after 
*  the node pointed to by the parameter ptr, and updates the next and 
*  prev pointers.
**************************************************************************/

void VDM_DLL::InsertNode(VDM_DLL_Node* ptr, cGeneric_p d_e, cGeneric_p m_c) {
  ptr = ptr->next_elm_ptr;
  if (m_c == NULL)
    ptr->prev_elm_ptr->next_elm_ptr = new VDM_DLL_Node(d_e);
  else
    ptr->prev_elm_ptr->next_elm_ptr = new VDM_DLL_Node(d_e,m_c);
  ptr->prev_elm_ptr->next_elm_ptr->prev_elm_ptr = ptr->prev_elm_ptr;
  ptr->prev_elm_ptr = ptr->prev_elm_ptr->next_elm_ptr;
  ptr->prev_elm_ptr->next_elm_ptr = ptr;
  list_length++;
}


/**************************************************************************
*  VDM_DLL::DeleteNode deletes the node in the double linked list
*  pointed to by the parameter ptr, and updates the next and 
*  prev pointers. After the call ptr points to the next element
*  it is NULL, if the list is empty. DeleteNode also updates
*  the last_node_ptr pointer.
**************************************************************************/

void VDM_DLL::DeleteNode(VDM_DLL_Node*& ptr) {
  if (list_length == 1) {
    delete last_node_ptr;
    last_node_ptr = NULL;
    ptr = NULL;
  }
  else {
    if (last_node_ptr == ptr)
      last_node_ptr = ptr->prev_elm_ptr;
    ptr->next_elm_ptr->prev_elm_ptr = ptr->prev_elm_ptr;
    ptr = ptr->next_elm_ptr;
    delete ptr->prev_elm_ptr->next_elm_ptr;;
    ptr->prev_elm_ptr->next_elm_ptr = ptr;
  }
  list_length--;
}


/**************************************************************************
*  VDM_DLL::free_data deletes all elements in the list, and sets
*  last_node_ptr to NULL and list_length to 0.
**************************************************************************/

void VDM_DLL::free_data() {
  if (last_node_ptr == NULL) 
    return;
  VDM_DLL_Node* tmp1_node_ptr = last_node_ptr;
  VDM_DLL_Node* tmp2_node_ptr = tmp1_node_ptr->next_elm_ptr;
  for( int i = 2; i<= list_length; i++) {
    delete tmp1_node_ptr;
    tmp1_node_ptr = tmp2_node_ptr;
    tmp2_node_ptr = tmp2_node_ptr->next_elm_ptr;
  }
  delete tmp1_node_ptr;
  list_length = 0;
  last_node_ptr = NULL;
}

VDM_DLL::VDM_DLL(const VDM_DLL &dll) {
  last_node_ptr = NULL;
  list_length = 0;

  if (dll.last_node_ptr != NULL) {
    VDM_DLL_Node* traverse_node_ptr = dll.last_node_ptr;
    InsertBaseNode(traverse_node_ptr->data_elm,
                   traverse_node_ptr->map_content);
    VDM_DLL_Node* create_node_ptr = last_node_ptr;
    for (int i = 2; i<=dll.list_length; i++) {
      traverse_node_ptr = traverse_node_ptr->next_elm_ptr;
      InsertNode(create_node_ptr, traverse_node_ptr->data_elm,
                 traverse_node_ptr->map_content);
      create_node_ptr = create_node_ptr->next_elm_ptr;
    }
  }
}

VDM_DLL::VDM_DLL() {
 last_node_ptr = NULL;
 list_length = 0;
}

VDM_DLL::~VDM_DLL() { 
  free_data(); 
}


/**************************************************************************
*  Deletes its own double linked list and then copy the list
*  contained in dll.
**************************************************************************/

VDM_DLL& VDM_DLL::operator=(const VDM_DLL & dll) {
  free_data();
  if (dll.last_node_ptr != NULL) {
    VDM_DLL_Node* traverse_node_ptr = dll.last_node_ptr;
    InsertBaseNode(traverse_node_ptr->data_elm,
                   traverse_node_ptr->map_content);
    VDM_DLL_Node* create_node_ptr = last_node_ptr;
    for (int i = 2; i<=dll.list_length; i++) {
      traverse_node_ptr = traverse_node_ptr->next_elm_ptr;
      InsertNode(create_node_ptr, traverse_node_ptr->data_elm,
                 traverse_node_ptr->map_content);
      create_node_ptr = create_node_ptr->next_elm_ptr;
    }
  }
  return *this;
}


/**************************************************************************
*  VDM_DLL::Compare compares this with dll and returns :
*  -1 if list < dll
*   0 if list == dll
*   1 if list > dll
**************************************************************************/

int VDM_DLL::Compare(const VDM_DLL& dll) const {

  if (list_length < dll.list_length) return -1;
  if (list_length > dll.list_length) return 1;
  if (list_length ==0 && dll.list_length == 0) return 0;

  VDM_DLL_Node* traverse_dll_ptr = dll.last_node_ptr->next_elm_ptr;
  VDM_DLL_Node* traverse_me_ptr = last_node_ptr->next_elm_ptr;
  int tmp1,tmp2;
  for (int i = 1; i <= list_length; i++) {
    tmp1 = traverse_me_ptr->data_elm->Compare(*traverse_dll_ptr->data_elm);
    if (tmp1 != 0) return tmp1;
    if (traverse_me_ptr->map_content != NULL) {
      tmp2 = traverse_me_ptr->map_content->Compare(*traverse_dll_ptr->map_content);
      if (tmp2 != 0) return tmp2;
    }
    traverse_me_ptr = traverse_me_ptr->next_elm_ptr;
    traverse_dll_ptr = traverse_dll_ptr->next_elm_ptr;
  }
  return 0;
}


int VDM_DLL::report_length_of_list() const {
  return list_length;
}


/**************************************************************************
*  VDM_DLL::first_elm returns the value of data_elm for the first element 
*  (last_node_ptr->next_elm_ptr) in the list. traverse_node_ptr is
*  a L"var" parameter which points to the first element in the list
*  on return.
**************************************************************************/

Generic_p VDM_DLL::first_elm(VDM_DLL_Node*& traverse_node_ptr) const {
  if (last_node_ptr != NULL) {
    traverse_node_ptr = last_node_ptr->next_elm_ptr;
    return (*traverse_node_ptr).data_elm;
  }
  return NULL;
}


/**************************************************************************
*  VDM_DLL::next_elm returns the value of data_elm for the next element 
*  (traverse_node_ptr->next_elm_ptr) in the list. traverse_node_ptr is
*  a L"var" parameter which points to the next element in the list
*  on return.
**************************************************************************/

Generic_p VDM_DLL::next_elm(VDM_DLL_Node*& traverse_node_ptr) const {
  traverse_node_ptr = (*traverse_node_ptr).next_elm_ptr;
  if (traverse_node_ptr != last_node_ptr->next_elm_ptr) {
    return (*traverse_node_ptr).data_elm;
  }
  traverse_node_ptr = NULL;
  return NULL;
}

  


/**************************************************************************
*
*  VDM_Ordered_Set
*
**************************************************************************/


VDM_Ordered_Set::VDM_Ordered_Set() {}


VDM_Ordered_Set::~VDM_Ordered_Set() {}


/**************************************************************************
*  VDM_Ordered_Set::seek seeks for an element in the list which
*  data_elm points to a value equal to the value pointed to by
*  d_e. If it succeds it returns 0 and the L"var" parameter ptr 
*  points to the L"seeked" node in the list. If it fails and the "seeked" 
*  element is larger than any element in the list, it returns -1 and ptr 
*  points to last_node_ptr. Otherwise it returns 1, and ptr points to 
*  the first element in the list larger than the L"seeked". 
**************************************************************************/

int VDM_Ordered_Set::seek(cGeneric_p d_e,VDM_DLL_Node*& ptr ) const {
  if (list_length == 0) {
    ptr = NULL;
    return -1;
  }
  ptr = last_node_ptr;
  int cmp = -1;
  for(int i = 1; i <= list_length && cmp < 0; i++) {
    ptr = ptr->next_elm_ptr;
    cmp = (*ptr->data_elm).Compare(*d_e);
  }
  return cmp; 
}

  
/**************************************************************************
*  VDM_Ordered_Set::Remove removes the node which data_elm equals
*  d_e from the list.
**************************************************************************/

void VDM_Ordered_Set::Remove(cGeneric_p d_e) {
  if (list_length == 0)
    return;

  VDM_DLL_Node* tmp_node_ptr;
  if (seek(d_e,tmp_node_ptr) != 0)
    return;

  DeleteNode(tmp_node_ptr);
}
  


Generic_p VDM_Ordered_Set::Last() {
  if (last_node_ptr != NULL)
    return last_node_ptr->data_elm;
  else
    ReportError(ML_OP_ON_EMPTY_SET,L"VDM_Ordered_Set::Last");
    return NULL; /* AD */
}


/**************************************************************************
*  VDM_Ordered_Set::Union changes this to be the union of this 
*  and the parameter set.
**************************************************************************/

void VDM_Ordered_Set::Union(const VDM_Ordered_Set& set ) {
  if (set.list_length == 0) return;

  if (list_length == 0) {
    *this = set;
    return;
  }

  /************************************************************************
  *  Insert elements from set which are smaller than the
  *  smallest (first) element in the list.
  ************************************************************************/
  VDM_DLL_Node* trv_set_ptr = set.last_node_ptr->next_elm_ptr;
  VDM_DLL_Node* trv_me_ptr = last_node_ptr->next_elm_ptr;
  for(int set_index = 1; ((*trv_me_ptr->data_elm).Compare
      (*trv_set_ptr->data_elm) == 1) && (set_index <= set.list_length);
      set_index ++) {
    InsertNode(trv_me_ptr->prev_elm_ptr,trv_set_ptr->data_elm,
                                        trv_set_ptr->map_content);
    trv_set_ptr = trv_set_ptr->next_elm_ptr;
  }

  /************************************************************************
  *  Insert elements from set which are smaller than the
  *  largest (last) element in the list.
  ************************************************************************/
  int me_index = set_index;
  while((me_index < list_length) && (set_index <= set.list_length)) {
    if ((*trv_me_ptr->data_elm).Compare(*trv_set_ptr->data_elm) == 0) {
      set_index ++;
      trv_set_ptr = trv_set_ptr->next_elm_ptr;
    }
    else if ((*trv_me_ptr->next_elm_ptr->data_elm).Compare(*trv_set_ptr->
             data_elm) == 1) {
      InsertNode(trv_me_ptr,trv_set_ptr->data_elm,trv_set_ptr->map_content);
      set_index ++;
      me_index ++;
      trv_set_ptr = trv_set_ptr->next_elm_ptr;
      trv_me_ptr = trv_me_ptr->next_elm_ptr;
    }
    else {
      me_index ++;
      trv_me_ptr = trv_me_ptr->next_elm_ptr;
    }
  }

  /************************************************************************
  *  Insert elements from set which are larger than the
  *  largest (last) element in the list.
  ************************************************************************/
  if(me_index == list_length) {
    while (set_index <= set.list_length) {
      if ((*trv_me_ptr->data_elm).Compare(*trv_set_ptr->data_elm) == 0) {
        set_index ++;
        trv_set_ptr = trv_set_ptr->next_elm_ptr;
      }
      else {
        InsertNode(trv_me_ptr,trv_set_ptr->data_elm,trv_set_ptr->map_content);
        set_index ++;
        me_index ++;
        trv_set_ptr = trv_set_ptr->next_elm_ptr;
        trv_me_ptr = trv_me_ptr->next_elm_ptr;
      }
    }
    last_node_ptr = trv_me_ptr;    
  }
}


/**************************************************************************
*  VDM_Ordered_Set::Intersect changes this to be the intersection of this 
*  and the parameter set.
**************************************************************************/

void VDM_Ordered_Set::InterSect(const VDM_Ordered_Set& set ) {
  if (set.list_length == 0) {
    free_data();
    return;
  }

  if (list_length == 0) return;

  VDM_DLL_Node* trv_me_ptr = last_node_ptr->next_elm_ptr;
  VDM_DLL_Node* trv_set_ptr = set.last_node_ptr->next_elm_ptr;
  int me_index = 1;
  int set_index = 1;
  int cmp;
  while((me_index <= list_length) && (set_index <= set.list_length)){
    cmp = (*trv_me_ptr->data_elm).Compare(*trv_set_ptr->data_elm);
    if (cmp == -1) {
      DeleteNode(trv_me_ptr);
    }
    else if (cmp == 0) {
      set_index ++;
      me_index ++;
      trv_set_ptr = trv_set_ptr->next_elm_ptr;
      trv_me_ptr = trv_me_ptr->next_elm_ptr;
    }
    else {                   // cmp == 1
      set_index ++;
      trv_set_ptr = trv_set_ptr->next_elm_ptr;
    }
  }
  while (me_index <= list_length) {
    DeleteNode(trv_me_ptr);
  }
}


/**************************************************************************
*  VDM_Ordered_Set::Difference changes this to be the difference 
*  between this and the parameter set.
**************************************************************************/

void VDM_Ordered_Set::Difference(const VDM_Ordered_Set& set ) {
  if ((set.list_length == 0) || (list_length == 0)) return; 

  VDM_DLL_Node* trv_me_ptr = last_node_ptr->next_elm_ptr;
  VDM_DLL_Node* trv_set_ptr = set.last_node_ptr->next_elm_ptr;
  int me_index = 1;
  int set_index = 1;
  int cmp;
  while((me_index <= list_length) && (set_index <= set.list_length)){
    cmp = (*trv_me_ptr->data_elm).Compare(*trv_set_ptr->data_elm);
    if (cmp == -1) {
      me_index ++;
      trv_me_ptr = trv_me_ptr->next_elm_ptr;
    }
    else if (cmp == 0) {
      DeleteNode(trv_me_ptr);
      set_index ++;
      trv_set_ptr = trv_set_ptr->next_elm_ptr;
    }
    else {
      set_index ++;
      trv_set_ptr = trv_set_ptr->next_elm_ptr;
    }
  }
}


/**************************************************************************
*  VDM_Ordered_Set::SubSet returns 1 if this is a subset of set
*  and 0 otherwise.
**************************************************************************/

int VDM_Ordered_Set::SubSet(const VDM_Ordered_Set& set ) {
  if (list_length == 0) return 1; 

  if ((list_length > 0) && (set.list_length == 0)) return 0; 

  VDM_DLL_Node* trv_me_ptr = last_node_ptr->next_elm_ptr;
  VDM_DLL_Node* trv_set_ptr = set.last_node_ptr->next_elm_ptr;
  int me_index = 1;
  int set_index = 1;
  int cmp;
  while((me_index <= list_length) && (set_index <= set.list_length)){
    cmp = (*trv_me_ptr->data_elm).Compare(*trv_set_ptr->data_elm);
    if (cmp == -1) return 0;
    else if (cmp == 0) {
      me_index ++;
      trv_me_ptr = trv_me_ptr->next_elm_ptr;
      set_index ++;
      trv_set_ptr = trv_set_ptr->next_elm_ptr;
    }
    else {
      set_index ++;
      trv_set_ptr = trv_set_ptr->next_elm_ptr;
    }
  }
  if (me_index <= list_length) return 0;
  else return 1;
}


/**************************************************************************
*  VDM_Ordered_Set::Insert inserts the data_elm d_e and the map_content
*  m_c in the list. If override is true then the insertion is performed
*  even if a range conflict occurs.
**************************************************************************/

void VDM_Ordered_Set::Insert(cGeneric_p d_e, cGeneric_p m_c, int override) {

  if (list_length == 0) {
    InsertBaseNode(d_e,m_c);
    return;
  }

  VDM_DLL_Node* tmp_node_ptr;
  int seek_result = seek(d_e,tmp_node_ptr);
  if (seek_result == 0) {
    if (m_c != NULL)
      if ((*tmp_node_ptr->map_content).Compare(*m_c) != 0 ) {
        if (override) 
          *tmp_node_ptr->map_content = *m_c;
        else
          ReportError(ML_CONFLICTING_RNGVAL,L"VDM_Ordered_Set::Insert");
      }
    return;
  }

  if (seek_result == 1)
    InsertNode(tmp_node_ptr->prev_elm_ptr,d_e,m_c);
  else {
    InsertNode(tmp_node_ptr,d_e,m_c);
    last_node_ptr = last_node_ptr->next_elm_ptr;
  }
}




/**************************************************************************
*
*  VDM_DLL_List
*
**************************************************************************/


VDM_DLL_List::VDM_DLL_List() {}

VDM_DLL_List::~VDM_DLL_List() {}

void VDM_DLL_List::Append(cGeneric_p d_e) {
  if(list_length == 0)
    InsertBaseNode(d_e,NULL);
  else {
    InsertNode(last_node_ptr,d_e,NULL);
    last_node_ptr = last_node_ptr->next_elm_ptr;
  }
}

void VDM_DLL_List::Prepend(cGeneric_p d_e) {
  if(list_length == 0) 
    InsertBaseNode(d_e,NULL);
  else
    InsertNode(last_node_ptr,d_e,NULL);
}

Generic_p VDM_DLL_List::Index(int index) const {
  if(index < 1 ||  list_length < index) {
    ReportError(ML_INDEX_OUT_OF_RANGE,L"VDM_DLL_List::Index");
    return NULL; /* AD */
  }
  VDM_DLL_Node* tmp_node_ptr = last_node_ptr;
  for(int i = 1; i <= list_length - index; i++) 
  tmp_node_ptr = tmp_node_ptr->prev_elm_ptr;
  return tmp_node_ptr->data_elm;
}

   
Generic_p VDM_DLL_List::Hd() const {
  if(list_length == 0) {
    ReportError(ML_HD_ON_EMPTY_SEQUENCE,L"VDM_DLL_List::Hd");
    return NULL;
  }
  return last_node_ptr->next_elm_ptr->data_elm;
}

void VDM_DLL_List::RemElem(int i) {
  if(i<1 || i>list_length) 
    ReportError(ML_RANGE_ERROR,L"VDM_DLL_List::RemElem");
  else {
    VDM_DLL_Node* tmp = last_node_ptr->next_elm_ptr;
    for ( ; i>1; i--)
      tmp = tmp->next_elm_ptr;
    
    DeleteNode(tmp);
  }
}

void VDM_DLL_List::Tl() {
  if(list_length == 0) 
    ReportError(ML_TL_ON_EMPTY_SEQUENCE,L"VDM_DLL_List::ImpTl");
  else {
    VDM_DLL_Node* tmp = last_node_ptr->next_elm_ptr;
    DeleteNode(tmp);
  }
}

void VDM_DLL_List::Conc(const VDM_DLL_List& seq) {
  if(seq.list_length == 0) 
    return;

  if(list_length == 0) {
    *this = seq;
    return;
  }

  VDM_DLL_Node* traverse_node_ptr = seq.last_node_ptr->next_elm_ptr;
  for(int i = 1; i <= seq.list_length; i++) {
    InsertNode(last_node_ptr, traverse_node_ptr->data_elm, NULL);
    last_node_ptr = last_node_ptr->next_elm_ptr;
    traverse_node_ptr = traverse_node_ptr->next_elm_ptr;
  }
}

  


