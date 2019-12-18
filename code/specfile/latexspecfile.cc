/***
*  * WHAT
*  *   Handling of a Latex specification file
*  *   Based on old Latex pp and Preprocessor
*  *   that uses TesBreak
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/specfile/latexspecfile.cc,v $
*  * VERSION
*  *    $Revision: 1.17 $
*  * DATE
*  *    $Date: 2006/06/23 05:34:20 $
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
#ifndef _MSC_VER
#include <unistd.h>
#endif // _MSC_VER
#include <stdlib.h>

#include "specfile.h"
#include "tbutils.h"

#include "mixedtexvdm.h"
#include "testcoverage.h"
#include "vdmpars.h"

#include "preprocess.h"

#include "projectval.h"
#include "ToolMediator.h"
#include "Interface.h"
#include "Errors.h"
#include "tools.h"
#include "tb_exceptions.h"
#include "genlatex.h"
#include "settings.h"
#include <sstream>

//////////////////////////
// Latex SpecFile Parser
//////////////////////////

LatexSpecFileHandler::LatexSpecFileHandler()
{
  this->format = FORMAT_LATEX;
}

LatexSpecFileHandler::~LatexSpecFileHandler()
{
  if (this->specif.is_open()) this->specif.close();
  if (this->docif.is_open()) this->docif.close();
}

SpecFileHandler * LatexSpecFileHandler::dup()
{
  LatexSpecFileHandler * s = new LatexSpecFileHandler();
  return s;
}

// Split the spec file in spec and vdm part
// each part is kept in a separate file
// Creates a Preprocess object to handle the preprocessing

bool LatexSpecFileHandler::split(SpecFile & sf, bool vdm_only)
{
  if (vdm_only) 
  {
    Preprocess prep(sf.get_input_stream(), &(this->specif), NULL);
    return prep.good();
  }
  else
  {
    Preprocess prep(sf.get_input_stream(), &(this->specif), &(this->docif));
    return prep.good();
  }
}

///////////////////////////////////
// LatexSpecFileHandler
// precondition : read and split
///////////////////////////////////

bool LatexSpecFileHandler::parse_vdm(SpecFile & sf , Sequence & asts, ContextInfo & ci )
{
  ostringstream unichar;
  if (!sf.convertToUniChar(this->specif, unichar))
    return false;
  return parse_vdm_file(sf, unichar.str(), asts, ci);
}

/////////////////////////
// LatexSpecFileHandler: 
// precondition = The SpecFile has been read, split
//
// asts:
// The asts is the generated as a result of parsing the
// file for pretty printing.  That is, this asts are not
// the one that are contained in the specification
// library. That is, the contexids at the nodes in the
// asts are not the one that points to the current context
// information table.
/////////////////////////

bool LatexSpecFileHandler::pp(SpecFile & sf, Sequence & asts, ContextInfo & ci)
{
  // Files in use
  // temp. file, used for holding test coverage info
  wstring test_cov_file (TBUTILS::tb_tempnam (L"vdmd4"));
  // temp. latex file - generated latex file before merge 
  wstring gen_latex_file (TBUTILS::tb_tempnam (L"vdmd5"));
  // latex output file
  wstring output_file (sf.get_pp_output_name());
  // short name of the file to pretty print
  wstring short_file_name (sf.get_short_name());

  //////////////////
  /// Get the asts in the specification manager corresponding to
  /// the specfile
  //////////////

  Set sp_asts (ToolMediator::GetAstsOfFileName(sf.get_filename()));

  if (sp_asts.IsEmpty()) {
    vdm_log << L"Could not read spec" << endl << flush;
    ToolMediator::UpdatePP (sf.get_filename(), Bool (false));
    return false;
  }

  // Get hold of Test coverage info output to file <fname>.tmp3 - if any
  
  if (!TestCoverage::texvdmrti2texvdm (this->docif, test_cov_file, sp_asts, ci, short_file_name)) {
    vdm_log << L"Could not read test coverage information" << endl << flush;
    TBUTILS::remove_file(test_cov_file);
    ToolMediator::UpdatePP (sf.get_filename(), Bool (false)); // cast ???
    return false;
  }
  
  ///////////////////////
  // Extract the names of the defined functions from the spec
  // Used to generate index.
  ///////////////////////

#ifdef VDMSL    
  Sequence mds (ExtractDefinedFunctionNames (sp_asts)); // SEQ OF SET OF (OP|FUN)
#endif // VDMSL
#ifdef VDMPP
  Sequence mds;  // ??? FOR VDMPP
#endif // VDMPP

  ///////////////////////
  // Generate Latex
  ///////////////////////

  try {
    GenLatexOutput lo(gen_latex_file, 
                      sp_asts,              // the modules/definitions
                      Settings.GetIndex(),  // should an index be generated
                      mds,                  // list of set of (ops|fcts)
                      sf.get_fname(),       // the name of the file to pp
                      ci);                  // the context info
  }
  catch (TB_Exception& e) {

    vdm_err << L"Internal Error" << endl << flush;
        
        // remove temporary files
    TBUTILS::remove_file(gen_latex_file);
    TBUTILS::remove_file(test_cov_file);

    ToolMediator::UpdatePP (sf.get_filename(), Bool (false));
    return false;
  }

  ////////////////////////////////////////////////
  // Merge the generated Latex with the text part of the document
  // and save the result in <fname>.tex
  ////////////////////////////////////////////////

  if (!MIXEDTEXT::vdmmerge (test_cov_file, gen_latex_file, output_file)) {
    //      vdm_log << L"Some error occurred when executing vdmmerge" << endl;

    TBUTILS::remove_file(gen_latex_file);
    TBUTILS::remove_file(test_cov_file);
      
    ToolMediator::UpdatePP (sf.get_filename(), Bool (false));
    return false;
  }

  // CLEANUP
  
  TBUTILS::remove_file(gen_latex_file);
  TBUTILS::remove_file(test_cov_file);
  return true;
}

const char * LatexSpecFileHandler::symbols[] = {"\\begin", "{", "}", "vdm_al", "\\end", NULL};

bool LatexSpecFileHandler::is_it_your_format(SpecFile & sf )
{
  return sf.look_for(symbols);
}

#ifdef VDMPP
bool LatexSpecFileHandler::file_update(SpecFile &sf, wstring & err)
{
/*
  TBUTILS::remove_file(sf.give_old_name());  // Remove old copy.
  TBUTILS::file_copy(sf.give_old_name(), sf.get_fname());

  ofstream pp_out;
  pp_out.open(TBWSTR::wstring2fsstr(sf.get_fname()).c_str(),ios::out);
  if (!pp_out.good())
  {
    err = L"Could not open the file for writing.";
    return false;
  }
  pp_out << TBWSTR::wstring2mbstr(sf.get_merged_classes());
  pp_out.close();
  return true;
*/
  err = L"Update for tex format is't supported.";
  return false;
}

bool LatexSpecFileHandler::file_create(SpecFile &sf, wstring & err)
{
/*
  ofstream pp_out;
  pp_out.open(TBWSTR::wstring2fsstr(sf.get_fname()).c_str(),ios::out);
  if (!pp_out.good())
  {
    err = L"Could not open the file for writing.";
    return false;
  }
  pp_out << TBWSTR::wstring2mbstr(sf.get_merged_classes());
  pp_out.close();
  return true;
*/
  err = L"Create for tex format is't supported.";
  return false;
}

wstring LatexSpecFileHandler::give_old_name(SpecFile &sf)
{
  return sf.get_fname() + L"_old.tex";
}
#endif // VDMPP

