%%%%
%  % WHAT
%  %
%  % Id
%  %    $Id: contextinfo.w,v 1.53 2006/07/03 05:19:19 vdmtools Exp $
%  % SPECIFICATION VERSION
%  %    contextinfo.vdm 1.14
%%%%
% This is a nuweb document to be processed by nuweb. -*- mode: text -*-

%% ID
%%   $Id: contextinfo.w,v 1.53 2006/07/03 05:19:19 vdmtools Exp $
%% COPYRIGHT
%%   Kyushu University 2016

\documentclass{article}
\usepackage{toolbox}
\usepackage{latexsym}

\begin{document}

% Comment: at-sign is the command prefix and must be duplicated (@@)
% where it is needed verbatim.

\docdef{ContextInfo Implementation}
{The VDM-SL Tool Group}
{980126}
{contextinfo.w}
{Code documentation}
{In development}
{Restricted}
{\copyright\ Kyushu University}
{\item[V1.0] First version}
{}

\tableofcontents
\newpage

\section{Introduction}

This web-program contains the implementation of the ContextInfo module
as specified in the astdoc.


\section{Predefined FileId's}
\label{sec:predefids}

The implementation has the predefined fileids, which the SpecMan is
supposed not to use. We start allocating fileids from 0.

@d reserved fileids @{

enum ci_reserved_fid {
  CI_FID_NOT_FOUND = 0,         // Repository::GetFileId returns this
  CI_FID_NO_CI   = -1,
  CI_FID_CMD_LINE  = 1,
  CI_FID_CG,
  CI_FID_TC,
  CI_FID_TMP_FILE,
  CI_FID_STDIN,
};

@}

@O contextinfo.h -d @{

#ifndef __contextinfo_h__
#define __contextinfo_h__
@<This file is generated@>
@<Header part@>
@<reserved fileids@>
@<Context declaration@>
#endif // __contextinfo_h__
@}

@D Header part @{

#include "metaiv.h"
#include "CI.h"
#include "tokenlist.h"
#include <deque>
#include <fstream>
#include <string>

//extern const TYPE_CI_ContextId NilContextId;
#define nilcontextid -1
#define NilContextId Int(nilcontextid)

@}

\section{class ContextInfo}

The intended use of the Context class is like this
\begin{itemize}
\item Use \verb-UseContextTab(fileid, true)- to reset the associated table for the given fileid.
\item Or use \verb-Push(fileid)- to push an empty context. Use the Set methods on the returned cid.
\end{itemize}

@d Context declaration @{

@<Context Publics@>
@<Context Privates@>

@}

@d Context Publics @{

class cntxt_file_info;

class cntxt_file_info_p
{
public:
  cntxt_file_info_p(cntxt_file_info *cp) : p(cp) { }
  cntxt_file_info_p() : p(NULL) { }
  cntxt_file_info_p(const cntxt_file_info_p& c) : p(c.p) { }
  cntxt_file_info * operator->() { return p; }
  cntxt_file_info * p;
};

//typedef vector<cntxt_file_info_p> cntxt_file_tab_t;
typedef deque<cntxt_file_info_p> cntxt_file_tab_t;

class ContextInfo {
public:
  ContextInfo() { this->cur_fid_max = 0; };
  ~ContextInfo();

  void clear();

  Generic GetTypeInfo(const TYPE_CI_ContextId &);
  void SetTypeInfo(const TYPE_CI_ContextId &, const Generic&);
  bool HasTypeInfo(const TYPE_CI_ContextId &);

  Map  GetPidM(const TYPE_CI_ContextId &);
  void SetPidM(const TYPE_CI_ContextId &, const Map&);
  bool HasPidM(const TYPE_CI_ContextId &);

  //bool IsTypeChecked(); ???
  Generic GetPos(const TYPE_CI_ContextId &);
  void SetPos(const TYPE_CI_ContextId &, const TYPE_CI_TokenSpan &);
  Tuple GetFileLineColPos(const TYPE_CI_ContextId & cid);
  Tuple GetFilePos(const TYPE_CI_ContextId &);

  int GetTestCoverageInfo(const TYPE_CI_ContextId &);
  void IncTestCoverageInfo(const TYPE_CI_ContextId &);

  void ResetTestCoverageInfo();
  bool HasTestCoverage(const TYPE_CI_ContextId &);

  void UseContextTab(const TYPE_CI_FileId & fid, bool reset);

  TYPE_CI_ContextId Push(const TYPE_CI_FileId & fid);

  void ResetCGTable() {UseContextTab(TYPE_CI_FileId(CI_FID_CG), true);}
  void ResetTCTable() {UseContextTab(TYPE_CI_FileId(CI_FID_TC), true);}

  TYPE_CI_ContextId PushCGPosType(const TYPE_CI_TokenSpan &, const Generic &);
  TYPE_CI_ContextId PushCGType(const Generic &);

  TokenList & GetTokenList(const TYPE_CI_FileId & fid);

  void SaveCounters(const wstring &s, const Map & num_map, wostream & log);
  void LoadCounters(const wstring &s, const Map & long_names, const Map & short_names, wostream & log, bool);

  Tuple GetCidAtPos(const SEQ<Char> & file, const Int & line, const Int & col);
  bool IsCidBreakable(const TYPE_CI_ContextId & cid);
  void SetBreakable(const TYPE_CI_ContextId & cid);
  void SetBreakpoint(const TYPE_CI_ContextId & cid);
  void RemoveBreakpoint(const TYPE_CI_ContextId & cid);
  bool IsBreakpointAtCid(const TYPE_CI_ContextId & cid);

  Generic GetFidForFile(const SEQ<Char> & file);

  Sequence getGUITokenInfo(const SEQ<Char> & file);
  void DumpTokenList(const TYPE_CI_FileId & fid, wostream& wos);
  void DumpNodeList(const TYPE_CI_FileId & fid, wostream& wos);
  void AdjustPosInfoForMultibyteText(const TYPE_CI_FileId & fid);
  bool CheckToken(const TYPE_CI_ContextId & cid, const wstring & name);
  Generic GetContextNodeInfo(const TYPE_CI_ContextId& cid);
  Generic GetTokenInfos(const SEQ<Char> & file);
  Generic GetContextNodeInfos(const SEQ<Char> & file);
  bool IsAValidFileId(const TYPE_CI_FileId & fid) const;
  bool IsAValidContextId(const TYPE_CI_ContextId & cid);

  static TYPE_CI_ContextId CreateCid(int64_t fid, int64_t nid);
  static Tuple SplitCid(const TYPE_CI_ContextId & cid);

  Tuple GetSize(const TYPE_CI_FileId & fid);
@}

@d Context Privates @{

private:
  cntxt_file_tab_t tll_ci;
  int64_t cur_fid_max;
  static TokenList nulltk;

  bool IsAValidContextId(const TYPE_CI_ContextId & cid, TYPE_CI_ContextNodeInfo * & cnip);
  void resizetab(int64_t fid);

  void complain(const wstring&, int64_t number);
  void print_counter_line(ostream &f, const wstring &s, int64_t fid);

  Tuple FindPosInContextInfo(const SEQ<TYPE_CI_ContextNodeInfo> & ci, const TokenList &tll, int line, int col);
  bool PointCloser(int sp_x, int sp_y, int cp_x, int cp_y, int ca_x, int ca_y) const;
  int64_t cid2int(const TYPE_CI_ContextId & c) const;

public:
  // For test purposes
  int64_t get_ftab_size() { return tll_ci.size(); }
  int64_t get_itab_size(int64_t fid);
  void pp(ostream & os, bool vdm_format);
};

ContextInfo& GetCI();
@}

\section{Class definition}

@O contextinfo.cc -d @{

@<This file is generated@>

#include "binops.h"
#include "tb_wstring.h"
#include "intconvquotes.h"
#include <iterator>

#define node_bits          24
#define MaxNid     0x01000000 // (1 << node_bits);
#define MaxNidMask 0x00FFFFFF // MaxNid - 1
#define MaxFid     0x00000400 // (1 << (34 - node_bits));
#define MaxFidMask 0x000003FF // MaxFid - 1

#include "contextinfo.h"
#include "vdmpars.h" // for GetTokenText
#include "tools.h"   // for GetFileMapNameNum, GetFileMapNumName

@<class cntxt_file_info@>

TokenList ContextInfo::nulltk;
static ContextInfo global_context_info;

@<ContextInfo methods@>

@}

The \verb-cntxt_file_info- struct contains a list of
ContextInfoNode's, a list of tokens and a boolean \verb-has_coverage-
that is true iff the LoadCounter method has modified the counters in
\verb-node_l-. The flag is reset by ResetTestCoverageInfo() and when
\verb-UseContextTab- is called with true second parameter.

@d class cntxt_file_info @{

class cntxt_file_info
{
public:
  SEQ<TYPE_CI_ContextNodeInfo> node_l;
  TokenList token_l;
  bool has_coverage;

  cntxt_file_info() : has_coverage(false) { }
};

@}

@d ContextInfo methods @{

ContextInfo::~ContextInfo()
{
  clear();
}

void ContextInfo::clear()
{
  cntxt_file_tab_t::iterator i;
  for (i = tll_ci.begin(); i != tll_ci.end(); ++i) {
    (*i).p->node_l.Clear();
    (*i).p->token_l.Clear();
    delete (*i).p;
  }
  tll_ci.erase(tll_ci.begin(), tll_ci.end());

  this->cur_fid_max = 0;
}

@}


\section{IO of test coverage info}
\label{sec:IOrti}

The format of a test coverage file is like this:
\begin{verbatim}
version: <num>
file: <filename1>
counters: <size> 0 1 0 0 ...
file: <filename2>
counters: <size2> 10 0 0 10 ...
...
end
\end{verbatim}

The first lines contains the magic marker VDMRTI and the version
number of the file format.

The file names are output in sorted order. The names may contain
strange characters ($<=32$ in which case these are output in
quoted-printable format (e.g., '=FE'). The file names may contain a
leading relative or absolute path.

A map (from int to filename) must be given to the in and out methods.
When reading a test coverage file its filenames are compared first
with the full file name, then with its base file name in the map
range. If exactly one entry matches, it is accepted, otherwise the
file name is ignored and a error wstring is set to signify this.

It should be possible to concatenate more test coverage files and
achieve a summation of their counters.

Method SaveCounters print the entries in the ContextInfo list whose
node\_l is non-empty and whose file id can be mapped to a file name by
the given function pointer.

@d ContextInfo methods @{

void ContextInfo::SaveCounters(const wstring & s, const Map & num_map, wostream & log)
{
  ofstream f (TBWSTR::wstring2fsstr(s).c_str());
  if (!f) {
    log << L"Couldn't create file `" << s << L"'" << endl;
    return;
  }
  for (size_t i = 0; i < this->tll_ci.size(); i++) {
    if ((this->tll_ci[i]->node_l.Length() > 0) && num_map.DomExists(Int(i))) {
      Sequence fnm (num_map[Int(i)]);
      wstring fname (fnm.GetString());
      print_counter_line(f, fname, i);
    }
  }
  f.flush();
  f.close();
}
@}

Method LoadCounters prints the entries in the ContextInfo list whose
node\_l is non-empty and whose file id can be mapped to a file name by
the given Maps. They map (seq of wchar_t to int) file names to their
fileids. The first map contains the full names and the second map the
base file names.
Only the file in the maps are printed into the ContextInfo list.
If the {\em all} flag is set a warning is given if the test coverage
file contains file names that are not contained in the two maps.

The method sets true the \verb-has_coverage- flags for the files read
in.

@d ContextInfo methods @{

#include "tbutils.h"

void ContextInfo::LoadCounters(const wstring & s, const Map & long_names, const Map & short_names_,
                               wostream & log, bool all)
{
  Map short_names (short_names_); 
  ifstream f (TBWSTR::wstring2fsstr(s).c_str());
  if (!f) {
    log << L"Could not open file: `" << s << L"'" << endl;
    return;
  }

  string fn;
  while (!f.eof()) {
    string t;
    f >> t;                     // Read field name.
    if (t.compare("file:") != 0) {
      log << s << L": missing `file:' keyword:`" << TBWSTR::string2wstring(t) << L"'" << endl;
      return;
    }
    (void) f.get();             // Skip one space after file:
    getline(f, fn);             // Read file name.
    // If we read a windows generated coverage file on unix there
    // will be a \r as the last character on the line.
    if ((fn.length() > 0) && (fn[fn.length()-1] == '\r')) {
      fn.erase(fn.length()-1, 1);
    }
    {
      // transform =xx to a single wchar_t
      string res;
      for (string::size_type i = 0; i < fn.size(); ) {
        if ((fn[i] == '=') && (i+2 < fn.size())) {
          string buf;
          buf += fn[i+1];
          buf += fn[i+2];
          unsigned int h;
          sscanf(buf.c_str(), "%x", &h);
          res += (char) h;
          i += 3;
        }
        else {
          res += fn[i];
          i++;
        }
      }
      fn = res;
    }
    SEQ<Char> short_fn (TBUTILS::tb_getbasename(TBWSTR::string2wstring(fn)));
    Generic g;
    bool found = (long_names.DomExists(Sequence(TBWSTR::string2wstring(fn)), g) ||
                  short_names.DomExists(short_fn, g));
    f >> t;
    if (t.compare("counters:") != 0) {
      log << s << L": missing `counters:' keyword :`" << TBWSTR::string2wstring(t) << L"'" << endl;
      return;
    }
    unsigned int len = 0;
    f >> len;
    // log << L"read len=" << len << endl;
    deque<int> v;
    istream_iterator<int> i(f), end;
    for ( ; !(i == end); ++i) {
        v.push_back(*i);
    }
    if (found) {
      if( short_names.DomExists(Sequence(short_fn)) ) {
        short_names.RemElem(Sequence(short_fn));
      }
      int64_t fid = Int(g).GetValue();
      if (fid >= (int64_t)(this->tll_ci.size())) {
        log << L"Internal inconsistence in CI file map" << endl;
        return;
      }
      if (len != this->tll_ci[fid]->node_l.Length()) {
        log << L"Ignored coverage info from file `" << TBWSTR::string2wstring(fn) << L"' "
            << L"since it is inconsistent with the syntax checked file." << endl;
      }
      else {
        this->tll_ci[fid]->has_coverage = true;
        // Copy the counters into the node_l list.
        deque<int>::const_iterator first = v.begin(), last=v.end();
        SEQ<TYPE_CI_ContextNodeInfo> cni_s (this->tll_ci[fid]->node_l);
        int index = 1;
        for (; !(first == last); ++first, ++index) {
          TYPE_CI_ContextNodeInfo cni (cni_s[index]);
          cni.set_coverage(Int(cni.GetIntValue(pos_CI_ContextNodeInfo_coverage) + *first));
          this->tll_ci[fid]->node_l.ImpModify(index, cni);
        }
      }
    }
    else {
      if (all) {
        log << L"Ignored coverage info from unknown file: `" << TBWSTR::string2wstring(fn) << L"'" << endl;
      }
    }
    //log << L"read size=" << v.size() << endl;
    //log << L"f.flags=" << f.rdstate() << endl;
    f.clear(f.rdstate() & ~ios::failbit);
    //log << L"f.flags=" << f.rdstate() << endl;
  }
  f.close();

  if (! short_names.IsEmpty()) {
    Set dom_short_names (short_names.Dom());
    if (short_names.Size() == 1) {
      Generic g (dom_short_names.GetElem());
      log << L"Could not find coverage info for file: " << g.ascii() << endl;
    }
    else {
      log << L"Could not find coverage info for the files: ";
      Generic g;
      bool first (true);
      for (bool bb = dom_short_names.First(g); bb; bb = dom_short_names.Next(g)) {
        if (first) {
          first = false;
        }
        else {
          log << L", ";
        }
        log << g.ascii();
      }
    }
  }
}

@}

Define a function object whose purpose is to return the testcoverage
field of a ContextNodeInfo.

@d ContextInfo methods @{
@}


@d ContextInfo methods @{
void ContextInfo::print_counter_line(ostream & f, const wstring & ws, int64_t fileid)
{
  f << "file: ";
  string s (TBWSTR::wstring2string(ws));
  for (string::size_type i = 0; i < s.length(); i++) {
    if ((unsigned char)s[i] <= ' ' || s[i] == '=') {
      char b[1+2+1];
      sprintf(b, "=%02x", (unsigned int)s[i]);
      f << b;
    }
    else {
      f << s[i];
    }
  }
  f << endl;
  f << "counters:";
  SEQ<TYPE_CI_ContextNodeInfo> cni_l (this->tll_ci[fileid]->node_l);
  // output list of testcoverage field.
  size_t len_cni_l = cni_l.Length();
  f << " " << len_cni_l << " ";
  for (size_t idx = 1; idx <= len_cni_l; idx++) {
    f << cni_l[idx].GetIntValue(pos_CI_ContextNodeInfo_coverage) << " ";
  }
  f << endl;
}
@}

@d ContextInfo methods @{
// CreateCid
// fid : FileId
// nid : NodeId
// ==> ContextId
TYPE_CI_ContextId ContextInfo::CreateCid(int64_t fid, int64_t nid)
{
  return TYPE_CI_ContextId ((fid << node_bits) + nid);
}

// SplitCid
// cid : ContextId
// -> FileId * NodeId
Tuple ContextInfo::SplitCid(const TYPE_CI_ContextId & cid)
{
  int64_t ci = cid.GetValue();
  return mk_(TYPE_CI_FileId((ci >> node_bits) & MaxFidMask), TYPE_CI_NodeId((ci & MaxNidMask)));
}

// IsAValidFileId
// fid : FileId
// ==> bool
bool ContextInfo::IsAValidFileId(const TYPE_CI_FileId & fileid) const
{
  int64_t fid = fileid.GetValue();
  return ( 0 <= fid ) && ( fid < this->cur_fid_max );
}

// IsAValidContextId
// cid : ContextId
// ==> bool
bool ContextInfo::IsAValidContextId(const TYPE_CI_ContextId & cid)
{
  if ( cid != NilContextId ) {
    int64_t ci = cid.GetValue();
    int64_t fid = ci >> node_bits;
    int64_t nid = ci & MaxNidMask;
    return ((fid < this->cur_fid_max) && (nid <= this->tll_ci[fid]->node_l.Length()));
  }
  return false;
}

// IsAValidContextId (not in spec)
// cid : ContextId
// ==> bool * ContextNodeInfo
bool ContextInfo::IsAValidContextId(const TYPE_CI_ContextId & cid, TYPE_CI_ContextNodeInfo * & cp)
{
  if ( cid != NilContextId ) {
    int64_t ci = cid.GetValue();
    int64_t fid = ci >> node_bits;
    int64_t nid = ci & MaxNidMask;

    if ((fid < this->cur_fid_max) && (nid <= this->tll_ci[fid]->node_l.Length()) ) {
      cp = &(this->tll_ci[fid]->node_l.GetIndexRef(nid));
      return true;
    }
  }
  return false;
}

// complain
// s : seq of char
// number : int
void ContextInfo::complain(const wstring & s, int64_t number)
{
  ios::fmtflags f = cerr.flags();
  wcerr.setf(ios::uppercase);
  wcerr << s << L": " << number << " 0x" << hex << number << endl;
  wcerr.flags(f);
}

// Push
// fid : FileId
// ==> ContextId
TYPE_CI_ContextId ContextInfo::Push(const TYPE_CI_FileId & fileid)
{
  if (IsAValidFileId(fileid)) {
    TYPE_CI_ContextNodeInfo cni;
    cni.Init(Nil(), Int(0), Nil(), Nil(), Int(0), Bool(false), Bool(false));

    int fid = fileid.GetValue();
    this->tll_ci[fid]->node_l.ImpAppend(cni);
    return CreateCid(fid, this->tll_ci[fid]->node_l.Length());
  }
  else {
    complain(L"ContextInfo::Push", fileid.GetValue());
    return NilContextId;
  }
}

// Resize to proper size.

// resizetab
// fileid : int
void ContextInfo::resizetab(int64_t fileid)
{
  if ((int64_t)(this->tll_ci.size()) < fileid + 1) {
    while ((int64_t)(this->tll_ci.size()) < fileid + 1) {
      this->tll_ci.push_back (cntxt_file_info_p (new cntxt_file_info()));
    }
  }
  this->cur_fid_max = this->tll_ci.size();
}

// GetTokenList
// fileid : CI`FileId
// ==> TokenList
TokenList& ContextInfo::GetTokenList(const TYPE_CI_FileId & fileid)
{
  if (IsAValidFileId(fileid)) {
    return tll_ci[fileid.GetValue()]->token_l;
  }
  else {
    complain(L"ContextInfo::GetTokenList", fileid.GetValue());
    return nulltk;
  }
}

// UseContextTab
// fileid : FileId
// reset : bool
void ContextInfo::UseContextTab(const TYPE_CI_FileId & fileid, bool reset)
{
  // If fileid is not in range then extend the file table to the
  // proper size.
  int fid = fileid.GetValue();
  if (fid < 0 || MaxFid <= fid) {
    complain(L"UseContextTab: invalid fileid", fid);
    return;
  }

  resizetab(fid);

  if (reset) {
    // Now tab[fid] is a valid entry. Clear it.
    tll_ci[fid].p->node_l.Clear();
    tll_ci[fid].p->token_l.Clear();
    tll_ci[fid].p->has_coverage = false;
  }
}

int64_t ContextInfo::cid2int(const TYPE_CI_ContextId & c) const
{
  return c.GetValue();
}

// GetTypeInfo
// cid : ContextId
// ==> [REP`TypeRep]
Generic ContextInfo::GetTypeInfo(const TYPE_CI_ContextId & cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    return p->get_typeinfo();
  }
  else {
    complain(L"GetTypeInfo: invalid cid", cid2int(cid));
    return Nil();
  }
}

// SetTypeInfo
// cid : ContextId
// type : REP`TypeRep
void ContextInfo::SetTypeInfo(const TYPE_CI_ContextId & cid, const Generic& type)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    p->set_typeinfo(type);
  }
  else {
    complain(L"SetTypeInfo: invalid cid", cid2int(cid));
  }
}

// HasTypeInfo
// cid : ContextId
// ==> bool
bool ContextInfo::HasTypeInfo(const TYPE_CI_ContextId & cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    return !p->get_typeinfo().IsNil();
  }
  else {
    complain(L"HasTypeInfo: invalid cid", cid2int(cid));
    return false;
  }
}

// ResetTestCoverageInfo
void ContextInfo::ResetTestCoverageInfo()
{
  for (int64_t fid = 0; fid < this->cur_fid_max; fid++) {
    tll_ci[fid]->has_coverage = false;
    SEQ<TYPE_CI_ContextNodeInfo> cni_l (tll_ci[fid]->node_l);
    for (int64_t nid = 1; nid <= cni_l.Length(); nid++) {
      TYPE_CI_ContextNodeInfo cni (cni_l[nid]);
      cni.set_coverage(Int(0));
      tll_ci[fid]->node_l.ImpModify(nid, cni);
    }
  }
}

// HasTestCoverage
// cid : CI`ContextId
// ==> bool
bool ContextInfo::HasTestCoverage(const TYPE_CI_ContextId & cid)
{
  if (IsAValidContextId(cid)) {
    Tuple sc (SplitCid(cid));
    return tll_ci[sc.GetIntValue(1)]->has_coverage;
  }
  if (cid != NilContextId) {
    complain(L"HasTestCoverage: invalid cid", cid2int(cid));
  }
  return false;
}

// GetTestCoverageInfo
// cid : ContextId
// ==> TestCoverageInfo
int ContextInfo::GetTestCoverageInfo(const TYPE_CI_ContextId & cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    return p->get_coverage().GetValue();
  }
  complain(L"GetTestCoverageInfo: invalid cid", cid2int(cid));
  return 0;
}

// IncTestCoverageInfo
// cid : ContextId
void ContextInfo::IncTestCoverageInfo(const TYPE_CI_ContextId & cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    p->set_coverage(p->get_coverage().Incr());
  }
  else {
    complain(L"IncTestCoverageInfo: invalid cid", cid2int(cid));
  }
}

// GetPidM
// cid : ContextId
// ==> map AS`Name to REP`TypeRep
Map ContextInfo::GetPidM(const TYPE_CI_ContextId & cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    return p->get_pid_um();
  }
  complain(L"GetPidM: invalid cid", cid2int(cid));
  return Map();
}

// SetPidM
// cid : ContextId
// g : map AS`Name to REP`TypeRep
void ContextInfo::SetPidM(const TYPE_CI_ContextId & cid, const Map& pidm)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    p->set_pid_um(pidm);
  }
  else {
    complain(L"SetPidM: invalid cid", cid2int(cid));
  }
}

// HasPidM
// cid : ContextId
// ==> bool
bool ContextInfo::HasPidM(const TYPE_CI_ContextId & cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    return !p->get_pid_um().IsNil();
  }
  complain(L"HasPidM: invalid cid", cid2int(cid));
  return false;
}

// GetFileLineColPos
// cid : ContextId
// ==> seq of char * Line * Column
Tuple ContextInfo::GetFileLineColPos(const TYPE_CI_ContextId & cid)
{
  Tuple gfp (GetFilePos(cid));
  if (gfp.GetBoolValue(1)) {
    const TYPE_CI_FileId & fid (gfp.GetInt(2));
    const TYPE_CI_TokenPos & pos_ast (gfp.GetRecord(4));

    Map m (ToolMediator::GetFileMapNumName());
    Generic fileName;
    if (m.DomExists(fid, fileName)) {
      return mk_(fileName, pos_ast.GetInt(pos_CI_TokenPos_abs_uline), pos_ast.GetInt(pos_CI_TokenPos_column));
    }
  }
  if (cid != NilContextId) {
    complain(L"GetFileLineColPos: invalid cid", cid2int(cid));
  }
  return mk_(Nil(), Nil(), Nil());
}

// GetPos
// cid : ContextId
// ==> [TokenSpan]
Generic ContextInfo::GetPos(const TYPE_CI_ContextId & cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    return p->get_tokenpos();
  }
  complain(L"GetPos: invalid cid", cid2int(cid));
  return Nil();
}

// GetFilePos
// cid : ContextId
// ==> bool * FileId * TokenPos * TokenPos * TokenPos
Tuple ContextInfo::GetFilePos(const TYPE_CI_ContextId & cid)
{
  TYPE_CI_ContextNodeInfo * cp = NULL;
  if (IsAValidContextId(cid, cp)) {
    Generic tsg (cp->get_tokenpos());
    if (!tsg.IsNil()) {
      TYPE_CI_TokenSpan ts (tsg);

      Tuple sc (SplitCid(cid));
      const TYPE_CI_FileId & fileid (sc.GetInt(1));
      TokenList & tll = tll_ci[fileid.GetValue()].p->token_l;
      return mk_(Bool(true),
                 fileid,
                 tll.Get(ts.GetIntValue(pos_CI_TokenSpan_token_ust)).get_pos_st(),
                 tll.Get(ts.GetIntValue(pos_CI_TokenSpan_token_uast)).get_pos_st(),
                 tll.Get(ts.GetIntValue(pos_CI_TokenSpan_token_uend)).get_pos_end());
    }
  }
  if (cid != NilContextId) {
    complain(L"GetFilePos: invalid cid", cid2int(cid));
  }
  return mk_(Bool(false), Nil(), Nil(), Nil(), Nil());
}

// SetPos
// cid : CI`ContextId
// ts : CI`TokenSpan
void ContextInfo::SetPos(const TYPE_CI_ContextId & cid, const TYPE_CI_TokenSpan & ts)
{
  TYPE_CI_ContextNodeInfo * cp = NULL;
  if (IsAValidContextId(cid, cp)) {
    cp->set_tokenpos(ts);
  }
  else {
    complain(L"SetPos: invalid cid", cid2int(cid));
  }
}

// PushCGPosType
// pos : [TokenSpan ]
// type : [REP`TypeRep]
// ==> ContextId
TYPE_CI_ContextId ContextInfo::PushCGPosType(const TYPE_CI_TokenSpan & pos, const Generic & type)
{
  TYPE_CI_ContextId cid (Push(CI_FID_CG));
  SetTypeInfo(cid, type);
  SetPos(cid, pos);
  return cid;
}

// PushCGType
// type : [REP`TypeRep]
// ==> ContextId
TYPE_CI_ContextId ContextInfo::PushCGType(const Generic & type)
{
  TYPE_CI_ContextId cid (Push(CI_FID_CG));
  SetTypeInfo(cid, type);
  return cid;
}

// pp
// os : ostream
// vdm_format : bool
void ContextInfo::pp(ostream & os, bool vdm_format)
{
  os << "[ " << endl;
  cntxt_file_tab_t::iterator itr = tll_ci.begin();
  if (itr != tll_ci.end()) {
    ++itr;    // the first element is empty. Don't print this empty
              // element. The if makes sure that problems don't occur
              // when tab is empty.
  }
  for ( ; itr != tll_ci.end(); ++itr) {
    SEQ<TYPE_CI_ContextNodeInfo> cni_l ((*itr)->node_l);
    TokenList &tll = (*itr)->token_l;

    os << " mk_CI`TokenContextInfo([";

    if (tll.Length() != 0) {
      os << endl;
      size_t len_tll = tll.Length();
      for (size_t i = 1; i <= len_tll; i++) {
        if (i > 1) {
          os << "," << endl;
        }
        os << "  ";
        tll.Get(i).pp(os);
      }
      os << endl << " ";
    }
    os << "],[";

    if (!cni_l.IsEmpty()) {
      os << endl;

      for (int64_t j = 1; j <= cni_l.Length(); j++) {
        TYPE_CI_ContextNodeInfo c (cni_l[j]);
        os << "  mk_CI`ContextNodeInfo(";

        if( c.get_tokenpos().IsNil() ) {
          os << "mk_CI`TokenSpan(-1,-1,-1)" ;
        }
        else {
          TYPE_CI_TokenSpan ts (c.get_tokenpos());
          os << "mk_CI`TokenSpan("
             << ts.get_token_ust().GetValue() << ","
             << ts.get_token_uast().GetValue() << ","
             << ts.get_token_uend().GetValue() << ")";
        }
        os << ", "
           << c.get_coverage().GetValue() << ", "
           << TBWSTR::wstring2string(INT2Q::TransformIntegers(c.get_typeinfo()).ascii()) << ", "
           << TBWSTR::wstring2string(INT2Q::TransformIntegers(c.get_pid_um()).ascii()) << ", "
           << Int(c.get_index()).GetValue() << ", "
           << (c.get_isBreakable().GetValue() ? "true" : "false") << ", "
           << (c.get_breakpoint().GetValue() ? "true" : "false") << ")";

        if (j < cni_l.Length()) {
          os << ",";
        }
        os << endl;
      }
      os << " ";
    }
    os << "])";
    cntxt_file_tab_t::iterator k (itr);
    if (++k != tll_ci.end())
      os << ",";
    os << endl;
  }
  os << "]";
}

// get_itab_size
// 
int64_t ContextInfo::get_itab_size(int64_t fileid)
{
  return IsAValidFileId(fileid) ? tll_ci[fileid].p->node_l.Length() : 0;
}

// GetCidAtPos
// name : seq of char
// line : int
// col : int
// ==> bool * seq of char * [ContextId]
Tuple ContextInfo::GetCidAtPos(const SEQ<Char> & file, const Int & line, const Int & col)
{
  Generic Fid (GetFidForFile(file));

  if (Fid.IsNil()) {
    return mk_(Bool(false), Sequence(L"No such file"), Nil());
  }

  int64_t fid = TYPE_CI_FileId(Fid).GetValue();
  if ( this->cur_fid_max <= fid ) {
    return mk_(Bool(false), Sequence(L"No such file"), Nil());
  }

  SEQ<TYPE_CI_ContextNodeInfo> ci (tll_ci[fid].p->node_l);
  TokenList &tll = tll_ci[fid].p->token_l;

  Tuple fpici (FindPosInContextInfo(ci, tll, line, col));

  if (fpici.GetBoolValue(1)) {
    int64_t nid = fpici.GetIntValue(2);
    return mk_(Bool(true), Sequence(), CreateCid(fid, nid));
  }
  else {
    return mk_(Bool(false), Sequence(L"Position beyond end of file"), Nil());
  }
}

// GetFidForFile
// name : seq of char
// ==> [FileId]
Generic ContextInfo::GetFidForFile(const SEQ<Char> & file)
{
  Tuple gfmnn (ToolMediator::GetFileMapNameNum(SET< SEQ<Char> >(), true));
  const Map & longNames (gfmnn.GetMap(1));
  const Map & shortNames (gfmnn.GetMap(2));

  Generic Fid;
  if (longNames.DomExists(file, Fid) || shortNames.DomExists(file, Fid)) {
    return Fid;
  }
  else {
    return Generic(Nil());
  }
}

// FindPosInContextInfo
// ci : ContextInfo
// tll : TokenList
// line : int
// col : int
// ==> bool * [NodeId]
Tuple ContextInfo::FindPosInContextInfo(const SEQ<TYPE_CI_ContextNodeInfo> & ci,
                                        const TokenList &tll,
                                        int line, int col)
{
  int closestX = 2<<29;
  int closestY = 2<<29;

  int64_t nid = 0; // replacement for minNid

  for (int64_t index = 1; index <= ci.Length(); ++index) {
    TYPE_CI_ContextNodeInfo cni (ci[index]);
    Generic tokenpos (cni.get_tokenpos());
    bool isBreakable = cni.get_isBreakable().GetValue();

    if (tokenpos.IsNil() || !isBreakable) {
      continue;
    }
    TYPE_CI_TokenSpan ts (tokenpos);
    int token_ast = ts.get_token_uast();
    const TokenInfo &token_info =  tll.Get(token_ast);
    const TYPE_CI_TokenPos &token_st = token_info.get_pos_st();

    const wstring & tokenText = token_info.get_text();
    int token_len;
    if (tokenText.length() == 0) {
      wstring text = VDMPARSER::GetTokenText(token_info.get_id());
      token_len = text.length();
    }
    else {
      token_len = tokenText.length();
    }

    int curLine = token_st.get_abs_uline().GetValue();
    int curCol = token_st.get_column().GetValue();

    if (PointCloser(col, line, closestX, closestY, curCol+token_len-1, curLine)) {
      closestX = curCol;
      closestY = curLine;
      nid = index;
    }
  }

  if( nid == 0 ) {
    return mk_(Bool(false), Int(0));
  }
  else {
    return mk_(Bool(true), Int(nid));
  }
}

bool ContextInfo::PointCloser(int sp_x, int sp_y, int cp_x, int cp_y, int ca_x, int ca_y) const
{
  if (ca_y < sp_y || ((ca_y == sp_y) && (ca_x < sp_x))) {
    return false;
  }
  else {
    return ca_y < cp_y || ((ca_y == cp_y) && (ca_x < cp_x));
  }
}

// IsCidBreakable
// cid : ContextId
// ==> bool
bool ContextInfo::IsCidBreakable(const TYPE_CI_ContextId& cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  return IsAValidContextId(cid, p) && p->get_isBreakable().GetValue();
}

// SetBreakable
// cid : ContextId
void ContextInfo::SetBreakable(const TYPE_CI_ContextId& cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    p->set_isBreakable(Bool(true));
  }
  else {
    complain(L"SetBreakable: invalid cid", cid2int(cid));
  }
}

// SetBreakpoint
// cid : ContextId
void ContextInfo::SetBreakpoint(const TYPE_CI_ContextId& cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    p->set_breakpoint(Bool(true));
  }
  else {
    complain(L"SetBreakpoint: invalid cid", cid2int(cid));
  }
}

// RemoveBreakpoint
// cid : ContextId
void ContextInfo::RemoveBreakpoint(const TYPE_CI_ContextId& cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    p->set_breakpoint(Bool(false));
  }
  else
    complain(L"RemoveBreakpoint: invalid cid", cid2int(cid));
}

// IsBreakpointAtCid
// cid : ContextId
//  ==> bool
bool ContextInfo::IsBreakpointAtCid(const TYPE_CI_ContextId& cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  return IsAValidContextId(cid, p) && p->get_breakpoint().GetValue();
}

Sequence ContextInfo::getGUITokenInfo(const SEQ<Char> & file)
{
  Generic fid (GetFidForFile( file ));
  if ( !fid.IsNil() ) {
    TokenList & tl = this->GetTokenList(fid);
    SEQ<TYPE_CI_TokenSpan> ts_l;
    SEQ<TYPE_CI_ContextNodeInfo> citt (this->tll_ci[TYPE_CI_FileId(fid)]->node_l);
    for (int64_t i = 1; i <= citt.Length(); i++ ) {
      TYPE_CI_ContextNodeInfo ni (citt[i]);
      Generic tokenpos (ni.get_tokenpos());
      if (!tokenpos.IsNil()) {
        TYPE_CI_TokenSpan ts (tokenpos);
        const TokenInfo & stk = tl.Get(ts.get_token_ust().GetValue());
        if (stk.isClassOrModuleToken()) {
          ts_l.ImpAppend(ts);
        }
      }
    }
    return tl.getGUIInfo(ts_l);
  }
  return Sequence();
}

void ContextInfo::DumpTokenList( const TYPE_CI_FileId & fid, wostream& wos )
{
  TokenList & tl = this->GetTokenList(fid);
  tl.dump( wos );
}

void ContextInfo::DumpNodeList( const TYPE_CI_FileId & fid, wostream& wos )
{
  TokenList & tl = this->GetTokenList(fid);
  SEQ<TYPE_CI_ContextNodeInfo> citt (this->tll_ci[fid]->node_l);
  for (int64_t i = 1; i <= citt.Length(); i++ ) {
    TYPE_CI_ContextNodeInfo ni (citt[i]);
    Generic tokenpos (ni.get_tokenpos());
    if (!tokenpos.IsNil()) {
      TYPE_CI_TokenSpan ts (tokenpos);
      const TokenInfo & stk = tl.Get(ts.get_token_ust().GetValue());
      const TokenInfo & etk = tl.Get(ts.get_token_uend().GetValue());

      TYPE_CI_ContextId cid (CreateCid( fid, i ));
      wos << cid << L" ";

      char buf[50];
      string format = "(%3d,%3d,%3d)";
      sprintf( buf, format.c_str(), ts.get_token_ust().GetValue(),
                                    ts.get_token_uast().GetValue(),
                                    ts.get_token_uend().GetValue());
      string str = buf;
      wos << TBWSTR::string2wstring( str );

      wos << L", "
          << ni.get_coverage()
          << L", "
          << INT2Q::TransformIntegers(ni.get_typeinfo()).ascii()
          << L", "
          << INT2Q::TransformIntegers(ni.get_pid_um()).ascii()
          << L", "
          << ni.get_index()
          << L", "
          << (ni.get_isBreakable().GetValue() ? L"true" : L"false")
          << L", "
          << (ni.get_breakpoint().GetValue() ? L"true" : L"false");

      if( ni.get_isBreakable().GetValue() ) {
        wos << L" --> count: " << ni.get_coverage();
        if( ni.get_coverage().GetValue() == 0 ) {
          wos << L" <= UNCOV =";
        }
      }
      wos << endl << flush;
      stk.dump( wos );
      etk.dump( wos );
    }
  }
}

void ContextInfo::AdjustPosInfoForMultibyteText( const TYPE_CI_FileId & fileid )
{
  TokenList & tl = this->GetTokenList( fileid );
  tl.AdjustPosInfoForMultibyteText();
}

bool ContextInfo::CheckToken(const TYPE_CI_ContextId & cid, const wstring & name)
{
  Tuple gfp (GetFilePos(cid));
  if (gfp.GetBoolValue(1)) {
    const TYPE_CI_FileId & fileid (gfp.GetInt(2));
    const TYPE_CI_TokenPos & firstpos (gfp.GetRecord(3));
    //const TYPE_CI_TokenPos & astpos (gfp.GetRecord(4));;
    const TYPE_CI_TokenPos & lastpos (gfp.GetRecord(5));;

    TokenList& tl = GetTokenList(fileid);
    bool exists = false;
    int len_tl = tl.Length();
    for( int i = 1; (i <= len_tl ) && !exists; i++ ) {
      const TokenInfo & ti = tl.Get(i);
      if((ti.get_pos_st() == firstpos) && (ti.get_pos_end() == lastpos)) {
        exists = (ti.get_text() == name);
      }
    }
    return exists;
  }
  return false;
}

// GetContextNodeInfo
// file : seq of char
// ==> [CI`ContextNodeInfo]
Generic ContextInfo::GetContextNodeInfo(const TYPE_CI_ContextId& cid)
{
  TYPE_CI_ContextNodeInfo * p = NULL;
  if (IsAValidContextId(cid, p)) {
    return (*p);
  }
  else {
    return Nil();
  }
}

// GetTokehInfo
// file : seq of char
// ==> [seq of CI`TokenInfo]
Generic ContextInfo::GetTokenInfos(const SEQ<Char> & file)
{
  Generic fid (GetFidForFile( file ));
  if (!fid.IsNil()) {
    SEQ<TYPE_CI_TokenInfo> res;
    TokenList & tl = this->GetTokenList(fid);
    for (size_t i = 1; i <= tl.Length(); i++) {
      const TokenInfo & ti = tl.Get(i);
  
      TYPE_CI_TextId text ((Generic)Sequence(BINOPS::id2str(ti.get_id(), ti.get_text())));

      TYPE_CI_TokenInfo cti;
      cti.Init(Int(ti.get_id()), text, ti.get_pos_st(), ti.get_pos_end());

      res.ImpAppend(cti);
    }
    return res;
  }
  else {
    return Nil();
  }
}

// GetContextNodeInfo
// file : seq of char
// ==> [seq of CI`ContextNodeInfo]
Generic ContextInfo::GetContextNodeInfos(const SEQ<Char> & file)
{
  Generic fileid (GetFidForFile( file ));
  if (!fileid.IsNil()) {
    int fid = TYPE_CI_FileId(fileid).GetValue();
    return this->tll_ci[fid]->node_l;
  }
  else {
    return Nil();
  }
}

// GetSize
// fid : FileId
// ==> nat * nat
Tuple ContextInfo::GetSize(const TYPE_CI_FileId & fid)
{
  return mk_(Int(GetTokenList(fid).Length()), Int(this->tll_ci[fid]->node_l.Length()));
}

ContextInfo& GetCI()
{
  return global_context_info;
}

@}

\section{Test main program}

@o contextinfotest.cc -d @{
#include "contextinfo.h"
#include "metaiv.h"

int check(bool ok, const wstring & s)
{
  wcout << s << L" " << (ok? L"ok" : L"error") << endl;
}

//void test(int max, ContextInfo& c, vector<TYPE_CI_ContextId>& v)
void test(int max, ContextInfo& c, deque<TYPE_CI_ContextId>& v)
{
  int i;
  v.erase(v.begin(), v.end());
  for (i=0; i<max; i++) {
    c.UseContextTab(i, true);
    for (int j = i+1==max ? max : 1;  j>0; --j) {
      TYPE_CI_ContextId cid = c.Push(i);
      v.push_back(cid);
      c.SetTypeInfo(cid, mk_(Int(1), Int(2), Int(i)));
      Map m;
      m.Insert(Int(i), Int(j));
      c.SetPidM(cid, m);
      context_pos pos;
      //pos.i = i;
      c.SetPos(cid, pos);
      c.IncTestCoverageInfo(cid);
      wcout << L"TC counter(" << i << L" " << cid.GetValue()%(1<<20)
       	    << L" =" << c.GetTestCoverageInfo(cid) << endl;
    }
  }
}

Map MapNumName(int max)
{
  wchar_t b[80];
  Map m;
  for (int i = 0; i<=max; i++) {
    swprintf(b, 80, L"file_%d", i);
    m.Insert(Int(i), Sequence(wstring(b)));
  }
  return m;
}

Map MapNameNum(int max)
{
  wchar_t b[80];
  Map m;
  for (int i = 0; i<=max; i++) {
    swprintf(b, 80, L"file_%d", i);
    m.Insert(Sequence(wstring(b)), Int(i));
  }
  return m;
}

int main()
{
  ContextInfo c;
  const int max = 6;
  int i;
//  vector<TYPE_CI_ContextId> cidv;
  deque<TYPE_CI_ContextId> cidv;
  cidv.reserve(max);
  cidv.insert(cidv.begin(), max, NilContextId);
  test(max, c, cidv);
  test(max, c, cidv);
  cout << L"Last CID = " << hex << cidv.back().ascii() << dec << endl;
  // c.pp(cout, true);
  cout << L"writing rti.out" << endl << flush;
  c.SaveCounters(L"rti.out", MapNumName(max), cout);
  cout << L"reading rti.out" << endl << flush;
  Map long_short_names = MapNameNum(max);
  //c.clear();
  cout << L"Testcoverage of last node=" << c.GetTestCoverageInfo(cidv.back()) << endl;
  c.ResetTestCoverageInfo();
  cout << L"Testcoverage of last node=" << c.GetTestCoverageInfo(cidv.back()) << endl;
  cout << L"HasTestCoverage(last) = " << c.HasTestCoverage(max-1) << endl;
  c.LoadCounters(L"rti.out", long_short_names, long_short_names, cout);
  cout << L"HasTestCoverage(last) = " << c.HasTestCoverage(max-1) << endl;
  cout << L"Testcoverage of last node=" << c.GetTestCoverageInfo(cidv.back()) << endl;
  cout << L" " << c.get_itab_size(max-1) << endl;
  cout << L"Done reading rti.out" << endl << flush;

  //For having the process remain so its memory usage can be seen.
  //{
  //  int i;
  //  cout << L"enter integer:";
  //  cin >> i;
  //}
}

@}

@d This file is generated
@{/* This file is generated from preprocess.w. Do not modify! */ @}


% Output the following section to preprocess.h

\end{document}

