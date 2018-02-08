#include "parserservice.h"
#include "tools.h"
#include "astaux.h"

// Parser Internal Service

// When the interaction between the service manager and specification
// manager is finalized, this code should be tidied up. Until then it
// is rather untidy.

Generic ParserService::MediateRequest(const wchar_t* req, const Sequence & args) const
{
  if (wstring(req) == L"GetASTs")
    return GetASTS();
  else if (wstring(req) == L"GetAST"){
    wstring s;
    Sequence nm(args.Hd());
    nm.GetString(s);
    return GetAST(s.c_str());
  }
  else if (wstring(req) == L"GetClassNames")
  {
    return GetClassNames();
  }
  return Nil();
}

Generic  ParserService::GetASTS() const
{
  return ToolMediator::GetVDMASTs();
}

Set ParserService::GetClassNames() const
{
  return ToolMediator::GetAllVDMModuleNames();
}

Generic ParserService::GetAST(const wchar_t* nm) const
{
#ifdef VDMPP
  SEQ<TYPE_AS_Class> classes (GetASTS());
  TYPE_AS_Name anm (ASTAUX::MkName(wstring(nm)));

  Generic ast;
  bool exists = false;
  size_t len_classes = classes.Length();
  for (size_t idx = 1; (idx <= len_classes) && !exists; idx++) {
    const TYPE_AS_Class & cl (classes[idx]);
    const TYPE_AS_Name & clnm (cl.GetRecord(pos_AS_Class_nm));
    const TYPE_AS_Ids & ids (clnm.GetSequence(pos_AS_Name_ids));
    const TYPE_AS_Id & id (ids[1]);
    if (id.GetString() == nm) {
      exists = true;
      ast = cl;
    }
  }

  if (exists) {
    return ast;
  }
  else
#endif //VDMPP
    return Nil();

}
