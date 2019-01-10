//
// AS backend - generates textual version of an AS value
// $Id: as2ascii.cc,v 1.15 2005/10/25 05:00:24 vdmtools Exp $
//

#include "as2ascii.h"
#include "astaux.h"
#include "asquotes.h"

wstring AS2ASCII::GetComments(const TYPE_CI_ContextId & cid_)
{
  wstring comments; comments.erase();

  int cid = cid_.GetValue();
  if(cid > 0) {
    Sequence ci(pTokenci->get_ci());
    Sequence tll(pTokenci->get_tll());
    TYPE_CI_ContextNodeInfo cni(ci[cid]);
    TYPE_CI_TokenSpan tokenpos (cni.get_tokenpos());
    TYPE_CI_TokenRef token_st(tokenpos.get_token_ust());
    TYPE_CI_TokenRef token_end(tokenpos.get_token_uend());

    wstring indnt; indnt.erase();
    for (int j = 0; j < indent; j++){
      indnt += L" ";
    }

    for(int i = token_st; i <= token_end; i++)
    {
      TYPE_CI_TokenInfo ti(tll[i]);
      wstring text(ti.get_text().GetString());
      wstring subst(L"\n");

      subst += indnt + L"--";

      int pos = 0;
      while (-1 != (pos=text.find(L"\n",pos))) {
	if((pos > 1) && (text[pos-1] == L'r') && (text[pos-2] == L'\\')) {
	  text.replace(pos-2, 3, L"\r"+subst);
	} else {
	  text.replace(pos, 1, subst);
        }
	pos += subst.length();
	// delete all spaces before a non-space...
	unsigned end = pos;
	while((end < text.length()) && (text[end] == L' ')) {
	  end++;
	}
	// delete * ...
	if((end < text.length()) && (text[end] == L'*')) {
	  if((end+1 < text.length()) && (text[end+1] != L'/')) {
	    end++;
	  }
	}
        wstring beforeSpace; beforeSpace.erase();
        wstring afterSpace; afterSpace.erase();
	if(end > (unsigned)pos)
        {
	  beforeSpace = text.substr(0, (pos <= 0 ? 0 : pos));
 	  if(end < text.length()) afterSpace = text.substr(end);
          text = beforeSpace + afterSpace;
        }
      }

      if(text[0] == L'/')
      {
        text = L" " + text.substr(2);
      }

      int len = text.length();

      if((len >= 2) && (text[len-1] == L'/') && (text[len-2] == L'*')) {
	text = text.substr(0,len-2);
      }
      if((len >= 2) && (text[len-1] == L'r') && (text[len-2] == L'\\')) {
        text = text.substr(0,len-2);
      }
      if(i != token_st) {
	  comments += indnt + L"\n";
      }
      comments += L"--" + text;
    }
  }
  return comments;
}

#ifdef VDMPP
void AS2ASCII::Class2ASCII(const TYPE_AS_Class & parmClass, wostream & result, const TYPE_CI_TokenContextInfo& tci)
{
  pTokenci = &tci;
  //
  // Find comments...
  //
  const TYPE_CI_ContextId & cid (parmClass.GetInt(pos_AS_Class_cid));
  wstring comments (GetComments(cid));
  if(!comments.empty()) {
      result << comments;
      Newline(result);
  }
  //
  MakeKeyword(L"class", result);
  MakeSpace(result);
  wstring classname (ExtractName(parmClass.GetRecord(pos_AS_Class_nm)));
  MakeNameDef(classname, result);

  // Super classes
  const SEQ<TYPE_AS_Name> & supercls (parmClass.GetSequence(pos_AS_Class_supercls));
  if (!supercls.IsEmpty())
  {
    MakeSpace(result);
    MakeKeyword(L"is subclass of", result);
    MakeSpace(result);

    size_t len_supercls = supercls.Length();
    for (size_t idx = 1; idx <= len_supercls; idx++)
    {
      wstring supernmval (ExtractName(supercls[idx]));
      MakeNameRef(supernmval, result);
      if (idx < len_supercls)
        result << L", ";
    }
  }
  Newline(result);

  // Definitions
  if (!parmClass.GetField(pos_AS_Class_defs).IsNil())
    Definitions2ASCII(parmClass.GetRecord(pos_AS_Class_defs), result);

  // Close class
  Newline(result);
  MakeKeyword(L"end", result);
  MakeSpace(result);
  result << classname;
//JR  result << <p>" << endl;
  result << endl;
}
#endif // VDMPP

void AS2ASCII::Definitions2ASCII(const TYPE_AS_Definitions & parmDefs, wostream & result)
{
  const MAP<TYPE_AS_Name,TYPE_AS_TypeDef> & typem (parmDefs.GetMap(pos_AS_Definitions_typem));
  if (!typem.IsEmpty()) {
    TypeDefs2ASCII(typem, result);
  }

  const SEQ<TYPE_AS_ValueDef> & valuem (parmDefs.GetSequence(pos_AS_Definitions_valuem));
  if (!valuem.IsEmpty()) {
    ValueDefs2ASCII(valuem, result);
  }

#ifdef VDMPP
  const SEQ<TYPE_AS_InstanceVarDef> & instvars (parmDefs.GetSequence(pos_AS_Definitions_instvars));
  if (!instvars.IsEmpty()) {
    InstVars2ASCII(instvars, result);
  }
#endif // VDMPP

  const MAP<TYPE_AS_Name,TYPE_AS_FnDef> & fnm (parmDefs.GetMap(pos_AS_Definitions_fnm));
  if (!fnm.IsEmpty()) {
    FuncDefs2ASCII(fnm, result);
  }

  const MAP<TYPE_AS_Name,TYPE_AS_OpDef> & opm (parmDefs.GetMap(pos_AS_Definitions_opm));
  if (!opm.IsEmpty()) {
    OpDefs2ASCII(opm, result);
  }

#ifdef VDMPP
  const SEQ<TYPE_AS_SyncDef> & syncs (parmDefs.GetSequence(pos_AS_Definitions_syncs));
  if (!syncs.IsEmpty()) {
    Syncs2ASCII(syncs, result);
  }

  const Generic & thread (parmDefs.GetField(pos_AS_Definitions_threaddef));
  if (!thread.IsNil()) {
    Thread2ASCII(thread, result);
  }
#endif // VDMPP
}

void AS2ASCII::TypeDefs2ASCII(const MAP<TYPE_AS_Name, TYPE_AS_TypeDef> & typem, wostream & result)
{
  Newline(result);
  MakeKeyword(L"types", result);
  Newline(result, 2);
  SET<TYPE_AS_Name> dom_typem (typem.Dom());
  size_t index = typem.Size();
  Generic nm;
  for (bool bb = dom_typem.First(nm); bb; bb = dom_typem.Next(nm)) {
    TypeDef2ASCII(typem[nm], result);
    if (index != 1) {
      result << L";";
      Newline(result);
    }
    index--;
  }
  Newline(result);
}

void AS2ASCII::TypeDef2ASCII(const TYPE_AS_TypeDef & td, wostream & result)
{
  Access2ASCII(td.GetField(pos_AS_TypeDef_access), result);
  if (!td.GetRecord(pos_AS_TypeDef_shape).Is(TAG_TYPE_AS_CompositeType))
  {
    wstring nm (ExtractName(td.GetRecord(pos_AS_TypeDef_nm)));
    MakeNameRef(nm, result);
    MakeSpace(result);
    result << L"==";
    MakeSpace(result);
  }

  Type2ASCII(td.GetRecord(pos_AS_TypeDef_shape), result);
  if (!td.GetField(pos_AS_TypeDef_Inv).IsNil())
  {
    Newline(result);
    Invariant2ASCII(td.GetRecord(pos_AS_TypeDef_Inv), result);
  }
}

void AS2ASCII::ValueDefs2ASCII(const SEQ<TYPE_AS_ValueDef> & values, wostream & result)
{
  Newline(result);
  MakeKeyword(L"values", result);
  IncIndent();
  Newline(result);
  size_t len_values = values.Length();
  for (size_t idx = 1; idx <= len_values; idx++)
  {
    ValueDef2ASCII(values[idx], result);
    if (idx < len_values) {
      result << L";";
      Newline(result);
    }
  }
  DecIndent();
  Newline(result);
}

void AS2ASCII::ValueDef2ASCII(const TYPE_AS_ValueDef & vd, wostream & result)
{
  Access2ASCII(vd.GetField(pos_AS_ValueDef_access), result);
  Pattern2ASCII(vd.GetRecord(pos_AS_ValueDef_pat), result);
  if (!vd.GetField(pos_AS_ValueDef_tp).IsNil())
  {
    MakeSpace(result);
    result << L":";
    MakeSpace(result);
    Type2ASCII(vd.GetRecord(pos_AS_ValueDef_tp), result);
  }
  MakeSpace(result);
  result << L"=";
  MakeSpace(result);
  Expr2ASCII(vd.GetRecord(pos_AS_ValueDef_val), result);
}

void AS2ASCII::FuncDefs2ASCII(const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fnm, wostream & result)
{
  Newline(result);
  MakeKeyword(L"functions", result);
  Newline(result);

  SET<TYPE_AS_Name> dom_fnm (fnm.Dom());
  Generic nm;
  for (bool bb = dom_fnm.First(nm); bb; bb = dom_fnm.Next(nm))
  {
    FuncDef2ASCII(fnm[nm], result);
    Newline(result, 2);
  }
}

void AS2ASCII::FuncDef2ASCII(const TYPE_AS_FnDef & fd, wostream & result)
{
  switch (fd.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: { ExplFuncDef2ASCII(fd, result); break; }
    case TAG_TYPE_AS_ImplFnDef: { ImplFuncDef2ASCII(fd, result); break; }
    case TAG_TYPE_AS_ExtExplFnDef: { ExtExplFuncDef2ASCII(fd, result); break; }
    default: { break; }
  }
}

void AS2ASCII::ExplFuncDef2ASCII(const TYPE_AS_ExplFnDef & fn, wostream & result)
{
  Access2ASCII(fn.GetField(pos_AS_ExplFnDef_access), result);
  wstring fnname (ExtractName(fn.GetRecord(pos_AS_ExplFnDef_nm)));
  MakeNameDef(fnname, result);
  MakeSpace(result);
  result << L":";
  MakeSpace(result);

  TypeVarList2ASCII(fn.GetSequence(pos_AS_ExplFnDef_tpparms), result);

  Type2ASCII(fn.GetRecord(pos_AS_ExplFnDef_tp), result);
  Newline(result);
  result << fnname << L"(";

  const TYPE_AS_ParametersList & parms (fn.GetSequence(pos_AS_ExplFnDef_parms));
  size_t len_parms = parms.Length();
  for (size_t idx = 1; idx <= len_parms; idx++)
  {
    PatternSequence2ASCII(parms[idx], result);
    if (idx < len_parms)
      result << L",";
  }
  result << L")";

  MakeSpace(result);
  result << L"==" << endl;
  Newline(result);

  FnBody2ASCII(fn.GetRecord(pos_AS_ExplFnDef_body), result);

  if (!fn.GetField(pos_AS_ExplFnDef_fnpre).IsNil())
  {
    MakeKeyword(L"pre", result);
    MakeSpace(result);
    Expr2ASCII(fn.GetRecord(pos_AS_ExplFnDef_fnpre), result);
    Newline(result);
  }

  if (!fn.GetField(pos_AS_ExplFnDef_fnpost).IsNil()) {
    MakeKeyword(L"post", result);
    MakeSpace(result);
    Expr2ASCII(fn.GetRecord(pos_AS_ExplFnDef_fnpost), result);
    Newline(result);
  }
}

void AS2ASCII::FnBody2ASCII(const TYPE_AS_FnBody & body, wostream& result)
{
  const Generic & body_body (body.GetField(pos_AS_FnBody_body));
  if ((body_body == Int(NOTYETSPEC)) || (body_body == Quote(L"NOTYETSPEC")))
    MakeKeyword(L"is not yet specified", result);
#ifdef VDMPP
  else if ((body_body == Int(SUBRESP)) || (body_body == Quote(L"SUBRESP")))
    MakeKeyword(L"is subclass responsibility", result);
#endif // VDMPP
  else
    Expr2ASCII(body_body, result);
}

void AS2ASCII::ImplFuncDef2ASCII(const TYPE_AS_ImplFnDef& fn, wostream& result)
{
  Access2ASCII(fn.GetField(pos_AS_ImplFnDef_access), result);
  wstring fnname (ExtractName(fn.GetRecord(pos_AS_ImplFnDef_nm)));
  MakeNameDef(fnname, result);

  TypeVarList2ASCII(fn.GetSequence(pos_AS_ImplFnDef_params), result);

  result << L"(";
  ParameterTypes2ASCII(fn.GetSequence(pos_AS_ImplFnDef_partps), result);
  result << L")";

  const SEQ<TYPE_AS_NameType> & resnmtps (fn.GetSequence(pos_AS_ImplFnDef_resnmtps));
  size_t len_resnmtps = resnmtps.Length();
  for (size_t idx = 1; idx <= len_resnmtps; idx++)
  {
    NameType2ASCII(resnmtps[idx], result);
    if (idx < len_resnmtps)
      result << L",";
  }
  Newline(result);

  if (!fn.GetField(pos_AS_ImplFnDef_fnpre).IsNil())
  {
    MakeKeyword(L"pre", result);
    MakeSpace(result);
    Expr2ASCII(fn.GetRecord(pos_AS_ImplFnDef_fnpre), result);
    Newline(result);
  }

  MakeKeyword(L"post", result);
  MakeSpace(result);
  Expr2ASCII(fn.GetRecord(pos_AS_ImplFnDef_fnpost), result);
  Newline(result);
}

void AS2ASCII::NameType2ASCII(const TYPE_AS_NameType & nt, wostream & result)
{
  result << ExtractName(nt.GetRecord(pos_AS_NameType_nm));
  MakeSpace(result);
  result << L":";
  MakeSpace(result);
  Type2ASCII(nt.GetRecord(pos_AS_NameType_tp), result);
}

void AS2ASCII::ExtExplFuncDef2ASCII(const TYPE_AS_ExtExplFnDef& fn, wostream& result)
{
  Access2ASCII(fn.GetField(pos_AS_ExtExplFnDef_access), result);
  wstring fnname (ExtractName(fn.GetRecord(pos_AS_ExtExplFnDef_nm)));
  MakeNameDef(fnname, result);

  TypeVarList2ASCII(fn.GetSequence(pos_AS_ExtExplFnDef_params), result);

  result << L"(";
  ParameterTypes2ASCII(fn.GetSequence(pos_AS_ExtExplFnDef_partps), result);
  result << L")";

  const SEQ<TYPE_AS_NameType> & resnmtps (fn.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
  size_t len_resnmtps = resnmtps.Length();
  for (size_t idx = 1; idx <= len_resnmtps; idx++)
  {
    NameType2ASCII(resnmtps[idx], result);
    if (idx < len_resnmtps)
      result << L",";
  }
  MakeSpace(result);
  result << L"==";
  Newline(result);

  FnBody2ASCII(fn.GetRecord(pos_AS_ExtExplFnDef_body), result);

  if (!fn.GetField(pos_AS_ExtExplFnDef_fnpre).IsNil()) {
    MakeKeyword(L"pre", result);
    MakeSpace(result);
    Expr2ASCII(fn.GetRecord(pos_AS_ExtExplFnDef_fnpre), result);
    Newline(result);
  }

  if (!fn.GetField(pos_AS_ExtExplFnDef_fnpost).IsNil()) {
    MakeKeyword(L"post", result);
    MakeSpace(result);
    Expr2ASCII(fn.GetRecord(pos_AS_ExtExplFnDef_fnpost), result);
    Newline(result);
  }
}

void AS2ASCII::OpDefs2ASCII(const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opm, wostream & result)
{
  Newline(result);
  MakeKeyword(L"operations", result);
  Newline(result);

  SET<TYPE_AS_Name> dom_opm (opm.Dom());
  Generic nm;
  for (bool bb = dom_opm.First(nm); bb; bb = dom_opm.Next(nm)) {
    OpDef2ASCII(opm[nm], result);
    result << L";";
    Newline(result,2);
  }
}

void AS2ASCII::OpDef2ASCII(const TYPE_AS_OpDef & op, wostream & result)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef: { ExplOpDef2ASCII(op, result); break; }
    case TAG_TYPE_AS_ImplOpDef: { ImplOpDef2ASCII(op, result); break; }
    case TAG_TYPE_AS_ExtExplOpDef: { ExtExplOpDef2ASCII(op, result); break; }
    default: { break; }
  }
}

void AS2ASCII::Static2ASCII(const Bool & isStatic, wostream & result)
{
  if(isStatic == Bool(true)) {
    MakeKeyword(L"static", result);
    MakeSpace(result);
  }
}

void AS2ASCII::ExplOpDef2ASCII(const TYPE_AS_ExplOpDef & op, wostream & result)
{
  const TYPE_CI_ContextId & cid (op.GetInt(pos_AS_ExplOpDef_cid));
  wstring comments (GetComments(cid));
  if(!comments.empty()) {
    result << comments;
    Newline(result);
  }
  Access2ASCII(op.GetField(pos_AS_ExplOpDef_access), result);
  Static2ASCII(op.GetBool(pos_AS_ExplOpDef_stat), result);
  wstring opname (ExtractName(op.GetRecord(pos_AS_ExplOpDef_nm)));
  MakeNameDef(opname, result);
  MakeSpace(result);
  result << L":";
  MakeSpace(result);

  Type2ASCII(op.GetRecord(pos_AS_ExplOpDef_tp), result);

  Newline(result);
  result << opname << L"(";

  PatternSequence2ASCII(op.GetSequence(pos_AS_ExplOpDef_parms), result);
  result << L")";

  MakeSpace(result);
  result << L"==" << endl;
  MakeSpace(result);
  //  Newline(result);

  OpBody2ASCII(op.GetRecord(pos_AS_ExplOpDef_body), result);
}

void AS2ASCII::OpBody2ASCII(const TYPE_AS_OpBody & body, wostream & result)
{
  const Generic & body_body (body.GetField(pos_AS_OpBody_body));
  if (body_body == Int(NOTYETSPEC) || body_body == Quote(L"NOTYETSPEC"))
    MakeKeyword(L"is not yet specified", result);
#ifdef VDMPP
  else if (body_body == Int(SUBRESP) || body_body == Quote(L"SUBRESP"))
    MakeKeyword(L"is subclass responsibility", result);
#endif // VDMPP
  else
    Stmt2ASCII(body_body, result);
}

void AS2ASCII::ImplOpDef2ASCII(const TYPE_AS_ImplOpDef & op, wostream & result)
{
  Access2ASCII(op.GetField(pos_AS_ImplOpDef_access), result);
  wstring opname (ExtractName(op.GetRecord(pos_AS_ImplOpDef_nm)));
  MakeNameDef(opname, result);

  result << L"(";
  ParameterTypes2ASCII(op.GetSequence(pos_AS_ImplOpDef_partps), result);
  result << L")";

  const SEQ<TYPE_AS_NameType> & resnmtps (op.GetSequence(pos_AS_ImplOpDef_resnmtps));
  size_t len_resnmtps = resnmtps.Length();
  for (size_t idx = 1 ; idx <= len_resnmtps; idx++)
  {
    NameType2ASCII(resnmtps[idx], result);
    if (idx < len_resnmtps)
      result << L",";
  }
  Newline(result);

  const SEQ<TYPE_AS_ExtInf> & opext (op.GetSequence(pos_AS_ImplOpDef_opext));
  if (!opext.IsEmpty())
  {
    MakeKeyword(L"ext", result);
    MakeSpace(result);
    size_t len_opext = opext.Length();
    for (size_t idx = 1; idx <= len_opext; idx++)
    {
      ExtInf2ASCII(opext[idx], result);
      if (idx < len_opext)
        result << L",";
    }
    Newline(result);
  }

  if (!op.GetField(pos_AS_ImplOpDef_oppre).IsNil())
  {
    MakeKeyword(L"pre", result);
    MakeSpace(result);
    Expr2ASCII(op.GetRecord(pos_AS_ImplOpDef_oppre), result);
    Newline(result);
  }

  MakeKeyword(L"post", result);
  MakeSpace(result);
  Expr2ASCII(op.GetRecord(pos_AS_ImplOpDef_oppost), result);
  Newline(result);
}

void AS2ASCII::ExtInf2ASCII(const TYPE_AS_ExtInf & ei, wostream & result)
{
  if (ei.GetField(pos_AS_ExtInf_mode) == Int(READ))
    MakeKeyword(L"rd", result);
  else
    MakeKeyword(L"wr", result);
  MakeSpace(result);

  ExprSequence2ASCII(ei.GetSequence(pos_AS_ExtInf_vars), L",", result);
  if (!ei.GetField(pos_AS_ExtInf_tp).IsNil()) {
    MakeSpace(result);
    result << L":";
    MakeSpace(result);
    Type2ASCII(ei.GetRecord(pos_AS_ExtInf_tp), result);
  }
}

void AS2ASCII::ExtExplOpDef2ASCII(const TYPE_AS_ExtExplOpDef & op, wostream & result)
{
  // TODO: sync
  Access2ASCII(op.GetField(pos_AS_ExtExplOpDef_access), result);
  wstring opname (ExtractName(op.GetRecord(pos_AS_ExtExplOpDef_nm)));
  MakeNameDef(opname, result);

  result << L"(";
  ParameterTypes2ASCII(op.GetSequence(pos_AS_ExtExplOpDef_partps), result);
  result << L")";

  const SEQ<TYPE_AS_NameType> & resnmtps (op.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
  size_t len_resnmtps = resnmtps.Length();
  for (size_t idx = 1; idx <= len_resnmtps; idx++)
  {
    NameType2ASCII(resnmtps[idx], result);
    if (idx < len_resnmtps)
      result << L",";
  }
  Newline(result);

  MakeSpace(result);
  result << L"==";
  MakeSpace(result);
  OpBody2ASCII(op.GetRecord(pos_AS_ExtExplOpDef_body), result);
  Newline(result);

  const SEQ<TYPE_AS_ExtInf> & opext (op.GetSequence(pos_AS_ExtExplOpDef_opext));
  if (!opext.IsEmpty())
  {
    MakeKeyword(L"ext", result);
    MakeSpace(result);
    size_t len_opext = opext.Length();
    for (size_t idx = 1; idx <= len_opext; idx++)
    {
      ExtInf2ASCII(opext[idx], result);
      if (idx < len_opext)
        result << L",";
    }
    Newline(result);
  }

  if (!op.GetField(pos_AS_ExtExplOpDef_oppre).IsNil()) {
    MakeKeyword(L"pre", result);
    MakeSpace(result);
    Expr2ASCII(op.GetRecord(pos_AS_ExtExplOpDef_oppre), result);
    Newline(result);
  }

  MakeKeyword(L"post", result);
  MakeSpace(result);
  Expr2ASCII(op.GetRecord(pos_AS_ExtExplOpDef_oppost), result);
  Newline(result);
}

#ifdef VDMPP
void AS2ASCII::InstVars2ASCII(const SEQ<TYPE_AS_InstanceVarDef> & instvars, wostream & result)
{
  Newline(result);
  MakeKeyword(L"instance variables", result);
  IncIndent();
  Newline(result);
  size_t len_instvars = instvars.Length();
  for (size_t idx = 1; idx <= len_instvars; idx++)
  {
    InstVar2ASCII(instvars[idx], result);
    if (idx < len_instvars) {
      result << L";";
      Newline(result);
    }
  }
  DecIndent();
  Newline(result);
}

void AS2ASCII::InstVar2ASCII(const TYPE_AS_InstanceVarDef & ivd, wostream & result)
{
  switch(ivd.GetTag()) {
    case TAG_TYPE_AS_InstAssignDef: {
      const TYPE_CI_ContextId & cid (ivd.GetInt(pos_AS_InstAssignDef_cid));
      wstring comments (GetComments(cid));
      if(!comments.empty()) {
        result << comments;
        Newline(result);
      }
      Access2ASCII(ivd.GetField(pos_AS_InstAssignDef_access), result);
      Static2ASCII(ivd.GetBool(pos_AS_InstAssignDef_stat), result);
      AssignDef2ASCII(ivd.GetRecord(pos_AS_InstAssignDef_ad), result);
      break;
    }
    case TAG_TYPE_AS_InstanceInv: {
      MakeKeyword(L"inv", result);
      MakeSpace(result);
      Expr2ASCII(ivd.GetRecord(pos_AS_InstanceInv_expr), result);
      break;
    }
  }
}

void AS2ASCII::Syncs2ASCII(const SEQ<TYPE_AS_SyncDef> & syncs, wostream & result)
{
  Newline(result);
  MakeKeyword(L"sync", result);
  MakeSpace(result);
  size_t len_syncs = syncs.Length();
  for (size_t idx = 1; idx <= len_syncs; idx++)
  {
    SyncDef2ASCII(syncs[idx], result);
    if (idx < len_syncs) {
      result << L";";
      Newline(result);
    }
  }
  Newline(result);
}

void AS2ASCII::SyncDef2ASCII(const TYPE_AS_SyncDef& sd, wostream& result)
{
  switch (sd.GetTag()) {
    case TAG_TYPE_AS_Permission: {
      MakeKeyword(L"per", result);
      MakeSpace(result);
      MakeNameRef(ExtractName(sd.GetRecord(pos_AS_Permission_spec)), result);
      MakeSpace(result);
      result << L"=>";
      MakeSpace(result);
      Expr2ASCII(sd.GetRecord(pos_AS_Permission_guard), result);
      break;
    }
    case TAG_TYPE_AS_Mutex: {
      MakeKeyword(L"mutex", result);
      MakeSpace(result);
      const Generic & ops (sd.GetField(pos_AS_Mutex_ops));
      if (!ops.IsNil()) {
        result << L"(";
        ExprSequence2ASCII(ops, L",", result);
        result << L")";
      }
      break;
    }
  }
}

void AS2ASCII::Thread2ASCII(const TYPE_AS_ThreadDef & thread, wostream & result)
{
  Newline(result);
  MakeKeyword(L"thread", result);
  Newline(result);

  switch(thread.GetTag()) {
    case TAG_TYPE_AS_PerObl: {
      MakeKeyword(L"periodic", result);
      result << L"(";
      Expr2ASCII(thread.GetRecord(pos_AS_PerObl_period), result);
      result << L",";
      Expr2ASCII(thread.GetRecord(pos_AS_PerObl_jitter), result);
      result << L",";
      Expr2ASCII(thread.GetRecord(pos_AS_PerObl_delay), result);
      result << L",";
      Expr2ASCII(thread.GetRecord(pos_AS_PerObl_offset), result);
      result << L")(";
      Expr2ASCII(thread.GetRecord(pos_AS_PerObl_mtd), result);
      result << L")";
      break;
    }
    case TAG_TYPE_AS_SpoObl: {
      MakeKeyword(L"sporadic", result);
      result << L"(";
      Expr2ASCII(thread.GetRecord(pos_AS_SpoObl_delay), result);
      result << L",";
      Expr2ASCII(thread.GetRecord(pos_AS_SpoObl_bound), result);
      result << L",";
      Expr2ASCII(thread.GetRecord(pos_AS_SpoObl_offset), result);
      result << L")(";
      Expr2ASCII(thread.GetRecord(pos_AS_SpoObl_mtd), result);
      result << L")";
      break;
    }
    default: {
      Stmt2ASCII(thread, result);
      break;
    }
  }
  Newline(result);
}
#endif // VDMPP

void AS2ASCII::MakeKeyword(const wstring & kw, wostream & result)
{
//JR  result << L"<b>" << kw << "</b> ";
  result << kw;
}

void AS2ASCII::MakeSpace(wostream & result)
{
  result << L" ";
}

void AS2ASCII::Newline(wostream & result, int i)
{
  for (int l = 0; l < i; l++)
    //   result << endl << L"<br>" << endl;
    result << endl;

  // Insert indentation here if necessary
  for (int j = 0; j < indent; j++) {
    result << L" ";
  }
}

void AS2ASCII::BeginDocument(wostream& result)
{
//  result << L"<html><body><pre>\n";
  result << L"";
}

void AS2ASCII::EndDocument(wostream& result)
{
//  result << L"</pre></body></html>\n";
  result << L"";
}

wstring AS2ASCII::ExtractName(const TYPE_AS_Name & nm)
{
  TYPE_AS_Ids ids (nm.GetSequence(pos_AS_Name_ids));
#ifdef VDMSL
  if (ids[1] == SEQ<Char>(ASTAUX::GetDefaultModName())) {
    ids.ImpTl();
  }
#endif //VDMSL

  wstring result(L"");
  size_t len_ids = ids.Length();
  for (size_t idx = 1; idx <= len_ids; idx++) {
    if (idx > 1) {
      result += L"`";
    }
    result += ids[idx].GetString();
  }
  return result;
}

void AS2ASCII::MakeNameDef(const wstring & nm, wostream & result)
{
//JR result << L"<a name=" << nm << ">" << nm << "</a>";
  result << nm;
}

void AS2ASCII::MakeNameDef(const wstring & url, const wstring & nm, wostream& result)
{
//JR ? result << L"<a name=" << url << ">" << nm << "</a>";
  result << nm;
}


// Note that this works incorrectly if the ref is to another class
// (e.g. in a new expression or in the subclass part of the header)
void AS2ASCII::MakeNameRef(const wstring & nm, wostream & result)
{
//JR ?  result << L"<a href=#" << nm << ">" << nm << "</a>";
  result << nm ;
}

void AS2ASCII::Access2ASCII(const TYPE_AS_Access & acc, wostream & result)
{
  if ((acc == Quote(L"DEFAULT_AS")) || (acc == Int(DEFAULT_AS)))
    ;
  else if ((acc == Quote(L"PRIVATE_AS")) || (acc == Int(PRIVATE_AS)))
  { 
    MakeKeyword(L"private", result);
    MakeSpace(result);
  }
  else if ((acc == Quote(L"PROTECTED_AS")) || (acc == Int(PRIVATE_AS)))
  {
    MakeKeyword(L"protected", result);
    MakeSpace(result);
  }
  else if ((acc == Quote(L"PUBLIC_AS")) || (acc == Int(PRIVATE_AS)))
  {
    MakeKeyword(L"public", result);
    MakeSpace(result);
  }
}

void AS2ASCII::Invariant2ASCII(const TYPE_AS_Invariant & inv, wostream & result)
{
  MakeKeyword(L"inv", result);
  MakeSpace(result);
  Pattern2ASCII(inv.GetRecord(pos_AS_Invariant_pat), result);
  result << L" == ";
  Expr2ASCII(inv.GetRecord(pos_AS_Invariant_expr), result);
}

/////////////////////////////////////////////////////////////////////
// Types
/////////////////////////////////////////////////////////////////////

void AS2ASCII::Type2ASCII(const TYPE_AS_Type & tp, wostream & result)
{
  switch(tp.GetTag()) {
    case TAG_TYPE_AS_BooleanType:
      BooleanType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_NumericType:
      NumericType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_TokenType:
      TokenType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_CharType:
      CharType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_VoidType:
      VoidType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_QuoteType:
      QuoteType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_CompositeType:
      CompositeType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_UnionType:
      UnionType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_ProductType:
      ProductType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_OptionalType:
      OptionalType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_Set0Type:
      Set0Type2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_Set1Type:
      Set1Type2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_Seq0Type:
      Seq0Type2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_Seq1Type:
      Seq1Type2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_GeneralMap0Type:
      GeneralMap0Type2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_GeneralMap1Type:
      GeneralMap1Type2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_InjectiveMap0Type:
      InjectiveMap0Type2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_InjectiveMap1Type:
      InjectiveMap1Type2ASCII(tp, result);
      break;
   case TAG_TYPE_AS_TypeName:
     TypeName2ASCII(tp, result);
     break;
    case TAG_TYPE_AS_PartialFnType:
      PartialFnType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_TotalFnType:
      TotalFnType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_BracketedType:
      BracketedType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_OpType:
      OpType2ASCII(tp, result);
      break;
    case TAG_TYPE_AS_TypeVar:
      TypeVar2ASCII(tp, result);
      break;
    default:
      break;
  }
}

void AS2ASCII::BooleanType2ASCII(const TYPE_AS_BooleanType & tp, wostream & result)
{
  MakeKeyword(L"bool", result);
}

void AS2ASCII::NumericType2ASCII(const TYPE_AS_NumericType & tp, wostream & result)
{
  const Generic & i (tp.GetField(1));
  if ((i == Quote(L"NATONE")) || (i == Int(NATONE)))
    MakeKeyword(L"nat1", result);
  else if ((i == Quote(L"NAT")) || (i == Int(NAT)))
    MakeKeyword(L"nat", result);
  else if ((i == Quote(L"INTEGER")) || (i == Int(INTEGER)))
    MakeKeyword(L"int", result);
  else if ((i == Quote(L"RAT")) || (i == Int(RAT)))
    MakeKeyword(L"rat", result);
  else if ((i == Quote(L"REAL")) || (i == Int(REAL)))
    MakeKeyword(L"real", result);
  else
    MakeKeyword(L"unknown basic type", result);
}

void AS2ASCII::TokenType2ASCII(const TYPE_AS_TokenType & tp, wostream & result)
{
  MakeKeyword(L"token", result);
}

void AS2ASCII::CharType2ASCII(const TYPE_AS_CharType & tp, wostream & result)
{
  MakeKeyword(L"char", result);
}

void AS2ASCII::VoidType2ASCII(const TYPE_AS_VoidType & tp, wostream & result)
{
  MakeKeyword(L"()", result);
}

void AS2ASCII::QuoteType2ASCII(const TYPE_AS_QuoteType & tp, wostream & result)
{
  QuoteLit2ASCII(tp.GetRecord(pos_AS_QuoteType_lit), result);
}

void AS2ASCII::CompositeType2ASCII(const TYPE_AS_CompositeType & tp, wostream & result)
{
  wstring nm (ExtractName(tp.GetRecord(pos_AS_CompositeType_name)));
  MakeNameDef(nm, result);
  result << L"::";
  const SEQ<TYPE_AS_Field> & fields (tp.GetSequence(pos_AS_CompositeType_fields));
  size_t len_fields = fields.Length();
  for (size_t idx = 1; idx <= len_fields; idx++) {
    const TYPE_AS_Field & field (fields[idx]);
    if (!field.GetField(pos_AS_Field_sel).IsNil()) {
      result << ExtractName(field.GetRecord(pos_AS_Field_sel));
      if (field.GetBoolValue(pos_AS_Field_dc))
        result << L":- ";
      else
        result << L": ";
      Type2ASCII(field.GetRecord(pos_AS_Field_type), result);
      if (idx != len_fields)
        Newline(result);
    }
  }
}

void AS2ASCII::UnionType2ASCII(const TYPE_AS_UnionType & tp, wostream & result)
{
  SEQ<TYPE_AS_Type> tps (tp.GetSequence(pos_AS_UnionType_tps));
  TypeSequence2ASCII(tps, L'|', result);
}

void AS2ASCII::ProductType2ASCII(const TYPE_AS_ProductType & tp, wostream & result)
{
  SEQ<TYPE_AS_Type> tps (tp.GetSequence(pos_AS_ProductType_tps));
  TypeSequence2ASCII(tps, L'*', result);
}

void AS2ASCII::TypeSequence2ASCII(const SEQ<TYPE_AS_Type> & tps, wchar_t sep, wostream & result)
{
  if (tps.IsEmpty())
    result << L"()";
  else {
    size_t len_tps = tps.Length();
    for (size_t idx = 1; idx <= len_tps; idx++)
    {
      Type2ASCII(tps[idx], result);
      if (idx < len_tps) {
	//        result << L" " << sep << " ";
        result.put(sep);
        MakeSpace(result);
      }
    }
  }
}

void AS2ASCII::TypeVarList2ASCII(const TYPE_AS_TypeVarList & tvl, wostream & result)
{
  if (!tvl.IsEmpty()) {
    result << L"[";
    size_t len_tvl = tvl.Length();
    for (size_t idx = 1; idx <= len_tvl; idx++)
    {
      TypeVar2ASCII(tvl[idx], result);
      if (idx < len_tvl)
      {
        result << L",";
        MakeSpace(result);
      }
    }
    result << L"]";
  }
}

void AS2ASCII::OptionalType2ASCII(const TYPE_AS_OptionalType & tp, wostream & result)
{
  result << L"[";
  Type2ASCII(tp.GetRecord(pos_AS_OptionalType_tp), result);
  result << L"]";
}

void AS2ASCII::Set0Type2ASCII(const TYPE_AS_Set0Type & tp, wostream & result)
{
  MakeKeyword(L"set of", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_Set0Type_elemtp), result);
}

void AS2ASCII::Set1Type2ASCII(const TYPE_AS_Set1Type & tp, wostream & result)
{
  MakeKeyword(L"set1 of", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_Set1Type_elemtp), result);
}

void AS2ASCII::Seq0Type2ASCII(const TYPE_AS_Seq0Type & tp, wostream & result)
{
  MakeKeyword(L"seq of", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_Seq0Type_elemtp), result);
}

void AS2ASCII::Seq1Type2ASCII(const TYPE_AS_Seq1Type & tp, wostream & result)
{
  MakeKeyword(L"seq1 of", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_Seq1Type_elemtp), result);
}

void AS2ASCII::GeneralMap0Type2ASCII(const TYPE_AS_GeneralMap0Type & tp, wostream & result)
{
  MakeKeyword(L"map", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_GeneralMap0Type_mapdom), result);
  MakeSpace(result);
  MakeKeyword(L"to", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_GeneralMap0Type_maprng), result);
}

void AS2ASCII::GeneralMap1Type2ASCII(const TYPE_AS_GeneralMap1Type & tp, wostream & result)
{
  MakeKeyword(L"map1", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_GeneralMap1Type_mapdom), result);
  MakeSpace(result);
  MakeKeyword(L"to", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_GeneralMap1Type_maprng), result);
}

void AS2ASCII::InjectiveMap0Type2ASCII(const TYPE_AS_InjectiveMap0Type & tp, wostream & result)
{
  MakeKeyword(L"inmap", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom), result);
  MakeSpace(result);
  MakeKeyword(L"to", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_InjectiveMap0Type_maprng), result);
}

void AS2ASCII::InjectiveMap1Type2ASCII(const TYPE_AS_InjectiveMap1Type & tp, wostream & result)
{
  MakeKeyword(L"inmap1", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom), result);
  MakeSpace(result);
  MakeKeyword(L"to", result);
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_InjectiveMap1Type_maprng), result);
}

void AS2ASCII::TypeName2ASCII(const TYPE_AS_TypeName & tp, wostream & result)
{
  wstring nm (ExtractName(tp.GetRecord(pos_AS_TypeName_name)));
  MakeNameRef(nm, result);
}

void AS2ASCII::PartialFnType2ASCII(const TYPE_AS_PartialFnType & tp, wostream & result)
{
  const SEQ<TYPE_AS_Type> fndom (tp.GetSequence(pos_AS_PartialFnType_fndom));
  if (fndom.IsEmpty()) 
  {
    result << L"()";
  }
  else
  {
    TypeSequence2ASCII(fndom, L'*', result);
  }
  MakeSpace(result);
  result << L"->";
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_PartialFnType_fnrng), result);
}

void AS2ASCII::TotalFnType2ASCII(const TYPE_AS_TotalFnType & tp, wostream & result)
{
  const SEQ<TYPE_AS_Type> fndom (tp.GetSequence(pos_AS_TotalFnType_fndom));
  if (fndom.IsEmpty()) 
  {
    result << L"()";
  }
  else
  {
    TypeSequence2ASCII(fndom, L'*', result);
  }
  MakeSpace(result);
  result << L"+>";
  MakeSpace(result);
  Type2ASCII(tp.GetField(pos_AS_TotalFnType_fnrng), result);
}

void AS2ASCII::BracketedType2ASCII(const TYPE_AS_BracketedType & tp, wostream & result)
{
  result << L"(";
  Type2ASCII(tp.GetRecord(pos_AS_BracketedType_tp), result);
  result << L")";
}

void AS2ASCII::OpType2ASCII(const TYPE_AS_OpType & tp, wostream & result)
{
  const SEQ<TYPE_AS_Type> opdom (tp.GetSequence(pos_AS_OpType_opdom));
  if (opdom.IsEmpty()) 
  {
    result << L"()";
  }
  else
  {
    TypeSequence2ASCII(opdom, L'*', result);
  }
  MakeSpace(result);
  result << L"==>";
  MakeSpace(result);
  Type2ASCII(tp.GetRecord(pos_AS_OpType_oprng), result);
//  Newline(result);
}

void AS2ASCII::TypeVar2ASCII(const TYPE_AS_TypeVar & tp, wostream & result)
{
  result << L"@";
  result << ExtractName(tp.GetRecord(pos_AS_TypeVar_name));
}

// Expressions

void AS2ASCII::Expr2ASCII(const TYPE_AS_Expr & expr, wostream & result)
{
  switch (expr.GetTag()) {
    case TAG_TYPE_AS_BracketedExpr: {
      result << L"(";
      Expr2ASCII(expr.GetRecord(pos_AS_BracketedExpr_expr), result);
      result << L")";
      break;
    }
    case TAG_TYPE_AS_DefExpr: {
      DefExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_LetExpr: {
      LetExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_LetBeSTExpr: {
      LetBeSTExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_AllOrExistsExpr: {
      AllOrExistsExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_ExistsUniqueExpr: {
      ExistsUniqueExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_IotaExpr: {
      IotaExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_ApplyExpr: {
      ApplyExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_FieldSelectExpr: {
      FieldSelectExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_IfExpr: {
      IfExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_CasesExpr: {
      CasesExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_CaseAltn: {
      CaseAltn2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_PrefixExpr: {
      PrefixExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_BinaryExpr: {
      BinaryExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SetRangeExpr: {
      SetRangeExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SubSequenceExpr: {
      SubSequenceExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SetEnumerationExpr: {
      SetEnumExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      SeqEnumExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_MapEnumerationExpr: {
      MapEnumExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SetComprehensionExpr: {
      SetComprehensionExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SeqComprehensionExpr: {
      SeqComprehensionExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_MapComprehensionExpr: {
      MapComprehensionExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_TupleConstructorExpr: {
      TupleConstructorExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_RecordConstructorExpr: {
      RecordConstructorExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_RecordModifierExpr: {
      RecordModifierExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: {
      SeqModifyMapOverrideExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_LambdaExpr: {
      LambdaExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_FctTypeInstExpr: {
      FctTypeInstExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_IsExpr: {
      IsExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_TokenConstructorExpr: {
      TokenConstructorExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_TupleSelectExpr: {
      TupleSelectExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_TypeJudgementExpr: {
      TypeJudgementExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_NarrowExpr: {
      NarrowExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_PreConditionApplyExpr: {
      PreConditionApplyExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_Name: {
      NameExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_OldName: {
      OldNameExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_BoolLit: {
      BoolLit2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_NilLit: {
      NilLit2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_RealLit: {
      RealLit2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_NumLit: {
      NumLit2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_TextLit: {
      TextLit2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_CharLit: {
      CharLit2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_QuoteLit: {
      QuoteLit2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_UndefinedExpr: {
      MakeKeyword(L"undefined", result);
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr: {
      SelfExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_NewExpr: {
      NewExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_IsOfClassExpr: {
      IsOfClassExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_IsOfBaseClassExpr: {
      IsOfBaseClassExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SameBaseClassExpr: {
      SameBaseClassExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_SameClassExpr: {
      SameClassExpr2ASCII(expr, result);
      break;
    }
    case TAG_TYPE_AS_ThreadIdExpr: {
      ThreadIdExpr2ASCII(expr, result);
      break;
    }
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr: {
      CurTimeExpr2ASCII(expr, result);
      break;
    }
#endif // VICE
#endif // VDMPP
    case TAG_TYPE_AS_Maplet: {
      Maplet2ASCII(expr, result);
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ActExpr:
    case TAG_TYPE_AS_FinExpr:
    case TAG_TYPE_AS_ActiveExpr:
    case TAG_TYPE_AS_WaitingExpr:
    case TAG_TYPE_AS_ReqExpr: {
      HistoryExpr2ASCII(expr, result);
      break;
    }
#endif // VDMPP
    case TAG_TYPE_AS_Macro: {
      Macro2ASCII(expr, result);
      break;
    }
    default: {
      break;
    }
  }
}

void AS2ASCII::ExprSequence2ASCII(const SEQ<TYPE_AS_Expr> & exprs, wstring sep, wostream & result)
{
  if (!exprs.IsEmpty())
  {
    size_t len_exprs = exprs.Length();
    for (size_t idx = 1; idx <= len_exprs; idx++)
    {
      if (idx > 1)
        result << sep << L" ";
      Expr2ASCII(exprs[idx], result);
      //if (idx < len_exprs) 
      //  result << L" " << sep << L" ";
    }
  }
}

void AS2ASCII::LocalDef2ASCII(const TYPE_AS_LocalDef & ld, wostream & result)
{
  switch(ld.GetTag()) {
    case TAG_TYPE_AS_ValueDef: {
      ValueDef2ASCII(ld, result);
      break;
    }
    case TAG_TYPE_AS_ImplFnDef:
    case TAG_TYPE_AS_ExplFnDef:
    case TAG_TYPE_AS_ExtExplFnDef: {
      FuncDef2ASCII(ld, result);
      break;
    }
  }
}

void AS2ASCII::DefExpr2ASCII(const TYPE_AS_DefExpr & expr, wostream & result)
{
  MakeKeyword(L"def", result);
  MakeSpace(result);
  DefBinds2ASCII(expr.GetSequence(pos_AS_DefExpr_Def), result);
  MakeKeyword(L"in", result);
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_DefExpr_In), result);
}

void AS2ASCII::DefBinds2ASCII(const Sequence & vals, wostream & result)
{
  size_t len_vals = vals.Length();
  for (size_t idx = 1; idx <= len_vals; idx++)
  {
    Tuple t(vals[idx]); // (PatternBind * Expr)
    PatternBind2ASCII(t.GetRecord(1), result);
    MakeSpace(result);
    result << L"=";
    MakeSpace(result);
    Expr2ASCII(t.GetRecord(2), result);
    if (idx < len_vals)
      result << L";";
    Newline(result);
  }
}

void AS2ASCII::LetExpr2ASCII(const TYPE_AS_LetExpr & expr, wostream & result)
{
  MakeKeyword(L"let", result);
  MakeSpace(result);
  LocalDefs2ASCII(expr.GetSequence(pos_AS_LetExpr_localdef), result);
  MakeKeyword(L"in", result);
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_LetExpr_body), result);

}

void AS2ASCII::LocalDefs2ASCII(const SEQ<TYPE_AS_LocalDef> & localdefs, wostream& result)
{
  size_t len_localdefs = localdefs.Length();
  for (size_t idx = 1; idx <= len_localdefs; idx++) {
    if (idx > 1) {
      result << L", ";
    }
    LocalDef2ASCII(localdefs[idx], result);
  }
  MakeSpace(result);
}

void AS2ASCII::LetBeSTExpr2ASCII(const TYPE_AS_LetBeSTExpr & expr, wostream & result)
{
  MakeKeyword(L"let", result);
  BindList2ASCII(expr.GetSequence(pos_AS_LetBeSTExpr_lhs), result);
  if (!expr.GetField(pos_AS_LetBeSTExpr_St).IsNil()) {
    MakeSpace(result);
    MakeKeyword(L"be st", result);
    MakeSpace(result);
    Expr2ASCII(expr.GetRecord(pos_AS_LetBeSTExpr_St), result);
  }
  MakeSpace(result);
  MakeKeyword(L"in", result);
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_LetBeSTExpr_In), result);
}

void AS2ASCII::AllOrExistsExpr2ASCII(const TYPE_AS_AllOrExistsExpr & expr, wostream & result)
{
  if (expr.GetField(pos_AS_AllOrExistsExpr_quant) == Int(EXISTS))
    MakeKeyword(L"exists", result);
  else
    MakeKeyword(L"forall", result);
  MakeSpace(result);

  BindList2ASCII(expr.GetSequence(pos_AS_AllOrExistsExpr_bind), result);
  MakeSpace(result);
  //result << L"&amp";
  result << L"&";
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_AllOrExistsExpr_pred), result);
}

void AS2ASCII::ExistsUniqueExpr2ASCII(const TYPE_AS_ExistsUniqueExpr & expr, wostream & result)
{
  MakeKeyword(L"exists1", result);
  MakeSpace(result);
  Bind2ASCII(expr.GetRecord(pos_AS_ExistsUniqueExpr_bind), result);
  MakeSpace(result);
  //result << L"&amp";
  result << L"&";
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_ExistsUniqueExpr_pred), result);
}

void AS2ASCII::IotaExpr2ASCII(const TYPE_AS_IotaExpr & expr, wostream & result)
{
  MakeKeyword(L"iota", result);
  MakeSpace(result);
  Bind2ASCII(expr.GetRecord(pos_AS_IotaExpr_bind), result);
  MakeSpace(result);
  //result << L"&amp";
  result << L"&";
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_IotaExpr_pred), result);
}

void AS2ASCII::ApplyExpr2ASCII(const TYPE_AS_ApplyExpr & expr, wostream & result)
{
  Expr2ASCII(expr.GetRecord(pos_AS_ApplyExpr_fct), result);
  result << L"(";
  ExprSequence2ASCII(expr.GetSequence(pos_AS_ApplyExpr_arg), L",", result);
  result << L")";
}

void AS2ASCII::FieldSelectExpr2ASCII(const TYPE_AS_FieldSelectExpr & expr, wostream & result)
{
  Expr2ASCII(expr.GetRecord(pos_AS_FieldSelectExpr_rec), result);
  result << L".";
  Expr2ASCII(expr.GetRecord(pos_AS_FieldSelectExpr_nm), result);
}

void AS2ASCII::IfExpr2ASCII(const TYPE_AS_IfExpr & expr, wostream & result)
{
  MakeKeyword(L"if", result);
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_IfExpr_test), result);
  MakeSpace(result);
  MakeKeyword(L"then", result);
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_IfExpr_cons), result);

  const SEQ<TYPE_AS_ElseifExpr> & elsif (expr.GetSequence(pos_AS_IfExpr_elsif));
  if (!elsif.IsEmpty())
  {
    size_t len_elsif = elsif.Length();
    for (size_t idx = 1; idx <= len_elsif; idx++)
      ElseifExpr2ASCII(elsif[idx], result);
  }

  MakeSpace(result);
  MakeKeyword(L"else", result);
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_IfExpr_altn), result);
}

void AS2ASCII::ElseifExpr2ASCII(const TYPE_AS_ElseifExpr & expr, wostream & result)
{
  MakeKeyword(L"elseif", result);
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_ElseifExpr_test), result);
  Expr2ASCII(expr.GetRecord(pos_AS_ElseifExpr_cons), result);
}

void AS2ASCII::CasesExpr2ASCII(const TYPE_AS_CasesExpr & expr, wostream & result)
{
  MakeKeyword(L"cases", result);
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_CasesExpr_sel), result);
  result << L": ";
  Newline(result);
  const SEQ<TYPE_AS_CaseAltn> & altns (expr.GetSequence(pos_AS_CasesExpr_altns));
  if (!altns.IsEmpty())
  {
    size_t len_altns = altns.Length();
    for (size_t idx = 1; idx <= len_altns; idx++)
    {
      CaseAltn2ASCII(altns[idx], result);
      if (idx < len_altns)
        result << L",";
      Newline(result);
    }
  }
  if (!expr.GetField(pos_AS_CasesExpr_Others).IsNil())
  {
    MakeKeyword(L"others", result);
    MakeSpace(result);
    result << L"->";
    MakeSpace(result);
    Expr2ASCII(TYPE_AS_Expr(expr.GetField(pos_AS_CasesExpr_Others)), result);
    Newline(result);
  }
  MakeKeyword(L"end", result);
}

void AS2ASCII::CaseAltn2ASCII(const TYPE_AS_CaseAltn& altn, wostream& result)
{
  PatternSequence2ASCII(altn.GetSequence(pos_AS_CaseAltn_match), result);
  result << L"-> ";
  Expr2ASCII(TYPE_AS_Expr(altn.GetField(pos_AS_CaseAltn_body)), result);
}

void AS2ASCII::PrefixExpr2ASCII(const TYPE_AS_PrefixExpr & expr, wostream & result)
{
  const Generic & i (expr.GetField(pos_AS_PrefixExpr_opr));
  if ((i == Quote(L"NUMPLUS")) || (i == Int(NUMPLUS)))
    result << L"+";
  else if ((i == Quote(L"NUMMINUS")) || (i == Int(NUMMINUS)))
    result << L"-";
  else if ((i == Quote(L"NUMABS")) || (i == Int(NUMABS)))
    MakeKeyword(L"abs", result);
  else if ((i == Quote(L"FLOOR")) || (i == Int(FLOOR)))
    MakeKeyword(L"floor", result);
  else if ((i == Quote(L"NOT")) || (i == Int(NOT)))
    MakeKeyword(L"not", result);
  else if ((i == Quote(L"SETCARD")) || (i == Int(SETCARD)))
    MakeKeyword(L"card", result);
  else if ((i == Quote(L"SETDISTRUNION")) || (i == Int(SETDISTRUNION)))
    MakeKeyword(L"dunion", result);
  else if ((i == Quote(L"SETDISTRINTERSECT")) || (i == Int(SETDISTRINTERSECT)))
    MakeKeyword(L"dinter", result);
  else if ((i == Quote(L"SETPOWER")) || (i == Int(SETPOWER)))
    MakeKeyword(L"power", result);
  else if ((i == Quote(L"SEQLEN")) || (i == Int(SEQLEN)))
    MakeKeyword(L"len", result);
  else if ((i == Quote(L"SEQDISTRCONC")) || (i == Int(SEQDISTRCONC)))
    MakeKeyword(L"conc", result);
  else if ((i == Quote(L"SEQELEMS")) || (i == Int(SEQELEMS)))
    MakeKeyword(L"elems", result);
  else if ((i == Quote(L"SEQINDICES")) || (i == Int(SEQINDICES)))
    MakeKeyword(L"indices", result);
  else if ((i == Quote(L"SEQTAIL")) || (i == Int(SEQTAIL)))
    MakeKeyword(L"tl", result);
  else if ((i == Quote(L"SEQHEAD")) || (i == Int(SEQHEAD)))
    MakeKeyword(L"hd", result);
  else if ((i == Quote(L"SEQREVERSE")) || (i == Int(SEQREVERSE)))
    MakeKeyword(L"reverse", result);
  else if ((i == Quote(L"MAPDOM")) || (i == Int(MAPDOM)))
    MakeKeyword(L"dom", result);
  else if ((i == Quote(L"MAPRNG")) || (i == Int(MAPRNG)))
    MakeKeyword(L"rng", result);
  else if ((i == Quote(L"MAPDISTRMERGE")) || (i == Int(MAPDISTRMERGE)))
    MakeKeyword(L"merge", result);
  else if ((i == Quote(L"MAPINVERSE")) || (i == Int(MAPINVERSE)))
    MakeKeyword(L"inverse", result);

  MakeSpace(result);

  Expr2ASCII(expr.GetRecord(pos_AS_PrefixExpr_arg), result);
}

void AS2ASCII::BinaryExpr2ASCII(const TYPE_AS_BinaryExpr & expr, wostream & result)
{
  Expr2ASCII(expr.GetRecord(pos_AS_BinaryExpr_left), result);
  MakeSpace(result);

  const Generic & i (expr.GetField(pos_AS_BinaryExpr_opr));
  if ((i == Quote(L"NUMPLUS")) || (i == Int(NUMPLUS)))
    result << L"+";
  else if ((i == Quote(L"NUMMINUS")) || (i == Int(NUMMINUS)))
    result << L"-";
  else if ((i == Quote(L"NUMMULT")) || (i == Int(NUMMULT)))
    result << L"*";
  else if ((i == Quote(L"NUMDIV")) || (i == Int(NUMDIV)))
    result << L"/";
  else if ((i == Quote(L"NUMREM")) || (i == Int(NUMREM)))
    MakeKeyword(L"rem", result);
  else if ((i == Quote(L"NUMMOD")) || (i == Int(NUMMOD)))
    MakeKeyword(L"mod", result);
  else if ((i == Quote(L"NUMEXP")) || (i == Int(NUMEXP)))
    result << L"**";
  else if ((i == Quote(L"NUMLT")) || (i == Int(NUMLT)))
    result << L"<";
  else if ((i == Quote(L"NUMLE")) || (i == Int(NUMLE)))
    result << L"<=";
  else if ((i == Quote(L"NUMGT")) || (i == Int(NUMGT)))
    result << L">";
  else if ((i == Quote(L"NUMGE")) || (i == Int(NUMGE)))
    result << L">=";
  else if ((i == Quote(L"INTDIV")) || (i == Int(INTDIV)))
    MakeKeyword(L"div", result);
  else if ((i == Quote(L"AND")) || (i == Int(AND)))
    MakeKeyword(L"and", result);
  else if ((i == Quote(L"OR")) || (i == Int(OR)))
    MakeKeyword(L"or", result);
  else if ((i == Quote(L"IMPLY")) || (i == Int(IMPLY)))
    result << L"=>";
  else if ((i == Quote(L"EQUIV")) || (i == Int(EQUIV)))
    result << L"<=>";
  else if ((i == Quote(L"EQ")) || (i == Int(EQ)))
    result << L"=";
  else if ((i == Quote(L"NE")) || (i == Int(NE)))
    result << L"<>";
  else if ((i == Quote(L"SETUNION")) || (i == Int(SETUNION)))
    MakeKeyword(L"union", result);
  else if ((i == Quote(L"SETINTERSECT")) || (i == Int(SETINTERSECT)))
    MakeKeyword(L"inter", result);
  else if ((i == Quote(L"SETMINUS")) || (i == Int(SETMINUS)))
    result << L"\\";
  else if ((i == Quote(L"SUBSET")) || (i == Int(SUBSET)))
    MakeKeyword(L"subset", result);
  else if ((i == Quote(L"PROPERSUBSET")) || (i == Int(PROPERSUBSET)))
    MakeKeyword(L"psubset", result);
  else if ((i == Quote(L"INSET")) || (i == Int(INSET)))
    MakeKeyword(L"in set", result);
  else if ((i == Quote(L"NOTINSET")) || (i == Int(NOTINSET)))
    MakeKeyword(L"not in set", result);
  else if ((i == Quote(L"SEQCONC")) || (i == Int(SEQCONC)))
    result << L"^";
  else if ((i == Quote(L"MAPMERGE")) || (i == Int(MAPMERGE)))
    result << L"munion";
  else if ((i == Quote(L"MAPDOMRESTTO")) || (i == Int(MAPDOMRESTTO)))
    result << L"<:";
  else if ((i == Quote(L"MAPDOMRESTBY")) || (i == Int(MAPDOMRESTBY)))
    result << L"<-:";
  else if ((i == Quote(L"MAPRNGRESTTO")) || (i == Int(MAPRNGRESTTO)))
    result << L">:";
  else if ((i == Quote(L"MAPRNGRESTBY")) || (i == Int(MAPRNGRESTBY)))
    result << L">-:";
  else if ((i == Quote(L"COMPOSE")) || (i == Int(COMPOSE)))
    MakeKeyword(L"comp", result);

  MakeSpace(result);

  Expr2ASCII(expr.GetRecord(pos_AS_BinaryExpr_right), result);
}

void AS2ASCII::SetRangeExpr2ASCII(const TYPE_AS_SetRangeExpr & expr, wostream & result)
{
  result << L"{";
  Expr2ASCII(expr.GetRecord(pos_AS_SetRangeExpr_lb), result);
  result << L",...,";
  Expr2ASCII(expr.GetRecord(pos_AS_SetRangeExpr_ub), result);
  result << L"}";
}

void AS2ASCII::SubSequenceExpr2ASCII(const TYPE_AS_SubSequenceExpr & expr, wostream & result)
{
  Expr2ASCII(expr.GetRecord(pos_AS_SubSequenceExpr_sequence), result);
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_SubSequenceExpr_frompos), result);
  result << L",...,";
  Expr2ASCII(expr.GetRecord(pos_AS_SubSequenceExpr_topos), result);
  result << L")";
}

void AS2ASCII::SetEnumExpr2ASCII(const TYPE_AS_SetEnumerationExpr & expr, wostream & result)
{
  result << L"{";
  ExprSequence2ASCII(expr.GetSequence(pos_AS_SetEnumerationExpr_els), L",", result);
  result << L"}";
}

void AS2ASCII::SeqEnumExpr2ASCII(const TYPE_AS_SeqEnumerationExpr & expr, wostream & result)
{
  const SEQ<TYPE_AS_Expr> & expr_l (expr.GetSequence(pos_AS_SeqEnumerationExpr_els));
  size_t len_expr_l = expr_l.Length();
  bool forall = (len_expr_l > 0);
  for (size_t idx = 1; (idx <= len_expr_l) && forall; idx++) {
    forall = expr_l[idx].Is(TAG_TYPE_AS_CharLit); 
  }
  if (forall) {
    result << L"\"";
    for (size_t i = 1; i <= len_expr_l; i++) {
      const TYPE_AS_CharLit & cl (expr_l[i]);
      result << cl.GetChar(pos_AS_CharLit_val).GetValue();
    }
    result << L"\"";
    return ;
  }
  result << L"[";
  MakeSpace(result);
  ExprSequence2ASCII(expr.GetSequence(pos_AS_SeqEnumerationExpr_els), L",", result);
  MakeSpace(result);
  result << L"]";
}

void AS2ASCII::MapEnumExpr2ASCII(const TYPE_AS_MapEnumerationExpr & expr, wostream & result)
{
  const SEQ<TYPE_AS_Maplet> & els (expr.GetSequence(pos_AS_MapEnumerationExpr_els));
  result << L"{";
  if (!els.IsEmpty()) {
    ExprSequence2ASCII(els, L",", result);
  } else {
    result << L"|->";
  }
  result << L"}";
}

void AS2ASCII::SetComprehensionExpr2ASCII(const TYPE_AS_SetComprehensionExpr & expr, wostream & result)
{
  result << L"{";
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_SetComprehensionExpr_elem), result);
  MakeSpace(result);
  result << L"|";
  MakeSpace(result);
  BindList2ASCII(expr.GetSequence(pos_AS_SetComprehensionExpr_bind), result);
  if (!expr.GetField(pos_AS_SetComprehensionExpr_pred).IsNil())
  {
    MakeSpace(result);
    //result << L" &amp ";
    result << L"&";
    MakeSpace(result);
    Expr2ASCII(expr.GetRecord(pos_AS_SetComprehensionExpr_pred), result);
  }
  MakeSpace(result);
  result << L"}";
}

void AS2ASCII::SeqComprehensionExpr2ASCII(const TYPE_AS_SeqComprehensionExpr & expr, wostream & result)
{
  result << L"[";
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_SeqComprehensionExpr_elem), result);
  MakeSpace(result);
  result << L"|";
  MakeSpace(result);
  //Bind2ASCII(expr.GetRecord(pos_AS_SeqComprehensionExpr_bind), result);
  const Record & bind (expr.GetRecord(pos_AS_SeqComprehensionExpr_bind));
  switch (bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      SetBind2ASCII(expr.GetRecord(pos_AS_SeqComprehensionExpr_bind), result);
      break;
    }
    case TAG_TYPE_AS_SeqBind: {
      SeqBind2ASCII(expr.GetRecord(pos_AS_SeqComprehensionExpr_bind), result);
      break;
    }
  }
  if (!expr.GetField(pos_AS_SeqComprehensionExpr_pred).IsNil()) {
    MakeSpace(result);
    //result << L" &amp ";
    result << L"&";
    MakeSpace(result);
    Expr2ASCII(expr.GetRecord(pos_AS_SeqComprehensionExpr_pred), result);
  }
  MakeSpace(result);
  result << L"]";
}

void AS2ASCII::MapComprehensionExpr2ASCII(const TYPE_AS_MapComprehensionExpr & expr, wostream & result)
{
  result << L"{";
  MakeSpace(result);
  Maplet2ASCII(expr.GetRecord(pos_AS_MapComprehensionExpr_elem), result);
  MakeSpace(result);
  result << L"|";
  MakeSpace(result);
  BindList2ASCII(expr.GetSequence(pos_AS_MapComprehensionExpr_bind), result);
  if (!expr.GetField(pos_AS_MapComprehensionExpr_pred).IsNil())
  {
    MakeSpace(result);
    //result << L" &amp ";
    result << L"&";
    MakeSpace(result);
    Expr2ASCII(expr.GetRecord(pos_AS_MapComprehensionExpr_pred), result);
  }
  MakeSpace(result);
  result << L"}";
}

void AS2ASCII::Maplet2ASCII(const TYPE_AS_Maplet & maplet, wostream & result)
{
  Expr2ASCII(maplet.GetRecord(pos_AS_Maplet_mapdom), result);
  MakeSpace(result);
  result << L"|->";
  MakeSpace(result);
  Expr2ASCII(maplet.GetRecord(pos_AS_Maplet_maprng), result);
}

void AS2ASCII::TupleConstructorExpr2ASCII(const TYPE_AS_TupleConstructorExpr & expr, wostream & result)
{
  //  MakeKeywoExprd(L"mk_", result);
  result << L"mk_(";
  ExprSequence2ASCII(expr.GetSequence(pos_AS_TupleConstructorExpr_fields), L",", result);
  result << L")";
}

void AS2ASCII::RecordConstructorExpr2ASCII(const TYPE_AS_RecordConstructorExpr & expr, wostream & result)
{
  wstring tag (ExtractName(expr.GetRecord(pos_AS_RecordConstructorExpr_tag)));
  result << L"mk_";
  MakeNameRef(tag, result);
  result << L"(";
  ExprSequence2ASCII(expr.GetSequence(pos_AS_RecordConstructorExpr_fields), L",", result);
  result << L")";
}

void AS2ASCII::RecordModifierExpr2ASCII(const TYPE_AS_RecordModifierExpr & expr, wostream & result)
{
  MakeKeyword(L"mu", result);
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_RecordModifierExpr_rec), result);
  result << L",";

  const SEQ<TYPE_AS_RecordModification> & modifiers (expr.GetSequence(pos_AS_RecordModifierExpr_modifiers));
  size_t len_modifiers = modifiers.Length();
  for (size_t idx = 1; idx <= len_modifiers; idx++)
  {
    RecordModification2ASCII(modifiers[idx], result);
    if (idx < len_modifiers)
      result << L",";
  }
  result << L")";
}

void AS2ASCII::RecordModification2ASCII(const TYPE_AS_RecordModification & expr, wostream & result)
{
  MakeSpace(result);
  result << ExtractName(expr.GetRecord(pos_AS_RecordModification_field));
  result << L"|->";
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_RecordModification_newexpr), result);
}

void AS2ASCII::SeqModifyMapOverrideExpr2ASCII(const TYPE_AS_SeqModifyMapOverrideExpr & expr, wostream & result)
{
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap), result);
  result << L"++";
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp), result);
}

void AS2ASCII::LambdaExpr2ASCII(const TYPE_AS_LambdaExpr & expr, wostream & result)
{
  MakeKeyword(L"lambda", result);
  MakeSpace(result);

  const SEQ<TYPE_AS_TypeBind> & parm (expr.GetSequence(pos_AS_LambdaExpr_parm));
  size_t len_parm = parm.Length();
  for (size_t idx = 1; idx <= len_parm; idx++)
  {
    Bind2ASCII(parm[idx], result);
    if (idx < len_parm)
      result << L",";
  }
  MakeSpace(result);
  //result << L"&amp";
  result << L"&";
  MakeSpace(result);
  Expr2ASCII(expr.GetRecord(pos_AS_LambdaExpr_body), result);
}

void AS2ASCII::FctTypeInstExpr2ASCII(const TYPE_AS_FctTypeInstExpr & expr, wostream & result)
{
  wstring polyfct (ExtractName(expr.GetRecord(pos_AS_FctTypeInstExpr_polyfct)));
  MakeNameRef(polyfct, result);
  result << L"[";
  TypeSequence2ASCII(expr.GetSequence(pos_AS_FctTypeInstExpr_inst), L',', result);
  result << L"]";
}

void AS2ASCII::IsExpr2ASCII(const TYPE_AS_IsExpr & expr, wostream& result)
{
  const Record & type (expr.GetRecord(pos_AS_IsExpr_type));
  result << L"is_";
  //result << L"(";
  //Expr2ASCII(expr.GetRecord(pos_AS_IsExpr_arg), result);
  //result << L", ";
  if (type.Is(TAG_TYPE_AS_BooleanType) || type.Is(TAG_TYPE_AS_NumericType) ||
      type.Is(TAG_TYPE_AS_TokenType) || type.Is(TAG_TYPE_AS_CharType))
  {
    Type2ASCII(type, result);
  }
  else {
    //wstring typenm (ExtractName(TYPE_AS_Name(type)));
    wstring typenm (ExtractName(type));
    MakeNameRef(typenm, result);
  }
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_IsExpr_arg), result);
  result << L")";
}

void AS2ASCII::TokenConstructorExpr2ASCII(const TYPE_AS_TokenConstructorExpr & expr, wostream & result)
{
  MakeKeyword(L"mk_token", result);
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_TokenConstructorExpr_field), result);
  result << L")";
}

void AS2ASCII::TupleSelectExpr2ASCII(const TYPE_AS_TupleSelectExpr & expr, wostream & result)
{
  Expr2ASCII(expr.GetRecord(pos_AS_TupleSelectExpr_tuple),result);
  result << L".#";
  RealLit2ASCII(expr.GetRecord(pos_AS_TupleSelectExpr_no),result);
}

void AS2ASCII::TypeJudgementExpr2ASCII(const TYPE_AS_TypeJudgementExpr & expr, wostream & result)
{
  MakeKeyword(L"is_", result);
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_TypeJudgementExpr_expr), result);
  result << L", ";
  Type2ASCII(expr.GetRecord(pos_AS_TypeJudgementExpr_type), result);
  result << L")";
}

void AS2ASCII::NarrowExpr2ASCII(const TYPE_AS_NarrowExpr & expr, wostream & result)
{
  MakeKeyword(L"narrow_", result);
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_NarrowExpr_expr), result);
  result << L", ";
  Type2ASCII(expr.GetRecord(pos_AS_NarrowExpr_type), result);
  result << L")";
}

void AS2ASCII::PreConditionApplyExpr2ASCII(const TYPE_AS_PreConditionApplyExpr & expr, wostream & result)
{
  MakeKeyword(L"pre_", result);
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_PreConditionApplyExpr_fct), result);
  const SEQ<TYPE_AS_Expr> & arg (expr.GetSequence(pos_AS_PreConditionApplyExpr_arg));
  if (!arg.IsEmpty()) {
    result << L", ";
    ExprSequence2ASCII(arg, L",", result);
  }
  result << L")";
}

void AS2ASCII::NameExpr2ASCII(const TYPE_AS_Name & expr, wostream & result)
{
  result << ExtractName(expr);
}

void AS2ASCII::OldNameExpr2ASCII(const TYPE_AS_OldName & expr, wostream & result)
{
  TYPE_AS_Name nm;
  nm.SetField(pos_AS_Name_ids, expr.GetField(pos_AS_OldName_ids));
  nm.SetField(pos_AS_Name_cid, expr.GetField(pos_AS_OldName_cid));

  result << ExtractName(nm) << L"~ ";
}

void AS2ASCII::BoolLit2ASCII(const TYPE_AS_BoolLit & expr, wostream & result)
{
  if (expr.GetBool(pos_AS_BoolLit_val) == Bool(true))
    MakeKeyword(L"true", result);
  else
    MakeKeyword(L"false", result);
}

void AS2ASCII::NilLit2ASCII(const TYPE_AS_NilLit &, wostream & result)
{
  MakeKeyword(L"nil", result);
}

void AS2ASCII::RealLit2ASCII(const TYPE_AS_RealLit & expr, wostream & result)
{
  result << expr.GetReal(pos_AS_RealLit_val);
}

void AS2ASCII::NumLit2ASCII(const TYPE_AS_NumLit & expr, wostream & result)
{
  result << expr.GetReal(pos_AS_NumLit_val);
}

void AS2ASCII::TextLit2ASCII(const TYPE_AS_TextLit & expr, wostream & result)
{
  result << expr.GetSequence(pos_AS_TextLit_val);
}

void AS2ASCII::CharLit2ASCII(const TYPE_AS_CharLit & expr, wostream & result)
{
  result << expr.GetChar(pos_AS_CharLit_val);
}

#ifdef VDMPP
void AS2ASCII::SelfExpr2ASCII(const TYPE_AS_SelfExpr &, wostream & result)
{
  MakeKeyword(L"self", result);
}

void AS2ASCII::NewExpr2ASCII(const TYPE_AS_NewExpr & expr, wostream & result)
{
  MakeKeyword(L"new", result);
  MakeSpace(result);
  MakeNameRef(ExtractName(expr.GetRecord(pos_AS_NewExpr_cls)), result);
  //  result << L"()";
  result << L"(";
  ExprSequence2ASCII(expr.GetSequence(pos_AS_NewExpr_args), L",", result);
  result << L")";
}

void AS2ASCII::IsOfClassExpr2ASCII(const TYPE_AS_IsOfClassExpr & expr, wostream & result)
{
  MakeKeyword(L"isofclass", result);
  result << L"(";
  MakeNameRef(ExtractName(expr.GetRecord(pos_AS_IsOfClassExpr_cls)), result);
  result << L",";
  Expr2ASCII(expr.GetRecord(pos_AS_IsOfClassExpr_arg), result);
  result << L")";
}

void AS2ASCII::IsOfBaseClassExpr2ASCII(const TYPE_AS_IsOfBaseClassExpr & expr, wostream & result)
{
  MakeKeyword(L"isofbaseclass", result);
  result << L"(";
  MakeNameRef(ExtractName(expr.GetRecord(pos_AS_IsOfBaseClassExpr_cls)), result);
  result << L",";
  Expr2ASCII(expr.GetRecord(pos_AS_IsOfBaseClassExpr_arg), result);
  result << L")";
}

void AS2ASCII::SameBaseClassExpr2ASCII(const TYPE_AS_SameBaseClassExpr & expr, wostream & result)
{
  MakeKeyword(L"samebaseclass", result);
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_SameBaseClassExpr_expr1), result);
  result << L",";
  Expr2ASCII(expr.GetRecord(pos_AS_SameBaseClassExpr_expr2), result);
  result << L")";
}

void AS2ASCII::SameClassExpr2ASCII(const TYPE_AS_SameClassExpr & expr, wostream & result)
{
  MakeKeyword(L"sameclass", result);
  result << L"(";
  Expr2ASCII(expr.GetRecord(pos_AS_SameClassExpr_expr1), result);
  result << L",";
  Expr2ASCII(expr.GetRecord(pos_AS_SameClassExpr_expr2), result);
  result << L")";
}

void AS2ASCII::ThreadIdExpr2ASCII(const TYPE_AS_ThreadIdExpr&, wostream& result)
{
  MakeKeyword(L"threadid", result);
}
#ifdef VICE
void AS2ASCII::CurTimeExpr2ASCII(const TYPE_AS_CurTimeExpr&, wostream& result)
{
  MakeKeyword(L"time", result);
}
#endif // VICE

void AS2ASCII::HistoryExpr2ASCII(const TYPE_AS_Expr & expr, wostream & result)
{
  switch(expr.GetTag()) {
    case TAG_TYPE_AS_ActExpr:
      MakeKeyword(L"#act", result);
      break;
    case TAG_TYPE_AS_FinExpr:
      MakeKeyword(L"#fin", result);
      break;
    case TAG_TYPE_AS_ActiveExpr:
      MakeKeyword(L"#active", result);
      break;
    case TAG_TYPE_AS_WaitingExpr:
      MakeKeyword(L"#wait", result);
      break;
    case TAG_TYPE_AS_ReqExpr:
      MakeKeyword(L"#req", result);
      break;
    default:
      result << L"unknown history expr";
      break;
  }
  result << L"(";

  const SEQ<TYPE_AS_Name> & mthd (expr.GetSequence(1));
  size_t len_mthd = mthd.Length();
  for (size_t idx = 1; idx <= len_mthd; idx++)
  {
    MakeNameRef(ExtractName(mthd[idx]), result);
    if (idx < len_mthd)
      result << L",";
  }
  result << L")";
}
#endif // VDMPP

void AS2ASCII::Macro2ASCII(const TYPE_AS_Macro & expr, wostream& result)
{
  const Int& tp (expr.GetInt(pos_AS_Macro_tp));
  switch (tp.GetValue()) {
    case MACRO_FILE: {
      MakeKeyword(L"__FILE__", result);
      break;
    }
    case MACRO_LINE: {
      MakeKeyword(L"__LINE__", result);
      break;
    }
    case MACRO_COLUMN: {
      MakeKeyword(L"__COLUMN__", result);
      break;
    }
    case MACRO_CLMOD: {
      MakeKeyword(L"__CLMOD__", result);
      break;
    }
    case MACRO_FNOP: {
      MakeKeyword(L"__FNOP__", result);
      break;
    }
  }
}

// Statements
void AS2ASCII::Stmt2ASCII(const TYPE_AS_Stmt & stmt, wostream & result)
{
  switch (stmt.GetTag()) {
    case TAG_TYPE_AS_DefStmt: {
      DefStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_LetStmt: {
      LetStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_LetBeSTStmt: {
      LetBeSTStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_AssignStmt: {
      AssignStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_AtomicAssignStmt: {
      AtomicAssignStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_SeqForLoopStmt: {
      SeqForLoopStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_SetForLoopStmt: {
      SetForLoopStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_IndexForLoopStmt: {
      IndexForLoopStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_WhileLoopStmt: {
      WhileLoopStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_CallStmt: {
      CallStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_ReturnStmt: {
      ReturnStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_IfStmt: {
      IfStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_CasesStmt: {
      CasesStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_CasesStmtAltn: {
      CasesStmtAltn2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_ErrorStmt: {
      ErrorStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_ExitStmt: {
      ExitStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_AlwaysStmt: {
      AlwaysStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_TrapStmt: {
      TrapStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_RecTrapStmt: {
      RecTrapStmt2ASCII(stmt, result);
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_StartStmt: {
      StartStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_StartListStmt: {
      StartListStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_StopStmt: {
      StopStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_StopListStmt: {
      StopListStmt2ASCII(stmt, result);
      break;
    }
#endif // VDMPP
    case TAG_TYPE_AS_BlockStmt: {
      BlockStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_NonDetStmt: {
      NonDetStmt2ASCII(stmt, result);
      break;
    }
    case TAG_TYPE_AS_IdentStmt: {
      MakeKeyword(L"skip", result);
      break;
    }
    case TAG_TYPE_AS_SpecificationStmt: {
      SpecStmt2ASCII(stmt, result);
      break;
    }
    default:
      break;
  }

}

void AS2ASCII::DefStmt2ASCII(const TYPE_AS_DefStmt & stmt, wostream & result)
{
  MakeKeyword(L"def", result);
  MakeSpace(result);
  DefBinds2ASCII(stmt.GetSequence(pos_AS_DefStmt_value), result);
  MakeKeyword(L"in", result);
  MakeSpace(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_DefStmt_In), result);
}

void AS2ASCII::LetStmt2ASCII(const TYPE_AS_LetStmt & stmt, wostream & result)
{
  MakeKeyword(L"let", result);
  MakeSpace(result);
  LocalDefs2ASCII(stmt.GetSequence(pos_AS_LetStmt_localdef), result);
  MakeKeyword(L"in", result);
  IncIndent();
  Newline(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_LetStmt_In), result);
  DecIndent();
}

void AS2ASCII::LetBeSTStmt2ASCII(const TYPE_AS_LetBeSTStmt & stmt, wostream & result)
{
  MakeKeyword(L"let", result);
  MakeSpace(result);
  BindList2ASCII(stmt.GetSequence(pos_AS_LetBeSTStmt_bind), result);
  if (!stmt.GetField(pos_AS_LetBeSTStmt_St).IsNil()) {
    MakeSpace(result);
    MakeKeyword(L"be st", result);
    MakeSpace(result);
    Expr2ASCII(stmt.GetRecord(pos_AS_LetBeSTStmt_St), result);
  }

  MakeKeyword(L"in", result);
  MakeSpace(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_LetBeSTStmt_In), result);
}

void AS2ASCII::AssignDef2ASCII(const TYPE_AS_AssignDef & stmt, wostream& result)
{
  wstring var (ExtractName(stmt.GetRecord(pos_AS_AssignDef_var)));
  MakeNameDef(var, result);
  result << L" : ";
  Type2ASCII(stmt.GetRecord(pos_AS_AssignDef_tp), result);
  if (!stmt.GetField(pos_AS_AssignDef_dclinit).IsNil()) {
    result << L" := ";
    Expr2ASCII(stmt.GetRecord(pos_AS_AssignDef_dclinit), result);
  }
}

void AS2ASCII::AssignStmt2ASCII(const TYPE_AS_AssignStmt & stmt, wostream & result)
{
  StateDesignator2ASCII(stmt.GetRecord(pos_AS_AssignStmt_lhs), result);
  result << L" := ";
  Expr2ASCII(stmt.GetRecord(pos_AS_AssignStmt_rhs), result);
}

void AS2ASCII::AtomicAssignStmt2ASCII(const TYPE_AS_AtomicAssignStmt & stmt, wostream & result)
{
  MakeKeyword(L"atomic", result);
  result << L"(";
  Newline(result);
  const SEQ<TYPE_AS_AssignStmt> & atm (stmt.GetSequence(pos_AS_AtomicAssignStmt_atm));
  size_t len_atm = atm.Length();
  for (size_t idx = 1; idx <= len_atm; idx++)
  {
    AssignStmt2ASCII(atm[idx], result);
    if (idx < len_atm)
      result << L";";
  }
  result << L")";
}

void AS2ASCII::SeqForLoopStmt2ASCII(const TYPE_AS_SeqForLoopStmt & stmt, wostream & result)
{
  MakeKeyword(L"for", result);
  MakeSpace(result);
  PatternBind2ASCII(stmt.GetRecord(pos_AS_SeqForLoopStmt_cv), result);
  MakeSpace(result);
  MakeKeyword(L"in", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_SeqForLoopStmt_fseq), result);
  MakeSpace(result);
  MakeKeyword(L"do", result);
  MakeSpace(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_SeqForLoopStmt_body), result);
}

void AS2ASCII::SetForLoopStmt2ASCII(const TYPE_AS_SetForLoopStmt & stmt, wostream & result)
{
  MakeKeyword(L"for all", result);
  MakeSpace(result);
  Pattern2ASCII(stmt.GetRecord(pos_AS_SetForLoopStmt_cv), result);
  MakeSpace(result);
  MakeKeyword(L"in set", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_SetForLoopStmt_fset), result);
  MakeSpace(result);
  MakeKeyword(L"do", result);
  MakeSpace(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_SetForLoopStmt_body), result);
}

void AS2ASCII::IndexForLoopStmt2ASCII(const TYPE_AS_IndexForLoopStmt & stmt, wostream & result)
{
  MakeKeyword(L"for", result);
  MakeSpace(result);
  wstring cv (ExtractName(stmt.GetRecord(pos_AS_IndexForLoopStmt_cv)));
  MakeNameDef(cv, result);
  MakeSpace(result);
  result << L"=";
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_IndexForLoopStmt_lb), result);
  MakeSpace(result);
  MakeKeyword(L"to", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_IndexForLoopStmt_ub), result);
  if (!stmt.GetField(pos_AS_IndexForLoopStmt_By).IsNil())
  {
    MakeSpace(result);
    MakeKeyword(L"by", result);
    MakeSpace(result);
    Expr2ASCII(stmt.GetRecord(pos_AS_IndexForLoopStmt_By), result);
  }
  MakeSpace(result);
  MakeKeyword(L"do", result);
  MakeSpace(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_IndexForLoopStmt_body), result);
}

void AS2ASCII::WhileLoopStmt2ASCII(const TYPE_AS_WhileLoopStmt & stmt, wostream & result)
{
  MakeKeyword(L"while", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_WhileLoopStmt_test), result);
  MakeSpace(result);
  MakeKeyword(L"do", result);
  MakeSpace(result);
  Newline(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_WhileLoopStmt_body), result);
}

void AS2ASCII::CallStmt2ASCII(const TYPE_AS_CallStmt & stmt, wostream & result)
{
#ifdef VDMPP
  if (!stmt.GetField(pos_AS_CallStmt_obj).IsNil())
  {
    Expr2ASCII(stmt.GetRecord(pos_AS_CallStmt_obj), result);
    result << L".";
  }
#endif // VDMPP
  wstring oprt (ExtractName(stmt.GetRecord(pos_AS_CallStmt_oprt)));
  MakeNameRef(oprt, result);
  result << L"(";
  ExprSequence2ASCII(stmt.GetSequence(pos_AS_CallStmt_args), L",", result);
  result << L")";
}

void AS2ASCII::ReturnStmt2ASCII(const TYPE_AS_ReturnStmt & stmt, wostream & result)
{
  MakeKeyword(L"return", result);
  if (!stmt.GetField(pos_AS_ReturnStmt_val).IsNil())
    MakeSpace(result);
    Expr2ASCII(stmt.GetRecord(pos_AS_ReturnStmt_val), result);
}

void AS2ASCII::IfStmt2ASCII(const TYPE_AS_IfStmt & stmt, wostream & result)
{
  MakeKeyword(L"if", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_IfStmt_test), result);
  Newline(result);
  MakeKeyword(L"then", result);
  IncIndent();
  Newline(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_IfStmt_cons), result);
  //  Newline(result);
  DecIndent();

  const SEQ<TYPE_AS_ElseifStmt> & elsif (stmt.GetSequence(pos_AS_IfStmt_elsif));
  if (!elsif.IsEmpty())
  {
    size_t len_elsif = elsif.Length();
    for (size_t idx = 1; idx <= len_elsif; idx++)
      ElseifStmt2ASCII(elsif[idx], result);
  }

  if (!stmt.GetField(pos_AS_IfStmt_altn).IsNil())
  {
    Newline(result);
    MakeKeyword(L"else", result);
    IncIndent();
    Newline(result);
    Stmt2ASCII(stmt.GetRecord(pos_AS_IfStmt_altn), result);
    DecIndent();
  }
}

void AS2ASCII::ElseifStmt2ASCII(const TYPE_AS_ElseifStmt & stmt, wostream & result)
{
  Newline(result);
  MakeKeyword(L"elseif", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_ElseifStmt_test), result);
  Newline(result);
  MakeKeyword(L"then ", result);
  IncIndent();
  Newline(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_ElseifStmt_cons), result);
  DecIndent();
}

void AS2ASCII::CasesStmt2ASCII(const TYPE_AS_CasesStmt & stmt, wostream & result)
{
  MakeKeyword(L"cases", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_CasesStmt_sel), result);
  result << L":";
  IncIndent();
  Newline(result);
  const SEQ<TYPE_AS_CasesStmtAltn> & altns (stmt.GetSequence(pos_AS_CasesStmt_altns));

  size_t len_altns = altns.Length();
  for (size_t idx = 1; idx <= len_altns; idx++)
  {
    CasesStmtAltn2ASCII(altns[idx], result);
    if (idx < len_altns || !stmt.GetField(pos_AS_CasesStmt_Others).IsNil())
      result << L",";
    Newline(result);
  }
  if (!stmt.GetField(pos_AS_CasesStmt_Others).IsNil())
  {
    MakeKeyword(L"others", result);
    MakeSpace(result);
    result << L"-> ";
    IncIndent();
    Newline(result);
    Stmt2ASCII(stmt.GetRecord(pos_AS_CasesStmt_Others), result);
    DecIndent();
  }
  DecIndent();
  Newline(result);
  MakeKeyword(L"end", result);
}

void AS2ASCII::CasesStmtAltn2ASCII(const TYPE_AS_CasesStmtAltn& altn, wostream& result)
{
  PatternSequence2ASCII(altn.GetSequence(pos_AS_CasesStmtAltn_match), result);
  result << L" -> ";
  IncIndent();
  Newline(result);
  Stmt2ASCII(altn.GetRecord(pos_AS_CasesStmtAltn_body), result);
  DecIndent();
}

void AS2ASCII::ErrorStmt2ASCII(const TYPE_AS_ErrorStmt& stmt, wostream& result)
{
  MakeKeyword(L"error", result);
}

void AS2ASCII::ExitStmt2ASCII(const TYPE_AS_ExitStmt& stmt, wostream& result)
{
  MakeKeyword(L"exit", result);
  if (!stmt.GetField(pos_AS_ExitStmt_expr).IsNil())
  {
    MakeSpace(result);
    Expr2ASCII(stmt.GetRecord(pos_AS_ExitStmt_expr), result);
  }
}

void AS2ASCII::AlwaysStmt2ASCII(const TYPE_AS_AlwaysStmt & stmt, wostream & result)
{
  MakeKeyword(L"always", result);
  IncIndent();
  Newline(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_AlwaysStmt_Post), result);
  DecIndent();
  Newline(result);
  MakeKeyword(L"in", result);
  IncIndent();
  Newline(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_AlwaysStmt_body), result);
  DecIndent();
}

void AS2ASCII::TrapStmt2ASCII(const TYPE_AS_TrapStmt & stmt, wostream & result)
{
  MakeKeyword(L"trap", result);
  MakeSpace(result);
  PatternBind2ASCII(stmt.GetRecord(pos_AS_TrapStmt_pat), result);
  MakeSpace(result);
  MakeKeyword(L"with", result);
  IncIndent();
  Newline(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_TrapStmt_Post), result);
  DecIndent();
  Newline(result);
  MakeKeyword(L"in", result);
  IncIndent();
  Newline(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_TrapStmt_body), result);
  DecIndent();
}

void AS2ASCII::RecTrapStmt2ASCII(const TYPE_AS_RecTrapStmt & stmt, wostream & result)
{
  MakeKeyword(L"tixe", result);

  MakeSpace(result);
  result << L"{";
  const SEQ<TYPE_AS_Trap> & traps (stmt.GetSequence(pos_AS_RecTrapStmt_traps));
  size_t len_traps = traps.Length();
  for (size_t idx = 1; idx <= len_traps; idx++)
  {
    Trap2ASCII(traps[idx], result);
    if (idx < len_traps)
      result << L",";
  }
  result << L"}";
  MakeSpace(result);
  MakeKeyword(L"in", result);
  MakeSpace(result);
  Stmt2ASCII(stmt.GetRecord(pos_AS_RecTrapStmt_body), result);
}

void AS2ASCII::Trap2ASCII(const TYPE_AS_Trap & trap, wostream & result)
{
  PatternBind2ASCII(trap.GetRecord(pos_AS_Trap_match), result);
  MakeSpace(result);
  result << L"|->";
  MakeSpace(result);
  Stmt2ASCII(trap.GetRecord(pos_AS_Trap_trappost), result);
}

#ifdef VDMPP
void AS2ASCII::StartStmt2ASCII(const TYPE_AS_StartStmt & stmt, wostream & result)
{
  MakeKeyword(L"start", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_StartStmt_expr), result);
}

void AS2ASCII::StartListStmt2ASCII(const TYPE_AS_StartListStmt & stmt, wostream & result)
{
  MakeKeyword(L"startlist", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_StartListStmt_expr), result);
}
void AS2ASCII::StopStmt2ASCII(const TYPE_AS_StopStmt & stmt, wostream & result)
{
  MakeKeyword(L"stop", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_StopStmt_expr), result);
}

void AS2ASCII::StopListStmt2ASCII(const TYPE_AS_StopListStmt & stmt, wostream & result)
{
  MakeKeyword(L"stoplist", result);
  MakeSpace(result);
  Expr2ASCII(stmt.GetRecord(pos_AS_StopListStmt_expr), result);
}
#endif // VDMPP

void AS2ASCII::BlockStmt2ASCII(const TYPE_AS_BlockStmt & stmt, wostream & result)
{
  result << L"(";
  IncIndent();
  const SEQ<TYPE_AS_AssignDef> & dcls (stmt.GetSequence(pos_AS_BlockStmt_dcls));
  if (!dcls.IsEmpty())
  {
    MakeKeyword(L"dcl", result);
    MakeSpace(result);
    IncIndent(); IncIndent(); IncIndent();
    size_t len_dcls = dcls.Length();
    for (size_t idx = 1; idx <= len_dcls; idx++)
    {
      AssignDef2ASCII(dcls[idx], result);
      if (idx < len_dcls)
      {
        result << L",";
        Newline(result);
      }
      else
        result << L";";
    }
    DecIndent(); DecIndent(); DecIndent();
    Newline(result);
  }

  StmtSequence2ASCII(stmt.GetSequence(pos_AS_BlockStmt_stmts), result);
  DecIndent();
  Newline(result);
  result << L")";
}

void AS2ASCII::NonDetStmt2ASCII(const TYPE_AS_NonDetStmt & stmt, wostream & result)
{
  result << L"||";
  MakeSpace(result);
  result << L"(";
  StmtSequence2ASCII(stmt.GetSequence(pos_AS_NonDetStmt_stmts), result);
  result << L")";
}

void AS2ASCII::SpecStmt2ASCII(const TYPE_AS_SpecificationStmt & stmt, wostream & result)
{
  result << L"SpecStmt2ASCII: To be implemented";
}

void AS2ASCII::StateDesignator2ASCII(const TYPE_AS_StateDesignator & sd, wostream & result)
{
  switch(sd.GetTag()) {
    case TAG_TYPE_AS_Name: {
      MakeNameRef(ExtractName(sd), result);
      break;
    }
    case TAG_TYPE_AS_FieldRef: {
      StateDesignator2ASCII(sd.GetRecord(pos_AS_FieldRef_var), result);
      result << L".";
      result << ExtractName(sd.GetRecord(pos_AS_FieldRef_sel));
      break;
    }
    case TAG_TYPE_AS_MapOrSeqRef: {
      StateDesignator2ASCII(sd.GetRecord(pos_AS_MapOrSeqRef_var), result);
      result << L"(";
      Expr2ASCII(sd.GetRecord(pos_AS_MapOrSeqRef_arg), result);
      result << L")";
      break;
    }
    case TAG_TYPE_AS_NarrowRef: {
      result << L"narrow_(";
      Expr2ASCII(sd.GetRecord(pos_AS_NarrowRef_var), result);
      result << L", ";
      Type2ASCII(sd.GetRecord(pos_AS_NarrowRef_type), result);
      result << L")";
      break;
    }
    default: {
      result << L"unknown stage designator";
      break;
    }
  }
}

void AS2ASCII::StmtSequence2ASCII(const SEQ<TYPE_AS_Stmt> & stmts, wostream & result)
{
  if (!stmts.IsEmpty())
  {
    size_t len_stmts = stmts.Length();
    for (size_t idx = 1; idx <= len_stmts; idx++)
    {
      Stmt2ASCII(stmts[idx], result);
      if (idx < len_stmts)
      {
        result << L";";
        Newline(result);
      }
    }
  }
}

// Patterns and binding
void AS2ASCII::Pattern2ASCII(const TYPE_AS_Pattern & pat, wostream & result)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      PatternName2ASCII(pat, result);
      break;
    }
    case TAG_TYPE_AS_MatchVal: {
      MatchVal2ASCII(pat, result);
      break;
    }
    case TAG_TYPE_AS_SetEnumPattern:
    case TAG_TYPE_AS_SetUnionPattern: {
      SetPattern2ASCII(pat, result);
      break;
    }
    case TAG_TYPE_AS_SeqEnumPattern:
    case TAG_TYPE_AS_SeqConcPattern: {
      SeqPattern2ASCII(pat, result);
      break;
    }
    case TAG_TYPE_AS_TuplePattern: {
      TuplePattern2ASCII(pat, result);
      break;
    }
    case TAG_TYPE_AS_RecordPattern: {
      RecordPattern2ASCII(pat, result);
      break;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      result << L"{";
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      size_t len_mls = mls.Length();
      for (size_t idx = 1; idx <= len_mls; idx++) {
        const TYPE_AS_MapletPattern & mp (mls[idx]);
        Pattern2ASCII(mp.GetRecord(pos_AS_MapletPattern_dp), result);
        result << L" |-> ";
        Pattern2ASCII(mp.GetRecord(pos_AS_MapletPattern_rp), result);
        if (idx < len_mls) {
          result << L",";
        }
      }
      result << L"}";
      break;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      Pattern2ASCII(pat.GetRecord(pos_AS_MapMergePattern_lp), result);
      result << L" munion ";
      Pattern2ASCII(pat.GetRecord(pos_AS_MapMergePattern_rp), result);
      break;
    }
    default: {
      result << L"unknown pattern";
      break;
    }
  }
}

void AS2ASCII::PatternName2ASCII(const TYPE_AS_PatternName & pat, wostream & result)
{
  const Generic & nm (pat.GetField(pos_AS_PatternName_nm));
  if (nm.IsRecord()) {
    if (nm.Is(TAG_TYPE_AS_OldName))
      ;
    else {
      result << ExtractName(nm);
    }
  }
  else
    result << L"-";
}

void AS2ASCII::MatchVal2ASCII(const TYPE_AS_MatchVal & pat, wostream & result)
{
  Expr2ASCII(pat.GetRecord(pos_AS_MatchVal_val), result);
}

void AS2ASCII::SetPattern2ASCII(const TYPE_AS_SetPattern & pat, wostream & result)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_SetEnumPattern: {
      result << L"{";
      PatternSequence2ASCII(pat.GetSequence(pos_AS_SetEnumPattern_Elems), result);
      result << L"}";
      break;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      Pattern2ASCII(pat.GetRecord(pos_AS_SetUnionPattern_lp), result);
      MakeSpace(result);
      MakeKeyword(L"union", result);
      MakeSpace(result);
      Pattern2ASCII(pat.GetRecord(pos_AS_SetUnionPattern_rp), result);
      break;
    }
  }
}

void AS2ASCII::SeqPattern2ASCII(const TYPE_AS_SeqPattern & pat, wostream & result)
{
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_SeqEnumPattern: {
      result << L"[";
      PatternSequence2ASCII(pat.GetSequence(pos_AS_SeqEnumPattern_els), result);
      result << L"]";
      break;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      Pattern2ASCII(pat.GetRecord(pos_AS_SeqConcPattern_lp), result);
      result << L" ^ ";
      Pattern2ASCII(pat.GetRecord(pos_AS_SeqConcPattern_rp), result);
      break;
    }
  }
}

void AS2ASCII::TuplePattern2ASCII(const TYPE_AS_TuplePattern & pat, wostream & result)
{
  result << L"mk_(";
  PatternSequence2ASCII(pat.GetSequence(pos_AS_TuplePattern_fields), result);
  result << L")";
}

void AS2ASCII::RecordPattern2ASCII(const TYPE_AS_RecordPattern & pat, wostream & result)
{
  result << L"mk_";
  result << ExtractName(pat.GetRecord(pos_AS_RecordPattern_nm));
  result << L"(";
  PatternSequence2ASCII(pat.GetSequence(pos_AS_RecordPattern_fields), result);
  result << L")";
}

void AS2ASCII::PatternBind2ASCII(const TYPE_AS_PatternBind & pat, wostream & result)
{
  switch(pat.GetTag()) {
    case TAG_TYPE_AS_PatternName:
    case TAG_TYPE_AS_MatchVal:
    case TAG_TYPE_AS_SetEnumPattern:
    case TAG_TYPE_AS_SetUnionPattern:
    case TAG_TYPE_AS_SeqEnumPattern:
    case TAG_TYPE_AS_SeqConcPattern:
    case TAG_TYPE_AS_TuplePattern:
    case TAG_TYPE_AS_RecordPattern: {
      Pattern2ASCII(pat, result);
      break;
    }
    case TAG_TYPE_AS_SetBind:
    case TAG_TYPE_AS_SeqBind:
    case TAG_TYPE_AS_TypeBind: {
      Bind2ASCII(pat, result);
      break;
    }
    default: {
      break;
    }
  }
}

void AS2ASCII::Bind2ASCII(const TYPE_AS_Bind & bind, wostream & result)
{
  switch(bind.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      SetBind2ASCII(bind, result);
      break;
    }
    case TAG_TYPE_AS_SeqBind: {
      SeqBind2ASCII(bind, result);
      break;
    }
    case TAG_TYPE_AS_TypeBind: {
      TypeBind2ASCII(bind, result);
      break;
    }
  }
}

void AS2ASCII::SetBind2ASCII(const TYPE_AS_SetBind & bind, wostream & result)
{
  Pattern2ASCII(bind.GetRecord(pos_AS_SetBind_pat), result);
  MakeSpace(result);
  MakeKeyword(L"in set", result);
  MakeSpace(result);
  Expr2ASCII(bind.GetRecord(pos_AS_SetBind_Set), result);
}

void AS2ASCII::SeqBind2ASCII(const TYPE_AS_SeqBind & bind, wostream & result)
{
  Pattern2ASCII(bind.GetRecord(pos_AS_SeqBind_pat), result);
  MakeSpace(result);
  MakeKeyword(L"in seq", result);
  MakeSpace(result);
  Expr2ASCII(bind.GetRecord(pos_AS_SeqBind_Seq), result);
}

void AS2ASCII::TypeBind2ASCII(const TYPE_AS_TypeBind & bind, wostream & result)
{
  Pattern2ASCII(bind.GetRecord(pos_AS_TypeBind_pat), result);
  MakeSpace(result);
  MakeKeyword(L":", result);
  MakeSpace(result);
  Type2ASCII(bind.GetRecord(pos_AS_TypeBind_tp), result);
}

void AS2ASCII::BindList2ASCII(const TYPE_AS_BindList & bindlist, wostream & result)
{
  size_t len_bindlist = bindlist.Length();
  for (size_t idx = 1; idx <= len_bindlist; idx++) {
    if (idx > 1) {
      result << L", ";
    }
    MultBind2ASCII(bindlist[idx], result);
  }
}

void AS2ASCII::MultBind2ASCII(const TYPE_AS_MultBind & mb, wostream & result)
{
  switch(mb.GetTag()) {
    case TAG_TYPE_AS_MultSetBind: {
      PatternSequence2ASCII(mb.GetSequence(pos_AS_MultSetBind_pat), result);
      MakeSpace(result);
      MakeKeyword(L"in set", result);
      MakeSpace(result);
      Expr2ASCII(mb.GetRecord(pos_AS_MultSetBind_Set), result);
      break;
    }
    case TAG_TYPE_AS_MultSeqBind: {
      PatternSequence2ASCII(mb.GetSequence(pos_AS_MultSeqBind_pat), result);
      MakeSpace(result);
      MakeKeyword(L"in seq", result);
      MakeSpace(result);
      Expr2ASCII(mb.GetRecord(pos_AS_MultSeqBind_Seq), result);
      break;
    }
    case TAG_TYPE_AS_MultTypeBind: {
      PatternSequence2ASCII(mb.GetSequence(pos_AS_MultTypeBind_pat), result);
      MakeSpace(result);
      MakeKeyword(L":", result);
      MakeSpace(result);
      Type2ASCII(mb.GetRecord(pos_AS_MultTypeBind_tp), result);
      break;
    }
  }
}

void AS2ASCII::ParameterTypes2ASCII(const TYPE_AS_ParameterTypes & partps, wostream & result)
{
  if (!partps.IsEmpty())
  {
    size_t len_partps = partps.Length();
    for (size_t idx = 1; idx <= len_partps; idx++)
    {
      const TYPE_AS_PatTypePair & ptp (partps[idx]);
      PatternSequence2ASCII(ptp.GetSequence(pos_AS_PatTypePair_pats), result);
      result << L" : ";
      Type2ASCII(ptp.GetRecord(pos_AS_PatTypePair_tp), result);
      if (idx < len_partps) {
        result << L",";
      }
    }
  }
}

void AS2ASCII::PatternSequence2ASCII(const SEQ<TYPE_AS_Pattern> & pats, wostream & result)
{
  size_t len_pats = pats.Length();
  for (size_t idx = 1; idx <= len_pats; idx++) {
    Pattern2ASCII(pats[idx], result);
    if (idx < len_pats) {
      result << L",";
    }
  }
}

// Literals
void AS2ASCII::QuoteLit2ASCII(const TYPE_AS_QuoteLit & lit, wostream & result)
{
  const Sequence & s (lit.GetSequence(pos_AS_QuoteLit_val));
  wstring valstr;
  s.GetString(valstr);
  result << L"<" << valstr << L">";
}

