/***
*  * WHAT
*  *    Handling of specification files in diferent
*  *    formats. Seen from the user of the SpecFile class
*  *    the format of the actual file will not be visible.
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/specfile.h,v $
*  * VERSION
*  *    $Revision: 1.26 $
*  * DATE
*  *    $Date: 2006/04/26 05:50:09 $
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

#ifndef __INCL_SPECFILE_H__
#define __INCL_SPECFILE_H__

#include <stdio.h>
#include <string.h>
#include <stringext.h>
#include <fstream>
#include "Errors.h"
#include "preprocess.h"
#include "tokenlist.h"
#include "contextinfo.h"
#include "chunks.h"
#ifdef VDMPP
#include "actionlist.h"
#include "AUML.h"
#include "ASTMERGE.h"
enum DummyFileId{VPPUML_DUMMY_FILE_ID};
#endif // VDMPP

///////////////////////////////////////////////
// The recognized specification file formats
// The order in the enumerated type indicates
// the order in which they will be tried out
// on the input file
///////////////////////////////////////////////

enum SpecFileFormatType {
  FORMAT_LATEX,      // Latex document with vdm_al ..
  FORMAT_RTF_WORD,   // RTF docuemnt written by MS Word or compatible
  FORMAT_PLAIN_TEXT, // Plain text ASCII format

  FORMAT_LAST,       // this must follow the parsed formats

  FORMAT_UNKNOWN,    // The format isnt recognized yet
  FORMAT_ERROR       // It hasn`t been possible to detect the format
};


//////////////////////////////////////
// Types
//////////////////////////////////////


//////////////////////////////////////
// The class SpecFileHandler handles the different parsers
//////////////////////////////////////

class SpecFile;
class SpecFileHandler;
class RTFWordSpecFileHandler;
class LatexSpecFileHandler;
class PlainTextSpecFileHandler;

//////////////////////////////////////
// SpecFileHandler
// Base class for the avaliable parsers for parsing
// specifications
// The main purpose of the base class is to handle
// the allocation of a parser to a given file
// Further more to declare the functions that must be
// implemented by the parsers
//////////////////////////////////////

class SpecFileHandler {
private :

  static LatexSpecFileHandler latex_sfh;
  static RTFWordSpecFileHandler rtf_sfh;
  static PlainTextSpecFileHandler pt_sfh;

  // Prevent the user of SpecFileHandler from copying a SpecFileHandler.
private:
  SpecFileHandler(const SpecFileHandler & ) {}
  void operator=(const SpecFileHandler &) {}

protected :
  //moved to contextinfo: TokenList tokens; // List of the vdm tokens contained in the file

  SpecFileFormatType format;  // the type of this parser

  // Pretty printer options

  bool generate_index; // index
  bool generate_rti; // test coverage

  // Should keywords and identifiers be L"pretty printed". This option
  // is necessary because rtf files with bold faced and italics text
  // inside VDM blocks are not parsed correctly if configured into the
  // project after they have been generated. This is a problem when
  // used with the UML Mapper.
  bool pp_key_and_ident;

#ifdef VDMPP
  // The list of actions used when merging two lists of tokens in the
  // function file_update()
  ActionList act_l;

  // The list of tokens for the new class added in UML
  ContextInfo new_ci;

  ActionList & get_action_list() { return act_l; }
#endif // VDMPP

  // Check if a given parser can read the contents of a given file
  // The parser will need read access to the file

  virtual bool is_it_your_format(SpecFile &) { return false; }

  bool parse_vdm_file(SpecFile &,    // the specfile we are dealing with
                      const string &,// spec to parse
                      Sequence &,    // AST that was read
                      ContextInfo& );// The contextinfo to update
#ifdef VDMSL
  Sequence ExtractDefinedFunctionNames (const Set & mds);
#endif // VDMSL

public :

  SpecFileHandler();
  virtual ~SpecFileHandler();

  // public functions that must be implemented by other parsers

  // return the name of the format of the document

  virtual const wchar_t * get_format_name() const { return L"UNKNOWN"; }

  virtual bool read(SpecFile & ) { return false; }
  virtual bool split(SpecFile &, bool = false) { return false; }
  virtual bool parse_vdm(SpecFile & , Sequence &, ContextInfo&) { return false; }
  virtual bool pp(SpecFile &, Sequence &, ContextInfo&) { return false; }

  // return the name of the pp output file
  virtual const wchar_t * pp_output_file_suffix() const { return L"unknown"; }

  virtual SpecFileHandler * dup() { return NULL; }

  // general public functions

  SpecFileFormatType get_format() { return format; }

  // Get hold of a parser for a given file
  // given an input stream to use for recogniztion of the
  // file

  static SpecFileHandler * get_parser(SpecFile &);

#ifdef VDMPP
public:
  // Update file with changes introduced in UML. If new classes are
  // introduced in UML new files will be created
  virtual bool file_update(SpecFile &, wstring &err) { return false; }

  // Used to create completely new specification files with:
  virtual bool file_create(SpecFile &, wstring &err) { return false; }

  virtual wstring give_old_name(SpecFile &){ return L""; }

  // This function creates the list of actions used to merge two lists
  // of tokens. The wstring is used to deliver any error messages
  TYPE_CI_TokenSpan start_and_end(const Record & node, ContextInfo & c_table);
  Map create_token_map(TokenList & tokl, int64_t from, int64_t to);
  Action * block_specifier_action(int ins_pos, const TYPE_ASTMERGE_AST_uNode & ast_node, ContextInfo & new_ci);
  int get_replace_end_pos(const TYPE_ASTMERGE_AST_uNode  &ast_node, ContextInfo & ci, TokenList & tokl);
  Int get_acc (const TYPE_ASTMERGE_AST_uNode& node);
  Bool get_stat (const TYPE_ASTMERGE_AST_uNode& node);
  int get_increment(const TYPE_ASTMERGE_AST_uNode& node);
  Generic get_parent_pre (TYPE_ASTMERGE_AST_uNode parent);
  bool extract_token_pos(ActionList & act_l,
                         TYPE_ASTMERGE_ActionList & acts,
                         ContextInfo & new_ci,
                         ContextInfo & old_ci,
                         TokenList & old_tokl);
  bool create_action_list(SpecFile &, const Sequence &, wstring &);
  bool parse_classes(const wstring &, ContextInfo &, Sequence &, int);
  virtual bool set_default_font(SpecFile& sf) { return true; }
#endif // VDMPP
};

//////////////////////////////////////
// The class SpecFile contains all the information that is necessary
// to handle a specification file
//////////////////////////////////////


////////////////////////////////////////
// Handling of a specifcation file
// From the user of a SpecFile object it is
// not visible in which format the specification file
// is stored. When an attempt is made to read the
// spec file the right parser will be found
//
// When a parser deals with the ifstream sp - it should
// new close the ifstream - that should only be handled
// by the SpecFile.
///////////////////////////////////////

class SpecFile {

protected:
  wstring short_name;        // short name for file
  wstring fname;             // the complete path to the file
  TYPE_ProjectTypes_FileName filename; // the name used in the VDM spec
  TYPE_ProjectTypes_FileId file_id;    // unique id for file in TB
  ifstream sp;              // input stream for reading the spec file
  ContextInfo *contextinfo;
  SpecFileHandler * parser; // the parser used for this spec file
#ifdef VDMPP
  bool file_created;        // True if this spec file represents a file created by the UML Link.
  bool file_success;        // True if it was possible to create a new file
  wstring merged_classes;    // The wstring representation of the merged class of this file
#endif // VDMPP
  wstring charset;          // character set
  wstring default_file;

protected:

  SpecFile();
  bool get_parser();

  bool close(); // close the file

public:

  SpecFile(const TYPE_ProjectTypes_FileName &, ContextInfo & );    // short name for file
  virtual ~SpecFile();

  ifstream & get_input_stream();  // get input stream for specification file

  wstring get_fname()      { return fname; }
  wstring get_short_name() { return short_name; }
  wstring get_pp_output_name() { return wstring(short_name)+pp_output_file_suffix(); }
  TYPE_ProjectTypes_FileName get_filename() { return filename; }
  TYPE_ProjectTypes_FileId get_file_id() { return file_id; }
  ContextInfo& get_contextinfo() { return *contextinfo; }

  void setTimestamp();
  bool init();  // initialize the file for reading
  bool rewind();  // rewind the file for input from the first position

  bool read();  // read the file
  bool split(bool = false); // split the file into chunks/or files
  bool parse_vdm(  // parse the vdm part and retrieve the AST
                 Sequence &     // The AST read
                 );
  bool pp();    // read and pretty print the spec file

  bool look_for(const char *[] ); // look for a list of symbols in the specfication

  const wchar_t * pp_output_file_suffix(); // get name of the pp output file suffix
  const wchar_t * get_format_name();       // get the name of the format of the spec file

  bool convertToUniChar(ifstream & inp, ostringstream & unichar);

  void SetCharset(const wstring & cs ) { this->charset = cs; }
  wstring & GetCharset() { return this->charset; }
  void SetDefaultFile(const wstring & df ) { this->default_file = df; }
  wstring & GetDefaultFile() { return this->default_file; }

#ifdef VDMPP
public:
  // The following ctor is used to create a specfile that is not
  // associated with a physical file. Used to handle classes
  // introduced in UML.
  SpecFile(const wstring & path, const wstring & name,
           const wstring & df, const wstring & cs, ContextInfo& ci);

  // Update the file
  bool file_update(wstring &, Sequence &);

  // Create new file:
  bool file_create(wstring &err);

  wstring get_merged_classes() { return this->merged_classes; };
  wstring give_old_name();

  void set_merged_classes(const wstring & s) { this->merged_classes = s; };

  void rtfnewclass(ofstream & o);
#endif // VDMLL

  friend class SpecFileHandler;
};

/////////////////////////////////////////////////
// ChunkSpecFileHandler
// Parser extended with facilities to handle
// chunks in sets
// Should be the base class for all new parsers
/////////////////////////////////////////////////

class ChunkSpecFileHandler : public SpecFileHandler {
protected:

  ChunkList chunks; // Chunks contained in the file

  friend class SpecFileHandler;
};

/////////////////////////////////////////////////
// RTFWordSpecFileHandler
/////////////////////////////////////////////////

class RTFWordSpecFileHandler : ChunkSpecFileHandler {

private:

  static const char * symbols[]; // sequence of synbols to look for
                                 // last symbol must be NULL
  static  bool parser_init;  // has the rtf parser been initiated

  ofstream pp_out;                // output stream for pretty printer

  static int nonAsciiFontNum;     // RTF font index used for non-ascii chars

  bool pp_tc_table(const string & mod_name,  // nodule/class to print
                   const Sequence & asts,
                   ContextInfo&);
  bool pp_section(SpecFile &,
                  int sec,        // section to print
                  bool * color_on,
                  TokenList& // coloring on
                 );
  bool pp_space(int num);
  bool pp_newline(int num);
  bool pp_keyword(const string&);
  bool pp_identifier(int, const string&);
  bool pp_ident( const string&);
  bool pp_token(const TokenInfo & tk);
  void setNonAsciiFontNum(int fn){ nonAsciiFontNum = fn; };
  static int getNonAsciiFontNum(){ return nonAsciiFontNum; };

  static void ConvertCharToRTF(const char, string&);
  static void ConvertHexQuadCharsToRTF(const string&, string&);

public:
  static void ConvertHexQuadStrToRTF(const string&, string&);

protected:

  RTFWordSpecFileHandler();
  ~RTFWordSpecFileHandler();

  const wchar_t * pp_output_file_suffix() const { return L".rtf"; }
  const wchar_t * get_format_name() const { return L"Word RTF"; }

  bool read(SpecFile &);
  bool split(SpecFile &, bool );
  bool parse_vdm(SpecFile &,Sequence &, ContextInfo &);
  bool pp(SpecFile &,Sequence &, ContextInfo &);
  SpecFileHandler * dup();
  bool is_it_your_format(SpecFile & );

#ifdef VDMPP
  bool file_update(SpecFile &, wstring &err);
  bool file_create(SpecFile &, wstring &err);
  bool update_section(SpecFile &, int, int &);
  bool create_section(SpecFile &, int);
  void pp_comment(const wstring&, const TokenInfo&);
  wstring give_old_name(SpecFile &);
  bool set_default_font(SpecFile&);
#endif // VDMPP

  friend class SpecFileHandler;
};

// LatexSpecFileHandler

class LatexSpecFileHandler : SpecFileHandler {

private:

  ifstream specif;       // stream used for spec part by prep
  ifstream docif;        // stream used for doc part by prep

  static const char * symbols[]; // sequence of synbols to look for
                           // last symbol must be NULL

protected:

  LatexSpecFileHandler();
  ~LatexSpecFileHandler();

  bool read(SpecFile &) { return true; } // read from the given file  // doesn`t do any thing

  bool split(SpecFile &, bool );   // should we only care about the VDM part

  bool parse_vdm(SpecFile &, Sequence & /* the resulting AST */, ContextInfo &);

  bool pp(SpecFile &,        // write the output to the given stream
          Sequence &,        // the AST of the spec
          ContextInfo &);

  SpecFileHandler * dup();

  const wchar_t * pp_output_file_suffix() const { return L".tex"; }
  const wchar_t * get_format_name() const { return L"Latex"; }

  bool is_it_your_format(SpecFile &);

#ifdef VDMPP
  bool file_update(SpecFile &, wstring &err);
  bool file_create(SpecFile &, wstring &err);
  wstring give_old_name(SpecFile &);
#endif // VDMPP

  friend class SpecFileHandler;
};


// PlainTextSpecFileHandler

class PlainTextSpecFileHandler : SpecFileHandler {

protected:

  PlainTextSpecFileHandler();
  ~PlainTextSpecFileHandler() {}

  SpecFileHandler * dup();

  bool read(SpecFile &) { return true; } // doesn`t do any thing
  bool split(SpecFile &, bool) { return true; }
  bool parse_vdm(SpecFile & , Sequence &, ContextInfo&);
  bool pp(SpecFile &, Sequence &, ContextInfo&);
  bool is_it_your_format(SpecFile & );

  const wchar_t * pp_output_file_suffix() const { return L".tex"; }
  const wchar_t * get_format_name() const { return L"Plain Text"; }

#ifdef VDMPP
  bool file_update(SpecFile &, wstring &err);
  bool file_create(SpecFile &, wstring &err);
  wstring give_old_name(SpecFile &);
#endif // VDMPP

friend class SpecFileHandler;
};

#endif // __INCL_SPECFILE_H__

