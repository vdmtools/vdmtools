/***
*  * WHAT
*  *    Handling of the tokens in a VDM ASCII text
*  *    giving precisie position information and
*  *    relative position to alle the tokens that occured
*  *    Position information includes : section, row, column
*  * Id
*  *    $Id: tokenlist.cc,v 1.6 2006/06/23 05:34:20 vdmtools Exp $
*  * Implements
*  *    contextinfo.w 1.14
***/

#include "tokenlist.h"
#include "binops.h"
#include "tb_wstring.h"
#include "m4lib_errmsg.h"
 
int vdm_Compare(const TokenInfo &v1, const TokenInfo &v2)
{
  return 0;
}

void vdm_ostream_out(const TokenInfo & value, wostream & o)
{
}

///////////////////////////////////////////////
// TokenElem
// Description of a single Token from the
// VDM parser
///////////////////////////////////////////////

bool TokenInfo::isClassOrModuleToken() const
{
  return BINOPS::isClassOrModule(this->get_id());
}

void TokenInfo::pp(ostream &f) const
{
  f << "mk_CI`TokenInfo(" 
    << id << ","
    << TBWSTR::wstring2string(Sequence(get_text()).ascii())
    << ",";  
  f << "mk_CI`TokenPos(" 
    << pos_st.get_section() << ','
    << pos_st.get_abs_uline() << ','
    << pos_st.get_relAbs_uline() << ','
    << pos_st.get_sec_uline() << ','
    << pos_st.get_column() 
    << ")"; 
  f << ","; 
  f << "mk_CI`TokenPos(" 
    << pos_end.get_section() << ','
    << pos_end.get_abs_uline() << ','
    << pos_end.get_relAbs_uline() << ','
    << pos_end.get_sec_uline() << ','
    << pos_end.get_column() 
    << ")"; 
  f << ")";
}

void TokenInfo::dump( wostream & wos ) const
{
  char buf[50]; 
  string format = "(%2d,%3d,%3d,%3d,%3d)";
  sprintf( buf, format.c_str(), pos_st.get_section().GetValue(),
                                pos_st.get_abs_uline().GetValue(),
                                pos_st.get_relAbs_uline().GetValue(),
                                pos_st.get_sec_uline().GetValue(),
                                pos_st.get_column().GetValue()); 
  string str = buf;
  wos << TBWSTR::string2wstring( str );

  wos << L",";
  sprintf( buf, format.c_str(), pos_end.get_section().GetValue(),
                                pos_end.get_abs_uline().GetValue(),
                                pos_end.get_relAbs_uline().GetValue(),
                                pos_end.get_sec_uline().GetValue(),
                                pos_end.get_column().GetValue()); 
  str = buf;
  wos << TBWSTR::string2wstring( str );

  wos << L",";

  wos << get_index_element();
  wos << L",";
  wos << get_test_coverage();
  wos << L",";

  int id = get_id();

  sprintf( buf, "%3d", id );
  string idstr = buf;
  wos << TBWSTR::string2wstring(idstr) << L",";
  wstring ws (BINOPS::id2str( id, get_text() ));
  wos << ws;
  wos << endl << flush;
}

///////////////////////////////////////////////
// TokenList
// List to hold tokens from the VDM part of a specification
// The TokenList is used for:
// o dealing with position information
// o pretty printing
///////////////////////////////////////////////

void TokenList::Clear()
{
  this->tokens.clear();
  this->token_l.Clear();
};

// Append token to the list. Return token number which 
// should be useable with the Get method. 
size_t TokenList::Add(const TokenInfo & t )
{ 
  this->tokens.push_back(t);
  return static_cast<size_t>(tokens.size()); 

/*
  TokenInfoCt tkc (t);
  this->token_l.ImpAppend(tkc);
  return this->token_l.Length();
*/
};

const TokenInfo & TokenList::Get(int64_t i) const
{
  if (i < 1 || i > (int64_t)tokens.size()) {
    M4LibError::ReportError(-1, L"Wrong index in TokenInfo::Get."); 
  }
  return this->tokens[i-1];  

/*
  return this->token_l[i].get_const_ref();
*/
};

TokenInfo & TokenList::GetForModify(int64_t i)
{
  if (i < 1 || i > (int64_t)tokens.size()) {
    M4LibError::ReportError(-1, L"Wrong index in TokenInfo::Get."); 
  }
  return this->tokens[i-1];  
/*
  return this->token_l.refIndex(i).get_shared_ref();
*/
};

// Mark the token as being an index element
void TokenList::set_index_element(const TYPE_CI_TokenSpan & pos, TokenInfo::IndexElementType ind_tp )
{
//  int i = pos.get_token_ust().GetValue();
  int64_t i = pos.GetIntValue(pos_CI_TokenSpan_token_ust);
  this->tokens[i-1].set_index_element(ind_tp);
}

// Mark the token as being covered
void TokenList::set_test_coverage(const TYPE_CI_TokenSpan & pos, int64_t cov)
{
  // from and to may be -1 when a default context_pos has been 
  // returned from ContextInfo::GetPos called with a NilContextId 
  int64_t from = pos.GetIntValue(pos_CI_TokenSpan_token_ust);
  int64_t to = pos.GetIntValue(pos_CI_TokenSpan_token_uend);
//wcout << from << L" - " << to << L" : " << cov << endl;
  if (from >= 1 && to >= 1) {
    for (int64_t index = from - 1; index < to; index++) 
      this->tokens[index].set_test_coverage(cov);
  }
}

// Mark all the tokens as being covered
void TokenList::set_test_coverage_all(int64_t cov)
{
  if (this->tokens.size() > 0)
  {
    TYPE_CI_TokenSpan pos;
    pos.Init(Int(1), Int(1), Int(tokens.size()));
    this->set_test_coverage(pos, cov);
  }
}

// Get the index of the first tokens that has a given section number
// 0 is returned if no token with that section number is found

size_t TokenList::GetSectionIndex(int64_t sec) const
{
  size_t len_tl = this->Length();
  for (size_t index = 0 ; index < len_tl; index++ ) {
    if (this->tokens[index].get_sec_st() == sec ) return (index + 1);
  }
  return 0;  
};

void TokenList::pp(ostream &f) const
{
  f << "["; 
  if (!tokens.empty())
    f << endl << " ";

  for (tokens_t::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
  {
    if (itr != tokens.begin())
      f << "," << endl << " "; 
    (*itr).pp(f); 
  }
  if (!tokens.empty())
    f << endl;
  f << "]"; 
}

void TokenList::AdjustPosInfoForMultibyteText()
{
  TYPE_CI_Section last_sec (-1);
  TYPE_CI_Line last_sec_line (-1);
  size_t length = this->Length();
  int offset = 0;
  for( size_t index = 1; index <= length; index++ )
  {
    TokenInfo & ti = this->GetForModify(index);
    const TYPE_CI_TokenPos & pos_st = ti.get_pos_st();
    const TYPE_CI_TokenPos & pos_end = ti.get_pos_end();

    if( last_sec != pos_st.GetInt(pos_CI_TokenPos_section) )
    {
      last_sec = pos_st.GetInt(pos_CI_TokenPos_section);
      last_sec_line = Int(-1);
    }
    if( last_sec_line != pos_st.GetInt(pos_CI_TokenPos_sec_uline) )
    {
      last_sec_line = pos_st.GetInt(pos_CI_TokenPos_sec_uline);
      offset = 0;
    }

    // calculate new start column value
    Int new_column_st (pos_st.GetIntValue(pos_CI_TokenPos_column) - offset);

    // calculate new offset value
    int textlen = ti.get_text().length();
    if( textlen > 0 )
    {
      int diff = pos_end.GetIntValue(pos_CI_TokenPos_column) - pos_st.GetIntValue(pos_CI_TokenPos_column);
      offset = offset + ( diff - textlen );
    }

    // if no changes then do nothing
    if (offset == 0) continue;

    // calculate new end column value
    Int new_column_end (pos_end.GetIntValue(pos_CI_TokenPos_column) - offset);

    // construct new TokenInfo
    TYPE_CI_TokenPos new_pos_st (pos_st);
    new_pos_st.SetField(pos_CI_TokenPos_column, new_column_st);
    
    TYPE_CI_TokenPos new_pos_end (pos_end);
    new_pos_end.SetField(pos_CI_TokenPos_column, new_column_end);

    ti.Init( ti.get_id(), ti.get_text(), new_pos_st, new_pos_end );
  }
}

void TokenList::dump( wostream & wos ) const
{
  size_t length = this->Length();
  int lastline = -1;
  for( size_t i = 1; i <= length; i++ )
  {
    const TokenInfo & ti = Get(i);
    if( lastline != ti.get_abs_line_st())
    {
      wos << L"-";
      lastline = ti.get_abs_line_st();
    }
    else
      wos << L" ";
    ti.dump( wos );
  }
}

Sequence TokenList::getGUIInfo(const SEQ<TYPE_CI_TokenSpan> & ts_l) const
{
  Set end_s;
  size_t len_ts_l = ts_l.Length();
  for (size_t index = 1; index <= len_ts_l; index++)
  {
    const TYPE_CI_TokenSpan & ts (ts_l[index]);
    const TokenInfo & etk = Get(ts.get_token_uend().GetValue());
    const TYPE_CI_TokenPos & pos_st = etk.get_pos_st();
    end_s.Insert(mk_(Int(pos_st.get_abs_uline()), Int(pos_st.get_column())));
  }

  Sequence ret;
  size_t length = this->Length();
  for( size_t i = 1; i <= length; i++ )
  {
    const TokenInfo & ti = Get(i);
    const TYPE_CI_TokenPos & pos_st = ti.get_pos_st();
    const TYPE_CI_TokenPos & pos_end = ti.get_pos_end();

    Int line (pos_st.get_abs_uline());
    Int col (pos_st.get_column());
    Int len (pos_end.get_column() - col);
    Int type (end_s.InSet(mk_(line, col)) ? TT_DEF : BINOPS::id2gtype(ti.get_id()));
    ret.ImpAppend( mk_(line, col, len, type) );
  }
  return ret;
}
