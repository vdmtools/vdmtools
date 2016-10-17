/***
*  * WHAT
*  *    Simple implementation of an 'action list' used when two token lists 
*  *    are merged during automatic file update in the UML Tool.
*  *    
*  **  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/uml/code/actionlist.h,v $
*  * VERSION
*  *    $Revision: 1.1 $
*  * DATE
*  *    $Date: 1998/09/17 12:03:19 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox.
*  * AUTHOR
*  *    Ole Storm + $Author: storm $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/


#ifndef __action_list_h__
#define __action_list_h__

#include <vector>
#include <algorithm>            // sort()

enum ActionType {
  ACTION_INSERT,
  ACTION_SKIP
};


class Action
{
protected:
  ActionType action_kind;
  int tk_pos;
  int ins_from;
  int ins_or_skip_to;
public:
  ActionType kind() const {return action_kind;}

  // Constructor for creating Skip Actions
  Action(int skip_from, int skip_to) {
    tk_pos = skip_from;
    ins_or_skip_to = skip_to;
    ins_from = 0;
    action_kind = ACTION_SKIP;
  };

  // Constructor for creating Insert Actions
  Action(int ins_pos, int from, int to){
    tk_pos = ins_pos;
    ins_from = from;
    ins_or_skip_to = to;
    action_kind = ACTION_INSERT;
  };

  int token_pos() const {
    return tk_pos; 
  };
  int from() const {
    return ins_from;
  };
  int to() const {
    return ins_or_skip_to;
  };
};


class ActionList
{
  typedef vector<Action*> action_list_t;
  action_list_t action_list;
  
public:

  ActionList() {};

  ~ActionList() {
    clean();
  };

  // Compare function used to sort the entries in the action list. The
  // entries are sorted according to the position in the token list
  // they manipulate.
  static bool less_action(Action *x, Action *y)
  {
    if(x->token_pos() == y->token_pos())
      return x->from() < y->from();
    else
      return x->token_pos() < y->token_pos();
  }

  // Sort the action list according to the start_pos
  // attribute. I.e. the entries are sorted according to the
  // start-position of the interval of the token list they manipulate.
  void sort_list() {
    std::sort(action_list.begin(), action_list.end(), ActionList::less_action);
  }
  
  // Insert a new action.
  void insert(Action *a) {
    action_list.push_back(a);
  }

  // Get number of actions in list
  int length() const {
    return action_list.size();
  }

  
  // Get i'th action (zero-indexed)
  const Action * get(int i) const {
    if(i < (int)action_list.size())
      return action_list[i];  
    else 
      return NULL;
  }

  void clean() {
    // First delete the pointers then clear the vector. 
    for (unsigned int i = 0; i < action_list.size() ; i++ )
      if ( action_list[i] != NULL ) delete action_list[i];
    action_list.erase(action_list.begin(), action_list.end()); 
  };


};

#endif // __action_list_h__
