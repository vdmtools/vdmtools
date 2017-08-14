/***********************************************************************
 * $Header: /home/vdmtools/cvsroot/toolbox/code/win32/print_name_vers.cc,v 1.12 2006/04/17 07:26:23 vdmtools Exp $
 ***********************************************************************
 * Function:
 *
 ***********************************************************************/

#include <wchar.h>

#include <iostream>

#include "tb_version.h"
#include "tb_wstring.h"

#include <string>

#if defined ( __linux__ ) || defined ( __CYGWIN__ )
#include <stdlib.h>
#include <string.h>
#endif // __linux__ || __CYGWIN__

struct Info {
  std::wstring ORIGINALFILENAME;
  bool useflexlm; 
  Info(std::wstring ONAME, bool useflm)
    : ORIGINALFILENAME(ONAME),
      useflexlm(useflm)
  { }
};

Info info_gui_sl (L"vdmgde.exe", false);
Info info_gui_pp (L"vppgde.exe", false);
Info info_gui_rt (L"vicegde.exe", false);

Info info_sl (L"vdmde.exe", false);
Info info_pp (L"vppde.exe", false);
Info info_rt (L"vicede.exe", false);

void wse_header()
{
  std::wcout << L"Document Type: WSE" << std::endl
             << L"item: Global" << std::endl
             << L"  Version=5.0" << std::endl
             << L"end" << std::endl;
}

void wse_set_variable(const wchar_t *var, const std::wstring &val)
{
  std::wcout << L"item: Set Variable" << std::endl
        << L"  Variable=" << var << std::endl
        << L"  Value=" << val << std::endl
        << L"end" << std::endl;
}

void output_wse(const TB_Version& tbvers, const Info& info)
{
  wse_header(); 
  wse_set_variable(L"APPNAME", tbvers.GiveToolTitle());
  wse_set_variable(L"APPNAMEVERS", tbvers.GiveToolTitle() + L" v" + tbvers.GetVersionId());
  wse_set_variable(L"APPVER", tbvers.GetVersionId());
  wse_set_variable(L"MENUTITLE", tbvers.GiveStartMenuToolTitle());
  wse_set_variable(L"USEFLEXLM", info.useflexlm ? L"1" : L"0");
}

std::wstring QT(const std::wstring s, bool zeroter)
{
  std::wstring res;
  res = L"\"";
  res += s;
  if (zeroter) {
    res += L"\\0";
  }
  res += L"\"";
  return res;
}

void output_rch(const TB_Version& tbvers, Info& info)
{
    unsigned short a,b,c,d;
    tbvers.GetVersionComponents(a,b,c,d); 
    std::wcout << L"#define FILEDESC " << QT(tbvers.GiveToolTitle(),true) << std::endl;
    std::wcout << L"#define ORIGINALFN " << QT(info.ORIGINALFILENAME,true) << std::endl;
    std::wcout << L"#define NUMFILEVERS "
          << a << L"," << b << L"," << c << L"," << d << std::endl;
    std::wcout << L"#define NUMPROVERS NUMFILEVERS" << std::endl; 
    std::wcout << L"#define FILEVERS " << QT(tbvers.GetVersionId(),true) << std::endl;
    std::wcout << L"#define PROVERS FILEVERS " << std::endl;
    std::wcout << L"#define PRODUCTNM FILEDESC" << std::endl;
}

void output_version(const TB_Version& tbvers, Info& info, bool shortversion)
{
  if (shortversion)  {
    std::wcout << tbvers.GetShortVersionId() << tbvers.GetExtraComment();
  }
  else {
    std::wcout << tbvers.GetVersionId() << tbvers.GetExtraComment();
  }
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    std::wcerr << L"Usage: " << TBWSTR::string2wstring(argv[0])
          << L"--outputrch | --outputwse" << std::endl;
    exit(1);
  }
  const TB_Version *tbvp = 0;
  Info *infop = 0;
  bool gui = false; 
  for  (int i=1; i<argc; i++) {
    if (strcmp(argv[i], "-gui")==0) {
      if (infop) {
        std::wcerr << L"-gui should be used before -sl/-pp" << std::endl;
        exit(1); 
      }
      gui = true;
      continue; 
    }
    
    if (strcmp(argv[i], "-sl")==0) { 
      tbvp = &tb_version_sl;
      infop = gui? &info_gui_sl : &info_sl;
      continue; 
    }
    if (strcmp(argv[i], "-pp")==0) {
      tbvp = &tb_version_pp;
      infop = gui? &info_gui_pp : &info_pp;
      continue; 
    }
    if (strcmp(argv[i], "-rt")==0) {
      tbvp = &tb_version_rt;
      infop = gui? &info_gui_rt : &info_rt;
      continue; 
    }
    if (strcmp(argv[i], "--outputrch")==0) {
      output_rch(*tbvp, *infop); 
      continue; 
    }
    if (strcmp(argv[i], "--outputwse")==0) {
      output_wse(*tbvp, *infop);
      continue; 
    }
    if (strcmp(argv[i], "--outputversion")==0) {
      output_version(*tbvp, *infop, false);
      continue; 
    }
    if (strcmp(argv[i], "--outputshortversion")==0) {
      output_version(*tbvp, *infop, true);
      continue; 
    }
    std::wcerr << L"Unknown option: " << TBWSTR::string2wstring(argv[i]) << std::endl;
    exit(1); 
  }
  return 0; 
}

