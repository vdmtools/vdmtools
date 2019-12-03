/***
*  * WHAT
*  *    Handling of specification files in diferent
*  *    formats. Seen from the user of the SpecFile class
*  *    the format of the actual file will not be visible.
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/specfile/specfile.cc,v $
*  * VERSION
*  *    $Revision: 1.155 $
*  * DATE
*  *    $Date: 2007/03/24 02:26:38 $
*  * STATUS
*  *    $State: Exp $
*  * REFERENCES
*  *
*  * PROJECT
*  *
*  * AUTHOR
*  *    Lars T. Olsen + $Author: vdmtools $
*  * LOG
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <stdio.h>
#include <string.h>
#include <stringext.h>
#include <fstream>
#include <sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif // _MSC_VER

#include "specfile.h"
#include "tbutils.h"

#include "testcoverage.h"
#include "vdmpars.h"
#include "projectval.h"

//#include "Repository.h"
#include "ToolMediator.h"
#include "Interface.h"
#include "Errors.h"

#include "tools.h"
#include "settings.h"

#ifdef VDMPP
#include "ASTMERGE.h"
#include "intconvquotes.h"
#ifdef VICE
#include "parsrt.tab.h"
#else
#include "parspp.tab.h"
#endif //VICE
#endif // VDMPP

#include "tb_exceptions.h"
#include "tb_wstring.h"
#include "genlatex.h"
#include <sstream>

/////////////////////////
// SpecFileHandler
/////////////////////////

LatexSpecFileHandler SpecFileHandler::latex_sfh;
RTFWordSpecFileHandler SpecFileHandler::rtf_sfh;
PlainTextSpecFileHandler SpecFileHandler::pt_sfh;

SpecFileHandler::SpecFileHandler()
{
  this->format = FORMAT_UNKNOWN;
}

bool SpecFileHandler::parse_vdm_file( SpecFile & sf, const string & inp, Sequence & asts, ContextInfo &ci)
{
  // Init token list handling in vdmpars.l (scanner)
  // tokens.Clean(); -> is reset when contextinfo(fileid) is reset.
  // CALL PARSER
  bool parse_error = VDMPARSER::pars_vdm_string(inp,
                                                true,    // include position info
                                                ToolMediator::Errs(),  // pointer to errors
                                                asts,    // the resulting AST
                                                ci,      //
                                                true,    // Reset contextinfo
                                                TYPE_CI_FileId(sf.get_file_id()));

  // Adjust ContextInfo for Multibyte text  20051216
  ci.AdjustPosInfoForMultibyteText( sf.get_file_id() );

  return !parse_error;
};

SpecFileHandler::~SpecFileHandler()
{
}

SpecFileHandler * SpecFileHandler::get_parser(SpecFile & sf)
{
  if (latex_sfh.is_it_your_format(sf)) {
    return latex_sfh.dup();
  }
  else if (rtf_sfh.is_it_your_format(sf)) {
    return rtf_sfh.dup();
  }
  else if (pt_sfh.is_it_your_format(sf)) {
    return pt_sfh.dup();
  }
  else {
    return (SpecFileHandler *)NULL;
  }
}

#ifdef VDMPP
/////////////////////////
// Functions that create a list of action used to update the specfile
// with changes introduced in UML
/////////////////////////

TYPE_CI_TokenSpan SpecFileHandler::start_and_end(const Record & node, ContextInfo & c_table)
{
  TYPE_CI_ContextId ci (node.GetField(node.Length())); // The context info field
  return c_table.GetPos(ci);
}


Map SpecFileHandler::create_token_map(TokenList & tokl, int64_t from, int64_t to)
  // This function iterates through the token list and creates a map
  // from certain AS tag values to token positions. The map is used to
  // locate blocks like 'instance variables', 'values', etc. when new
  // constructs introduced in UML are to be inserted.
{
  Map m;
  for(size_t i = from; (i <= (size_t)to) && (i <= tokl.Length()); i++) {
    switch(tokl.Get(i).get_id()) {
      case LEX_INSTANCE: {
        if(!m.Dom().InSet(Int(TAG_TYPE_AS_InstAssignDef))
           && (i < tokl.Length())
           && (i < (size_t)to)
           && (tokl.Get(i+1).get_id() == LEX_VARIABLES)) {
          m.Insert(Int(TAG_TYPE_AS_InstAssignDef), Int(i+2));
        }
        break;
      }
      case LEX_VALUES: {
        if(!m.Dom().InSet(Int(TAG_TYPE_AS_ValueDef))) {
          m.Insert(Int(TAG_TYPE_AS_ValueDef), Int(i+1));
        }
        break;
      }
      case LEX_FUNCTIONS: {
        if(!m.Dom().InSet(Int(TAG_TYPE_AS_ExplFnDef))) {
          m.Insert(Int(TAG_TYPE_AS_ExplFnDef), Int(i+1));
          m.Insert(Int(TAG_TYPE_AS_ImplFnDef), Int(i+1));
          m.Insert(Int(TAG_TYPE_AS_ExtExplFnDef), Int(i+1));
        }
        break;
      }
      case LEX_OPERATIONS: {
        if(!m.Dom().InSet(Int(TAG_TYPE_AS_ExplOpDef))) {
          m.Insert(Int(TAG_TYPE_AS_ExplOpDef), Int(i+1));
          m.Insert(Int(TAG_TYPE_AS_ImplOpDef), Int(i+1));
          m.Insert(Int(TAG_TYPE_AS_ExtExplOpDef), Int(i+1));
        }
        break;
      }
      default:
        break;
    }
  }
  return m;
}


Action * SpecFileHandler::block_specifier_action(int ins_pos,
                                const TYPE_ASTMERGE_AST_uNode & ast_node,
                                ContextInfo & new_ci)
  // This function searches *backwards* in the token list of new_ci
  // from the start position of the node ast_node to the position of
  // the token(s) for the block specifier for ast_node. I.e. if
  // ast_node is a value the token list is searched (backvards) from
  // the position of ast_node until the token LEX_VALUES is found (it
  // _must_ be there!!!). Subsequently an action, 'cutting' the block
  // specifier from the token list, is created and returned.
{
  TokenList & tokl = new_ci.GetTokenList(VPPUML_DUMMY_FILE_ID);

  // Search from where:
  TYPE_CI_TokenSpan pos (start_and_end(ast_node, new_ci));
  int i = pos.get_token_ust().GetValue();

  bool found = false;
  while(i > 0 && !found) {
    switch(tokl.Get(i).get_id()) {
      case LEX_INSTANCE: {
        if(tokl.Get(i+1).get_id() == LEX_VARIABLES) {
          found = (ast_node.GetTag() == TAG_TYPE_AS_InstAssignDef);
        }
        break;
      }
      case LEX_VALUES: {
        found = (ast_node.GetTag() == TAG_TYPE_AS_ValueDef);
        break;
      }
      case LEX_FUNCTIONS: {
        found = (ast_node.GetTag() == TAG_TYPE_AS_ExplFnDef ||
                 ast_node.GetTag() == TAG_TYPE_AS_ImplFnDef ||
                 ast_node.GetTag() == TAG_TYPE_AS_ExtExplFnDef);
        break;
      }
      case LEX_OPERATIONS: {
        found = (ast_node.GetTag() == TAG_TYPE_AS_ExplOpDef ||
                 ast_node.GetTag() == TAG_TYPE_AS_ImplOpDef ||
                 ast_node.GetTag() == TAG_TYPE_AS_ExtExplOpDef);
        break;
      }
    }
    i--;
  }
  Action * act;
  if(!found) {
    // TBD: Report some error!!!!!!
    act = NULL;
  }
  else {
    if(tokl.Get(i+1).get_id() == LEX_INSTANCE) {
      // Insert two tokens:
      act = new Action(ins_pos, i+1, i+2);
    }
    else {
      // Insert only one token:
      act = new Action(ins_pos, i+1, i+1);
    }
  }
  return act;
}


int SpecFileHandler::get_replace_end_pos(const TYPE_ASTMERGE_AST_uNode & ast_node,
                                         ContextInfo & ci,
                                         TokenList & tokl)
  // Returns the end position for a Replace Action on the node
  // ast_node. For functions, operations and classes the end position
  // (for replace actions) is not similar to the end position stored
  // in the AST-node. Reason: the body of these constructs must be
  // preserved.
{
  TYPE_CI_TokenSpan pos (start_and_end(ast_node, ci));
  switch(ast_node.GetTag()) {
    case TAG_TYPE_AS_InstAssignDef:
    case TAG_TYPE_AS_ValueDef: {
      int64_t endtoken = pos.get_token_uend().GetValue();
      // Take the semi-colon as well - if it exists.
      if(endtoken < (int64_t)tokl.Length() &&
         tokl.Get(endtoken + 1).get_id() == ';') {
        return endtoken + 1;
      }
      else {
        return endtoken;
      }
    }
    // functions:
    case TAG_TYPE_AS_ExplFnDef: {
      TYPE_AS_ExplFnDef temp(ast_node);
      TYPE_CI_TokenSpan pos (start_and_end(temp.get_body(), ci));
  //    return pos.starttoken-1;
      return pos.get_token_ust().GetValue() - 1;
    }
    case TAG_TYPE_AS_ImplFnDef: {
      TYPE_AS_ImplFnDef temp(ast_node);
      if(temp.get_fnpre().IsNil()) {
        // No pre condition specified - use start of post condition
        pos = start_and_end(temp.get_fnpost(), ci);
      }
      else {
        pos = start_and_end(temp.get_fnpre(), ci);
      }
      return pos.get_token_ust().GetValue() - 2;
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      TYPE_AS_ExtExplFnDef temp(ast_node);
      pos = start_and_end(temp.get_body(), ci);
      return pos.get_token_ust().GetValue() - 2;
    }
  
    // operations
    case TAG_TYPE_AS_ExplOpDef: {
      TYPE_AS_ExplOpDef temp(ast_node);
      pos = start_and_end(temp.get_body(), ci);
      return pos.get_token_ust().GetValue() - 1;
    }
    case TAG_TYPE_AS_ImplOpDef: {
       TYPE_AS_ImplOpDef temp(ast_node);
       // If implicit operation, post condition must be
       // present, so replace up to end of current post cond
       if (!temp.get_oppre().IsNil()) {
         pos = start_and_end(temp.get_oppre(), ci);
       }
       else {
         pos = start_and_end(temp.get_oppost(), ci);
       }
      return pos.get_token_ust().GetValue() - 2;
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      TYPE_AS_ExtExplOpDef temp(ast_node);
      pos = start_and_end(temp.get_body(), ci);
      return pos.get_token_ust().GetValue() - 2;
    }
    case TAG_TYPE_ASTMERGE_Condition: {
      TYPE_ASTMERGE_Condition cond(ast_node);
      TYPE_AS_Expr temp(cond.get_body());
      pos = start_and_end(temp, ci);
      return pos.get_token_uend();
    }
    case TAG_TYPE_AS_Class: {
      // A replace action on a class means that the inheritance
      // relations have changed:
      TYPE_AS_Class temp(ast_node);
      type_7AS_NameCL super_l(temp.get_supercls());
      if(!super_l.Length()) {
        // No super classes specified for this class. Return the end pos
        // of the class name:
        TYPE_AS_Name cl_nm(temp.get_nm());
        pos = start_and_end(cl_nm, ci);
        return pos.get_token_uend();
      }
      else {
        // There are super classes. Return the end position of the last
        // name of the list
        TYPE_AS_Name last_nm = super_l.Index(super_l.Length());
        pos = start_and_end(last_nm, ci);
        return pos.get_token_uend();
      }
    }
  }
  return 0; // dummy return;
}

Int SpecFileHandler::get_acc (const TYPE_ASTMERGE_AST_uNode& node)
{
  switch (node.GetTag()) {
    case TAG_TYPE_AS_ValueDef:      { return node.GetInt(pos_AS_ValueDef_access); }
    case TAG_TYPE_AS_InstAssignDef: { return node.GetInt(pos_AS_InstAssignDef_access); }
    case TAG_TYPE_AS_ExplFnDef:     { return node.GetInt(pos_AS_ExplFnDef_access); }
    case TAG_TYPE_AS_ImplFnDef:     { return node.GetInt(pos_AS_ImplFnDef_access); }
    case TAG_TYPE_AS_ExtExplFnDef:  { return node.GetInt(pos_AS_ExtExplFnDef_access); }
    case TAG_TYPE_AS_ExplOpDef:     { return node.GetInt(pos_AS_ExplOpDef_access); }
    case TAG_TYPE_AS_ImplOpDef:     { return node.GetInt(pos_AS_ImplOpDef_access); }
    case TAG_TYPE_AS_ExtExplOpDef:  { return node.GetInt(pos_AS_ExtExplOpDef_access); }
    default:                        { return Int(DEFAULT_AS); }
  }
}

Bool SpecFileHandler::get_stat (const TYPE_ASTMERGE_AST_uNode& node)
{
  switch (node.GetTag()) {
    case TAG_TYPE_AS_ValueDef:      { return node.GetBool(pos_AS_ValueDef_stat); }
    case TAG_TYPE_AS_InstAssignDef: { return node.GetBool(pos_AS_InstAssignDef_stat); }
    case TAG_TYPE_AS_ExplFnDef:     { return node.GetBool(pos_AS_ExplFnDef_stat); }
    case TAG_TYPE_AS_ImplFnDef:     { return node.GetBool(pos_AS_ImplFnDef_stat); }
    case TAG_TYPE_AS_ExtExplFnDef:  { return node.GetBool(pos_AS_ExtExplFnDef_stat); }
    case TAG_TYPE_AS_ExplOpDef:     { return node.GetBool(pos_AS_ExplOpDef_stat); }
    case TAG_TYPE_AS_ImplOpDef:     { return node.GetBool(pos_AS_ImplOpDef_stat); }
    case TAG_TYPE_AS_ExtExplOpDef:  { return node.GetBool(pos_AS_ExtExplOpDef_stat); }
    default:                        { return Bool(false); }
  }
}

int SpecFileHandler::get_increment(const TYPE_ASTMERGE_AST_uNode& node)
{
  int increment = 0;
  if (get_acc(node).GetValue() != DEFAULT_AS) {
    increment++;
  }
  if (get_stat(node).GetValue()) {
    increment++;
  }
  return increment;
}

// Function for returning precondition from an arbitrary parent method
Generic SpecFileHandler::get_parent_pre (TYPE_ASTMERGE_AST_uNode parent)
{
  //Generic precond;
  switch (parent.GetTag()) {
    case vdm_AS_ImplFnDef:{
      //TYPE_AS_ImplFnDef fn(parent);
      //precond = fn.get_fnpre();
      return Record(parent).GetField(pos_AS_ImplFnDef_fnpre);
      //break;
    }
    case vdm_AS_ExplFnDef:{
      //TYPE_AS_ExplFnDef fn(parent);
      //precond = fn.get_fnpre();
      return Record(parent).GetField(pos_AS_ExplFnDef_fnpre);
      //break;
    }
    case vdm_AS_ExtExplFnDef:{
      //TYPE_AS_ExtExplFnDef fn(parent);
      //precond = fn.get_fnpre();
      return Record(parent).GetField(pos_AS_ExtExplFnDef_fnpre);
      //break;
    }
    case vdm_AS_ImplOpDef:{
      //TYPE_AS_ImplOpDef op(parent);
      //precond = op.get_oppre();
      return Record(parent).GetField(pos_AS_ImplOpDef_oppre);
      //break;
    }
    case vdm_AS_ExplOpDef:{
      //TYPE_AS_ExplOpDef op(parent);
      //precond = op.get_oppre();
      return Record(parent).GetField(pos_AS_ExtExplOpDef_oppre);
      //break;
    }
    case vdm_AS_ExtExplOpDef:{
      //TYPE_AS_ExtExplOpDef op(parent);
      //precond = op.get_oppre();
      return Record(parent).GetField(pos_AS_ExtExplOpDef_oppre);
      //break;
    }
  }
  //return precond;
  return Nil();
}

bool SpecFileHandler::extract_token_pos(ActionList & act_l,
                                        TYPE_ASTMERGE_ActionList & acts,
                                        ContextInfo & new_ci,
                                        ContextInfo & old_ci,
                                        TokenList & old_tokl)
  // This function traverses the set of actions, acts, (created from
  // the code generated specification) and creates a new list of
  // actions of type ActionList. Each entry in this new list of
  // actions has detailed information of what portions of the old
  // token-list to skip and what portions of the new token list to
  // insert.
{
  Generic g;
  Map block_positions;  // This is a map from Class names to
                        // mapings created by create_token_map():
                        // map AS`Name to map nat to nat

  Map blocks_created; // A map from class names to sets of tag values.
                      // Used to keep track of when to create a new block
                      // specifier like 'instance variables', 'values', etc.

  TYPE_CI_TokenSpan pos;
  TYPE_ASTMERGE_AST_uNode ast_node, where_node, old_node;

  for(bool bb = acts.First(g); bb; bb = acts.Next(g)) {
    TYPE_ASTMERGE_Action act(g);
    switch(act.GetTag()) {
      case vdm_ASTMERGE_InsertAction: {
        int ins_pos;
        TYPE_ASTMERGE_InsertAction temp (act);
        ast_node = temp.get_ast_unode();
        pos = start_and_end(ast_node, new_ci);

        // We treat the case of a Condition node differently, as it is
        // not a first class definition (c.f. operations, functions etc)
        if( ast_node.GetTag() == vdm_ASTMERGE_Condition) {
          TYPE_ASTMERGE_Condition cond(ast_node);
          TYPE_ASTMERGE_AST_uNode parent(cond.get_parent());

          // Decide whether start pos should be after body or after precond
          TYPE_CI_TokenSpan start_pos;
          if ((cond.get_kind() == Quote(L"pre")) || (get_parent_pre(parent).IsNil())) {
            start_pos = start_and_end(parent, old_ci);
          }
          else {
            start_pos = start_and_end(get_parent_pre(parent), old_ci);
          }

          // This condition will be inserted after the last token in start_pos
          ins_pos = start_pos.get_token_uend().GetValue() + 1;

          // Add action to insert condition following ins_pos
          // Note the use of pos.starttoken - 1 here to get the "pre" or "post"
          act_l.insert(new Action(ins_pos, pos.get_token_ust().GetValue() - 1, pos.get_token_uend().GetValue()));

          break;
        }

        if( temp.get_where().IsNil() ) {
          // This is a new class
          ins_pos = 1;
          // Create an insert action
          act_l.insert(new Action(ins_pos, 1, pos.get_token_uend().GetValue()));
          // We use 1 as 'from' in stead of pos.starttoken because we
          // want to include the initial comments.
        }
        else {
          where_node = temp.get_where(); // [AS`Name]
          // Find insertion pos for this new entity:
          // First we find the start- and end-pos of the class it self:
          TYPE_CI_TokenSpan class_pos;
          Generic as_g (ToolMediator::GetAST(PTAUX::ASName2ModuleName(where_node)));
          if(!as_g.IsNil()) {
            class_pos = start_and_end(PTAUX::ExtractAstVal(as_g), old_ci);
          }
          else {
            return false;
          }
          Map local_m;
          if(!block_positions.Dom().InSet(where_node)) {
            // The class name in where_node is not in the map. Create
            // and insert it. The start- and end-pos of the class is
            // used to limit the search to only a part of the token
            // list:
            block_positions.Insert(where_node,
                                   local_m = create_token_map(old_tokl,
                                               class_pos.get_token_ust().GetValue(),
                                               class_pos.get_token_uend().GetValue()));
          }
          else {
            local_m = block_positions[where_node];
          }
          // The map local_m is now a mapping from AS tag values to
          // token positions. The map is used to locate existing blocks
          // like 'instance variables', 'values', etc. in the _old_
          // token list.
          if(local_m.Dom().InSet(Int(ast_node.GetTag())) ) {
            // The class already has a block for the new entity:
            ins_pos = Int(local_m[Int(ast_node.GetTag())]).GetValue();
          }
          else {
            // No block exists. The insertion position is then 'right
            // after' the class header
            TYPE_CI_TokenSpan class_nm_pos (start_and_end(where_node, old_ci));

            // If there is an inheritance clause, the insertion point is
            // after this, otherwise, the insertion point is after the
            // class name
            TYPE_AS_Class as_class (PTAUX::ExtractAstVal(as_g));
            if (as_class.get_supercls().IsEmpty()) {
              ins_pos = class_nm_pos.get_token_uend().GetValue() + 1;
            }
            else {
              Sequence as_superclasses = as_class.get_supercls();
              TYPE_CI_TokenSpan last_supercls_pos (start_and_end(
                            (Record) (as_superclasses[as_superclasses.Length()]),
                            old_ci));
              ins_pos = last_supercls_pos.get_token_uend().GetValue() + 1;
            }


            // Next, check if we should insert a block specifier like
            // 'instance varaibles', 'functions' etc. This should only
            // be inserted once, which is why we keep a mapping from
            // class names to sets of tag values:
            Set block_set;
            if(blocks_created.Dom().InSet(where_node)) {
              block_set = blocks_created[where_node];
            }
            else {
              block_set = Set();
            }
            if( !block_set.InSet(Int(ast_node.GetTag())) ) {
              // No block specifier inserted yet. Insert it:
              act_l.insert(block_specifier_action(ins_pos, ast_node, new_ci));
            }
            block_set.Insert(Int(ast_node.GetTag()));
            // Insert the new block set in the mapping:
            blocks_created.ImpModify(where_node, block_set);
          }
          // Create an insert action
          act_l.insert(new Action(ins_pos,
//                                  pos.get_token_ust().GetValue() - get_increment(ast_node),
                                  pos.get_token_ust().GetValue(),
                                  pos.get_token_uend().GetValue() + 1));

          // pos.endtoken+1 because we wish to get the ';' as
          // well. Since this is an _insert_ action we know that the
          // semi-colon exists.
        }
        break;
      }
      case vdm_ASTMERGE_SkipAction: {
        TYPE_ASTMERGE_SkipAction temp(act);
        pos = start_and_end(temp.get_ast_unode(), old_ci);
        // Create a skip action

        int startpos, endpos;

        // Treat Condition actions separately as they are not first class definitions

        if (temp.get_ast_unode().Is(vdm_ASTMERGE_Condition)) {
          startpos = pos.get_token_ust().GetValue() - 1;
        }
        else {
          startpos = pos.get_token_ust().GetValue() - get_increment(temp.get_ast_unode());
        }
        int64_t endtoken = pos.get_token_uend().GetValue();
        if(endtoken < (int64_t)old_tokl.Length() &&
           old_tokl.Get(endtoken + 1).get_id() == ';') {
          // Ok, this ugly, but how the hell do I remove semicolons in another manner?
          endpos = endtoken + 1;
        }
        else {
          endpos = endtoken;
        }
        act_l.insert(new Action(startpos, endpos));
        break;
      }
      case vdm_ASTMERGE_ReplaceAction:{
        // Create and insert two actions (skip and insert) that do the
        // replace:
        TYPE_ASTMERGE_ReplaceAction temp(act);
        ast_node = temp.get_new_unode();
        old_node = temp.get_old_unode();
        // Get positions:
        TYPE_CI_TokenSpan old_pos (start_and_end(old_node, old_ci));
        pos = start_and_end(ast_node, new_ci);
        // Skip action:
        int old_start;
        old_start = old_pos.get_token_ust().GetValue() - get_increment(old_node);

        act_l.insert(new Action(old_start, get_replace_end_pos(old_node, old_ci, old_tokl)));
                                                     // skip to
        // Insert action
        int end_pos = get_replace_end_pos(ast_node, new_ci, new_ci.GetTokenList(VPPUML_DUMMY_FILE_ID));
        act_l.insert(new Action(old_start, pos.get_token_ust().GetValue() - get_increment(ast_node), end_pos));
        break;                                      // to
      }
      case vdm_ASTMERGE_UnknownAction: {
        return false;
      }
      default: {
        return false;
      }
    }
  }
  return true;
}

bool SpecFileHandler::create_action_list(SpecFile & sf, const Sequence & old_asts, wstring & err)
{
  Sequence new_asts;

  // Get the token list for the file
  ContextInfo & old_ci = sf.get_contextinfo();
  old_ci.UseContextTab(sf.get_file_id(), false); // 20080804
  TokenList & old_tokl = old_ci.GetTokenList(sf.get_file_id());

  // Next we construct asts and context info for the classes of this module:

  if( !parse_classes(sf.get_merged_classes(), new_ci, new_asts, VPPUML_DUMMY_FILE_ID) ) {
    err = L"Merged model is syntactically incorrect.";
    return false;
  }

  TYPE_ASTMERGE_ActionList acts =
    INT2Q::g2hAS(vdm_ASTMERGE_CreateActionLists(INT2Q::h2gAS(new_asts), INT2Q::h2gAS(old_asts)));
  // The seqence of actions, acts, is now traversed and a 'real'
  // action list, act_l, is generated from token position information of the
  // AST nodes of the sequence.

  if(!extract_token_pos(act_l, acts, new_ci, old_ci, old_tokl)) {
    return false;
  }
  // Sort the action list according to the token position in the original token list:
  act_l.sort_list();
  return true;
}

// Parses the VDM++ class(es) in s and returns the contextinfo.
bool SpecFileHandler::parse_classes(const wstring & ws, ContextInfo & ci, Sequence & asts, int dummy_fid)
{
  string s (TBWSTR::ConvertToHexquad(ws));

//  VDMPARSER::orderly.Clear();
//  VDMPARSER::modorder.Clear();

  ToolMediator::Errs()->vdm_ClearAll();

  bool err = VDMPARSER::pars_vdm_string (s,
                                         true, // include position info
                                         ToolMediator::Errs(), // pointer to errors
                                         asts, // the resulting AST
                                         ci,
                                         true, // Reset contextinfo
                                         TYPE_CI_FileId(dummy_fid));

  ToolMediator::Errs()->vdm_AllDone();
  return !err;
}
#endif // VDMPP

//////////////////////////
// Plain Text SpecFile Parser
//////////////////////////

PlainTextSpecFileHandler::PlainTextSpecFileHandler()
{
  this->format = FORMAT_PLAIN_TEXT;
}

SpecFileHandler * PlainTextSpecFileHandler::dup()
{
  SpecFileHandler * s = new PlainTextSpecFileHandler();
  return s;
}

bool PlainTextSpecFileHandler::parse_vdm(SpecFile & sf, Sequence & asts, ContextInfo &ci)
{
  sf.rewind();

  ostringstream unichar;
  if(!sf.convertToUniChar(sf.get_input_stream(), unichar)) {
    return false;
  }
  return parse_vdm_file(sf, unichar.str(), asts, ci);
}

bool PlainTextSpecFileHandler::pp(SpecFile & sf, Sequence &asts,ContextInfo &ci)
{
  // Files in use
  wstring output_file (sf.get_pp_output_name());     // latex output file

  //////////////////
  /// Get the asts in the specification manager corresponding to
  /// the specfile
  //////////////

  Set sp_asts (ToolMediator::GetAstsOfFileName(sf.get_filename()));

  // Get hold of Test coverage info output to file <fname>.tmp3 - if any


  ///////////////////////
  // Extract the names of the defined functions from the spec
  // Used to generate index.
  ///////////////////////

#ifdef VDMSL
  Sequence mds (ExtractDefinedFunctionNames (sp_asts)); // SEQ OF SET OF (OP|FUN)
#else
  Sequence mds;  // ??? FOR VDMPP
#endif

  ///////////////////////
  // Generate Latex
  ///////////////////////

  try {
    GenLatexOutput lo(output_file,
                      sp_asts,              // the modules/definitions
                      Settings.GetIndex(),  // should an index be generated
                      mds,                  // list of set of (ops|fcts)
                      sf.get_fname(),       // the name of the file to pp
                      ci)          ;        // the contextinfo
  }
  catch (TB_Exception &e) {
    vdm_err << L"Internal Error" << endl << flush;

    // remove temporary files
    ToolMediator::UpdatePP (sf.get_filename(), Bool (false)); // cast ???
    return false;
  }
  return true;
}

#ifdef VDMPP
bool PlainTextSpecFileHandler::file_update(SpecFile &sf, wstring & err)
{
  TBUTILS::remove_file(sf.give_old_name());  // Remove old copy.
  TBUTILS::file_copy(sf.give_old_name(), sf.get_fname());

  ofstream pp_out;
  pp_out.open(TBWSTR::wstring2fsstr(sf.get_fname()).c_str(),ios::out);
  if (!pp_out.good()) {
    err = L"Could not open the file for writing.";
    return false;
  }
  pp_out << TBWSTR::wstring2mbstr(sf.get_merged_classes());
  pp_out.close();
  return true;
}

bool PlainTextSpecFileHandler::file_create(SpecFile &sf, wstring & err)
{
  ofstream pp_out;
  pp_out.open(TBWSTR::wstring2fsstr(sf.get_fname()).c_str(),ios::out);
  if (!pp_out.good()) {
    err = L"Could not open the file for writing.";
    return false;
  }
  pp_out << TBWSTR::wstring2mbstr(sf.get_merged_classes());
  pp_out.close();
  return true;
}

wstring PlainTextSpecFileHandler::give_old_name(SpecFile &sf)
{
  return sf.get_fname() + L"_old.vpp";
}
#endif // VDMPP

bool PlainTextSpecFileHandler::is_it_your_format(SpecFile & )
{
  return true; // Tries to accept any thing
}

//////////////////////////
// SPECFILE
//////////////////////////

bool SpecFile::get_parser()
{
  if (this->parser == NULL) {
    SpecFileHandler sfp;
    this->parser = sfp.get_parser(*this);
  }
  return (this->parser != NULL);
}

ifstream & SpecFile::get_input_stream()
{
  rewind();
  return this->sp;
}

SpecFile::SpecFile()
{
  this->short_name = L"";
  this->file_id   = 0;
  this->parser   = NULL;
  this->contextinfo = NULL;
#ifdef VDMPP
  this->file_created = false;
  this->file_success = false;
  this->merged_classes = L"";
#endif // VDMPP
}

#ifdef VDMPP
SpecFile::SpecFile(const wstring & path, const wstring & nm, const wstring & df, const wstring & cs, ContextInfo& ci)
{
  // This is a special constructor used when creating new files from
  // classes in introduced in UML
  this->short_name = nm;
  this->fname = TBUTILS::tb_JoinPaths(path,nm);
  this->filename = PTAUX::mk_FileName (fname);
  this->default_file = df;
  this->charset = cs;

  if(nm.rfind(L".rtf") != wstring::npos) {
    // This is an rtf file:
    // Copy the file L"NewClass.rtf" to the file indicated by path and nm.
    FILE *out;
    if( !this->default_file.empty() &&
        (out = fopen(TBWSTR::wstring2fsstr(this->default_file).c_str(), "r")) != NULL ) {
      fclose(out);
      this->file_success = TBUTILS::file_copy(this->fname, this->default_file);
    }
    else {
      vdm_log << L"Could't open file '" << this->default_file << L"'." << endl << flush;
      if( (out = fopen(TBWSTR::wstring2fsstr(this->fname).c_str(), "r")) != NULL ) {
        fclose(out);
        this->file_success = false;
      }
      else {
        ofstream o;
        o.open(TBWSTR::wstring2fsstr(this->fname).c_str(), ios::out);
        if (o.good()) {
          rtfnewclass(o);
          o.close();
          this->file_success = true;
        }
        else {
          this->file_success = false;
        }
      }
    }
  }
  else if (nm.rfind(L".tex") != wstring::npos) {
    // Tex text is assumed. Create a dummy file so that
    // Search_and_OpenFile will be able to find this file.
    FILE *out;
    // Make sure an existing file is not overridden:
    if( (out = fopen(TBWSTR::wstring2fsstr(this->fname).c_str(), "r")) != NULL ) {
      fclose(out);
      this->file_success = false;
    }
    else {
      out = fopen(TBWSTR::wstring2fsstr(this->fname).c_str(), "w");
      fclose(out);
      this->file_success = true;
    }
  }
  else {
    // Plain text is assumed. Create a dummy file so that
    // Search_and_OpenFile will be able to find this file.
    FILE *out;
    // Make sure an existing file is not overridden:
    if( (out = fopen(TBWSTR::wstring2fsstr(this->fname).c_str(), "r")) != NULL ) {
      fclose(out);
      this->file_success = false;
    }
    else {
      out = fopen(TBWSTR::wstring2fsstr(this->fname).c_str(), "w");
      fclose(out);
      this->file_success = true;
    }
  }
  this->file_created = true;
  this->file_id = 0;
  this->parser = NULL;
  this->contextinfo = &ci;
};
#endif // VDMPP

SpecFile::SpecFile(const TYPE_ProjectTypes_FileName & fn, ContextInfo& ci)
{
  this->filename = fn;
  this->fname = PTAUX::ExtractFileName(fn);
  this->short_name = TBUTILS::tb_getbasename(fname).GetString();
  this->file_id = 0;
  this->parser = NULL;
  this->contextinfo = &ci;
#ifdef VDMPP
  this->file_created = false;
#endif // VDMPP
};

SpecFile::~SpecFile()
{
  if (this->parser != NULL) {
    delete this->parser;
  }
  close();
};

bool SpecFile::read()
{
  if (this->parser != NULL) {
    return this->parser->read(*this);
  }
  else {
    return false;
  }
};

// parse the vdm part of a spec file
bool SpecFile::parse_vdm(Sequence & asts)
{
  if (this->parser != NULL) {
    return (this->parser->read(*this) &&
            this->parser->split(*this, true) &&
            this->parser->parse_vdm(*this, asts, *(this->contextinfo)));
  }
  else {
    return false;
  }
};

// pretty print the spec file to pf
bool SpecFile::pp()
{
  if (this->parser != NULL) {
    Sequence asts;
    ContextInfo ci;
    return (this->parser->read(*this) &&
            this->parser->split(*this) &&
            this->parser->parse_vdm(*this, asts, ci) &&
            this->parser->pp(*this, asts, ci));
  }
  else {
    return false;
  }
};

#ifdef VDMPP
// update the specfile according to the action-list al
bool SpecFile::file_update(wstring & err, Sequence & asts)
{
  return (this->parser &&
          this->parser->read(*this) &&
          this->parser->split(*this) &&
          this->parser->create_action_list(*this, asts, err) &&
          this->parser->file_update(*this, err));
}

bool SpecFile::file_create(wstring & err)
{
  Sequence seq;
  return (this->parser &&
          this->parser->read(*this) &&
          this->parser->split(*this) &&
          this->parser->create_action_list(*this, seq, err) &&
          this->parser->set_default_font(*this) &&
          this->parser->file_create(*this, err));
}

wstring SpecFile::give_old_name()
{
  if(this->parser != NULL) {
    return this->parser->give_old_name(*this);
  }
  else {
    return L"";
  }
}
#endif // VDMPP

// split the spec file

bool SpecFile::split(bool vdm_only)
{
  return (this->parser &&
          this->parser->read(*this) &&
          this->parser->split(*this,vdm_only));
};

const wchar_t * SpecFile::pp_output_file_suffix()
{
  if ( this->parser == NULL ) {
    return L"unknown";
  }
  else {
    return this->parser->pp_output_file_suffix();
  }
};

const wchar_t * SpecFile::get_format_name()
{// get the name of the format of the spec file
  if ( this->parser == NULL ) {
    return L"UNKNOWN";
  }
  else {
    return this->parser->get_format_name();
  }
};

// open the spec file

bool SpecFile::init()
{
#ifdef VDMPP
  // file_success is only relevant if file_created is true.
  if(this->file_created && !this->file_success) return false;
#endif // VDMPP

  int not_found;
  // find a parser for the file
  TBUTILS::Search_and_OpenFile(this->fname, this->fname, this->sp, not_found);
  if (not_found) {
    return false;
  }
  this->parser = SpecFileHandler::get_parser(*this);

  if (this->parser == NULL) {
    return false;
  }
  // Get unique file id

#ifdef VDMPP
  if( this->file_created ) {
    this->file_id = TYPE_CI_FileId(VPPUML_DUMMY_FILE_ID);
  }
  else
#endif // VDMPP
    this->file_id = ToolMediator::GetFileId(filename);

  if (this->file_id.GetValue() == 0) {
    // file not found in repository
    SET<TYPE_ProjectTypes_FileName> s;
    s.Insert (filename);
    ToolMediator::AddFiles (s);
    this->file_id = ToolMediator::GetFileId(filename);
  }
  return (!not_found);
}

void SpecFile::setTimestamp()
{
  // Set/update timestamp in repository
  wstring fn (PTAUX::ExtractFileName (this->filename));
  struct stat buf;
  long mtime = 0L;
  int s = stat (TBWSTR::wstring2fsstr(fn).c_str (), &buf);
  if (!s) {
    mtime = buf.st_mtime;
  }
  ToolMediator::SetFileTimestamp (filename, Int (mtime));
}

bool SpecFile::rewind()
{
  // Reset error flags which were set if the file was read until EOF.
  this->sp.clear();
  this->sp.seekg(0);
  return this->sp.good();
}

// close the spec file

bool SpecFile::close()
{
  this->sp.close();
  this->sp.clear();
  return true;
}

#define LOOK_FOR_TOKENS 20  // max number of tokens to look for

bool SpecFile::look_for(const char * tl[])
{
  int   i,
        len,  // number of elements + 1 in tl
        left; // number of tokens still not found
  const char *elems[LOOK_FOR_TOKENS] ; // list of the tokens not yet found
                 // a NULL elemnt means that the token has been found
  rewind();

  // copy the tokens to elems
  for (i = 0; (elems[i] = tl[i]) ; i++);
  len = i;
  left = len;

  // read the file until all tokens are found or eof

  string line;
  while (!this->sp.eof()) {
    getline(this->sp,line);

    // check if any of the missing tokens are in the buffer

    for (i = 0; i < len; i++) {
      if (elems[i]) {
        if (::strstr(line.c_str(), (const char*) elems[i])) {
          elems[i] = NULL;  // token found
          left--;
        }
      }
    }

    if (left == 0) {
      break;
    }
  }
  return (left == 0);
}

// convertToUniChar
bool SpecFile::convertToUniChar(ifstream & inp, ostringstream & unichar)
{
  bool res = true;
  while ( !inp.eof() ) {
    string line;
    getline(inp, line);

    line = TBWSTR::convertCrToNl(line);
    line += '\n';

    try {
      string converted (TBWSTR::ConvertToHexquad(TBWSTR::mbstr2wstring(line)));
      unichar.write(converted.c_str(), converted.length());
    }
    catch (...) {
      vdm_log << L"Invalid multibyte string encounted: "
              << TBWSTR::string2wstring(line)
              << L"abandoning parse"
              << endl;
      res = false;
      break;
    }
  }
  return res;
}

#ifdef VDMSL
///////////////////////////////////////////
// Name: ExtractDefinedFunctionNames
// Descr:
//   This functions returns a sequence of sets. Each set contains the
//   function and operation names defined in the corresponding module
//   in 'mds'.
//   Functions/operations which are not
//   imported without a name (import all)) in the corresponding module
//   in 'mds'.
///////////////////////////////////////////

// ExtractDefinedFunctionNames
// mds : set of (Module | DLModule) | set of Definitions
// ==> seq of set of AS`Name
Sequence SpecFileHandler::ExtractDefinedFunctionNames (const Set & mds)
{
  Sequence res;

  Record rc (mds.GetElem ());
  if (rc.Is (TAG_TYPE_AS_Definitions)) {
    ////////////////////////
    // mds is sequence of definitions - not seq of modules
    ////////////////////////

    Set def_fcts;
    TYPE_AS_Definitions defs (rc);
    def_fcts.ImpUnion (defs.get_fnm().Dom ()); //functions
    def_fcts.ImpUnion (defs.get_opm().Dom ()); //operations
    res.ImpAppend (def_fcts);
  }
  else { // mds.IsSequence()
    /////////////////////////////
    // for all modules in the AS
    /////////////////////////////

    Set mds_q (mds);
    Generic mod_g;
    for (bool b = mds_q.First(mod_g); b; b = mds_q.Next(mod_g)) {
      if (mod_g.Is(TAG_TYPE_AS_Module)) {
        TYPE_AS_Module mod (mod_g);
        TYPE_AS_Interface intf (mod.get_intf());
        Set imp_fcts;
        Map imp (intf.get_imp());

        /////////////////////////////
        //for all imports
        /////////////////////////////

        Generic gg, gisig;
        for (bool bb = imp.First(gg, gisig); bb; bb = imp.Next(gg, gisig)) {
          if (gisig.IsRecord ()) { //not import all
            ////////////////////////////
            // Create mapping from orig name to new name
            // (inverse ImportSig.ren mapping)
            ////////////////////////////

            TYPE_AS_ImportSig isig(gisig);
            Map ren (isig.get_ren()); // map Name to Name
            Map inv_ren;              // map Name to Name
            Generic old_nm, new_nm;
            for (bool cc = ren.First(old_nm, new_nm); cc; cc = ren.Next(old_nm, new_nm)) {
              inv_ren.Insert (new_nm, old_nm);
            }
            /////////////////////////////
            //for all imported functions
            /////////////////////////////

            Map fns (isig.get_fns());
            Generic fnnm;
            for (bool dd = fns.First(fnnm); dd; dd = fns.Next(fnnm)) {
              if (inv_ren.DomExists (fnnm)) {
                //if renamed use the new name
                imp_fcts.Insert (inv_ren[fnnm]);
              }
              else {
                TYPE_AS_Name nm (fnnm);
                TYPE_AS_Name nm2 (gg);
                TYPE_AS_Ids ids (nm.get_ids());
                //prepend with module name
                ids.ImpPrepend (nm2.get_ids().Hd());
                nm.set_ids(ids);
                imp_fcts.Insert (nm);
              }
            }

            /////////////////////////////
            //for all imported operations
            /////////////////////////////

            Map ops (isig.get_ops());
            Generic opnm;
            for (bool ee = ops.First(opnm); ee; ee = ops.Next(opnm)) {
              if (inv_ren.DomExists (opnm)) {
                //if renamed use the new name
                imp_fcts.Insert (inv_ren[opnm]);
              }
              else {
                TYPE_AS_Name nm (opnm);
                TYPE_AS_Name nm2(gg);

                TYPE_AS_Ids ids (nm.get_ids());
                //prepend with module name
                ids.ImpPrepend (nm2.get_ids().Hd());
                nm.set_ids(ids);
                imp_fcts.Insert (nm);
              }
            }
          }
        }

        Set def_fcts;
        def_fcts.ImpUnion (imp_fcts);

        /////////////////////////////
        // local definitions
        /////////////////////////////

        TYPE_AS_Definitions def (mod.get_defs ());
        def_fcts.ImpUnion (def.get_fnm().Dom ()); //functions
        def_fcts.ImpUnion (def.get_opm().Dom ()); //operations
        res.ImpAppend (def_fcts);
      }
      else if (mod_g.IsRecord() && Record (mod_g).Is(TAG_TYPE_AS_DLModule)) {
        res.ImpAppend (Set());
      }
      else {
        res.ImpAppend (Set());
      }
    }
  }
  return res;
}
#endif //VDMSL

