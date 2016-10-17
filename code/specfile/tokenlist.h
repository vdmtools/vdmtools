/***
*  * WHAT
*  *    Handling of the tokens in a VDM ASCII text
*  *    giving precisie position information and
*  *    relative position to alle the tokens that occured
*  *    Position information includes : section, row, column
*  * Id
*  *    $Id: tokenlist.h,v 1.18 2006/01/17 03:01:06 vdmtools Exp $
*  * Implements
*  *    contextinfo.w 1.14
***/

#ifndef _INCL_TOKENLIST_H
#define _INCL_TOKENLIST_H

#include <stdio.h>
#include <deque>
#include <string.h>
#include "metaiv.h"
#include "CI.h"

///////////////////////////////////////////////
// TokenElem
// Description of a single Token from the
// VDM parser
///////////////////////////////////////////////

class TokenList;

class TokenInfo {

public :

// Index elements in pretty print

  enum IndexElementType {
    no_index, // this element is not an index element
    fct_def,
    fct_inv,
    op_def,
    op_inv,
#ifdef VDMSL
    state_def,
    mod_def,
#endif
#ifdef VDMPP
    class_def,
#endif
    tp_def
};

private:

  int id;           // token id as given by the lex scanner
  wstring text;      // The text of an identifier etc. or NULL
  TYPE_CI_TokenPos pos_st;  // position of the first wchar_t in the token
  TYPE_CI_TokenPos pos_end; // position of last wchar_t in the token

  IndexElementType indtp; // the index type of this token
  int64_t cov;         // test coverage count

  // A TokenInfo instance is not meant to be copied, so the following
  // methods have no implementation. This will ensure that any new
  // methods in this class cannot copy or assign class instances.

public:
  
  int get_id() const { return this->id;};
  const wstring & get_text() const { return this->text;};
  const TYPE_CI_TokenPos & get_pos_st() const { return this->pos_st; };
  const TYPE_CI_TokenPos & get_pos_end() const { return this->pos_end; };

  // Mark the token as being an index element
  void set_index_element(IndexElementType ind_tp) { this->indtp = ind_tp; };

  // Get theindex element
  IndexElementType get_index_element() const { return this->indtp; };
  
  // Get coverage count
  int64_t get_test_coverage() const { return this->cov; };

  // Set coverage count
  void  set_test_coverage(int64_t c) { this->cov = c; };

  int64_t get_sec_st() const { return pos_st.get_section().GetValue();};
  int64_t get_abs_line_st() const { return pos_st.get_abs_uline().GetValue();};
  int64_t get_relAbs_line_st() const { return pos_st.get_relAbs_uline().GetValue();};
  int64_t get_sec_line_st() const { return pos_st.get_sec_uline().GetValue();};
  int64_t get_col_st() const { return pos_st.get_column().GetValue();};

  int64_t get_sec_end() const { return pos_end.get_section().GetValue();};
  int64_t get_abs_line_end() const { return pos_end.get_abs_uline().GetValue();};
  int64_t get_relAbs_line_end() const { return pos_end.get_relAbs_uline().GetValue();};
  int64_t get_sec_line_end() const { return pos_end.get_sec_uline().GetValue();};
  int64_t get_col_end() const { return pos_end.get_column().GetValue();};

  bool isClassOrModuleToken() const;

  // is quoted correct before it is printed out.
  void pp(ostream &f) const;
  void dump( wostream& wos ) const;

public:

  TokenInfo()
  {
    text = L"";
    id = -1;
    cov = 0;
    indtp = no_index;
  };

  ~TokenInfo() {};
  TokenInfo & Init(int tok, const wstring & txt,  const TYPE_CI_TokenPos & from, const TYPE_CI_TokenPos & to)
  {
    id = tok;
    text = txt;
    pos_st = from;  // start position
    pos_end = to;   // end position
    return (*this);
  };

  friend class TokenList;
};

typedef VDMContainer<TokenInfo> TokenInfoCt;

int vdm_Compare(const TokenInfo &v1, const TokenInfo &v2);
void vdm_ostream_out(const TokenInfo & value, wostream & o);

///////////////////////////////////////////////
// TokenList
// List to hold tokens from the VDM part of a specification
// The TokenList is used for:
// o dealing with position information
// o pretty printing
///////////////////////////////////////////////

class TokenList {
  typedef deque<TokenInfo> tokens_t;
  tokens_t tokens;  // The list of tokens

  SEQ<TokenInfoCt> token_l;

private: 
  // A TokenList can't be copied, these are meant to 
  // ensure this. 
  TokenList(const TokenList&); // has no implementation
  void operator=(const TokenList&); // has no implementation

  // Get token with given index starting from 1
  TokenInfo & GetForModify(int64_t i);

public:
  TokenList() {};
  ~TokenList() { Clear(); };
  void Clear();

  // Append token to the list. Return token number which 
  // should be useable with the Get method. 
  size_t Add(const TokenInfo & t );

  // Get number of Tokens in list
  size_t Length() const { return static_cast<size_t>(tokens.size()); };

  // Get token with given index starting from 1
  const TokenInfo & Get(int64_t i) const;

  // Mark the token as being an index element
  void set_index_element(const TYPE_CI_TokenSpan & pos, TokenInfo::IndexElementType ind_tp );

  // Mark the token as being covered
  void set_test_coverage(const TYPE_CI_TokenSpan & pos, int64_t cov);

  // Mark all the tokens as being covered
  void set_test_coverage_all(int64_t cov);

  // Get the index of the first tokens that has a given section number
  // 0 is returned if no token with that section number is found

  size_t GetSectionIndex(int64_t sec) const;
  Sequence getGUIInfo(const SEQ<TYPE_CI_TokenSpan> & ts_l) const;
  void pp(ostream &f) const;
  void dump( wostream& wos ) const;
  void AdjustPosInfoForMultibyteText();
};

#endif // _INCL_TOKENLIST_H
