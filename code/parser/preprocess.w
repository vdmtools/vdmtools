% This is a nuweb document to be processed by nuweb. -*- mode: text -*-

%% ID
%%   $Id: preprocess.w,v 1.15 2006/06/14 06:15:53 vdmtools Exp $
%% COPYRIGHT
%%   Kyushu University 2016

\documentclass{article}
\usepackage{toolbox}
\usepackage{latexsym}

\begin{document}

% Comment: at-sign is the command prefix and must be duplicated (@@)
% where is is needed verbatim. 

\docdef{A Preprocess class for separation of specification and documentation parts}
{The VDM-SL Tool Group}
{960918}
{preprocess.w}
{Code documentation}
{In developement}
{Restricted}
{\copyright\ Kyushu University}
{\item[V1.0] First version}
{}

\tableofcontents
\newpage

\section{Introduction}

This web-program defines a \verb|class Preprocess| whose purpose is to
read a VDM-SL specification file, which may or may not contain
\verb|\begin{vdm_al}| and \verb|\end{vdm_al}| sections, and split it
into a specification and a (perhaps empty) documentation part. The
class is based on a FLEX generated scanner which does the splitting.

This class replaces the functions texvdm2tex and texvdm2vdm from
mixedtexvdm.cc.

@o preprocess.l 
@{@<This file is generated@>
@<Code part@>
@<Flex options@>
@<Flex specification@>
@<File trailer@>
@}

@d Code part @{
%{ 
static char rcsid[] = "$Id: preprocess.w,v 1.15 2006/06/14 06:15:53 vdmtools Exp $";

#include <fstream>
#include <stdio.h> 
#ifndef _MSC_VER
#include <unistd.h>
#endif // _MSC_VER
#include <string>

#ifdef _MSC_VER
#define YY_NO_UNISTD_H 1
#endif // _MSC_VER

const char * texbreak = "##!!#!";

@<Preprocess definitions@>

%}

@}

\section{Class Preproces definition}

The constructor takes an input stream, two pointers to ifstreams and a
bool reference parameter. The istream 'i' is read and split into a
specification part and documentation part. The ifstream pointers can
be NULL in which case the parameter is ignored. If the pointers are
non-NULL the specification part can be read from specif and the
documentation part can be read from texif, after the constructor
returns. If anything goes wrong 'ok' will be set to false, otherwise
true. On destruction of the object, it will close the ifstreams and
unlink the temporary files they have been associated to.

% Output the following section to preprocess.h
@o preprocess.h @{
@<This file is generated@>

#ifndef __FLEX_LEXER_H
#include "FlexLexer.h"
#endif // __FLEX_LEXER_H
#include <string>
#include <fstream>

class Preprocess : public yyFlexLexer {
public: 
  Preprocess();
  Preprocess(std::istream & i, std::ifstream * specif, std::ifstream * texif);
  ~Preprocess(); 
  bool good() { return this->ok; };

private:
  void out(std::ostream & o, const char * s); 
  std::ofstream ofs1, ofs2; 
  std::string name1, name2; 
  bool ok;
private:
  int vdm_al_count;
private:
  int yylex(); 
};  
@}

\section{class Preprocess}

The Preprocess class has a constructor, a method out, and a destructor
which are defined below. 

\subsection{Preprocess constructor}

@d Preprocess definitions @{
#include "preprocess.h"
#include "tbutils.h"
#include "tb_wstring.h"


Preprocess::Preprocess(std::istream & i, std::ifstream * specif, std::ifstream * texif) : yyFlexLexer(&i, 0)
{
  this->ok = false; 
  @<Open temporary files@>
  this->vdm_al_count = 0; 
  yylex();
  @<Optional output of texbreak@>
  this->ofs1.flush();
  this->ofs2.flush(); 
  this->ofs1.close();
  this->ofs2.close(); 
  @<Open result files@>
  this->ok = true; 
}

@}

@d Open temporary files @{
this->name1 = TBWSTR::wstring2fsstr(TBUTILS::tb_tempnam(L"vdmd1")); 
this->name2 = TBWSTR::wstring2fsstr(TBUTILS::tb_tempnam(L"vdmd2")); 
if (this->name1.empty() || this->name2.empty()) return;
this->ofs1.open(this->name1.c_str()); 
if (!this->ofs1.good()) return;
this->ofs2.open(this->name2.c_str()); 
if (!this->ofs2.good()) return;
@}

The filter outputs a texbreak for every \verb|\begin{vdm_al}| to the
documentation file, and it outputs a texbreak to the specification
file for every \verb|\begin{vdm_al}| except the first. 

If there was no \verb|\begin{vdm_al}| i.e., $\verb|vdm_al_count|=0$ we
pretend there was anyway and output a texbreak to the, until now,
empty latex file \verb|ofs2| so the merge code will work as expected.

@d Optional output of texbreak @{
if (this->vdm_al_count == 0) {
  out(this->ofs2, texbreak); 
}
@}
Depending on whether we found any vdm\_al sections we find the
specification part in \verb|name2| or \verb|name1|.

@d Open result files @{
if (this->vdm_al_count == 0) {
  if (specif != NULL) 
    specif->open(this->name1.c_str());
  if (texif != NULL)
    texif->open(this->name2.c_str()); 
} else {
  if (specif != NULL) 
    specif->open(this->name2.c_str());
  if (texif != NULL)
    texif->open(this->name1.c_str());       
}
@}

\subsection{Method out.}

Output 's' to the stream 'o'.

@d Preprocess definitions @{
inline
void Preprocess::out(std::ostream & o, const char * s) 
{
  o.write(s, strlen(s)); 
}
@}

\subsection{The Preprocess destructor.}
 
Close the ifstreams, remove the temporary files and free the allocated
temporary file names.

@d Preprocess definitions @{
Preprocess::~Preprocess()
{
  if (!this->name1.empty()) {
    if (this->ofs1.is_open()) this->ofs1.close();
    unlink(this->name1.c_str()); 
  }
  if (!this->name2.empty()) {
    if (this->ofs2.is_open()) this->ofs2.close(); 
    unlink(this->name2.c_str()); 
  }       
}

@}

\section{Flex options}

The yyclass options results in a definition of Preprocess::yylex()
where the actions are in the scope of the class. 

@d Flex options @{
%option yyclass="Preprocess"
%option noyywrap
@}

\section{Scanner specification}

The scanner is created with an exclusive state 'invdm' that controls
whether we are inside or outside and \verb|vdm_al| section. We do not check
for bad placements of \verb|\begin{vdm_al}| or \verb|\end{vdm_al}|. 

The idea for the filter is to output 'inside' sections to ofs2 and
'outside' sections to 'ofs1'. 

@d Flex specification @{
/* Define exclusive state 'invdm'. */
%x invdm

%%
^[ \t]*"\\begin{vdm_al}"      { vdm_al_count++; 
                          out(ofs1, texbreak); 
                          if (vdm_al_count>1) {
                            out(ofs2, texbreak); 
                          }
                          BEGIN(invdm);  
                        }
<invdm>.                { // Output specification part. 
                          out(ofs2, yytext);
                        }
<invdm>\n               { // Output specification part. 
                          out(ofs2, yytext);
                          // Output \n to ofs1
                          // as the original texvdm2tex did
                          out(ofs1, yytext); 
                        }
<invdm>^[ \t]*"\\end{vdm_al}"  {
                           BEGIN(INITIAL);  
                         }  
.                       { 
                          out(ofs1, yytext); 
                        }
\n                      { 
                          out(ofs1, yytext); 
                          // Output newline to specfile to make
                          // the line count match
                          out(ofs2, yytext); 
                        }

%%
@}

@d This file is generated 
@{/* This file is generated from preprocess.w. Do not modify! */ @}

\section{Standalone test.}

@d File trailer @{ 
#ifdef STANDALONE
int main()
{
  std::ifstream i1, i2;
  std::istream i(cin); 
  std::cout.unsetf(ios::unitbuf); 
  std::cout.unsetf(ios::stdio); 
  bool b; 
  Preprocess prepro(i, &i1, &i2, b);
  std::cout << "VDM File:\n";
  char ch; 
  while (i1.get(ch)) {
    std::cout.put(ch);
  }
  std::cout << "TeX File:\n"; 
  while (i2.get(ch)) {
    std::cout.put(ch);
  }
  
  printf("count = %d\n", prepro.vdm_al_count); 
}
#endif

/** End of the lex-specification. ******************************/
@}

\end{document}
