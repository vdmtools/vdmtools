/***
*  * WHAT
*  *   Handling of a RTF (Rich Text Format) specification file
*  *   Based on the RTF parser downloaded from:
*  *      http://night.primate.wisc.edu:80/software/RTF/
*  *   The specification will be split up into chunks
*  *   rather than using TexBreaks
*  *   The PP is based on a token list
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/specfile/rtfspecfile.cc,v $
*  * VERSION
*  *    $Revision: 1.145 $
*  * DATE
*  *    $Date: 2007/03/24 06:35:54 $
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

#include "specfile.h"

#include "tbutils.h"
#include "testcoverage.h"
#include "vdmpars.h"

#include "projectval.h"
#include "Repository.h"
#include "ToolMediator.h"
#include "Interface.h"
#include "Errors.h"
#include "tools.h"
#include "astaux.h"
#include "tb_exceptions.h"
#include "rt_errmsg.h"

#include "extr_pp_info.h"
#include "testcoverage.h"
#include "tb_wstring.h"
#include "settings.h"
#include <sstream>
#include "scanner.h"

// FROM RTF PARSER

# include "rtf.h"
# include "rtf-unix.h"
# include "rtf2text.h"
# include "reader.h"

// FROM text-writer.cc

string get_vdm_cov();
string get_vdm_ncov();

// FROM VDM/PP PARSER

#ifdef VDMSL
#include "parssl.tab.h"
#endif // VDMSL
#ifdef VDMPP
#ifdef VICE
#include "parsrt.tab.h"
#else
#include "parspp.tab.h"
#endif //VICE
#endif //VDMPP

//vdm_Repository * Repos (); // To be removed ???

// Name of the Module/Class being pp
#ifdef VDMSL
  wstring mod_name = L"DefaultMod\\:";
#endif // VDMSL
#ifdef VDMPP
  wstring mod_name = L"DefaultClass\\:";
#endif // VDMPP

/////////////////////////////////////////////////////////////
// RTF SpecFile Parser
/////////////////////////////////////////////////////////////

RTFWordSpecFileHandler::RTFWordSpecFileHandler()
{
  format = FORMAT_RTF_WORD;
};

RTFWordSpecFileHandler::~RTFWordSpecFileHandler()
{
};


SpecFileHandler * RTFWordSpecFileHandler::dup()
{
  RTFWordSpecFileHandler * s = new RTFWordSpecFileHandler();
  return s;
}

bool RTFWordSpecFileHandler::read(SpecFile & )
{
    bool ok = true;
    return ok;
}

void RTFSetChunkSequence(ChunkList *);
void HandleLastChunk();
bool RTFWordSpecFileHandler::parser_init;

int RTFWordSpecFileHandler::nonAsciiFontNum;

bool RTFWordSpecFileHandler::split(SpecFile & sf, bool vdm_only)
{
  volatile bool ok = true;

  try {

    ////////////////////////
    // INIT RTF PARSER
    ////////////////////////

    if (!parser_init) {
      WriterInit ();  // Init the rtf writer
      parser_init = true;
    }

    // INIT CHUNK HANDLING
    this->chunks.Clean();

    RTFSetChunkSequence(&this->chunks);

    // PREPARE SPEC FILE FOR READING

    RTFInit ();    // prepare for new input
    sf.rewind();  // prepare spec file for reading


    RTFSetStream(&sf.get_input_stream());  // File to read

    // PARSE AND SPLIT THE RTF FILE
    BeginFile();
    RTFRead ();
    HandleLastChunk(); // handle the last chunk in the file
    setNonAsciiFontNum(RTFGetNonAsciiFontNum());

  }
  catch (TB_Exception & e) {
    switch (e.GetExType()) {
      case ERR_RTF_Parser: {
        vdm_err << L"Unable to read the file " << sf.get_fname();
        ok = false;
      }
    }
  }
  return ok;
}

//////////////////////////////////////////////////////////////////////
// RTFWordSpecFileHandler
// precondition : read and split
//////////////////////////////////////////////////////////////////////

bool RTFWordSpecFileHandler::parse_vdm(SpecFile & sf, Sequence & asts, ContextInfo & ci)
{
  ifstream vdm_inp;
  ofstream vdm_out;

  TYPE_ProjectTypes_FileName rfn (ToolMediator::Repos()->vdm_AddTempFile (PTAUX::mk_FileName (sf.get_fname ())));
  wstring temp_vdm_file = PTAUX::ExtractFileName (rfn);

  // write VDM chunks to file
  //
  vdm_out.open(TBWSTR::wstring2fsstr(temp_vdm_file).c_str(), ios::out);
  if (!vdm_out.good()) {
    vdm_log << endl
            << L"Couldn't open temporary file '" << temp_vdm_file
            << L"' for writing" << endl << flush ;
    return false;
  }

  for (int i = 1 ; i <= this->chunks.Length(); i++) {
    switch ( this->chunks.Get(i)->get_chunk_type() ) {
    case CHUNK_TYPE_VDM : {
      vdm_out << ((ChunkVDM*) this->chunks.Get(i))->vdm_ascii;
      break;
    }
    default : break;
    }
  }

  vdm_out.close();

  vdm_inp.open(TBWSTR::wstring2fsstr(temp_vdm_file).c_str(), ios::in);
  if (!vdm_inp.good())
    return false;

  ostringstream unichar;
  if (!sf.convertToUniChar(vdm_inp, unichar))
    return false;

  return parse_vdm_file(sf, unichar.str(), asts, ci);
}

//static void pp_tc_table_horizontal_line() {


///////////////////////////////////////////////////////////////////////////////
// RTFWordSpecFileHandler::pp_tc_table :
// Pretty print a testcoverage table for a given module/class
// all functions/operations/methods are printed
///////////////////////////////////////////////////////////////////////////////

bool RTFWordSpecFileHandler::pp_tc_table(const string & parm_mod_name,  // nodule/class to print
                                         const Sequence & asts,
                                         ContextInfo&ci)
{
  Set asts_set = asts.Elems();

  /////////////////////
  // Print Table Header
  /////////////////////

    /////////////////////
    // top row
    /////////////////////

    pp_out << "\n\\trowd \\trgaph108\\trrh112\\trleft-108"
    // row border top
    << "\\trbrdrt\\brdrs\\brdrw30\\brdrcf1 "
    // row border buttom
    << "\\trbrdrb\\brdrs\\brdrw30\\brdrcf1 "
    << "\\clvertalt"
    // top cell border
    << "\\clbrdrt\\brdrs\\brdrw30\\brdrcf1 "
    // left cell border
    << "\\clbrdrl\\brdrs\\brdrw10 \\clbrdrb\\brdrs\\brdrw15\\brdrcf1 "
    // right cell border
    << "\\clbrdrr\\brdrs\\brdrw15\\brdrcf1 \\cltxlrtb \\cellx4536"
    << "\\clvertalt"

    // top table cell border
    << "\\clbrdrt\\brdrs\\brdrw30\\brdrcf1 "
    // button table cell borderr
    << "\\clbrdrb\\brdrs\\brdrw15\\brdrcf1 \\cltxlrtb \\cellx5529"
    << "\\clvertalt"
    // top table cell border
    << "\\clbrdrt\\brdrs\\brdrw30\\brdrcf1 "
    // button table cell border
    << "\\clbrdrb\\brdrs\\brdrw15\\brdrcf1 "
    << "\\clbrdrr\\brdrs\\brdrw10 \\cltxlrtb \\cellx6663\\pard\\plain "
    // The title row
    << "\n\\nowidctlpar\\widctlpar\\intbl\\adjustright \\fs20\\cgrid {\\b\\i name\\cell #calls\\cell coverage\\cell }\\pard "
    << "\n\\nowidctlpar\\widctlpar\\intbl\\adjustright {\\i\\fs24\\lang1030 \\row }"
    // row formatting
    << "\n\\trowd \\trgaph108\\trleft-108"
    // row border top
    << "\\trbrdrt\\brdrs\\brdrw30\\brdrcf1 "
    // row border buttom
    << "\\trbrdrb\\brdrs\\brdrw30\\brdrcf1 "
    //
    << "\\clvertalt\\clbrdrl"
    << "\n\\brdrs\\brdrw10 "
    // cell borders
    << "\\clbrdrr\\brdrs\\brdrw15\\brdrcf1 \\cltxlrtb \\cellx4536\\clvertalt\\cltxlrtb \\cellx5529\\clvertalt"
    << "\\clbrdrr\\brdrs\\brdrw10 \\cltxlrtb \\cellx6663\\pard";


  /////////////////////
  // Print all items
  /////////////////////

    double pcoverage = TestCoverage::PrintEntireTestSuite (
                 pp_out,
                 ASTAUX::MkNameFromId (ASTAUX::MkId(TBWSTR::mbstr2wstring(parm_mod_name)), NilContextId),
                 asts_set,
                 FORMAT_RTF_WORD,
                 ci);

  /////////////////////
  // Print Total coverage
  /////////////////////

  // pp_out
  //  << L"\n\\nowidctlpar\\widctlpar\\adjustright{\n\\par }";  // horizontal line ???

    // The total row
  pp_out << "\n\\nowidctlpar\\widctlpar\\intbl\\adjustright \\fs20\\cgrid {\\b\\i\\fs24\\lang1030 total\\cell \\cell ";

  if (pcoverage == -1) {
    pp_out << "undef \\cell }\\pard ";
  } else {
    pp_out << pcoverage << "% \\cell }\\pard ";
  }

pp_out << "\n\\nowidctlpar\\widctlpar\\intbl\\adjustright {\\i\\fs24\\lang1030 \\row } \\pard";

  /////////////////////
  // Print Table Tail
  /////////////////////

  pp_out
    << "\n\\nowidctlpar\\widctlpar\\adjustright{\n\\par }";
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// RTFWordSpecFileHandler::pp_section :
// Pretty print all the tokens that belongs to the given section
// A token that starts in one section and ends in an other is printed
// in the section where it started. (E.g. a comment)
///////////////////////////////////////////////////////////////////////////////

bool RTFWordSpecFileHandler::pp_section(
                                  SpecFile &sf,
                                  int sec,         // section to print
                                  bool * color_on, // should color be turned on
                                  TokenList& tokens
                                 )
{
  //  TokenList & tokens = sf.get_contextinfo().GetTokenList(sf.get_file_id());
  int start_i = tokens.GetSectionIndex(sec); // Get index of first token in section
  int column = 1; // next free column
  int line   = 1; //  next free line

  // PRINT THE HEADER
  pp_out << ((ChunkVDM*) this->chunks.Get(sec))->header;

  // TURN COVERAGE ON ? FROM LAST SECTION ?
  if (this->generate_rti) {
    if (*color_on) {
      pp_out << "{" << get_vdm_ncov();
    } else {
      pp_out << "{" << get_vdm_cov();
    }
  }

  // for all tokens in this section
  if ( start_i != 0 )
  {
    for (size_t i = start_i; i <= tokens.Length(); i++) {

      const TokenInfo & tk = tokens.Get(i);

      if ( tk.get_sec_st() != sec ) break;

      if ( tk.get_sec_line_st() != line) column = 1;

      //printf ("Linenum = %4d line = %4d\n",tk->get_sec_line(),line);
      pp_newline(tk.get_sec_line_st() - line);  // print optional new lines
      pp_space( tk.get_col_st() - column);      // print optional spaces

      // print test coverage colors
      if (this->generate_rti) {
        if ( tk.get_test_coverage()) {
          if ( *color_on ) {
            pp_out << "}{" << get_vdm_cov();
            *color_on = false;
          }
        } else {
          if (! *color_on ) {
            pp_out << "}{" << get_vdm_ncov();
            *color_on = true;
          }
        }
      }

      pp_token(tk);                         // print the token

      line = tk.get_sec_line_end();
      column = tk.get_col_end();
    }
  }

  if (this->generate_rti) pp_out << "}"; // end of coloring

  // PRINT THE FOOTER

  pp_out << ((ChunkVDM*) this->chunks.Get(sec))->footer;

  return true;
}

//////////////////////////////////
// Print a keyword
//////////////////////////////////

bool RTFWordSpecFileHandler::pp_keyword( const string & k)
{
  if(this->pp_key_and_ident)
    pp_out << "{\\b "<< k << "}"  ; // bold face
  else
    pp_out << k;
  return true;
}

//////////////////////////////////
// Print an identifier
//////////////////////////////////


//
// RTFWordSpecFileHandler::ConvertHexQuadCharsToRTF
//
// This function converts 4 hex digits from a unicode character into
// the corresponding RTF representation
//
// Arguments:
//   const string& chars: a string containing a prefix with 4 hex
//                        digits
//   string& res:         the string to which the result will be
//                        appended
//
// Returns: no return value
//
void RTFWordSpecFileHandler::ConvertHexQuadCharsToRTF(const string& chars, string& res)
{
  if (chars.substr(0,2) != "00") {
    res.append("\\'");
    res.append(chars.substr(0,2));
  }
  res.append("\\'");
  res.append(chars.substr(2,2));
}

//
// RTFWordSpecFileHandler::ConvertCharToRTF
//
// This function converts an ascii character into the RTF \'
// representation.
//
// Arguments
//   const char ch: the character to be converted
//   string& res:   the string to which the converted representation
//                  will be appended
void RTFWordSpecFileHandler::ConvertCharToRTF(const char ch, string& res)
{
  int c = (int) ch;
  char buf[6];
  sprintf(buf, "\\'%x", c);
  res = res + buf;
}

//
// RTFWordSpecFileandler::ConvertHexQuadStrToRTF
//
// This function converts a string possibly containing unicode
// characters into a string in which these characters have been
// converted into their RTF representations.
//
// Arguments:
//   const char* txt: the string to be converted
//   string& res: the string in which the result will be placed
//void RTFWordSpecFileHandler::ConvertHexQuadStrToRTF(const string & tk_txt,
void RTFWordSpecFileHandler::ConvertHexQuadStrToRTF(const string & hqs, string& res)
{
//  const char * txt = tk_txt.c_str();
//  unsigned int i = 0, txtSize = strlen(txt);
  unsigned int i = 0, txtSize = hqs.size();
//  string hqs (txt);
  bool containsDoubleChars = false;

  while(i < txtSize)
  {
    if (i == txtSize - 1)
      ConvertCharToRTF(hqs[i++], res);
    else if (hqs[i] == '\\' && hqs[i+1]=='u' && i+5 < hqs.size())
    {
      containsDoubleChars = true;
      string str(hqs.substr(i, 6));
      res.append(TBWSTR::hexquard2rtfstring(str));
      i = i + 6;
    }
    else
      ConvertCharToRTF(hqs[i++], res);
  }
  if (containsDoubleChars) {
    char buf[12];
    sprintf(buf, "\\dbch\\f%d", getNonAsciiFontNum());
    res = buf + res;
  }
}

bool RTFWordSpecFileHandler::pp_ident( const string& txt)
{
  string rtf_tk_txt;
  ConvertHexQuadStrToRTF(txt, rtf_tk_txt);
  if(this->pp_key_and_ident)
    pp_out << "{\\i " << rtf_tk_txt << "}";
  else
    pp_out << rtf_tk_txt;
  return true;
}

bool RTFWordSpecFileHandler::pp_identifier( int tok, const string & k) {
  // ??? switch on different types : e.g. LEX_quote_lit
  switch (tok) {
    case  LEX_quote_lit : {
      pp_out << "<{\\caps " << k << "}>";
      break;
    }
    case LEX_char_lit : {
      pp_out << "\\'" << k << "\\'";
      break;
    }
    case LEX_text_lit : {
      pp_out << "\"" << k << "\"";
      break;
    }
    case LEX_int_lit :
    case LEX_real_lit : {
      pp_out << k;
      break;
    }
    default : {
      break;
    }
  }
  return true;
}


//////////////////////////////////////////////////////////////////////
// print new lines
//////////////////////////////////////////////////////////////////////

bool RTFWordSpecFileHandler::pp_newline(int num) {
 for (int i = 0; i < num; i++)
   pp_out << "\n\\par ";
 return true;
}

//////////////////////////////////////////////////////////////////////
// print spsace chars
//////////////////////////////////////////////////////////////////////

bool RTFWordSpecFileHandler::pp_space(int num) {
 for (int i = 0; i < num; i++)
   pp_out << " ";

 return true;
}
//////////////////////////////////////////////////////////////////////
// print a token
//////////////////////////////////////////////////////////////////////

bool RTFWordSpecFileHandler::pp_token(const TokenInfo & tk)
{
  string tk_txt = TBWSTR::wstring2string(tk.get_text());
  string rtf_tk_txt;
  ConvertHexQuadStrToRTF(tk_txt, rtf_tk_txt);
  string rtf_mod_name;
  ConvertHexQuadStrToRTF(TBWSTR::wstring2string(mod_name), rtf_mod_name);

  int id = tk.get_id();

  switch ( id ) {
    case LEX_COMMENT : {
      const char* comments = "\\\'2d\\\'2d";
      int i = rtf_tk_txt.find(comments);
      if(i>=0) rtf_tk_txt.replace(i,strlen(comments),"--");
      pp_out << rtf_tk_txt ;
      break;
    }
    case LEX_HOOK : {
      pp_out << "~";
      break;
    }
    case LEX_char_lit : {  // no quotes
      pp_out << "\'" << rtf_tk_txt << "\'" ;
      break;
    }
    case LEX_quote_lit : {   // handle quotes around wstring
      pp_out << "<" <<  rtf_tk_txt << ">";
      break;
    }
    case LEX_text_lit : {
      pp_out << "\"" << rtf_tk_txt << "\"";;
      break;
    }  case LEX_real_lit : {
      pp_out << rtf_tk_txt;
      break;
    }
    case LEX_PRIME : {
      pp_out << "`";
      break;
    }
    case LEX_IN_SET : {
      pp_keyword(tk_txt);
      break;
    }
    case LEX_NOT_IN_SET : {
      pp_keyword(rtf_tk_txt);
      break;
    }
    case LEX_IS_ : {
      pp_keyword(string("is_"));
      break;
    }
    case LEX_MK_ : {
      pp_keyword(string("mk_"));
      break;
    }
    case LEX_identifier : {
      switch (tk.get_index_element()) {
        case TokenInfo::no_index : {
          pp_ident(tk_txt);
            break;
        }
        case TokenInfo::fct_def : {
          pp_ident(tk_txt);
          if (this->generate_index)
            pp_out << "{\\xe {\\v " << rtf_mod_name << rtf_tk_txt << "\\bxe}}";
          break;
        }
        case TokenInfo::fct_inv : {
          pp_ident(tk_txt);
          if (this->generate_index)
            pp_out << "{\\xe {\\v " << rtf_mod_name << rtf_tk_txt << "\\ixe}}";
          break;
        }
        case TokenInfo::tp_def : {
          pp_ident(tk_txt);
          if (this->generate_index)
            pp_out << "{\\xe {\\v " << rtf_mod_name << rtf_tk_txt << "\\bxe}}";
          break;
        }
        case TokenInfo::op_def : {
          pp_ident(tk_txt);
          if (this->generate_index)
            pp_out << "{\\xe {\\v " << rtf_mod_name << rtf_tk_txt << "\\bxe}}";
          break;
        }
        case TokenInfo::op_inv : {
          pp_ident(tk_txt);
          if (this->generate_index)
            pp_out << "{\\xe {\\v " << rtf_mod_name << rtf_tk_txt << "\\ixe}}";
          break;
        }
#ifdef VDMSL
        case TokenInfo::mod_def : {
          mod_name = TBWSTR::string2wstring(tk_txt) + L"\\:";
          string rtf_mod_name;
          ConvertHexQuadStrToRTF(TBWSTR::wstring2string(mod_name), rtf_mod_name);
          pp_ident(tk_txt);
          if (this->generate_index)
            pp_out  << "{\\xe {\\v " << rtf_mod_name << rtf_tk_txt << "\\bxe}}";
          break;
        }
        case TokenInfo::state_def : {
          pp_ident(tk_txt);
          if (this->generate_index)
            pp_out << "{\\xe {\\v " << rtf_mod_name << rtf_tk_txt << "\\bxe}}";
          break;
        }
#endif // VDMSL
#ifdef VDMPP
        case TokenInfo::class_def : {
          pp_ident(tk_txt);
          mod_name = TBWSTR::string2wstring(tk_txt) + L"\\:";
          string rtf_mod_name;
          ConvertHexQuadStrToRTF(TBWSTR::wstring2string(mod_name), rtf_mod_name);
          if (this->generate_index)
            pp_out << "{\\xe {\\v " << rtf_mod_name << rtf_tk_txt << "\\bxe}}";
          break;
        }
#endif // VDMPP
        default : {
          pp_ident(tk_txt);
        }
      }
      break;
    }

#ifdef VDMPP
    case LEX_dollar_identifier : {
      if(this->pp_key_and_ident)
        pp_out << "${\\i " << rtf_tk_txt << "}";
      else
        pp_out << "$" << rtf_tk_txt;
      break;
    }
#endif

    case LEX_EQUAL : {
      pp_out << "=";
      break;
    }
    case LEX_LESS_THAN : {
      pp_out << "<";
      break;
    }
    case LEX_GREATER_THAN : {
      pp_out << ">";
      break;
    }
    case LEX_RAISED_DOT : {
      pp_out << "&";
      break;
    }
    case '*' : {
      pp_out << "*";
      break;
    }
    case LEX_LESS_THAN_OR_EQUAL : {
      pp_out << "<=";
      break;
    }
    case LEX_GREATER_THAN_OR_EQUAL : {
      pp_out << ">=";
      break;
    }
    case LEX_NOT_EQUAL : {
      pp_out << "<>";
      break;
    }
    case LEX_OPERATION_ARROW : {
      pp_out << "==>";
      break;
    }
    case LEX_ARROW : {
      pp_out << "->";
      break;
    }
    case LEX_TARROW : {
      pp_out << "+>";
      break;
    }
    case LEX_IMPLY : {
      pp_out << "=>";
      break;
    }
    case LEX_LOGICAL_EQUIVALENCE : {
      pp_out << "<=>";
      break;
    }
    case LEX_BAR_ARROW : {
      pp_out << "|->";
      break;
    }
    case LEX_IS_DEFINED_AS : {
      pp_out << "==";
      break;
    }
    case LEX_NONDET : {
      pp_out << "||";
      break;
    }
// 20060105
//#ifdef VDMPP
//    case LEX_APPROX : {
//       pp_out << "=~";
//       break;
//    }
//#endif // VDMPP
    case LEX_ASSIGN : {
      pp_out << ":=";
      break;
    }
    case LEX_RANGE_OVER : {
      pp_out << tk_txt;
      break;
    }
    case LEX_DOUBLE_COLON : {
      pp_out << "::";
      break;
    }
    case LEX_DOTHASH : {
      pp_out << ".#";
      break;
    }
    case LEX_EXP_OR_ITERATE : {
      pp_out << "**";
      break;
    }
    case LEX_MODIFY_BY : {
      pp_out << "++";
      break;
    }
    case LEX_MAP_DOMAIN_RESTRICT_TO : {
      pp_out << "<:";
      break;
    }
    case LEX_MAP_DOMAIN_RESTRICT_BY : {
      pp_out << "<-:";
      break;
    }
    case LEX_MAP_RANGE_RESTRICT_TO : {
      pp_out << ":>";
      break;
    }
    case LEX_MAP_RANGE_RESTRICT_BY : {
      pp_out << ":->";
      break;
    }
    case LEX_SEQUENCE_CONCATENATE : {
      pp_out << "^";
      break;
    }
    case LEX_LAST_RESULT : {
      pp_out << "$$";
      break;
    }
    case LEX_TEXBREAK : {
      pp_out << "##!!#!";
      break;
    }
    case LEX_SET_MINUS : {
      pp_out << "\\\\";
      break;
    }
    case ',' : {
      pp_out << ",";
      break;
    }
#ifdef VDMPP
    case '!' : {
      pp_out << "!";
      break;
    }
#endif // VDMPP
    case ':' : {
      pp_out << ":";
      break;
    }
    case LEX_DONTCARE : {
      pp_out << ":-";
      break;
    }
    case ';' : {
      pp_out << ";";
      break;
    }
    case ')' : {
      pp_out << ")";
      break;
    }
    case '(' : {
      pp_out << "(";
      break;
    }
    case '|' : {
      pp_out << "|";
      break;
    }
    case '-' : {
      pp_out << "-";
      break;
    }
    case '[' : {
      pp_out << "[";
      break;
    }
    case ']' : {
      pp_out << "]";
      break;
    }
    case '@' : {
      pp_out << "@";
      break;
    }
    case '{' : {
      pp_out << "\\{";
      break;
    }
    case '}' : {
      pp_out << "\\}";
      break;
    }
    case '+' : {
      pp_out << "+";
      break;
    }
    case '/' : {
      pp_out << "/";
      break;
    }
    case '.' : {
      pp_out << ".";
      break;
    }
    default : {
      // check if it is a key word
      const char* s =  SCANNER::SearchReservedName(id);
      if (s) {
        pp_keyword(string(s));
      }
      else
        {
          if (tk_txt.length() > 0) {
            pp_identifier(id, rtf_tk_txt);
          } else {
            // ERROR HANDLING ???
            vdm_log << "Unknown token in Tokenlist " << id << " ";
            vdm_log << TBWSTR::string2wstring( tk_txt ) << endl << flush;
          }
        }
    }
  }
  return true;
}

//////////////////////////////////////////////////
// RTFWordSpecFileHandler:
// precondition = The SpecFile has been read, split
//////////////////////////////////////////////////

bool RTFWordSpecFileHandler::pp(SpecFile & sf, Sequence & asts, ContextInfo &ci) {

  bool cov_on = false;// is coverage is currently turned off in VDM

  this->pp_key_and_ident = true;
  this->generate_index = Settings.GetIndex() != 0;
  this->generate_rti   = Settings.GetPrettyPrintRTI();

  // Read runtime info
  if (this->generate_rti) {
    SET< SEQ<Char> > loadfiles (mk_set(SEQ<Char>(sf.get_short_name())));
    ToolMediator::ResetTestCoverage(ci);
    ToolMediator::LoadTestCoverageFile(wstring(L"vdm.tc"), ci, loadfiles, false);
  }

#ifdef VDMSL
    mod_name = L"DefaultMod\\:";
#endif
#ifdef VDMPP
    mod_name = L"DefaultClass\\:"; // notice the colon in the end
#endif

    //    ContextInfo & ci = sf.get_contextinfo();
    TokenList & tokl = ci.GetTokenList(sf.get_file_id());

    tokl.set_test_coverage_all(1); // mark all as covered
    extr_pp_info(asts, ci, tokl, false);

  // open the output file

  wstring rtf_file =  sf.get_pp_output_name();

  pp_out.open(TBWSTR::wstring2fsstr(rtf_file).c_str(),ios::out);
  if (!pp_out.good()) return false;

  // loop through the chunks counting the section number

  for (int sec = 1; sec <= this->chunks.Length(); sec++)
  {
    switch (this->chunks.Get(sec)->get_chunk_type())  {
      case CHUNK_TYPE_VDM : {
        pp_section(sf, sec, &cov_on, tokl);
        break;
      }
      case CHUNK_TYPE_VDM_TC_TABLE : {
        pp_out << ((ChunkVDM_TC_TABLE*) this->chunks.Get(sec))->header;
        if (this->generate_rti) {
          pp_tc_table( ((ChunkVDM_TC_TABLE*) this->chunks.Get(sec))->mod_name,asts,ci);
        } else {
  //        pp_out << ((ChunkVDM_TC_TABLE*) this->chunks.Get(sec))->mod_name;  // should maybe be removed
          string rtfstring = TBWSTR::mbstr2rtfstring(((ChunkVDM_TC_TABLE*) this->chunks.Get(sec))->mod_name);
          pp_out << rtfstring;
        }
        pp_out << ((ChunkVDM_TC_TABLE*) this->chunks.Get(sec))->footer;
  
        break;
      }
      case CHUNK_TYPE_BLOB : {
        //      pp_out << ((ChunkBLOB*) this->chunks.Get(sec))->blob;
        string rtfstring = TBWSTR::mbstr2rtfstring(((ChunkBLOB*) this->chunks.Get(sec))->blob);
        pp_out << rtfstring;
        break;
      }
      default: break;
    }
  }
  pp_out.close();
  return true;
}
#ifdef VDMPP

void RTFWordSpecFileHandler::pp_comment(const wstring & txt, const TokenInfo & tk)
{
  TokenInfo comment_tk;
  TYPE_CI_TokenPos start, end;
  start.Init(Int(tk.get_sec_st()),
             Int(tk.get_abs_line_st()),
             Int(tk.get_relAbs_line_st()),
             Int(tk.get_sec_line_st()),
             Int(tk.get_col_st()));
  end.Init(Int(tk.get_sec_end()),
           Int(tk.get_abs_line_end()),
           Int(tk.get_relAbs_line_end()),
           Int(tk.get_sec_line_end()),
           Int(tk.get_col_end()));
  wstring out_txt = L"-- " + txt;
  comment_tk.Init(LEX_COMMENT, out_txt.c_str(), start, end);
  pp_token(comment_tk);
}

bool RTFWordSpecFileHandler::create_section(SpecFile & sf, int sec)
{
  TokenList & tokens = new_ci.GetTokenList(VPPUML_DUMMY_FILE_ID);

  int column = 1; // next free column
  int line   = 1; // next free line
//  int insert_pos = 0;  // position for last Inset Action

  // PRINT THE HEADER
  pp_out << ((ChunkVDM*) this->chunks.Get(sec))->header;

  // for all tokens in this section
  for (size_t i = 1; i <= tokens.Length() ; i++) {
    const TokenInfo & tk = tokens.Get(i);
    if ( tk.get_sec_line_st() != line) column = 1;

    pp_newline(tk.get_sec_line_st() - line);  // print optional new lines
    pp_space( tk.get_col_st() - column);      // print optional spaces
    pp_token(tk);                         // print the token
    line = tk.get_sec_line_end();
    column = tk.get_col_end();
  }
  // PRINT THE FOOTER
  pp_out << ((ChunkVDM*) this->chunks.Get(sec))->footer;
  return true;
}




#define min(a,b)  ( (a) < (b) ? (a) : (b) )

bool RTFWordSpecFileHandler::update_section(SpecFile &sf,
                                            int sec,
                                            int &action_index)
{
  // Retrieve the token list for the original file.
  ContextInfo & ci = sf.get_contextinfo();
  TokenList & tokens = ci.GetTokenList(sf.get_file_id());

  // The token list containing the new constructs from UML
  TokenList & new_tokl = new_ci.GetTokenList(VPPUML_DUMMY_FILE_ID);

  int64_t start_i = tokens.GetSectionIndex(sec); // Get index of first token in section
  int column = 1; // next free column
  int line   = 1; //  next free line
  int insert_pos = 0;  // position for last Inset Action

  ActionList & action_list = get_action_list();
  const Action * action; // The current action

  // PRINT THE HEADER
  pp_out << ((ChunkVDM*) this->chunks.Get(sec))->header;

  // for all tokens in this section
  action = action_list.get(action_index);

  if ( start_i != 0 )
  {
    int64_t i = start_i;
    while( action != NULL || i <= (int64_t)tokens.Length() )
    {
      const TokenInfo & tk = tokens.Get(min(i,(int64_t)tokens.Length()));
      if( tk.get_sec_st() != sec ) break;

      if(action && action->token_pos() <= i ){
        if (action->kind() == ACTION_SKIP){
          // Skip a part of the original token list:
          line = tk.get_sec_line_st();
          pp_newline(1);
//          pp_comment(L"Removed by the Rose-VDM++ Link:", tk); pp_newline(1);
          pp_comment(L"Removed by the UML-VDM++ Link:", tk); pp_newline(1);
          // Start of comment region:
          pp_comment(L"", tk);
          column = 1;
          int lastsec = tk.get_sec_st();
          while( i <= action->to() && i <= (int64_t)tokens.Length() )
          {
            const TokenInfo & tki= tokens.Get(i);
            lastsec = tki.get_sec_st();
            if( tki.get_sec_st() != sec) break;

            pp_newline(tki.get_sec_line_st() - line);
            if ( tki.get_sec_line_st() != line) {
              // Start of a new line. Print a comment:
              pp_comment(L"", tki);
              column = 1;
            }
            pp_space( tki.get_col_st() - column);      // print optional spaces
            pp_token(tki);                          // print the token

            line = tki.get_sec_line_end();
            column = tki.get_col_end();
            i++;
          }
          if( sec == lastsec )
            // Ok to fetch the next action. Otherwise the SKIP action
            // was interupted by a section break and the action should
            // be not be removed yet.
            action = action_list.get(++action_index);
        }
        else if (action->kind() == ACTION_INSERT){
          // Insert a portion of the newly created token list:
          if(insert_pos != action->token_pos()){
            // New block of insertions - print a comment:
            pp_newline(1); // Necessary because this comment token is virtually
            // a copy (with respect to position information) of tk
            pp_comment(L"Added by the UML-VDM++ Link:", tk);
          }
          insert_pos = action->token_pos();
          int64_t ins_line = new_tokl.Get(action->from()).get_sec_line_st();
          column = 1;
          pp_newline(1);
          for(int64_t j = action->from(); (j <= (int64_t)new_tokl.Length()) && (j <= action->to()); j++)
          {
            const TokenInfo & tki = new_tokl.Get(j);
            if ( tki.get_sec_line_st() != ins_line)
              column = 1;
            pp_newline( tki.get_sec_line_st() - ins_line );
            pp_space( tki.get_col_st() - column );
            pp_token( tki );

            ins_line = tki.get_sec_line_end();
            column = tki.get_col_end();
          }
          pp_newline(1); // Add a new line after the insertion for readability
          action = action_list.get(++action_index);
        }
      }
      else{
        if ( tk.get_sec_line_st() != line) column = 1;
        pp_newline(tk.get_sec_line_st() - line);  // print optional new lines
        pp_space( tk.get_col_st() - column);      // print optional spaces
        pp_token(tk);                         // print the token

        line = tk.get_sec_line_end();
        column = tk.get_col_end();
        i++;
      }
    }
  }
  // PRINT THE FOOTER
  pp_out << ((ChunkVDM*) this->chunks.Get(sec))->footer;

  return true;
}


bool RTFWordSpecFileHandler::file_create(SpecFile &sf, wstring & err)
{
  this->generate_index = false;
  this->generate_rti   = false;
  this->pp_key_and_ident = false;
  // open the output file

  pp_out.open(TBWSTR::wstring2fsstr(sf.get_fname()).c_str(),ios::out);
  if (!pp_out.good()){
    err = L"Could not open the file for writing. File is probably locked by another application";
    return false;
  }

  // loop through the chunks counting the section number

  for (int sec = 1; sec <= this->chunks.Length(); sec++)
  {
    switch (this->chunks.Get(sec)->get_chunk_type())  {
      case CHUNK_TYPE_VDM : {
        create_section(sf, sec);
        break;
      }
      case CHUNK_TYPE_BLOB : {
        pp_out << ((ChunkBLOB*) this->chunks.Get(sec))->blob;
        break;
      }
      default: break;
    }
  }
  pp_out.close();

  return true;
}


bool RTFWordSpecFileHandler::file_update(SpecFile &sf, wstring & err)
{
//  int tl_count = 1;   // next element in token list to print
//  int sec_count =1;   // current section number

  this->generate_index = false;
  this->generate_rti   = false;
  this->pp_key_and_ident = false;
#ifdef VDMSL
  mod_name = "DefaultMod\\:";
#endif
#ifdef VDMPP
  mod_name = L"DefaultClass\\:"; // notice the colon in the end
#endif

  // open the output file

  TBUTILS::remove_file(sf.give_old_name());  // Remove old copy.
  TBUTILS::file_copy(sf.give_old_name(), sf.get_fname());

  pp_out.open(TBWSTR::wstring2fsstr(sf.get_fname()).c_str(),ios::out);
  if (!pp_out.good()) {
    err = L"Could not open the file for writing.";
    return false;
  }

  int action_index = 0;
  // loop through the chunks counting the section number

  for (int sec = 1; sec <= this->chunks.Length(); sec++)
  {
    switch (this->chunks.Get(sec)->get_chunk_type())  {
      case CHUNK_TYPE_VDM : {
        update_section(sf, sec, action_index);
        break;
      }
      case CHUNK_TYPE_VDM_TC_TABLE : {
        pp_out << ((ChunkVDM_TC_TABLE*) this->chunks.Get(sec))->header;
        // Newer generate rti:
        pp_out << ((ChunkVDM_TC_TABLE*) this->chunks.Get(sec))->mod_name;
        pp_out << ((ChunkVDM_TC_TABLE*) this->chunks.Get(sec))->footer;
        break;
      }
      case CHUNK_TYPE_BLOB : {
        pp_out << ((ChunkBLOB*) this->chunks.Get(sec))->blob;
        break;
      }
      default: break;
    }
  }
  pp_out.close();

  return true;
}

wstring RTFWordSpecFileHandler::give_old_name(SpecFile& sf)
{
  return sf.get_fname()+ L"_old.rtf";
}

bool RTFWordSpecFileHandler::set_default_font(SpecFile & sf)
{
  int fnum = RTFGetFontNumByCharset( sf.GetCharset() );
  if( fnum != -1 )
    setNonAsciiFontNum(fnum);
  return true;
}
#endif // VDMPP

//////////////////////////////////////////////////
//
//////////////////////////////////////////////////

const char * RTFWordSpecFileHandler::symbols[] = {"\\rtf", "{", "}", "\\deff", "\\stylesheet", NULL};

bool RTFWordSpecFileHandler::is_it_your_format(SpecFile & sf )
{
  return sf.look_for(symbols);
}

