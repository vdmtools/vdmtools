//***********************************************************************
//* $Id: vdmfixproject.cc,v 1.5 2005/01/21 03:16:06 vdmtools Exp $
//***********************************************************************

#include <fstream>

#include "metaiv.h"

#ifdef _MSC_VER
#include "string.h"
#define STRNCASECMP _wcsnicmp
#else
#define STRNCASECMP wcsncasecmp
#endif

void croak(const wstring & s) 
{
  wcerr << s << endl; 
  exit(1); 
}

wstring replace(wstring stok, char** argv, int argc)
{
  wstring orig = stok; 
  for (int j=0; j<argc; j++) {
    const wchar_t * p = string2wstring(argv[j]).c_str(); 
    wchar_t * m = new wchar_t[wcslen(p)+1]; 
    wcscpy(m,p); 
    p = m; 
    wchar_t * repl = wcschr(p, L'='); 
    if (repl) {
      *repl = '\0'; 
      repl++;
    } else {
      repl = L""; 
    }
    if (STRNCASECMP(p, stok.c_str(), wcslen(p)) == 0) {
      //cout << L"Found suffix: " << p << '\n'; 
      stok.replace(0, wcslen(p), repl, wcslen(repl)); 
      //cout << L"New = " << stok << '\n';
      break;
    }
  }
  if (orig == stok) {
    wcout << L"=  : " << orig << endl; 
  } else {
    wcout << L"old: " << orig << endl; 
    wcout << L"new: " << stok << endl; 
  }
  return stok;
}  

int main(int argc, char **argv)
{
  if (argc <2) {
    cerr << "usage: " 
         << argv[0] 
         << " projectfile [ removeprefix | oldprefix=newprefix ... ]"
         << "\n"; 
    exit(1); 
  }
  string file = argv[1]; 
  ifstream f(file.c_str()); 
  if (! f) {
    cerr << L"failed to open: " << file << L"\n"; 
    exit(1); 
  }
  Generic g; 
  g = ReadVal(f); 
  // cout << g << '\n';
  if (! g.IsSequence()) {
    croak(L"unknown format"); 
  }
  Sequence s(g); 
  if (s.Length() <3) {
    croak(L"unexpected length <3\n"); 
  }
  if (! s[3].IsInt()) {
    croak(L"field 3 is not int\n"); 
    exit(1); 
  }
  int len = Int(s[3]).GetValue(); 
  if (len+3 > s.Length()) {
    croak(L"missing file  elements\n");
  }
  Sequence orig_s = s; 
  for (int i=0; i<len; i++) {
    Generic t(s[4+i]); 
    if (! t.IsTuple()) 
      croak(L"expected tuple\n"); 
    Tuple tu(t); 
    if (tu.Length() != 2) 
      croak(L"expected tuple size=2\n"); 
    if (! tu.GetField(2).IsToken()) 
      croak(L"expected file as token"); 
    Token tok(tu.GetField(2)); 
    tok = Token(replace(tok.GetValue(), argv+2, argc-2)); 
    tu.SetField(2, tok);
    s.ImpModify(4+i, tu); 
  }
  string newfile = file + ".fixed"; 
  ofstream o(newfile.c_str()); 
  if (! o) {
    croak(wstring(L"Failed to write to: ") + string2wstring(newfile));
  }
  cout << "new project written to " << newfile << '\n'; 
  // cout << L"\n = " << s << '\n'; 
  s.WriteVal(o); 
  o.close();
  return 0;
}
