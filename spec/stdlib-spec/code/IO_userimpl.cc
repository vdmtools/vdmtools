#include <fstream>
using namespace std;

#include "cg.h"
#include "metaiv.h"

extern string wstring2fsstr(const wstring&);

#ifdef VDMPP
type_bU2P vdm_IO::vdm_freadval(const type_cL& v)
#endif
#ifdef VDMSL
type_bU2P vdm_IO_freadval(const type_cL& v)
#endif
{

  Generic metaivValue;
  wstring filename;
  v.GetString(filename);
  bool b = CGUTIL::ParseVDMValue(filename, metaivValue);
  type_bU2P res;
  res.SetField(1,Bool(b));
  res.SetField(2,metaivValue);
  
  return res;
}
#ifdef VDMPP
Bool vdm_IO::vdm_fwriteval(const type_cL &v1, const Generic& v2, const TYPE_IO_filedirective & v3)
#endif
#ifdef VDMSL
Bool vdm_IO_fwriteval(const type_cL &v1, const Generic& v2, const TYPE_IO_filedirective & v3)
#endif
{
  wstring filename;
  v1.GetString(filename);
  wofstream outs;
  if (v3 == Quote(L"start")) {
    outs.open(wstring2fsstr(filename).c_str(), ios::out);
  }
  else {
    outs.open(wstring2fsstr(filename).c_str(), ios::app);
  }
  if (!outs) {
    return Bool(false);
  }
  else {
    outs << v2;
    outs.flush();
    outs.close();
    return Bool(true);
  }
}
#ifdef VDMPP
Bool vdm_IO::vdm_writeval(const Generic& v2)
#endif VDMPP
#ifdef VDMSL
Bool vdm_IO_writeval(const Generic& v2)
#endif VDMPP
{
  wcout << v2 << endl;
  return Bool(true);
}

#ifdef VDMPP
Bool vdm_IO::vdm_fecho(const type_cL &v1, const type_cL& v2,const Generic& v3)
#endif
#ifdef VDMSL
Bool vdm_IO_fecho(const type_cL &v1, const type_cL& v2,const Generic& v3)
#endif
{
  wstring text;
  v2.GetString(text);
  if (v1 == Sequence(L"")) {
    wcout << text;
  }
  else { 
    wofstream outs;
    wstring filename;
    v1.GetString(filename);
    if (v3 == Quote(L"start")) {
      outs.open(wstring2fsstr(filename).c_str(), ios::out);
    }
    else {
      outs.open(wstring2fsstr(filename).c_str(), ios::app);
    }
    if (!outs) {
      return Bool(false);
    }
    else {
      outs << text;
      outs.flush();
      outs.close();
    }
  }

  return Bool(true);
}

#ifdef VDMPP
type_cL vdm_IO::vdm_ferror()
#endif
#ifdef VDMSL
type_cL vdm_IO_ferror()
#endif
{
  return type_cL();
}


