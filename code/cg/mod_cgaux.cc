/***
*  * What
*  *    mod_cgaux.cc: Implementation of mod_cgaux.vdm 1.101
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/cg/mod_cgaux.cc,v $
*  * VERSION
*  *    $Revision: 1.89 $
*  * DATE
*  *    $Date: 2006/07/03 05:21:15 $
*  * STATUS
*  *    Under development
*  * REFERENCES
*  *
*  * PROJECT
*  *    IDERES/AFRODITE
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <math.h>
#include "vdmcg.h"
#include "astaux.h"
#include "BC.h"
#include "REP.h"
#include "AS.h"
#include "projectval.h"
#ifdef VDMPP
#include "MANGLE.h"
#endif // VDMPP
#include "statsem.h"
#include "contextinfo.h"
#include "tbutils.h"
#include "intconvquotes.h"
//#include "mini-p-print.h"
#include "settings.h"

//wcout << L"infer: " << INT2Q::h2gAS(infer) << endl;

#define OLD 1
#define NEW 2

#ifdef VDMPP
#define OpDefAccess      PRIVATE_AS
#define FnDefAccess      PRIVATE_AS
#define InstvarDefAccess PRIVATE_AS
#define ValDefAccess     PRIVATE_AS
#define TypesDefAccess   PRIVATE_AS
#endif // VDMPP

#ifdef VDMPP
// JAVA basic keywords
wstring javabasics[]= {
  L"child",
  L"Math",
  L"Boolean",
  L"Character",
  L"Class",
  L"ClassLoader",
  L"Compiler",
  L"Number",
  L"Process",
  L"Runtime",
  L"SecurityManager",
  L"String",
  L"StringBuffer",
  L"System",
  L"Thread",
  L"ThreadGroup",
  L"Throwable",
  L"Void",
  L"Byte",
  L"Double",
  L"Float",
  L"Integer",
  L"Long",
  L"Short",
  L"Object",
  L"Cloneable",
  L"Runnable",
  L"Exception",
  L"ClassNotFoundException",
  L"CloneNotSupportedException",
  L"IllegalAccessException",
  L"InstantiationException",
  L"InterruptedException",
  L"NoSuchFieldException",
  L"NoSuchMethodException",
  L"RuntimeException",
  L"ArithmeticException",
  L"ArrayStoreException",
  L"ClassCastException",
  L"IllegalArgumentException",
  L"IllegalMonitorStateException",
  L"IllegalStateException",
  L"IndexOutOfBoundsException",
  L"NegativeArraySizeException",
  L"NullPointerException",
  L"SecurityException",
  L"IllegalThreadStateException",
  L"NumberFormatException",
  L"ArrayIndexOutOfBoundsException",
  L"StringIndexOutOfBoundsException",
  L"Error",
  L"LinkageError",
  L"ThreadDeath",
  L"VirtualMachineError",
  L"ClassCircularityError",
  L"ClassFormatError",
  L"ExceptionInInitializerError",
  L"IncompatibleClassChangeError",
  L"NoClassDefFoundError",
  L"UnsatisfiedLinkError",
  L"VerifyError",
  L"InternalError",
  L"OutOfMemoryError",
  L"StackOverflowError",
  L"UnknownError",
  L"AbstractMethodError",
  L"IllegalAccessError",
  L"InstantiationError",
  L"NoSuchFieldError",
  L"NoSuchMethodError",
  L"Serializable",
  L"Enumeration",
  L"clone",
  L"equals",
  L"getClass",
  L"hashCode",
  L"notify",
  L"notifyAll",
  L"toString",
  L"wait",
  L"finalize",
  L"catch",
  L"throw",
  L"abstract",
  L"boolean",
  L"break",
  L"byte",
  L"byvalue",
  L"case",
  L"cast",
  L"default",
  L"do",
  L"double",
  L"else",
  L"extends",
  L"false",
  L"final",
  L"goto",
  L"if",
  L"implements",
  L"import",
  L"inner",
  L"instanceof",
  L"int",
  L"operator",
  L"outer",
  L"package",
  L"private",
  L"protected",
  L"public",
  L"rest",
  L"synchronized",
  L"this",
  L"throws",
  L"transient",
  L"true",
  L"try",
  L"wchar_t",
  L"class",
  L"const",
  L"continue",
  L"enum",
  L"finally",
  L"float",
  L"for",
  L"future",
  L"generic",
  L"interface",
  L"long",
  L"native",
  L"new",
  L"null",
  L"return",
  L"short",
  L"static",
  L"super",
  L"switch",
  L"var",
  L"void",
  L"volatile",
  L"while",
  L""
};

wstring javabasics2[]= {
//  L"clone",
//  L"equals",
//  L"getClass",
//  L"hashCode",
//  L"notify",
//  L"notifyAll",
  L"toString",
//  L"wait",
  L"finalize",
  L"catch",
  L"throw",
  L"abstract",
  L"boolean",
  L"break",
  L"byte",
  L"byvalue",
  L"case",
  L"cast",
  L"default",
//  L"do",
  L"double",
//  L"else",
  L"extends",
//  L"false",
  L"final",
  L"goto",
//  L"if",
  L"implements",
  L"import",
  L"inner",
  L"instanceof",
//  L"int",
  L"operator",
  L"outer",
  L"package",
//  L"private",
//  L"protected",
//  L"public",
  L"rest",
  L"synchronized",
//  L"this",
  L"throws",
  L"transient",
//  L"true",
  L"try",
  L"wchar_t",
//  L"class",
  L"const",
  L"continue",
  L"enum",
  L"finally",
  L"float",
//  L"for",
  L"future",
  L"generic",
  L"interface",
  L"long",
  L"native",
//  L"new",
  L"null",
//  L"return",
  L"short",
//  L"static",
  L"super",
  L"switch",
  L"var",
  L"void",
  L"volatile",
//  L"while"
  L""
};

wstring java_util[] = {
  L"AbstractCollection",
  L"AbstractList",
  L"AbstractMap",
  L"AbstractSet",
  L"ArrayList",
  L"Calendar",
  L"Collection",
  L"Collections",
  L"Comparator",
//  L"Date",
  L"Deque",
  L"Dictionary",
  L"Enumeration",
  L"HashMap",
  L"HashSet",
  L"Hashtable",
  L"InvalidPropertiesFormatException",
  L"Iterator",
  L"List",
  L"ListIterator",
  L"Map",
  L"MissingResourceException",
  L"NavigableSet",
  L"Properties",
  L"Queue",
  L"Random",
  L"RandomAccess",
  L"Set",
  L"SortedMap",
  L"SortedSet",
  L"Time",
  L"TimeZone",
  L"TreeSet",
  L"Vector",
  L""
};

wstring java_sql[] = {
  L"Connection",
  L"DatabaseMetaData",
  L"Date",
  L"Driver",
  L"ParameterMetaData",
  L"PreparedStatement",
  L"ResultSet",
  L"ResultSetMetaData",
  L"SQLData",
  L"Statement",
  L"Timestamp",
  L"Types",
  L""
};

wstring java_io[] = {
  L"Closeable",
  L"Console",
  L"File",
  L"FileDescriptor",
  L"FileFilter",
  L"FileNotFoundException",
  L"FilenameFilter",
  L"FilterOutputStream",
  L"Flushable",
  L"IOException",
  L"InputStream",
  L"ObjectStreamException",
  L"OutputStream",
  L"PrintStream",
  L"PrintWriter",
  L"Reader",
  L"Serializable",
  L"SyncFailedException",
  L"UnsupportedEncodingException",
  L"Writer",
  L""
};

wstring java_net[] = {
  L"ContentHandler",
  L"ContentHandlerFactory",
  L"FileNameMap",
  L"InetAddress",
  L"MalformedURLException",
  L"NetworkInterface",
  L"Proxy",
  L"SocketAddress",
  L"SocketException",
  L"URI",
  L"URISyntaxException",
  L"URL",
  L"URLConnection",
  L"URLStreamHandler",
  L"URLStreamHandlerFactory",
  L"UnknownHostException",
  L""
};

wstring java_security[] = {
  L"CodeSigner",
  L"CodeSource",
  L"GeneralSecurityException",
  L"Guard",
  L"InvalidKeyException",
  L"Key",
  L"KeyException",
  L"NoSuchAlgorithmException",
  L"NoSuchProviderException",
  L"Permission",
  L"PermissionCollection",
  L"Principal",
  L"ProtectionDomain",
  L"PublicKey",
  L"SignatureException",
//  L"Timestamp",
  L""
};

wstring java_security_cert[] = {
  L"CertPath",
  L"Certificate",
  L"CertificateEncodingException",
  L"CertificateException",
  L""
};

wstring java_math[] = {
  L"BigDecimal",
  L"BigInteger",
  L""
};

wstring java_nio[] = {
  L"Buffer",
  L"ByteBuffer",
  L"ByteOrder",
  L"Channel",
  L"CharBuffer",
  L"CharacterCodingException",
  L"Charset",
  L"CharsetDecoder",
  L"CharsetEncoder",
  L"CoderResult",
  L"CodingErrorAction",
  L"DoubleBuffer",
  L"FloatBuffer",
  L"IntBuffer",
  L"LongBuffer",
  L"ShortBuffer",
  L""
};

struct ClassMethodTable {
  const wchar_t * classname;
  const wchar_t * methodname;
};

ClassMethodTable boolT[] = {
 {L"*", L"equals"},
 {L"File", L"canExecute"},
 {L"File", L"canRead"},
 {L"File", L"canWrite"},
 {L"File", L"createNewFile"},
 {L"File", L"delete"},
 {L"File", L"exists"},
 {L"File", L"isAbsolute"},
 {L"File", L"isDirectory"},
 {L"File", L"isFile"},
 {L"File", L"isHidden"},
 {L"File", L"mkdir"},
 {L"File", L"mkdirs"},
 {L"File", L"renameTo"},
 {L"File", L"setExecutable"},
 {L"File", L"setLastModified"},
 {L"File", L"setReadable"},
 {L"File", L"setWritable"},
 {L"Log", L"isTraceEnabled"},
 {L"Log", L"isDebugEnabled"},
 {L"Log", L"isInfoEnabled"},
 {L"Log", L"isWarnEnabled"},
 {L"Log", L"isErrorEnabled"},
 {L"Log", L"isFatalEnabled"},
 {NULL, NULL}
};

ClassMethodTable intT[] = {
 {L"*", L"compareTo"},
 {L"*", L"intValue"},
 {L"*", L"hashCode"},
 {L"Process", L"waitFor"},
 {L"Calendar", L"get"},
 {L"Calendar", L"ERA"},
 {L"Calendar", L"YEAR"},
 {L"Calendar", L"MONTH"},
 {L"Calendar", L"WEEK_OF_YEAR"},
 {L"Calendar", L"WEEK_OF_MONTH"},
 {L"Calendar", L"DATE"},
 {L"Calendar", L"DAY_OF_MONTH"},
 {L"Calendar", L"DAY_OF_YEAR"},
 {L"Calendar", L"DAY_OF_WEEK"},
 {L"Calendar", L"DAY_OF_WEEK_IN_MONTH"},
 {L"Calendar", L"AM_PM"},
 {L"Calendar", L"HOUR"},
 {L"Calendar", L"HOUR_OF_DAY"},
 {L"Calendar", L"MINUTE"},
 {L"Calendar", L"SECOND"},
 {L"Calendar", L"MILLISECOND"},
 {L"Calendar", L"ZONE_OFFSET"},
 {L"Calendar", L"DST_OFFSET"},
 {L"Calendar", L"FIELD_COUNT"},
 {L"BigDecimal", L"ROUND_CEILING"},
 {L"BigDecimal", L"ROUND_DOWN"},
 {L"BigDecimal", L"ROUND_FLOOR"},
 {L"BigDecimal", L"ROUND_HALF_DOWN"},
 {L"BigDecimal", L"ROUND_HALF_EVEN"},
 {L"BigDecimal", L"ROUND_HALF_UP"},
 {L"BigDecimal", L"ROUND_UNNECESSARY"},
 {L"BigDecimal", L"ROUND_UP"},
 {L"BigDecimal", L"precision"},
 {L"BigDecimal", L"scale"},
 {L"BigDecimal", L"signum"},
 {L"*", L"byteValue"},
 {L"*", L"longValue"},
 {L"*", L"shortValue"},
 {NULL, NULL}
};

ClassMethodTable realT[] = {
 {L"*", L"doubleValue"},
 {L"*", L"floatValue"},
 {NULL, NULL}
};

// JavaAccess
// acc : AS`Access
// -> CPP`Modifier
TYPE_CPP_Modifier vdmcg::JavaAccess (const Int & acc)
{
 switch ( acc.GetValue() ) {
   case DEFAULT_AS:   { return vdm_BC_GenModifier(quote_PRIVATE); }
   case PRIVATE_AS:   { return vdm_BC_GenModifier(quote_PRIVATE); }
   case PROTECTED_AS: { return vdm_BC_GenModifier(quote_PROTECTED); }
   case PUBLIC_AS:    { return vdm_BC_GenModifier(quote_PUBLIC); }
   default:           { return vdm_BC_GenModifier(quote_PRIVATE); }
 }
}

// UnQClassType
// tp : REP`ObjRefTypeRep
// -> REP`ObjRefTypeRep
TYPE_REP_ObjRefTypeRep vdmcg::UnQClassType (const TYPE_REP_ObjRefTypeRep& tp)
{
  const TYPE_AS_Name & nm (tp.GetRecord(pos_REP_ObjRefTypeRep_nm));
  const TYPE_AS_Ids & ids (nm.GetSequence(pos_AS_Name_ids));
  if ((ids.Length () == 2) && isclassType (nm)) {
    return mk_REP_ObjRefTypeRep(ASTAUX::MkNameFromIds (ids.Tl (), nm.GetInt(pos_AS_Name_cid)));
  }
  else {
    return tp;
  }
}
#endif //VDMPP

// CGAUX functions and operations

// FindType
// expr : AS`Expr | AS`Pattern | AS`FieldRef | AS`MapOrSeqRef
// -> [REP`TypeRep]
Generic vdmcg::FindType(const Record & expr)
{
//wcout << L"FindType: " << INT2Q::h2gAS(expr) << endl;
  switch(expr.GetTag()) {
    case TAG_TYPE_AS_TokenConstructorExpr:
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr:
#endif // VDMPP
    case TAG_TYPE_AS_SetEnumerationExpr:
    case TAG_TYPE_AS_SeqEnumerationExpr:
    case TAG_TYPE_AS_MapEnumerationExpr:
    case TAG_TYPE_AS_TupleConstructorExpr:
    case TAG_TYPE_AS_Name:
    case TAG_TYPE_AS_OldName:
    case TAG_TYPE_AS_BracketedExpr:

    case TAG_TYPE_AS_PatternName:
    case TAG_TYPE_AS_SetEnumPattern:
    case TAG_TYPE_AS_SeqEnumPattern:
    case TAG_TYPE_AS_MapEnumPattern:
    case TAG_TYPE_AS_TuplePattern:
    case TAG_TYPE_AS_DefExpr:
    case TAG_TYPE_AS_LetExpr:
    case TAG_TYPE_AS_PrefixExpr:
    case TAG_TYPE_AS_ExistsUniqueExpr:
    case TAG_TYPE_AS_SetRangeExpr:
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr:
    case TAG_TYPE_AS_RecordConstructorExpr:
    case TAG_TYPE_AS_RecordModifierExpr:
    case TAG_TYPE_AS_ApplyExpr:
    case TAG_TYPE_AS_FieldSelectExpr:
    case TAG_TYPE_AS_IsExpr:
    case TAG_TYPE_AS_IotaExpr:
    case TAG_TYPE_AS_RecordPattern:
    case TAG_TYPE_AS_FctTypeInstExpr:
    case TAG_TYPE_AS_LambdaExpr:
#ifdef VDMPP
    case TAG_TYPE_AS_IsOfClassExpr:
    case TAG_TYPE_AS_IsOfBaseClassExpr:
    case TAG_TYPE_AS_SameBaseClassExpr:
    case TAG_TYPE_AS_SameClassExpr:
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr:
#endif // VICE
    case TAG_TYPE_AS_ObjectPattern:
#endif //VDMPP

    case TAG_TYPE_AS_LetBeSTExpr:
    case TAG_TYPE_AS_CasesExpr:
    case TAG_TYPE_AS_BinaryExpr:
    case TAG_TYPE_AS_AllOrExistsExpr:
    case TAG_TYPE_AS_SetComprehensionExpr:
    case TAG_TYPE_AS_SeqComprehensionExpr:
    case TAG_TYPE_AS_SubSequenceExpr:
    case TAG_TYPE_AS_MapComprehensionExpr:
    case TAG_TYPE_AS_IfExpr:
    case TAG_TYPE_AS_MatchVal:
    case TAG_TYPE_AS_FieldRef:
    case TAG_TYPE_AS_MapOrSeqRef:
    case TAG_TYPE_AS_CallStmt:
    case TAG_TYPE_AS_NarrowExpr:
    case TAG_TYPE_AS_Macro:
    case TAG_TYPE_AS_TupleSelectExpr: {

      TYPE_CI_ContextId cid (ASTAUX::GetCid (expr));
      Generic cti (GetCI().GetTypeInfo(cid));

      Generic qti;
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        qti = cti;
      }
      else
#endif // VDMPP
      {
        qti = GenRQComp(GiveCurCASName(), cti);
      }

      Generic reptp = Nil();
      if (!qti.IsNil()) {
        reptp = CleanFlatType(qti);
      }

//wcout << "expr: " << expr << endl;
//wcout << "reptp: " << reptp << endl;

#ifdef VDMPP
      if (reptp.Is (TAG_TYPE_REP_ObjRefTypeRep)) {
        return UnQClassType (reptp);
      }
      else
#endif // VDMPP
      {
        return reptp;
      }
    }

    // Basic types
    case TAG_TYPE_AS_BoolLit:
    case TAG_TYPE_AS_TypeJudgementExpr:
    case TAG_TYPE_AS_PreConditionApplyExpr: {
      return TYPE_REP_BooleanTypeRep();
    }
    case TAG_TYPE_AS_CharLit: {
      return TYPE_REP_CharTypeRep();
    }
    case TAG_TYPE_AS_TextLit: {
      return mk_REP_SeqTypeRep(mk_REP_CharTypeRep());
    }
    case TAG_TYPE_AS_QuoteLit: {
      TYPE_AS_Id val (expr.GetSequence(pos_AS_QuoteLit_val));
      const TYPE_CI_ContextId & cid (expr.GetInt(pos_AS_QuoteLit_cid));
#ifdef VDMPP
      InsertQuote(val);
#endif // VDMPP
      return mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId(val, cid));
    }
    case TAG_TYPE_AS_RealLit: {
      const Real & v (expr.GetReal(pos_AS_RealLit_val));
      const TYPE_CI_ContextId & cid (expr.GetInt(pos_AS_RealLit_cid));
      if (cid != NilContextId) {
        TYPE_REP_TypeRep tp (GetCI().GetTypeInfo(cid));
        if (tp.Is(TAG_TYPE_REP_NumericTypeRep)) {
          return tp;
        }
      }
      if (v.IsInt()) {
        if (v.IsNatOne()) {
          return mk_REP_NumericTypeRep(Int(NATONE));
        }
        else if (v.IsNat()) {
          return mk_REP_NumericTypeRep(Int(NAT));
        }
        else {
          return mk_REP_NumericTypeRep(Int(INTEGER));
        }
      }
      else {
        return mk_REP_NumericTypeRep(Int(REAL));
      }
    }
    case TAG_TYPE_AS_NumLit: {
      const Real & v (expr.GetReal(pos_AS_NumLit_val));
      const TYPE_CI_ContextId & cid (expr.GetInt(pos_AS_NumLit_cid));
      if (cid != NilContextId) {
        TYPE_REP_TypeRep tp (GetCI().GetTypeInfo(cid));
        if (tp.Is(TAG_TYPE_REP_NumericTypeRep))
          return tp;
      }
       if (v.IsNatOne()) {
        return mk_REP_NumericTypeRep(Int(NATONE));
      }
      else if (v.IsNat()) {
        return mk_REP_NumericTypeRep(Int(NAT));
      }
      else {
        return mk_REP_NumericTypeRep(Int(INTEGER));
      }
    }
    case TAG_TYPE_AS_NilLit: {
      return mk_REP_NilTypeRep();
    }
    case TAG_TYPE_AS_UndefinedExpr: {
      return mk_REP_AllTypeRep();
    }
    // Other VDM++ constructs
#ifdef VDMPP
    case TAG_TYPE_AS_NewExpr: {
      TYPE_AS_NewExpr ne (expr);
      return mk_REP_ObjRefTypeRep(expr.GetRecord(pos_AS_NewExpr_cls));
    }
#endif //VDMPP
    case TAG_TYPE_AS_SetUnionPattern:
    case TAG_TYPE_AS_SeqConcPattern:
    case TAG_TYPE_AS_MapMergePattern:
    case TAG_TYPE_AS_MapletPattern: {
      return mk_REP_AllTypeRep();
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ActExpr:
    case TAG_TYPE_AS_FinExpr:
    case TAG_TYPE_AS_ActiveExpr:
    case TAG_TYPE_AS_WaitingExpr:
    case TAG_TYPE_AS_ReqExpr:
    case TAG_TYPE_AS_ThreadIdExpr: {
      return mk_REP_NumericTypeRep(Int(NAT));
    }
    case TAG_TYPE_AS_GuardExpr: {
      return TYPE_REP_BooleanTypeRep();
    }
#endif // VDMPP
    default: {
      ReportUndefined(L"FindType");
      return Record(0,0); // To avoid warnings
    }
  }
}

#ifdef VDMPP
// ExchangeDefAccess
// rec : AS`InstAssignDef|AS`InstanceInv|AS`TypeDef|AS`FnDef|AS`OpDef|AS`ValueDef
// -> AS`InstAssignDef|AS`InstanceInv|AS`TypeDef|AS`FnDef|AS`OpDef|AS`ValueDef
Record vdmcg::ExchangeDefAccess (const Record & rec)
{
  switch (rec.GetTag ()) {
    case TAG_TYPE_AS_InstAssignDef:
      if (rec.GetField(pos_AS_InstAssignDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_InstAssignDef rr (rec);
        rr.set_access (Int (InstvarDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_InstanceInv:
      if (rec.GetField(pos_AS_InstanceInv_access) == Int (DEFAULT_AS)) {
        TYPE_AS_InstanceInv rr (rec);
        rr.set_access (Int (InstvarDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_TypeDef:
      if (rec.GetField(pos_AS_TypeDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_TypeDef rr (rec);
        rr.set_access (Int (TypesDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_ExplFnDef:
      if (rec.GetField(pos_AS_ExplFnDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_ExplFnDef rr (rec);
        rr.set_access (Int (FnDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_ImplFnDef:
      if (rec.GetField(pos_AS_ImplFnDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_ImplFnDef rr (rec);
        rr.set_access (Int (FnDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_ExtExplFnDef:
      if (rec.GetField(pos_AS_ExtExplFnDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_ExtExplFnDef rr (rec);
        rr.set_access (Int (FnDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_ExplOpDef:
      if (rec.GetField(pos_AS_ExplOpDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_ExplOpDef rr (rec);
        rr.set_access (Int (OpDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_ImplOpDef:
      if (rec.GetField(pos_AS_ImplOpDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_ImplOpDef rr (rec);
        rr.set_access (Int (OpDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_ExtExplOpDef:
      if (rec.GetField(pos_AS_ExtExplOpDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_ExtExplOpDef rr (rec);
        rr.set_access (Int (OpDefAccess));
        return rr;
      }
      break;
    case TAG_TYPE_AS_ValueDef:
      if (rec.GetField(pos_AS_ValueDef_access) == Int (DEFAULT_AS)) {
        TYPE_AS_ValueDef rr (rec);
        rr.set_access (Int (ValDefAccess));
        return rr;
      }
      break;
  }
  return rec;
}

// ConvAsAccess2CPP
// a : AS`Access
// -> [<PRIVATE>|<PROTECTED>|<PUBLIC>]
Generic vdmcg::ConvAsAccess2CPP (const TYPE_AS_Access & a)
{
  if (a.IsInt()) {
    switch (Int(a).GetValue ()) {
      case PRIVATE_AS:   { return quote_PRIVATE; }
      case PROTECTED_AS: { return quote_PROTECTED; }
      case PUBLIC_AS:    { return quote_PUBLIC; }
      default:           { return Nil (); }
    }
  }
  return Nil (); // dummy
}
#endif // VDMPP

// FindPatIdMap
// expr : AS`Expr
// -> map AS`Name to REP`TypeRep
MAP<TYPE_AS_Name, TYPE_REP_TypeRep> vdmcg::FindPatIdMap(const TYPE_AS_Expr& expr)
{
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> pid_m (GetCI().GetPidM(ASTAUX::GetCid(expr)));
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> res_m;
  SET<TYPE_AS_Name> dom_pid_m (pid_m.Dom());
  Generic nm;
  for (bool bb = dom_pid_m.First(nm); bb; bb = dom_pid_m.Next(nm)) {
    TYPE_REP_TypeRep tp (pid_m[nm]);
    if (IsCyclicTypeRep(tp)) {
      tp = RemoveInvType(CleanFlatType(tp));
    }
    else {
      TYPE_REP_TypeRep newtp (RemoveInvType(CleanFlatType(tp)));
      while (tp != newtp) {
        tp = newtp;
        newtp = RemoveInvType(CleanFlatType(tp));
      }
    }
    res_m.Insert(nm, tp);
  }
  return res_m;
}

// IsSeqOfCPPStmt
// cp : (seq of CPP`Stmt) | CPP`Expr
// -> bool
bool vdmcg::IsSeqOfCPPStmt(const Generic & cp )
{
  if (cp.IsSequence() && !Sequence (cp).IsEmpty()) {
    SEQ<TYPE_CPP_Stmt> stmts (cp);
    TYPE_CPP_Stmt stmt (stmts.Hd());
    switch(stmt.GetTag()) {
      case TAG_TYPE_CPP_LabelStmt:
      case TAG_TYPE_CPP_ExpressionStmt:
      case TAG_TYPE_CPP_CompoundStmt:
      case TAG_TYPE_CPP_IfStmt:
      case TAG_TYPE_CPP_SwitchStmt:
      case TAG_TYPE_CPP_WhileStmt:
      case TAG_TYPE_CPP_DoWhileStmt:
      case TAG_TYPE_CPP_ForStmt:
      case TAG_TYPE_CPP_Break:
      case TAG_TYPE_CPP_Continue:
      case TAG_TYPE_CPP_Return:
      case TAG_TYPE_CPP_Goto:
      case TAG_TYPE_CPP_DeclarationStmt:
      case TAG_TYPE_CPP_IncludeStmt: {
        return true;
      }
      default: {
        return false;
      }
    }
  }
  else {
    return false;
  }
}

// CleanFlatType
// reptp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::CleanFlatType(const TYPE_REP_TypeRep & reptp)
{
  TYPE_REP_TypeRep tp;
  SET<TYPE_REP_TypeNameRep> ss;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (reptp.Is(TAG_TYPE_REP_TypeNameRep)) {
      const TYPE_AS_Name & nm (reptp.GetRecord(pos_REP_TypeNameRep_nm));
      Generic qnm;
      qnm = GenRootType(GenQName(nm)); // [AS`Name] | REP`TypeRep
      if (qnm.IsNil()) {
        tp = reptp;
      }
      else if (!qnm.Is(TAG_TYPE_AS_Name)) {
        tp = qnm;
      }
      else {
        tp = mk_REP_TypeNameRep(qnm);
      }
    }
    else {
      tp = reptp;
    }
    Generic extp (ExpandTypeRep(tp, Set()));
    TYPE_REP_TypeRep thistp (CleanAndFlattenType(extp));
    if (tp.Is(TAG_TYPE_REP_TypeNameRep) &&
        thistp.Is(TAG_TYPE_REP_UnionTypeRep) &&
        IsCompositeType(thistp)) {
      ss.Insert(tp);
    }
  }
  else
#endif // VDMPP
  { // C++
    tp = reptp;
  }
  Generic exp_tp (ExpandTypeRep(tp, ss));
  return CleanAndFlattenType(exp_tp);
}

// ExpandTypeRep
// tp : REP`TypeRep
// nms : set of REP`TypeNameRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::ExpandTypeRep(const TYPE_REP_TypeRep & tp, const SET<TYPE_REP_TypeNameRep> & nms)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ts (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> s;
      Generic g;
      for (bool bb = ts.First(g); bb; bb = ts.Next(g) ) {
        s.Insert(ExpandTypeRep(g, nms));
      }
      return mk_REP_UnionTypeRep(s);
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tpl (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> l;
      size_t len_tpl = tpl.Length();
      for (size_t idx = 1; idx <= len_tpl; idx++) {
        l.ImpAppend(ExpandTypeRep(tpl[idx], nms));
      }
      return mk_REP_ProductTypeRep(l);
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(ExpandTypeRep(tp.GetRecord(pos_REP_SetTypeRep_elemtp), nms));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return mk_REP_EmptySetTypeRep(ExpandTypeRep(tp.GetRecord(pos_REP_EmptySetTypeRep_elemtp), nms));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(ExpandTypeRep(tp.GetRecord(pos_REP_SeqTypeRep_elemtp), nms));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return mk_REP_EmptySeqTypeRep(ExpandTypeRep(tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp), nms));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(ExpandTypeRep(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom), nms),
                                      ExpandTypeRep(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng), nms));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(ExpandTypeRep(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom), nms),
                                        ExpandTypeRep(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng), nms));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_REP_EmptyMapTypeRep(ExpandTypeRep(tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom), nms),
                                    ExpandTypeRep(tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng), nms));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      const TYPE_AS_Name & name (tp.GetRecord(pos_REP_InvTypeRep_name));
      SET<TYPE_REP_TypeNameRep> new_nms (nms);
      new_nms.Insert(TYPE_REP_TypeNameRep().Init(name));
      return mk_REP_InvTypeRep(name,
                               ExpandTypeRep(tp.GetRecord(pos_REP_InvTypeRep_shape), new_nms),
                               tp.GetRecord(pos_REP_InvTypeRep_invariant));
    }
    case TAG_TYPE_REP_TypeNameRep: {
      if (nms.InSet(tp)) {
        return tp;
      }
      else {
        return ExpandTypeRep(LOT(tp), nms.Union(mk_set(tp)));
      }
    }
#ifdef VDMSL
    case TAG_TYPE_REP_CompositeTypeRep: {
      const TYPE_AS_Name & tag (tp.GetRecord(pos_REP_CompositeTypeRep_nm));
      Set imported (get_imported_types());
      if (imported.InSet(tag)) {
        return ExpandTypeRep(LOT(mk_REP_TypeNameRep(tag)), nms);
      }
      return tp; 
    }
#endif // VDMSL
    case TAG_TYPE_REP_OpTypeRep: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        return tp;
      }
      else
#endif // VDMPP
      {
        const SEQ<TYPE_REP_TypeRep> & dtpl (tp.GetSequence(pos_REP_OpTypeRep_Dom));
        SEQ<TYPE_REP_TypeRep> new_dtpl;
        size_t len_dtpl = dtpl.Length();
        for (size_t idx = 1; idx <= len_dtpl; idx++) {
          new_dtpl.ImpAppend(ExpandTypeRep(dtpl[idx], nms));
        }
        return mk_REP_OpTypeRep(new_dtpl, ExpandTypeRep(tp.GetRecord(pos_REP_OpTypeRep_Rng), nms));
      }
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        return tp;
      }
      else
#endif // VDMPP
      {
        const SEQ<TYPE_REP_TypeRep> & dtpl (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
        SEQ<TYPE_REP_TypeRep> new_dtpl;
        size_t len_dtpl = dtpl.Length();
        for (size_t idx = 1; idx <= len_dtpl; idx++) {
          new_dtpl.ImpAppend(ExpandTypeRep(dtpl[idx], nms));
        }
        return mk_REP_PartialFnTypeRep(new_dtpl,
                                       ExpandTypeRep(tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng), nms));
      }
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
#ifdef VDMPP
      if (vdm_CPP_isJAVA()) {
        return tp;
      }
      else
#endif // VDMPP
      {
        const SEQ<TYPE_REP_TypeRep> & dtpl (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
        SEQ<TYPE_REP_TypeRep> new_dtpl;
        size_t len_dtpl = dtpl.Length();
        for (size_t idx = 1; idx <= len_dtpl; idx++) {
          new_dtpl.ImpAppend(ExpandTypeRep(dtpl[idx], nms));
        }
        return mk_REP_TotalFnTypeRep(new_dtpl, ExpandTypeRep(tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng), nms));
      }
    }
    default: {
      return tp;
    }
  }
}

// CleanAndFlattenType
// type : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::CleanAndFlattenType (const TYPE_REP_TypeRep& type)
{
  SET<TYPE_REP_TypeRep> tps_clean (CleanAndFlattenTypeAux(type));
  if (tps_clean.Card() == 1) {
    return tps_clean.GetElem();
  }
  else {
    return mk_REP_UnionTypeRep(tps_clean);
  }
}

// CleanAndFlattenTypeAux
// type : REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> vdmcg::CleanAndFlattenTypeAux (const TYPE_REP_TypeRep& type)
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_SetTypeRep : {
      TYPE_REP_SetTypeRep s;
      s.Init(CleanAndFlattenType(type.GetRecord(pos_REP_SetTypeRep_elemtp)));
      SET<TYPE_REP_TypeRep> res;
      return (res.Insert(s));
    }
    case TAG_TYPE_REP_SeqTypeRep : {
      TYPE_REP_SeqTypeRep s;
      s.Init(CleanAndFlattenType(type.GetRecord(pos_REP_SeqTypeRep_elemtp)));
      SET<TYPE_REP_TypeRep> res;
      return (res.Insert(s));
    }
  
    case TAG_TYPE_REP_CompositeTypeRep : {
      const SEQ<TYPE_REP_FieldRep> & fl (type.GetSequence(pos_REP_CompositeTypeRep_fields));
      SEQ<TYPE_REP_FieldRep> fl_new;
      size_t len_fl = fl.Length(); 
      for (size_t idx = 1; idx <= len_fl; idx++) {
        TYPE_REP_FieldRep f (fl[idx]);
        f.SetField(pos_REP_FieldRep_tp, CleanAndFlattenType(fl[idx].GetRecord(pos_REP_FieldRep_tp)));
        fl_new.ImpAppend(f);
      }
      SET<TYPE_REP_TypeRep> res;
      return (res.Insert(mk_REP_CompositeTypeRep(type.GetRecord(pos_REP_CompositeTypeRep_nm), fl_new)));
    }
  
    case TAG_TYPE_REP_UnionTypeRep : {
      SET<TYPE_REP_TypeRep> ts (type.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> type_s;
      Generic t;
      for (bool bb = ts.First(t); bb; bb = ts.Next(t)) {
        type_s.ImpUnion(CleanAndFlattenTypeAux(t));
      }
      SET<TYPE_REP_TypeRep> emset;
      Generic s;
      for (bool cc = type_s.First(s); cc; cc = type_s.Next(s)) {
        TYPE_REP_TypeRep t (s);
        switch (t.GetTag()) {
          case TAG_TYPE_REP_SetTypeRep: {
            emset.Insert(mk_REP_EmptySetTypeRep(t.GetRecord(pos_REP_SetTypeRep_elemtp)));
            break;
          }
          case TAG_TYPE_REP_SeqTypeRep: {
            emset.Insert(mk_REP_EmptySeqTypeRep(t.GetRecord(pos_REP_SeqTypeRep_elemtp)));
            break;
          }
          case TAG_TYPE_REP_GeneralMapTypeRep: {
            emset.Insert(mk_REP_EmptyMapTypeRep(t.GetRecord(pos_REP_GeneralMapTypeRep_mapdom),
                                                t.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
            break;
          }
          case TAG_TYPE_REP_InjectiveMapTypeRep: {
            emset.Insert(mk_REP_EmptyMapTypeRep(t.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom),
                                                t.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
            break;
          }
        }
      }
      return OptimizeType(type_s.ImpDiff(emset));
    }
  
    case TAG_TYPE_REP_ProductTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & fl (type.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> fl_new;
      size_t len_fl = fl.Length();
      for (size_t idx = 1; idx <= len_fl; idx++) {
        fl_new.ImpAppend(CleanAndFlattenType(fl[idx]));
      }
      SET<TYPE_REP_TypeRep> res;
      return res.Insert(TYPE_REP_ProductTypeRep().Init(fl_new));
    }
  
    case TAG_TYPE_REP_GeneralMapTypeRep : {
      TYPE_REP_GeneralMapTypeRep r;
      r.Init(CleanAndFlattenType(type.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
             CleanAndFlattenType(type.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
      SET<TYPE_REP_TypeRep> res;
      return res.Insert(r);
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep : {
      TYPE_REP_InjectiveMapTypeRep r;
      r.Init(CleanAndFlattenType(type.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
             CleanAndFlattenType(type.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
      SET<TYPE_REP_TypeRep> res;
      return res.Insert(r);
    }
// 20110626 -->
    case TAG_TYPE_REP_InvTypeRep : {
      SET<TYPE_REP_TypeRep> res;
      //return res.Insert(CleanAndFlattenType(type.GetRecord(pos_REP_InvTypeRep_shape)));
      TYPE_REP_InvTypeRep r (type);
      r.SetField(pos_REP_InvTypeRep_shape, CleanAndFlattenType(type.GetRecord(pos_REP_InvTypeRep_shape)));
      return res.Insert(r);
    }
// <-- 20110626
    default: {
      SET<TYPE_REP_TypeRep> res;
      return res.Insert(type);
    }
  }
}

// OptimizeType
// tp : set of REP`TypeRep
// -> set of REP`TypeRep
SET<TYPE_REP_TypeRep> vdmcg::OptimizeType(const SET<TYPE_REP_TypeRep>& tps_)
{
  SET<TYPE_REP_TypeRep> settypes, seqtypes, genmaptypes, injmaptypes;
  SET<TYPE_REP_TypeRep> tps (tps_);
  Generic t;
  for (bool bb = tps.First(t); bb; bb = tps.Next(t)) {
    switch (Record(t).GetTag()) {
      case TAG_TYPE_REP_SetTypeRep: {
        settypes.Insert(t);
        break;
      }
      case TAG_TYPE_REP_SeqTypeRep: {
        seqtypes.Insert(t);
        break;
      }
      case TAG_TYPE_REP_GeneralMapTypeRep: {
        genmaptypes.Insert(t);
        break;
      }
      case TAG_TYPE_REP_InjectiveMapTypeRep: {
        injmaptypes.Insert(t);
        break;
      }
    }
  }

  // Remove the stuff handled by combine
  tps.ImpDiff(settypes);
  tps.ImpDiff(seqtypes);
  tps.ImpDiff(genmaptypes);
  tps.ImpDiff(injmaptypes);

  // Insert the combined stuff
  tps.ImpUnion(CombineSetTypes (settypes));
  tps.ImpUnion(CombineSeqTypes (seqtypes));
  tps.ImpUnion(CombineGenMapTypes (genmaptypes));
  tps.ImpUnion(CombineInjMapTypes (injmaptypes));

  return tps;
}

// CombineSetTypeRep
// ts : set of REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::CombineSetTypeRep(const SET<TYPE_REP_TypeRep> & ts)
{
  if ( ts.Card() == 1 ) {
    return ts.GetElem();
  }
  else {
    SET<TYPE_REP_TypeRep> tsc_s;
    SET<TYPE_REP_TypeRep> ts_q (ts);
    Generic e;
    for (bool bb = ts_q.First(e); bb; bb = ts_q.Next(e)) {
      TYPE_REP_TypeRep t (e);
      if (t.Is(TAG_TYPE_REP_SetTypeRep)) {
        tsc_s.Insert(t.GetRecord(pos_REP_SetTypeRep_elemtp) );
      }
      else if (t.Is(TAG_TYPE_REP_EmptySetTypeRep)) {
        tsc_s.Insert(t.GetRecord(pos_REP_EmptySetTypeRep_elemtp) );
      }
      else {
        ReportError(L"CombineSetTypeRep");
      }
    }
    if (tsc_s.Card() == 1) {
      return mk_REP_SetTypeRep(tsc_s.GetElem());
    }
    else {
      return mk_REP_SetTypeRep(mk_REP_UnionTypeRep(tsc_s));
    }
  }
}

// CombineSeqTypeRep
// ts : set of REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::CombineSeqTypeRep(const SET<TYPE_REP_TypeRep>& ts)
{
  if ( ts.Card() == 1 ) {
    return ts.GetElem();
  }
  else {
    SET<TYPE_REP_TypeRep> ts_q (ts);
    SET<TYPE_REP_TypeRep> tsc_s;
    Generic e;
    for (bool bb = ts_q.First(e); bb; bb = ts_q.Next(e)) {
      TYPE_REP_TypeRep t (e);
      if (t.Is(TAG_TYPE_REP_SeqTypeRep)) {
        tsc_s.Insert(t.GetRecord(pos_REP_SeqTypeRep_elemtp) );
      }
      else if (t.Is(TAG_TYPE_REP_EmptySeqTypeRep)) {
        tsc_s.Insert(t.GetRecord(pos_REP_EmptySeqTypeRep_elemtp) );
      }
      else {
        ReportError(L"CombineSeqTypeRep");
      }
    }
    if (tsc_s.Card() == 1) {
      return mk_REP_SeqTypeRep(tsc_s.GetElem());
    }
    else {
      return mk_REP_SeqTypeRep(mk_REP_UnionTypeRep(tsc_s));
    }
  }
}

// CombineInjectiveMapTypeRep
// ts : set of REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::CombineInjectiveMapTypeRep(const SET<TYPE_REP_TypeRep>& ts)
{
  if ( ts.Card() == 1 ) {
    return ts.GetElem();
  }
  else {
    SET<TYPE_REP_TypeRep> md_s, mr_s;
    SET<TYPE_REP_TypeRep> ts_q (ts);
    Generic e;
    for (bool bb = ts_q.First(e); bb; bb = ts_q.Next(e)) {
      TYPE_REP_TypeRep t (e);
      if (t.Is(TAG_TYPE_REP_InjectiveMapTypeRep)) {
        md_s.Insert(t.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
        mr_s.Insert(t.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
      }
      else if (t.Is(TAG_TYPE_REP_EmptyMapTypeRep)) { // 20110626
        md_s.Insert(t.GetRecord(pos_REP_EmptyMapTypeRep_mapdom));
        mr_s.Insert(t.GetRecord(pos_REP_EmptyMapTypeRep_maprng));
      }
      else {
        ReportError(L"CombineInjectiveMapTypeRep");
      }
    }
    return mk_REP_InjectiveMapTypeRep(mk_REP_UnionTypeRep(md_s), mk_REP_UnionTypeRep(mr_s));
  }
}

// CombineGeneralMapTypeRep
// ts : set of REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::CombineGeneralMapTypeRep(const SET<TYPE_REP_TypeRep>& ts)
{
  if ( ts.Card() == 1 ) {
    return ts.GetElem();
  }
  else {
    SET<TYPE_REP_TypeRep> md_s, mr_s;
    SET<TYPE_REP_TypeRep> ts_q (ts);
    Generic e;
    for (bool bb = ts_q.First(e); bb; bb = ts_q.Next(e)) {
      TYPE_REP_TypeRep t (e);
      if (t.Is(TAG_TYPE_REP_GeneralMapTypeRep)) {
        md_s.Insert(t.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
        mr_s.Insert(t.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
      }
      else if (t.Is(TAG_TYPE_REP_EmptyMapTypeRep)) { // 20110626
        md_s.Insert(t.GetRecord(pos_REP_EmptyMapTypeRep_mapdom));
        mr_s.Insert(t.GetRecord(pos_REP_EmptyMapTypeRep_maprng));
      }
      else {
        ReportError(L"CombineGeneralMapTypeRep");
      }
    }
    return mk_REP_GeneralMapTypeRep(mk_REP_UnionTypeRep(md_s), mk_REP_UnionTypeRep(mr_s));
  }
}

// FindPossibleMapType
// arg : REP`UnionTypeRep | REP`GeneralMapTypeRep | REP`InjectiveMapTypeRep
// -> REP`GeneralMapTypeRep | REP`InjectiveMapTypeRep
TYPE_REP_TypeRep vdmcg::FindPossibleMapType(const TYPE_REP_TypeRep & arg)
{
  switch(arg.GetTag()) {
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return arg;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      return mk_REP_GeneralMapTypeRep(mk_REP_AllTypeRep(), mk_REP_AllTypeRep());
    }
    default: {
      ReportError(L"FindPossibleMapType");
      return TYPE_REP_TypeRep();
    }
  }
}

// FindPossibleSetType
// - : REP`UnionTypeRep
// -> REP`SetTypeRep
TYPE_REP_SetTypeRep vdmcg::FindPossibleSetType(const TYPE_REP_UnionTypeRep &)
{
  return mk_REP_SetTypeRep(mk_REP_AllTypeRep());
}

// FindPosResSetTypeInSetDistrUnion
// argtype : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::FindPosResSetTypeInSetDistrUnion(const TYPE_REP_TypeRep & argtype)
{
//  return FindPosSetSeq(rt, TAG_TYPE_REP_SetTypeRep);
  switch(argtype.GetTag()) {
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (argtype.GetSet(pos_REP_UnionTypeRep_tps));
      Generic tp;
      for (bool bb = tps.First(tp); bb; bb = tps.Next(tp)) {
        TYPE_REP_TypeRep s (CleanFlatType(tp));
        if (s.Is(TAG_TYPE_REP_SetTypeRep)) {
          return FindPosResSetTypeInSetDistrUnion(s);
        }
      }
      break;
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return CleanFlatType(argtype.GetRecord(pos_REP_SetTypeRep_elemtp));
    }
    default: {
      // pre condition violated
      ReportUndefined(L"FindPosResSetTypeInSetDistrUnion");
    }
  }
  return TYPE_REP_TypeRep(); // To avoid warnings
}

// FindPosResSeqTypeInSeqDistr
// argtype : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::FindPosResSeqTypeInSeqDistr(const TYPE_REP_TypeRep & argtype)
{
  switch(argtype.GetTag()) {
    case TAG_TYPE_REP_InvTypeRep: {
      const TYPE_REP_TypeRep & shape (argtype.GetRecord(pos_REP_InvTypeRep_shape));
      return FindPosResSeqTypeInSeqDistr(shape);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (argtype.GetSet(pos_REP_UnionTypeRep_tps));
      Generic tp;
      for (bool bb = tps.First(tp); bb; bb = tps.Next(tp)) {
        TYPE_REP_TypeRep s (CleanFlatType(tp));
        if (s.Is(TAG_TYPE_REP_SeqTypeRep)) {
          return FindPosResSeqTypeInSeqDistr(s);
        }
      }
      break;
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return CleanFlatType(argtype.GetRecord(pos_REP_SeqTypeRep_elemtp));
    }
    default: {
      // pre condition violated
      ReportUndefined(L"FindPosResSeqTypeInSeqDistr");
    }
  }
  return TYPE_REP_TypeRep(); // To avoid warnings
}

// CombineSetTypes
// s : set of REP`SetTypeRep
// -> set of REP`SetTypeRep
SET<TYPE_REP_SetTypeRep> vdmcg::CombineSetTypes(const SET<TYPE_REP_SetTypeRep> & s)
{
  switch (s.Card()) {
    case 0:
    case 1: {
      return s;
    }
    default: {
      SET<TYPE_REP_SetTypeRep> tps (s);
      SET<TYPE_REP_TypeRep> st;
      Generic e;
      for (bool bb = tps.First(e); bb; bb = tps.Next(e)) {
        st.Insert (Record(e).GetRecord(pos_REP_SetTypeRep_elemtp));
      }
      SET<TYPE_REP_SetTypeRep>  res;
      res.Insert (mk_REP_SetTypeRep(mk_REP_UnionTypeRep(st)));
      return res;
    }
  }
}

// CombineSeqTypes
// s : set of REP`SeqTypeRep
// -> set of REP`SeqTypeRep
SET<TYPE_REP_SeqTypeRep> vdmcg::CombineSeqTypes(const SET<TYPE_REP_SeqTypeRep> & s)
{
  switch (s.Card()) {
    case 0:
    case 1: {
      return s;
    }
    default: {
      SET<TYPE_REP_SeqTypeRep> tps (s);
      SET<TYPE_REP_TypeRep> st;
      Generic e;
      for (bool bb = tps.First(e); bb; bb = tps.Next(e)) {
        st.Insert (Record(e).GetRecord(pos_REP_SeqTypeRep_elemtp));
      }
      SET<TYPE_REP_SeqTypeRep> res;
      res.Insert (mk_REP_SeqTypeRep(mk_REP_UnionTypeRep(st)));
      return res;
    }
  }
}

// CombineGenMapTypes
// s : set of REP`GeneralMapTypeRep
// -> set of REP`GeneralMapTypeRep
SET<TYPE_REP_GeneralMapTypeRep> vdmcg::CombineGenMapTypes(
                                  const SET<TYPE_REP_GeneralMapTypeRep> & s)
{
  switch (s.Card()) {
    case 0:
    case 1: {
      return s;
    }
    default: {
      SET<TYPE_REP_GeneralMapTypeRep> tps (s);
      SET<TYPE_REP_TypeRep> md_s, mr_s;
      Generic e;
      for (bool bb = tps.First(e); bb; bb = tps.Next(e)) {
        TYPE_REP_GeneralMapTypeRep t (e);
        md_s.Insert (t.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
        mr_s.Insert (t.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
      }

      SET<TYPE_REP_GeneralMapTypeRep> res;
      res.Insert (mk_REP_GeneralMapTypeRep(mk_REP_UnionTypeRep(md_s),
                                           mk_REP_UnionTypeRep(mr_s)));
      return res;
    }
  }
}

// CombineInjMapTypes
// s : set of REP`InjectiveMapTypeRep
// -> set of REP`InjectiveMapTypeRep
SET<TYPE_REP_InjectiveMapTypeRep> vdmcg::CombineInjMapTypes(
                                    const SET<TYPE_REP_InjectiveMapTypeRep> & s)
{
  switch (s.Card()) {
    case 0:
    case 1: {
      return s;
    }
    default: {
      SET<TYPE_REP_InjectiveMapTypeRep> tps (s);
      SET<TYPE_REP_TypeRep> md_s, mr_s;
      Generic e;
      for (bool bb = tps.First(e); bb; bb = tps.Next(e)) {
        TYPE_REP_InjectiveMapTypeRep t (e);
        md_s.Insert (t.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
        mr_s.Insert (t.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
      }

      SET<TYPE_REP_InjectiveMapTypeRep> res;
      res.Insert (mk_REP_InjectiveMapTypeRep(mk_REP_UnionTypeRep(md_s),
                                             mk_REP_UnionTypeRep(mr_s)));
      return res;
    }
  }
}

// FromAS2RepType
// atype : [AS`Type]
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::FromAS2RepType(const Generic & atype)
{
  TYPE_REP_TypeRep exp_tp (FromAS2RepTypeAux(atype));
  return CleanFlatType(exp_tp);
}

// FromAS2RepTypeAux
// atype : [AS`Type]
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::FromAS2RepTypeAux(const Generic & atype)
{
  if (atype.IsNil()) {
    return  TYPE_REP_NilTypeRep();
  }
  TYPE_AS_Type at (atype);
  switch (at.GetTag()) {
    case TAG_TYPE_AS_BooleanType: {
      return mk_REP_BooleanTypeRep();
    }
    case TAG_TYPE_AS_NumericType: {
      return mk_REP_NumericTypeRep(at.GetInt(pos_AS_NumericType_qtp));
    }
    case TAG_TYPE_AS_TokenType: {
      return mk_REP_TokenTypeRep();
    }
    case TAG_TYPE_AS_CharType: {
      return mk_REP_CharTypeRep();
    }
    case TAG_TYPE_AS_VoidType: {
      return mk_REP_UnitTypeRep();
    }
    case TAG_TYPE_AS_QuoteType: {
      const TYPE_AS_QuoteLit & t (at.GetRecord(pos_AS_QuoteType_lit));
      const TYPE_AS_Id & name (t.GetSequence(pos_AS_QuoteLit_val));
      const TYPE_CI_ContextId & cid (t.GetInt(pos_AS_QuoteLit_cid)); 
#ifdef VDMPP
      InsertQuote(name);
#endif // VDMPP
      return mk_REP_QuoteTypeRep(ASTAUX::MkNameFromId(name, cid));
    }
    case TAG_TYPE_AS_CompositeType: {
      const SEQ<TYPE_AS_Field> & fields (at.GetSequence(pos_AS_CompositeType_fields));
      SEQ<TYPE_REP_FieldRep> fds;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        const TYPE_AS_Field & field (fields[idx]);
        fds.ImpAppend(mk_REP_FieldRep(field.GetField(pos_AS_Field_sel),
                                      FromAS2RepTypeAux(field.GetRecord(pos_AS_Field_type)),
                                      field.GetBool(pos_AS_Field_dc)));
      }
      return mk_REP_CompositeTypeRep(at.GetRecord(pos_AS_CompositeType_name), fds);
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & type_l (at.GetSequence(pos_AS_UnionType_tps));
      SET<TYPE_REP_TypeRep> ti_s;
      size_t len_type_l = type_l.Length();
      for (size_t idx = 1; idx <= len_type_l; idx++) {
        ti_s.Insert(FromAS2RepTypeAux(type_l[idx]));
      }
      return mk_REP_UnionTypeRep(ti_s);
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & type_l (at.GetSequence(pos_AS_ProductType_tps));
      SEQ<TYPE_REP_TypeRep> ti_l;
      size_t len_type_l = type_l.Length();
      for (size_t idx = 1; idx <= len_type_l; idx++) {
        ti_l.ImpAppend(FromAS2RepTypeAux(type_l[idx]));
      }
      return mk_REP_ProductTypeRep(ti_l);
    }
    case TAG_TYPE_AS_OptionalType: {
      SET<TYPE_REP_TypeRep> s;
      s.Insert(FromAS2RepTypeAux(at.GetRecord(pos_AS_OptionalType_tp)));
      s.Insert(mk_REP_NilTypeRep());
      return mk_REP_UnionTypeRep(s);
    }
    case TAG_TYPE_AS_Set0Type: {
      return mk_REP_SetTypeRep(FromAS2RepTypeAux(at.GetRecord(pos_AS_Set0Type_elemtp)));
    }
    case TAG_TYPE_AS_Set1Type: {
      return mk_REP_SetTypeRep(FromAS2RepTypeAux(at.GetRecord(pos_AS_Set1Type_elemtp)));
    }
    case TAG_TYPE_AS_Seq0Type: {
      return mk_REP_SeqTypeRep(FromAS2RepTypeAux(at.GetRecord(pos_AS_Seq0Type_elemtp)));
    }
    case TAG_TYPE_AS_Seq1Type: {
      return mk_REP_SeqTypeRep(FromAS2RepTypeAux(at.GetRecord(pos_AS_Seq1Type_elemtp)));
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      return mk_REP_GeneralMapTypeRep(
             FromAS2RepTypeAux(at.GetRecord(pos_AS_GeneralMap0Type_mapdom)),
             FromAS2RepTypeAux(at.GetRecord(pos_AS_GeneralMap0Type_maprng)));
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      return mk_REP_GeneralMapTypeRep(
             FromAS2RepTypeAux(at.GetRecord(pos_AS_GeneralMap1Type_mapdom)),
             FromAS2RepTypeAux(at.GetRecord(pos_AS_GeneralMap1Type_maprng)));
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      return mk_REP_InjectiveMapTypeRep(
             FromAS2RepTypeAux(at.GetRecord(pos_AS_InjectiveMap0Type_mapdom)),
             FromAS2RepTypeAux(at.GetRecord(pos_AS_InjectiveMap0Type_maprng)));
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      return mk_REP_InjectiveMapTypeRep(
             FromAS2RepTypeAux(at.GetRecord(pos_AS_InjectiveMap1Type_mapdom)),
             FromAS2RepTypeAux(at.GetRecord(pos_AS_InjectiveMap1Type_maprng)));
    }
    case TAG_TYPE_AS_OpType: {
      const SEQ<TYPE_AS_Type> & dom_l (at.GetSequence(pos_AS_OpType_opdom));
      SEQ<TYPE_REP_TypeRep> domrep_l;
      size_t len_dom_l = dom_l.Length();
      for (size_t idx = 1; idx <= len_dom_l; idx++) {
        domrep_l.ImpAppend(FromAS2RepTypeAux(dom_l[idx]));
      }
      return mk_REP_OpTypeRep(domrep_l, FromAS2RepTypeAux(at.GetRecord(pos_AS_OpType_oprng)));
    }
    case TAG_TYPE_AS_TypeVar: {
      return mk_REP_TypeVarRep(at.GetRecord(1));
    }
    case TAG_TYPE_AS_PartialFnType: {
      const SEQ<TYPE_AS_Type> & dom_l (at.GetSequence(pos_AS_PartialFnType_fndom));
      SEQ<TYPE_REP_TypeRep> domrep_l;
      size_t len_dom_l = dom_l.Length();
      for (size_t idx = 1; idx <= len_dom_l; idx++) {
        domrep_l.ImpAppend(FromAS2RepTypeAux(dom_l[idx]));
      }
      return mk_REP_PartialFnTypeRep(domrep_l, FromAS2RepTypeAux(at.GetRecord(pos_AS_PartialFnType_fnrng)));
    }
    case TAG_TYPE_AS_TotalFnType: {
      const SEQ<TYPE_AS_Type> & dom_l (at.GetSequence(pos_AS_TotalFnType_fndom));
      SEQ<TYPE_REP_TypeRep> domrep_l;
      size_t len_dom_l = dom_l.Length();
      for (size_t idx = 1; idx <= len_dom_l; idx++) {
        domrep_l.ImpAppend(FromAS2RepTypeAux(dom_l[idx]));
      }
      return mk_REP_TotalFnTypeRep(domrep_l, FromAS2RepTypeAux(at.GetRecord(pos_AS_TotalFnType_fnrng)));
    }
    case TAG_TYPE_AS_BracketedType: {
      return FromAS2RepTypeAux(at.GetRecord(pos_AS_BracketedType_tp));
    }
    case TAG_TYPE_AS_TypeName: {
#ifdef VDMSL
      Generic qnm (GenQName(at.GetRecord(pos_AS_TypeName_name)));
      return mk_REP_TypeNameRep(qnm);
#endif // VDMSL
#ifdef VDMPP
      const TYPE_AS_Name & nm (at.GetRecord(pos_AS_TypeName_name));
      if (IsClass(nm)) {
        return mk_REP_ObjRefTypeRep(nm); 
      }
      Generic gqm (GenQName(at.GetRecord(pos_AS_TypeName_name)));
      Generic qnm (GenRootType(gqm));
  
      if (qnm.IsNil()) {
        return mk_REP_TypeNameRep(at.GetRecord(pos_AS_TypeName_name));
      }
      // Note here that this differs from the spec, since in the spec
      // composite types are automatically qualified with the class
      // name; here this has to be done manually.
      else {
        switch(Record(qnm).GetTag()) {
          case TAG_TYPE_AS_Name: {
            return mk_REP_TypeNameRep(qnm);
          }
          case TAG_TYPE_REP_CompositeTypeRep: {
            TYPE_REP_CompositeTypeRep ctr (qnm);
            if (ctr.get_nm().get_ids().Length() == 1) {
              ctr.set_nm(QualiName(GiveCurCASName(), ctr.get_nm()));
              return ctr;
            }
            else {
              return qnm;
            }
          }
          default: {
            return qnm;
          }
        }
      }
#endif // VDMPP
      //  }
    }
    case TAG_TYPE_AS_AllType: {
      return mk_REP_AllTypeRep();
    }
    default:
      ReportUndefined(L"FromAS2RepTypeAux");
  }
  return Record(0,0); // To avoid warnings
}

// RunTime
// mess : seq of char
// -> CPP`Stmt
TYPE_CPP_Stmt vdmcg::RunTime(const wstring & mess)
{
  return vdm_BC_GenExpressionStmt(RunTimeExpr(ASTAUX::MkId(mess)));
}

// RunTimeExpr
// mess : seq of char
// -> CPP`Expr
TYPE_CPP_Expr vdmcg::RunTimeExpr(const TYPE_AS_Id & mess)
{
  TYPE_CPP_Identifier fname;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    fname = vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.RunTime"));
  }
  else
#endif // VDMPP
  {
    fname = vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::RunTime"));
  }
  return vdm_BC_GenFctCall(fname, mk_sequence(vdm_BC_GenStringLit(mess)));
}

// NotSupported
// cst : seq of char
// -> CPP`Stmt
TYPE_CPP_Stmt vdmcg::NotSupported(const wstring & cst, const Record & r)
{
  TYPE_CPP_Identifier id;
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    SetNotSupportedException(true);
    id = vdm_BC_GenIdentifier(ASTAUX::MkId(L"UTIL.NotSupported"));
  }
  else
#endif // VDMPP
  {
    id = vdm_BC_GenIdentifier(ASTAUX::MkId(L"CGUTIL::NotSupported"));
  }

  GetStatSem().GenErr(r, StatSem::WRN1, 357, mk_sequence(SEQ<Char>(cst)));

  return vdm_BC_GenExpressionStmt(vdm_BC_GenFctCall(id, mk_sequence(vdm_BC_GenStringLit(ASTAUX::MkId(cst)))));
}

// NumberTypeCheck
// cond : CPP`Expr
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::NumberTypeCheck(const TYPE_CPP_Expr & expr,
                                          const Generic & cond,
                                          const wstring & cst) {
  TYPE_CPP_Expr c (cond.IsNil() ? vdm_BC_GenNot(GenIsReal(expr)) : (TYPE_CPP_Expr)cond);
  wstring msg (cst.empty() ? L"A number was expected" : cst);
  return TypeCheck(c, msg);
}

// IntegerTypeCheck
// expr : CPP`Expr
// cond : [CPP`Expr]
// cst : [seq of char]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::IntegerTypeCheck(const TYPE_CPP_Expr & expr,
                                           const Generic & cond,
                                           const wstring & cst) {
  TYPE_CPP_Expr c (cond.IsNil() ? vdm_BC_GenNot(GenIsInt(expr)) : (TYPE_CPP_Expr)cond);
  wstring msg (cst.empty() ? L"A integer was expected" : cst);
  return TypeCheck(c, msg);
}

// BooleanTypeCheck
// expr : CPP`Expr
// cond : [CPP`Expr]
// cst : [seq of char]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::BooleanTypeCheck(const TYPE_CPP_Expr & expr,
                                           const Generic & cond,
                                           const wstring & cst) {
  TYPE_CPP_Expr c (cond.IsNil() ? vdm_BC_GenNot(GenIsBool(expr)) : (TYPE_CPP_Expr)cond);
  wstring msg (cst.empty() ? L"A boolean was expected" : cst);
  return TypeCheck(c, msg);
}

// SetTypeCheck
// expr : CPP`Expr
// cond : [CPP`Expr]
// cst : [seq of char]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::SetTypeCheck(const TYPE_CPP_Expr & expr,
                                       const Generic & cond,
                                       const wstring & cst) {
  TYPE_CPP_Expr c (cond.IsNil() ? vdm_BC_GenNot(GenIsSet(expr)) : (TYPE_CPP_Expr)cond);
  wstring msg (cst.empty() ? L"A set was expected" : cst);
  return TypeCheck(c, msg);
}

// SeqTypeCheck
// expr : CPP`Expr
// cond : [CPP`Expr]
// cst : [seq of char]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::SeqTypeCheck(const TYPE_CPP_Expr & expr,
                                       const Generic & cond,
                                       const wstring & cst) {
  TYPE_CPP_Expr c (cond.IsNil() ? vdm_BC_GenNot(GenIsSeq(expr)) : (TYPE_CPP_Expr)cond);
  wstring msg (cst.empty() ? L"A sequence was expected" : cst);
  return TypeCheck(c, msg);
}

// MapTypeCheck
// expr ; CPP`Expr
// cond : [CPP`Expr]
// cst : [seq of char]
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::MapTypeCheck(const TYPE_CPP_Expr & expr,
                                       const Generic & cond,
                                       const wstring & cst) {
  TYPE_CPP_Expr c (cond.IsNil() ? vdm_BC_GenNot(GenIsMap(expr)) : (TYPE_CPP_Expr)cond);
  wstring msg (cst.empty() ? L"A map was expected" : cst);
  return TypeCheck(c, msg);
}

// TypeCheck
// cond : CPP`Expr
// cst : wstring
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::TypeCheck(const TYPE_CPP_Expr & cond, const wstring & cst) {
  SEQ<TYPE_CPP_Stmt> rb_l;
  rb_l.ImpAppend(vdm_BC_GenIfStmt(cond, vdm_BC_GenBlock(mk_sequence(RunTime(cst))), nil));
  return rb_l;
}

// CannotCG
void vdmcg::CannotCG(const wstring & cst, const TYPE_AS_Name & r)
{
  GetStatSem().GenErr(r, StatSem::ERR, 361, mk_sequence(SEQ<Char>(cst)));
//  NO_OF_ERRORS++;
  TBUTILS::IncrementErrorsCount();
}

// GiveLastName
// p_name : ( AS`Name | AS`OldName )
// -> AS`Id
TYPE_AS_Id vdmcg::GiveLastName(const Record & p_name)
{
  switch(p_name.GetTag()) {
    case TAG_TYPE_AS_Name: {
      TYPE_AS_Ids l_ids (p_name.GetSequence(pos_AS_Name_ids));
      return CleanIdentifier(l_ids[l_ids.Length()]);
    }
    case TAG_TYPE_AS_OldName: {
      TYPE_AS_Ids l_ids (p_name.GetSequence(pos_AS_OldName_ids));
      return CleanIdentifier(l_ids[l_ids.Length()]);
    }
  }
  return TYPE_AS_Id();
}

// GiveFirstName
// p_name : AS`Name
// -> AS`Name
TYPE_AS_Name vdmcg::GiveFirstName(const TYPE_AS_Name & name)
{
  return ASTAUX::GetFirstName(name);
}

// GiveSecondName
// p_name : AS`Name
// -> AS`Name
TYPE_AS_Name vdmcg::GiveSecondName(const TYPE_AS_Name & name)
{
  return ASTAUX::GetSecondName(name);
}

// CleanIdentifier
// p_id : AS`Id
// ==> AS`Id
TYPE_AS_Id vdmcg::CleanIdentifier(const TYPE_AS_Id & p_id)
{
  wstring str;
  p_id.GetString(str);
  wstring res;
  size_t len = str.length();
  for (size_t i = 0; i < len; i++) {
    wchar_t c = str[i];
    switch (c) {
      case '\'': {
        res += L"_q";
        break;
      }
      case '_': {
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          res += c;
        }
        else
#endif // VDMPP
        {
          if ( ( (i == 2) && (str.find(L"eq") == 0) ) ||
               ( (i == 3) && ( (str.find(L"inv") == 0) || (str.find(L"pre") == 0) ||
                               (str.find(L"ord") == 0) ) ) ||
               ( (i == 4) && ( str.find(L"post") == 0 ) ) ) {
            res +=L"_";
          }
          else {
            res += L"_u";
          }
        }
        break;
      }
      default: {
        res += c;
        break;
      }
    }
  }

#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    bool isbasic = false;
    if ((res.size() > 4) && (res.substr(0,4) == wstring(L"JDK_"))) {
      wstring nres = res.substr(4);
      InsertJDKImport(nres);
      res = nres;
    }
    else if ((res.size() > 5) && (res.substr(0,5) == wstring(L"STUB_"))) {
      wstring nres = res.substr(5);
      res = nres;
    }
    else if ((res.size() > 11) && (res.substr(0,11) == wstring(L"JDKArrayOf_"))) {
      wstring nres = res.substr(11) + L"[]";
      res = nres;
    }
    else if (get_java_rename_option()) {
      int index = 0;
      while (!javabasics[index].empty() && !isbasic) {
        if (javabasics[index] == res) {
          isbasic = true;
        }
        index++;
      }
    }
    else {
      int index = 0;
      while (!javabasics2[index].empty() && !isbasic) {
        if (javabasics2[index] == res) {
          isbasic = true;
        }
        index++;
      }
    }
    if (isbasic) {
      res = L"vdm_" + res;
    }
  }
#endif // VDMPP

  return ASTAUX::MkId(res);
}

// AreOfSameType
// tp1 : REP`TypeRep
// tp2 : REP`TypeRep
// -> bool
bool vdmcg::AreOfSameType(const TYPE_REP_TypeRep & tp1, const TYPE_REP_TypeRep & tp2)
{
  if (tp1.Is(TAG_TYPE_REP_InvTypeRep)) {
    return AreOfSameType(tp1.GetRecord(pos_REP_InvTypeRep_shape), tp2);
  }
  if (tp2.Is(TAG_TYPE_REP_InvTypeRep)) {
    return AreOfSameType(tp1, tp2.GetRecord(pos_REP_InvTypeRep_shape));
  }
  int tag1 = tp1.GetTag();
  int tag2 = tp2.GetTag();

  return
   ((IsMapType(tp1) && IsMapType(tp2)) ||
    (IsSetType(tp1) && IsSetType(tp2)) ||
    (IsSeqType(tp1) && IsSeqType(tp2)) ||
    (IsIntType(tp1) && IsIntType(tp2)) ||
    (IsBoolType(tp1) && IsBoolType(tp2)) ||
    (IsRealType(tp1) && IsRealType(tp2)) ||
    (IsCharType(tp1) && IsCharType(tp2)) ||
    (IsQuoteType(tp1) && IsQuoteType(tp2)) ||
    (IsProductType(tp1) && IsProductType(tp2)) ||
   ((tag1 == TAG_TYPE_REP_CompositeTypeRep) && (tag2 == TAG_TYPE_REP_CompositeTypeRep)));
}

// IsMapType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsMapType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep:
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        forall = IsMapType(tp);
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsMapType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default : {
      return false;
    }
  }
}

// IsMapType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsPosMapType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_GeneralMapTypeRep:
    case TAG_TYPE_REP_InjectiveMapTypeRep:
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp))
        exists = IsPosMapType(tp);
      return exists;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsPosMapType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return true;
    }
    default : {
      return false;
    }
  }
}

// IsSeqType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsSeqType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_EmptySeqTypeRep:
    case TAG_TYPE_REP_SeqTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp))
        forall = IsSeqType(tp);
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsSeqType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default : {
      return false;
    }
  }
}

// IsPosSeqType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsPosSeqType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_EmptySeqTypeRep:
    case TAG_TYPE_REP_SeqTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp)) {
        exists = IsPosSeqType(tp);
      }
      return exists;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsPosSeqType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return true;
    }
    default : {
      return false;
    }
  }
}

// IsStringType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsStringType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      //return false;
      return IsCharType(type.GetRecord(pos_REP_EmptySeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return IsCharType(type.GetRecord(pos_REP_SeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        forall = IsStringType(tp);
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsStringType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return false;
    }
  }
}

// IsPossibleStringType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsPossibleStringType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return IsCharType(type.GetRecord(pos_REP_EmptySeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return IsCharType(type.GetRecord(pos_REP_SeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      tp_s.ImpDiff(mk_set(TYPE_REP_EmptySeqTypeRep()));
      if (tp_s.IsEmpty()) {
        return false;
      }
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        if (IsSeqType(tp)) {
          forall = IsStringType(tp);
        }
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsPossibleStringType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default : {
      return false;
    }
  }
}

// IsSetType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsSetType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_EmptySetTypeRep:
    case TAG_TYPE_REP_SetTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        forall = IsSetType(tp);
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsSetType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return false;
    }
  }
}

// IsPosSetType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsPosSetType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_EmptySetTypeRep:
    case TAG_TYPE_REP_SetTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp)) {
        exists = IsPosSetType(tp);
      }
      return exists;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsPosSetType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return true;
    }
    default : {
      return false;
    }
  }
}

// IsNumType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsNumType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_NumericTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
#ifdef VDMPP
        if (tp.Is(TAG_TYPE_REP_NilTypeRep)) {
          forall = vdm_CPP_isJAVA();
        }
        else
#endif //VDMPP
        {
          forall = IsNumType(tp);
        }
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsNumType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default : {
      return false;
    }
  }
}

// IsIntType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsIntType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_NumericTypeRep: {
      switch (type.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
        case INTEGER:
        case NATONE:
        case NAT: {
          return true;
        }
        default: {
          return false;
        }
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsIntType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default : {
      return false;
    }
  }
}

// IsRealType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsRealType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_NumericTypeRep: {
      switch (type.GetIntValue(pos_REP_NumericTypeRep_qtp)) {
        case RAT:
        case REAL: {
          return true;
        }
        default: {
          return false;
        }
      }
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsRealType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default : {
      return false;
    }
  }
}

// IsPosNumType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsPosNumType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_NumericTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        forall = IsPosNumType(tp);
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsPosNumType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return false;
    }
  }
}

// IsPosIntType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsPosIntType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_NumericTypeRep: {
      return IsIntType(type);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        forall = IsPosIntType(tp);
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsPosIntType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return false;
    }
  }
}

// IsBoolType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsBoolType(const TYPE_REP_TypeRep & type) const
{
  return (type.Is(TAG_TYPE_REP_BooleanTypeRep));
}

// IsCharType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsCharType(const TYPE_REP_TypeRep & type) const
{
  return (type.Is(TAG_TYPE_REP_CharTypeRep));
}

// IsPosCharType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsPosCharType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_CharTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp)) {
        exists = IsPosCharType(tp);
      }
      return exists; 
    }
    default: {
      return false;
    }
  }
}

// IsCompositeType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsCompositeType(const TYPE_REP_TypeRep & type)
{
  switch(type.GetTag()) {
    case TAG_TYPE_REP_CompositeTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        TYPE_REP_TypeRep t (LOT(tp));
        forall = IsCompositeType(t);
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsCompositeType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return false;
    }
  }
}

// IsPosCompositeType
// ti : REP`TypeRep
// -> bool
bool vdmcg::IsPosCompositeType (const TYPE_REP_TypeRep & ti)
{
  switch (ti.GetTag()) {
    case TAG_TYPE_REP_CompositeTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ti_s (ti.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = ti_s.First (tp); bb && !exists; bb = ti_s.Next (tp)) {
        TYPE_REP_TypeRep t (LOT(tp));
        exists = IsPosCompositeType(t);
      }
      return exists;
    }
    case TAG_TYPE_REP_TypeNameRep: {
      return IsPosCompositeType(LOT(ti));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsPosCompositeType(ti.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return true;
    }
    default: {
      return false;
    }
  }
}

// IsProductType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsProductType(const TYPE_REP_TypeRep & type)
{
  switch(type.GetTag()) {
    case TAG_TYPE_REP_ProductTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        TYPE_REP_TypeRep t (LOT(tp));
        forall = IsProductType(t);
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsProductType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return false;
    }
  }
}

// IsPosProductType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsPosProductType(const TYPE_REP_TypeRep & type)
{
  switch(type.GetTag()) {
    case TAG_TYPE_REP_ProductTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp)) {
        TYPE_REP_TypeRep t (LOT(tp));
        exists = IsPosProductType(t);
      }
      return exists;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsPosProductType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_AllTypeRep: {
      return true;
    }
    default: {
      return false;
    }
  }
}

// IsQuoteType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsQuoteType(const TYPE_REP_TypeRep& type)
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_QuoteTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        //forall = LOT (tp).Is(TAG_TYPE_REP_QuoteTypeRep);
        TYPE_REP_TypeRep t (LOT (tp));
        forall = IsQuoteType(t);
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsQuoteType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return false;
    }
  }
}

#ifdef VDMPP
// IsObjRefType
// type : REP`TypeRep
// -> bool
bool vdmcg::IsObjRefType(const TYPE_REP_TypeRep& type)
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_ObjRefTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        if (tp.Is(TAG_TYPE_REP_NilTypeRep)) {
          forall = vdm_CPP_isJAVA();
        }
        else {
          forall = IsObjRefType(LOT (tp));
        }
      }
      return forall;
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return IsObjRefType(type.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
      return false;
    }
  }
}
#endif //VDMPP

// PossibleFnType
// type : REP`TypeRep
// -> bool
bool vdmcg::PossibleFnType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_PartialFnTypeRep:
    case TAG_TYPE_REP_TotalFnTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp)) {
        exists = PossibleFnType(tp);
      }
      return exists;
    }
#ifdef VDMPP
    case TAG_TYPE_REP_OverTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_OverTypeRep_tps));
      bool exists = false;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && !exists; bb = tp_s.Next(tp)) {
        exists = PossibleFnType(tp);
      }
      return exists;
    }
#endif // VDMPP
    default: {
      return false;
    }
  }
}

// IsFctType
// type : REP`TypeRep -> bool
bool vdmcg::IsFctType(const TYPE_REP_TypeRep & type) const
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_PartialFnTypeRep:
    case TAG_TYPE_REP_TotalFnTypeRep: {
      return true;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        forall = IsFctType(tp);
      }
      return forall;
    }
#ifdef VDMPP
    case TAG_TYPE_REP_OverTypeRep: {
      SET<TYPE_REP_TypeRep> tp_s (type.GetSet(pos_REP_OverTypeRep_tps));
      bool forall = true;
      Generic tp;
      for (bool bb = tp_s.First(tp); bb && forall; bb = tp_s.Next(tp)) {
        forall = IsFctType(tp);
      }
      return forall;
    }
#endif // VDMPP
    default: { return false; }
  }
}

// IsUnionFunctionType
// ti : REP`TypeRep
// -> bool
bool vdmcg::IsUnionFunctionType (const TYPE_REP_TypeRep & ti) const
{
  switch(ti.GetTag()) {
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (ti.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false;
      Generic t;
      for (bool bb = tps.First (t); bb && !exists; bb = tps.Next (t)) {
        switch (Record(t).GetTag()) {
          case TAG_TYPE_REP_OpTypeRep:
          case TAG_TYPE_REP_TotalFnTypeRep:
          case TAG_TYPE_REP_PartialFnTypeRep: {
            exists = true;
            break;
          }
          default: {
            exists = IsUnionFunctionType(t);
            break;
          }
        }
      }
      return exists;
    }
#ifdef VDMPP
    case TAG_TYPE_REP_OverTypeRep: { return true; }
#endif // VDMPP
    default: { return false; }
  }
}

// IsSubType
// ti : REP`TypeRep
// -> bool
bool vdmcg::IsSubType (const TYPE_REP_TypeRep & tp1, const TYPE_REP_TypeRep & tp2) const
{
  switch (tp1.GetTag()) {
    case TAG_TYPE_REP_EmptySetTypeRep: { return IsPosSetType(tp2); }
    case TAG_TYPE_REP_EmptySeqTypeRep: { return IsPosSeqType(tp2); }
    case TAG_TYPE_REP_EmptyMapTypeRep: { return IsPosMapType(tp2); }
    default: { return GetStatSem().IsSubType(tp1, tp2, Set()); }
  }
}

// MakePreFct
// fd : (AS`FnDef | AS`OpDef)
// sd : FD`GenericStateDef
// -> AS`FnDef
TYPE_AS_ExplFnDef vdmcg::MakePreFct(const Record & fd, const Generic & sd)
{
  TYPE_AS_BooleanType btp;
  btp.Init(NilContextId);

  switch (fd.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      const SEQ<TYPE_AS_TypeVar> & tv_l (fd.GetSequence(pos_AS_ExplFnDef_tpparms));
      const TYPE_AS_FnType & tp (fd.GetRecord(pos_AS_ExplFnDef_tp));
      const SEQ<TYPE_AS_Parameters> & parm_ll (fd.GetSequence(pos_AS_ExplFnDef_parms));
      const Generic & fnpre (fd.GetField(pos_AS_ExplFnDef_fnpre));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ExplFnDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ExplFnDef_stat));
  
      TYPE_AS_FnBody fnbd;
      fnbd.Init(fnpre, NilContextId);

      TYPE_AS_TotalFnType ttp;
      ttp.Init(tp.GetSequence(1), btp, NilContextId);
  
      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetFnName(fd), ASTAUX::MkId(L"pre")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, tv_l, ttp, parm_ll, fnbd, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      const SEQ<TYPE_AS_TypeVar> & tv_l (fd.GetSequence(pos_AS_ExtExplFnDef_params));
      const SEQ<TYPE_AS_PatTypePair> & pt_l (fd.GetSequence(pos_AS_ExtExplFnDef_partps));
      const Generic & fnpre (fd.GetField(pos_AS_ExtExplFnDef_fnpre));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ExtExplFnDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ExtExplFnDef_stat));
  
      // ==> seq of AS`Type * seq of AS`Pattern
      Tuple spt (SplitParameterTypes(pt_l));
      const SEQ<TYPE_AS_Type> & fndom_l (spt.GetSequence(1));
      const SEQ<TYPE_AS_Pattern> & parm_l (spt.GetSequence(2));
  
      TYPE_AS_TotalFnType tp;
      tp.Init(fndom_l, btp, NilContextId);

      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(fnpre, NilContextId);
  
      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetFnName(fd), ASTAUX::MkId(L"pre")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, tv_l, tp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ImplFnDef: {
      const SEQ<TYPE_AS_TypeVar> & tv_l (fd.GetSequence(pos_AS_ImplFnDef_params));
      const SEQ<TYPE_AS_PatTypePair> & pt_l (fd.GetSequence(pos_AS_ImplFnDef_partps));
      const Generic & fnpre (fd.GetField(pos_AS_ImplFnDef_fnpre));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ImplFnDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ImplFnDef_stat));
  
      // ==> seq of AS`Type * seq of AS`Pattern
      Tuple spt (SplitParameterTypes(pt_l));
      const SEQ<TYPE_AS_Type> & fndom_l (spt.GetSequence(1));
      const SEQ<TYPE_AS_Pattern> & parm_l (spt.GetSequence(2));
  
      TYPE_AS_TotalFnType tp;
      tp.Init(fndom_l, btp, NilContextId);

      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(fnpre,NilContextId);
  
      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetFnName(fd), ASTAUX::MkId(L"pre")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, tv_l, tp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ExplOpDef: {
      const TYPE_AS_OpType & OT (fd.GetRecord(pos_AS_ExplOpDef_tp));
      SEQ<TYPE_AS_Pattern> parm_l (fd.GetSequence(pos_AS_ExplOpDef_parms));
      const Generic & oppre (fd.GetField(pos_AS_ExplOpDef_oppre));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ExplOpDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ExplOpDef_stat));
  
      SEQ<TYPE_AS_Type> odom (OT.GetSequence(pos_AS_OpType_opdom));
      parm_l.ImpConc(StateDefToPattern(sd, NEW));
      odom.ImpConc(StateDefToType(sd));

      TYPE_AS_TotalFnType tp;
      tp.Init(odom, btp, NilContextId);

      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(oppre,NilContextId);
  
      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetOpName(fd), ASTAUX::MkId(L"pre")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, Sequence(), tp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      const SEQ<TYPE_AS_PatTypePair> & pt_l (fd.GetSequence(pos_AS_ExtExplOpDef_partps));
      const Generic & oppre (fd.GetField(pos_AS_ExtExplOpDef_oppre));  
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ExtExplOpDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ExtExplOpDef_stat));
  
      // ==> seq of AS`Type * seq of AS`Pattern
      Tuple spt (SplitParameterTypes(pt_l));
      SEQ<TYPE_AS_Type> opdom_l (spt.GetSequence(1));
      SEQ<TYPE_AS_Pattern> parm_l (spt.GetSequence(2));
  
      parm_l.ImpConc(StateDefToPattern(sd, NEW));
      opdom_l.ImpConc(StateDefToType(sd));

      TYPE_AS_TotalFnType tp;
      tp.Init(opdom_l, btp, NilContextId);

      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(oppre, NilContextId);
  
      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetOpName(fd), ASTAUX::MkId(L"pre")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, Sequence(), tp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ImplOpDef: {
      const SEQ<TYPE_AS_PatTypePair> & pt_l (fd.GetSequence(pos_AS_ImplOpDef_partps));
      const Generic & oppre (fd.GetField(pos_AS_ImplOpDef_oppre));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ImplOpDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ImplOpDef_stat));
  
      // ==> seq of AS`Type * seq of AS`Pattern
      Tuple spt (SplitParameterTypes(pt_l));
      SEQ<TYPE_AS_Type> opdom_l (spt.GetSequence(1));
      SEQ<TYPE_AS_Pattern> parm_l (spt.GetSequence(2));
  
      parm_l.ImpConc(StateDefToPattern(sd, NEW));
      opdom_l.ImpConc(StateDefToType(sd));

      TYPE_AS_TotalFnType tp;
      tp.Init(opdom_l, btp, NilContextId);

      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(oppre, NilContextId);
  
      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetOpName(fd), ASTAUX::MkId(L"pre")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, Sequence(), tp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
  }

  return TYPE_AS_ExplFnDef(); // To avoid warnings
}

// MakePostFct
// fd : (AS`FnDef | AS`OpDef)
// sd : FD`GenericStateDef
// -> AS`FnDef
TYPE_AS_ExplFnDef vdmcg::MakePostFct(const Record & fd, const Generic & sd)
{
  TYPE_AS_BooleanType btp;
  btp.Init(NilContextId);

  TYPE_AS_Name resnm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"RESULT"), NilContextId));

  switch (fd.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      const SEQ<TYPE_AS_TypeVar> & tv_l (fd.GetSequence(pos_AS_ExplFnDef_tpparms));
      const TYPE_AS_FnType & tp (fd.GetRecord(pos_AS_ExplFnDef_tp));
      SEQ<TYPE_AS_Parameters> parm_ll (fd.GetSequence(pos_AS_ExplFnDef_parms));
      const Generic & fnpost (fd.GetField(pos_AS_ExplFnDef_fnpost));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ExplFnDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ExplFnDef_stat));

      SEQ<TYPE_AS_Type> domtp_l(tp.GetSequence(1));
      const TYPE_AS_Type & rtp (tp.GetRecord(2));

      domtp_l.ImpAppend(rtp);

      TYPE_AS_TotalFnType ttp;
      ttp.Init(domtp_l, btp, NilContextId);

      TYPE_CI_ContextId c_rtp (GetCI().PushCGType(FromAS2RepType(rtp)));

      TYPE_AS_PatternName parm;
      parm.Init(resnm, Nil(), c_rtp);

      SEQ<TYPE_AS_Pattern> l (parm_ll.Hd());
      l.ImpAppend(parm);
      parm_ll.ImpTl();
      parm_ll.ImpPrepend(l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(fnpost, NilContextId);

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetFnName(fd), ASTAUX::MkId(L"post")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, tv_l, ttp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      const SEQ<TYPE_AS_TypeVar> & tv_l (fd.GetSequence(pos_AS_ExtExplFnDef_params));
      const SEQ<TYPE_AS_PatTypePair> & pt_l (fd.GetSequence(pos_AS_ExtExplFnDef_partps));
      const SEQ<TYPE_AS_NameType> & nt_l (fd.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
      const Generic & fnpost (fd.GetField(pos_AS_ExtExplFnDef_fnpost));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ExtExplFnDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ExtExplFnDef_stat));

      // ==> seq of AS`Type * seq of AS`Pattern
      Tuple spt (SplitParameterTypes(pt_l));
      SEQ<TYPE_AS_Type> fndom_l (spt.GetSequence(1));
      SEQ<TYPE_AS_Pattern> parm_l (spt.GetSequence(2));

      Tuple mrp (MakeReturnPattern(nt_l)); // AS`Pattern * AS`Type
      const TYPE_AS_Pattern & parm (mrp.GetRecord(1));
      const TYPE_AS_Type & rtp (mrp.GetRecord(2));

      fndom_l.ImpAppend(rtp);

      TYPE_AS_TotalFnType tp;
      tp.Init(fndom_l, btp, NilContextId);

      parm_l.ImpAppend(parm);
      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy (TYPE_AS_FnBody().Init(fnpost,NilContextId));

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetFnName(fd), ASTAUX::MkId(L"post")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, tv_l, tp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ImplFnDef: {
      const SEQ<TYPE_AS_TypeVar> & tv_l (fd.GetSequence(pos_AS_ImplFnDef_params));
      const SEQ<TYPE_AS_PatTypePair> & pt_l (fd.GetSequence(pos_AS_ImplFnDef_partps));
      const SEQ<TYPE_AS_NameType> & nt_l (fd.GetSequence(pos_AS_ImplFnDef_resnmtps));
      const TYPE_AS_Expr & fnpost (fd.GetRecord(pos_AS_ImplFnDef_fnpost));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ImplFnDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ImplFnDef_stat));

      // ==> seq of AS`Type * seq of AS`Pattern
      Tuple spt (SplitParameterTypes(pt_l));
      SEQ<TYPE_AS_Type> fndom_l(spt.GetSequence(1));
      SEQ<TYPE_AS_Pattern> parm_l(spt.GetSequence(2));

      Tuple mrp (MakeReturnPattern(nt_l)); // ==> AS`Pattern * AS`Type
      const TYPE_AS_Pattern & parm (mrp.GetRecord(1));
      const TYPE_AS_Type & rtp (mrp.GetRecord(2));

      fndom_l.ImpAppend(rtp);

      TYPE_AS_TotalFnType tp;
      tp.Init(fndom_l, btp, NilContextId);

      parm_l.ImpAppend(parm);
      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(fnpost, NilContextId);

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetFnName(fd), ASTAUX::MkId(L"post")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, tv_l, tp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
#ifdef VDMSL
    case TAG_TYPE_AS_ExplOpDef: {
      const TYPE_AS_OpType & OT (fd.GetRecord(pos_AS_ExplOpDef_tp));
      SEQ<TYPE_AS_Pattern> parm_l(fd.GetSequence(pos_AS_ExplOpDef_parms));
      const Generic & oppost(fd.GetField(pos_AS_ExplOpDef_oppost));
      const TYPE_AS_Access & access (fd.GetField(pos_AS_ExplOpDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ExplOpDef_stat));

      SEQ<TYPE_AS_Type> odom (OT.GetSequence(pos_AS_OpType_opdom));
      const TYPE_AS_Type & rtp (OT.GetRecord(pos_AS_OpType_oprng));
      if (!rtp.Is(TAG_TYPE_AS_VoidType)) {
        TYPE_REP_TypeRep rep_rtp (FromAS2RepType(rtp));

        parm_l.ImpAppend(TYPE_AS_PatternName().Init(resnm, Nil(), GetCI().PushCGType(rep_rtp)));
        odom.ImpAppend(rtp);
      }
      SEQ<TYPE_AS_Type> tp_l (StateDefToType(sd));
      parm_l.ImpConc(StateDefToPattern(sd, OLD));
      parm_l.ImpConc(StateDefToPattern(sd, NEW));
      odom.ImpConc(tp_l).ImpConc(tp_l);

      TYPE_AS_TotalFnType tp;
      tp.Init(odom, btp, NilContextId);

      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(oppost, NilContextId);

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetOpName(fd), ASTAUX::MkId(L"post")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, Sequence(), tp, parm_ll, fnbdy, Nil(), Nil(), access, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      const SEQ<TYPE_AS_PatTypePair> & pt_l (fd.GetSequence(pos_AS_ExtExplOpDef_partps));
      const SEQ<TYPE_AS_NameType> & nt_l (fd.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
      const Generic & oppost (fd.GetField(pos_AS_ExtExplOpDef_oppost));
      const TYPE_AS_Access & acc (fd.GetField(pos_AS_ExtExplOpDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ExtExplOpDef_stat));

      // ==> seq of AS`Type * seq of AS`Pattern
      Tuple spt (SplitParameterTypes(pt_l));
      SEQ<TYPE_AS_Type> odom_l(spt.GetSequence(1));
      SEQ<TYPE_AS_Pattern> parm_l(spt.GetSequence(2));

      if (!nt_l.IsEmpty()) {
        Tuple mrp (MakeReturnPattern(nt_l)); // AS`Pattern * AS`Type
        parm_l.ImpAppend(mrp.GetRecord(1));
        odom_l.ImpAppend(mrp.GetRecord(2));
      }
      SEQ<TYPE_AS_Type> tp_l (StateDefToType(sd));
      parm_l.ImpConc(StateDefToPattern(sd, OLD));
      parm_l.ImpConc(StateDefToPattern(sd, NEW));
      odom_l.ImpConc(tp_l).ImpConc(tp_l);

      TYPE_AS_TotalFnType tp;
      tp.Init(odom_l, btp, NilContextId);

      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(oppost,NilContextId);

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetOpName(fd), ASTAUX::MkId(L"post")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, Sequence(), tp, parm_ll, fnbdy, Nil(), Nil(), acc, stat, Nil(), NilContextId);
      return efd;
    }
    case TAG_TYPE_AS_ImplOpDef: {
      const SEQ<TYPE_AS_PatTypePair> & pt_l (fd.GetSequence(pos_AS_ImplOpDef_partps));
      const SEQ<TYPE_AS_NameType> & nt_l (fd.GetSequence(pos_AS_ImplOpDef_resnmtps));
      const TYPE_AS_Expr & oppost (fd.GetRecord(pos_AS_ImplOpDef_oppost));
      const TYPE_AS_Access & acc (fd.GetField(pos_AS_ImplOpDef_access));
      const Bool & stat (fd.GetBool(pos_AS_ImplOpDef_stat));

      // ==> seq of AS`Type * seq of AS`Pattern
      Tuple spt (SplitParameterTypes(pt_l));
      SEQ<TYPE_AS_Type> odom_l(spt.GetSequence(1));
      SEQ<TYPE_AS_Pattern> parm_l(spt.GetSequence(2));

      if (!nt_l.IsEmpty()) {
        Tuple mrp (MakeReturnPattern(nt_l)); // AS`Pattern * AS`Type
        parm_l.ImpAppend(mrp.GetRecord(1));
        odom_l.ImpAppend(mrp.GetRecord(2));
      }
      SEQ<TYPE_AS_Type> tp_l (StateDefToType(sd));
      parm_l.ImpConc(StateDefToPattern(sd, OLD));
      parm_l.ImpConc(StateDefToPattern(sd, NEW));
      odom_l.ImpConc(tp_l).ImpConc(tp_l);

      TYPE_AS_TotalFnType tp;
      tp.Init(odom_l, btp, NilContextId);

      SEQ<TYPE_AS_Parameters> parm_ll;
      parm_ll.ImpAppend(parm_l);

      TYPE_AS_FnBody fnbdy;
      fnbdy.Init(oppost, NilContextId);

      TYPE_AS_Name fnm (vdm_BC_GivePrePostName(ASTAUX::GetOpName(fd), ASTAUX::MkId(L"post")));

      TYPE_AS_ExplFnDef efd;
      efd.Init(fnm, Sequence(), tp, parm_ll, fnbdy, Nil(), Nil(), acc, stat, Nil(), NilContextId);
      return efd;
    }
#endif //VDMSL
  }
  return TYPE_AS_ExplFnDef(); // To avoid warnings
}

// MakeReturnPattern
// nt_l : seq1 of AS`NameType
// ==> AS`Pattern * AS`Type
Tuple vdmcg::MakeReturnPattern(const SEQ<TYPE_AS_NameType> & nt_l)
{
  if (nt_l.Length() == 1) {
    const TYPE_AS_NameType & nt (nt_l[1]);
    const TYPE_AS_Name & rnm (nt.GetRecord(pos_AS_NameType_nm));
    const TYPE_AS_Type & rtp (nt.GetRecord(pos_AS_NameType_tp));
    TYPE_REP_TypeRep rep_rtp (FromAS2RepType(rtp));

    TYPE_AS_PatternName pnm;
    pnm.Init(rnm, Nil(), GetCI().PushCGType(rep_rtp)); // TODO
    //pnm.Init(rnm, rtp, GetCI().PushCGType(rep_rtp)); // TODO

    return mk_( pnm, rtp );
  }
  else {
    SEQ<TYPE_AS_Pattern> pat_l;
    SEQ<TYPE_AS_Type> type_l;
    size_t len_nt_l = nt_l.Length();
    for (size_t idx = 1; idx <= len_nt_l; idx++) {
      const TYPE_AS_NameType & nt (nt_l[idx]);
      const TYPE_AS_Name & rnm (nt.GetRecord(pos_AS_NameType_nm));
      const TYPE_AS_Type & rtp (nt.GetRecord(pos_AS_NameType_tp));
      TYPE_REP_TypeRep rep_rtp (FromAS2RepType(rtp));

      TYPE_AS_PatternName pnm;
      pnm.Init(rnm, Nil(), GetCI().PushCGType(rep_rtp)); // TODO
      //pnm.Init(rnm, rtp, GetCI().PushCGType(rep_rtp)); // TODO

      pat_l.ImpAppend(pnm);
      type_l.ImpAppend(rtp);
    }
    TYPE_AS_ProductType tp;
    tp.Init(type_l, NilContextId);

    TYPE_REP_TypeRep rep_tp (FromAS2RepType(tp));

    TYPE_AS_TuplePattern pat;
    pat.Init(pat_l, GetCI().PushCGType(rep_tp));

    return mk_( pat, tp );
  }
}

// StateDefToType
// sd : FD`GenericStateDef
// -> seq of AS`Type
SEQ<TYPE_AS_Type> vdmcg::StateDefToType(const Generic & sd)
{
#ifdef VDMSL
  if (sd.IsNil()) {
    return SEQ<TYPE_AS_Type>();
  }
  else {
    TYPE_AS_StateDef tsd (sd);
    SEQ<TYPE_AS_Type> tp_l;
    tp_l.ImpAppend(tsd.GetRecord(pos_AS_StateDef_tp));
    return tp_l;
  }
#endif // VDMSL
#ifdef VDMPP
  return SEQ<TYPE_AS_Type>();
#endif // VDMPP
}

// StateDefToPattern
// sd : FD`GenericStateDef
// sdtp : [<OLD>]
// -> seq of AS`Pattern
SEQ<TYPE_AS_Pattern> vdmcg::StateDefToPattern(const Generic & sd, int sdtp)
{
#ifdef VDMSL
  if (sd.IsNil()) {
    return SEQ<TYPE_AS_Pattern>();
  }
  else {
    TYPE_AS_StateDef tsd (sd);
    const TYPE_AS_CompositeType & tp (tsd.GetRecord(pos_AS_StateDef_tp));
    const TYPE_AS_Name & nm (tp.GetRecord(pos_AS_CompositeType_name));
    const SEQ<TYPE_AS_Field> & f_l (tp.GetSequence(pos_AS_CompositeType_fields));

    SEQ<TYPE_AS_Pattern> p_l;
    size_t len_f_l = f_l.Length();
    for (size_t idx = 1; idx <= len_f_l; idx++) {
      p_l.ImpAppend(Field2Pattern(f_l[idx], sdtp));
    }
    TYPE_REP_TypeRep ti (FromAS2RepType(tp));
    TYPE_CI_ContextId cid_ti (GetCI().PushCGType(ti));

    TYPE_AS_RecordPattern rp;
    rp.Init(nm, p_l, cid_ti);

    SEQ<TYPE_AS_Pattern> l;
    l.ImpAppend(rp);
    return l;
  }
#endif // VDMSL
#ifdef VDMPP
  return SEQ<TYPE_AS_Pattern>();
#endif // VDMPP
}

// Field2Pattern
// f : AS`Field
// sdtp : [<OLD>]
// -> AS`PatternName
TYPE_AS_PatternName vdmcg::Field2Pattern(const TYPE_AS_Field & f, int sdtp)
{
  const Generic & nm (f.GetField(pos_AS_Field_sel));
  TYPE_REP_TypeRep ttp (FromAS2RepType(f.GetRecord(pos_AS_Field_type)));
  TYPE_CI_ContextId cid_tpp (GetCI().PushCGType(ttp));
  if (nm.IsNil() || sdtp != OLD) {
    return TYPE_AS_PatternName().Init(nm, Nil(), cid_tpp);
  }
  else {
    TYPE_AS_Name nnm (nm);
    TYPE_AS_OldName old;
    old.Init(nnm.get_ids(), NilContextId);
    return TYPE_AS_PatternName().Init(old, Nil(), cid_tpp);
  }
}

// SplitParameterTypes
// pt_l : AS`ParameterTypes
// ==> seq of AS`Type * seq of AS`Pattern
Tuple vdmcg::SplitParameterTypes(const SEQ<TYPE_AS_PatTypePair> & pt_l)
{
  SEQ<TYPE_AS_Type> dom_l;
  SEQ<TYPE_AS_Pattern> parm_l;
  size_t len_pt_l = pt_l.Length();
  for (size_t idx = 1; idx <= len_pt_l; idx++) {
    const TYPE_AS_PatTypePair & ptp (pt_l[idx]);
    const SEQ<TYPE_AS_Pattern> & pat_l (ptp.GetSequence(pos_AS_PatTypePair_pats));
    const TYPE_AS_Type & tp (ptp.GetRecord(pos_AS_PatTypePair_tp));

    size_t len_pat_l = pat_l.Length();
    for (size_t i = 1; i <= len_pat_l; i++) {
      dom_l.ImpAppend(tp);
    }
    parm_l.ImpConc(pat_l);
  }
  return mk_(dom_l, parm_l);
}

// QuotePMExprWithClNm
// pat : (AS`Pattern | [AS`Expr])
// clnm : AS`Name
// lb : set of AS`Name
// ==> [AS`Pattern | AS`Expr]
Generic vdmcg::QuotePMExprWithClNm(const Generic & patg, const TYPE_AS_Name & clnm, const SET<TYPE_AS_Name> & lb)
{
  if (patg.IsNil()) {
    return patg;
  }

  Record pat (patg);
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      const Generic & nm (pat.GetField(pos_AS_PatternName_nm));
      if (nm.IsNil()) {
        return pat;
      }
      else {
        TYPE_AS_PatternName pn (pat);
        pn.SetField(pos_AS_PatternName_nm, ASTAUX::Combine2Names(clnm, nm));
        return pn;
      }
    }
    case TAG_TYPE_AS_MatchVal: {
      return pat;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & Elems (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      SEQ<TYPE_AS_Pattern> newElems;
      size_t len_Elems = Elems.Length();
      for (size_t i = 1; i <= len_Elems; i++) {
        newElems.ImpAppend(QuotePMExprWithClNm(Elems[i], clnm, lb));
      }
      TYPE_AS_SetEnumPattern sep (pat);
      sep.SetField(pos_AS_SetEnumPattern_Elems, newElems);
      return sep;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SetUnionPattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SetUnionPattern_rp));

      TYPE_AS_SetUnionPattern sup (pat);
      sup.SetField(pos_AS_SetUnionPattern_lp, QuotePMExprWithClNm(lp, clnm, lb));
      sup.SetField(pos_AS_SetUnionPattern_rp, QuotePMExprWithClNm(rp, clnm, lb));
      return sup;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      SEQ<TYPE_AS_Pattern> new_els;
      size_t len_els = els.Length();
      for (size_t i = 1; i <= len_els; i++) {
        new_els.ImpAppend(QuotePMExprWithClNm(els[i], clnm, lb));
      }
      TYPE_AS_SeqEnumPattern sep (pat);
      sep.SetField(pos_AS_SeqEnumPattern_els, new_els);
      return sep;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SeqConcPattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SeqConcPattern_rp));

      TYPE_AS_SeqConcPattern scp (pat);
      scp.SetField(pos_AS_SeqConcPattern_lp, QuotePMExprWithClNm(lp, clnm, lb));
      scp.SetField(pos_AS_SeqConcPattern_rp, QuotePMExprWithClNm(rp, clnm, lb));
      return scp;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      SEQ<TYPE_AS_MapletPattern> new_mls;
      size_t len_mls = mls.Length();
      for (size_t i = 1; i <= len_mls; i++) {
        const TYPE_AS_MapletPattern & mp (mls[i]);
        const TYPE_AS_Pattern & dp (mp.GetRecord(pos_AS_MapletPattern_dp));
        const TYPE_AS_Pattern & rp (mp.GetRecord(pos_AS_MapletPattern_rp));
        TYPE_AS_MapletPattern new_mp(mp);
        new_mp.SetField(pos_AS_MapletPattern_dp, QuotePMExprWithClNm(dp, clnm, lb));
        new_mp.SetField(pos_AS_MapletPattern_rp, QuotePMExprWithClNm(rp, clnm, lb));
        new_mls.ImpAppend(new_mp);
      }
      TYPE_AS_MapEnumPattern mep (pat);
      mep.SetField(pos_AS_MapEnumPattern_mls, new_mls);
      return mep;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_MapMergePattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_MapMergePattern_rp));

      TYPE_AS_MapMergePattern mmp (pat);
      mmp.SetField(pos_AS_MapMergePattern_lp, QuotePMExprWithClNm(lp, clnm, lb));
      mmp.SetField(pos_AS_MapMergePattern_rp, QuotePMExprWithClNm(rp, clnm, lb));
      return mmp;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_TuplePattern_fields));
      SEQ<TYPE_AS_Pattern> newfields;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++) {
        newfields.ImpAppend(QuotePMExprWithClNm(fields[i], clnm, lb));
      }
      TYPE_AS_TuplePattern tp (pat);
      tp.SetField(pos_AS_TuplePattern_fields, newfields);
      return tp;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const TYPE_AS_Name & nm (pat.GetRecord(pos_AS_RecordPattern_nm));
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_RecordPattern_fields));
      SEQ<TYPE_AS_Pattern> newfields;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i<= len_fields; i++) {
        newfields.ImpAppend(QuotePMExprWithClNm(fields[i], clnm, lb));
      }
      TYPE_AS_RecordPattern rp (pat);
      rp.SetField(pos_AS_RecordPattern_nm, QuotePMExprWithClNm(nm, clnm, lb));
      rp.SetField(pos_AS_RecordPattern_fields, newfields);
      return rp;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & fields (pat.GetSequence(pos_AS_ObjectPattern_fields));
      SEQ<TYPE_AS_FieldPattern> newfields;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i<= len_fields; i++) {
        const TYPE_AS_FieldPattern & fp (fields[i]);
        const TYPE_AS_Pattern & p (fp.GetRecord(pos_AS_FieldPattern_pat));
        TYPE_AS_FieldPattern new_fp (fp);
        new_fp.SetField(pos_AS_FieldPattern_pat, QuotePMExprWithClNm(p, clnm, lb));
        newfields.ImpAppend(new_fp);
      }
      TYPE_AS_ObjectPattern rp (pat);
      rp.SetField(pos_AS_ObjectPattern_fields, newfields);
      return rp;
    }
#endif // VDMPP
    case TAG_TYPE_AS_BracketedExpr: {
      TYPE_AS_BracketedExpr be (pat);
      be.set_expr(QuotePMExprWithClNm(pat.GetRecord(pos_AS_BracketedExpr_expr), clnm, lb));
      return be;
    }
    case TAG_TYPE_AS_DefExpr: {
      const type_dd2PL & def_l (pat.GetSequence(pos_AS_DefExpr_Def));
      const TYPE_AS_Expr & expr (pat.GetRecord(pos_AS_DefExpr_In));
  
      SET<TYPE_AS_Name> localbind (lb);
      type_dd2PL local_def;
      size_t len_def_l = def_l.Length();
      for (size_t i = 1; i <= len_def_l; i++) {
        type_dd2P as_def (def_l[i]);
        const TYPE_AS_PatternBind & p (as_def.GetRecord(1));
        const TYPE_AS_Expr & e (as_def.GetRecord(2));
        SET<TYPE_AS_Name> pnms (FindPatternId(p).Dom());
        as_def.SetField(2, QuotePMExprWithClNm(e, clnm, localbind));
        local_def.ImpAppend(as_def);
        localbind.ImpUnion(pnms);
      }

      TYPE_AS_DefExpr de (pat);
      de.SetField(pos_AS_DefExpr_Def, local_def);
      de.SetField(pos_AS_DefExpr_In, QuotePMExprWithClNm(expr, clnm, localbind));
      return de;
    }
    case TAG_TYPE_AS_LetExpr: {
      const SEQ<TYPE_AS_LocalDef> & locdef (pat.GetSequence(pos_AS_LetExpr_localdef));
      const TYPE_AS_Expr & body (pat.GetRecord(pos_AS_LetExpr_body));
  
      SET<TYPE_AS_Name> localbind (lb);
      SEQ<TYPE_AS_LocalDef> local_def;
      size_t len_locdef = locdef.Length();
      for (size_t i = 1; i <= len_locdef; i++) {
        const TYPE_AS_LocalDef & as_localdef (locdef[i]);
        switch(as_localdef.GetTag()) {
          case TAG_TYPE_AS_ExplFnDef:
          case TAG_TYPE_AS_ImplFnDef:
          case TAG_TYPE_AS_ExtExplFnDef: {
            local_def.ImpAppend(as_localdef);
            break;
          }
          case TAG_TYPE_AS_ValueDef: {
            const TYPE_AS_Pattern & pat (as_localdef.GetRecord(pos_AS_ValueDef_pat));
            const TYPE_AS_Expr & expr (as_localdef.GetRecord(pos_AS_ValueDef_val));
            SET<TYPE_AS_Name> pnms (FindPatternId(pat).Dom());
 
            TYPE_AS_ValueDef vd (as_localdef);
            vd.SetField(pos_AS_ValueDef_val, QuotePMExprWithClNm(expr, clnm, localbind));
            local_def.ImpAppend(vd);
            localbind.ImpUnion(pnms);
            break;
          }
        }
      }
      TYPE_AS_LetExpr le (pat);
      le.SetField(pos_AS_LetExpr_localdef, local_def);
      le.SetField(pos_AS_LetExpr_body, QuotePMExprWithClNm(body, clnm, localbind));
      return le;
    }
    case TAG_TYPE_AS_LetBeSTExpr: {
      const SEQ<TYPE_AS_MultBind> & bind (pat.GetSequence(pos_AS_LetBeSTExpr_lhs));
      const Generic & St (pat.GetField(pos_AS_LetBeSTExpr_St));
      const TYPE_AS_Expr & In (pat.GetRecord(pos_AS_LetBeSTExpr_In));

      SET<TYPE_AS_Name> localbind (lb);
      SEQ<TYPE_AS_MultBind> newbind;
      size_t len_bind = bind.Length(); 
      for (size_t idx = 1; idx <= len_bind; idx++) {
        const TYPE_AS_MultBind & mb (bind[idx]);
        switch(mb.GetTag()) {
          case TAG_TYPE_AS_MultSetBind: {
            const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSetBind_pat));
            const TYPE_AS_Expr & Set1 (mb.GetRecord(pos_AS_MultSetBind_Set));
  
            SET<TYPE_AS_Name> pnms;
            size_t len_pat_l = pat_l.Length();
            for (size_t i = 1; i <= len_pat_l; i++) {
              SET<TYPE_AS_Name> p_nms(FindPatternId(pat_l[i]).Dom());
              pnms.ImpUnion(p_nms);
            }
  
            localbind.ImpUnion(pnms);

            TYPE_AS_MultSetBind msb (mb);
            msb.SetField(pos_AS_MultSetBind_Set, QuotePMExprWithClNm(Set1, clnm, lb));
            newbind.ImpAppend(msb);
            break;
          }
          case TAG_TYPE_AS_MultTypeBind: {
            newbind.ImpAppend(mb);
            break;
          }
          default:
            break;
        }
      }
  
      TYPE_AS_LetBeSTExpr lbe (pat);
      lbe.SetField(pos_AS_LetBeSTExpr_lhs, newbind);
      lbe.SetField(pos_AS_LetBeSTExpr_St, QuotePMExprWithClNm(St, clnm, localbind));
      lbe.SetField(pos_AS_LetBeSTExpr_In, QuotePMExprWithClNm(In, clnm, localbind));
      return lbe;
    }
    case TAG_TYPE_AS_IfExpr: {
      const TYPE_AS_Expr & test (pat.GetRecord(pos_AS_IfExpr_test));
      const TYPE_AS_Expr & cons (pat.GetRecord(pos_AS_IfExpr_cons));
      const SEQ<TYPE_AS_ElseifExpr> & elsif (pat.GetSequence(pos_AS_IfExpr_elsif));
      const TYPE_AS_Expr & altn (pat.GetRecord(pos_AS_IfExpr_altn));
  
      SEQ<TYPE_AS_ElseifExpr> newelsif;
      size_t len_elsif = elsif.Length();
      for (size_t i = 1; i <= len_elsif; i++) {
        const TYPE_AS_ElseifExpr & eie (elsif[i]);
        const TYPE_AS_Expr & else_test (eie.GetRecord(pos_AS_ElseifExpr_test));
        const TYPE_AS_Expr & else_cons (eie.GetRecord(pos_AS_ElseifExpr_cons));

        TYPE_AS_ElseifExpr new_eie (eie);
        new_eie.SetField(pos_AS_ElseifExpr_test, QuotePMExprWithClNm(else_test, clnm, lb));
        new_eie.SetField(pos_AS_ElseifExpr_cons, QuotePMExprWithClNm(else_cons, clnm, lb));
        newelsif.ImpAppend(new_eie);
      }
      TYPE_AS_IfExpr ie (pat);
      ie.SetField(pos_AS_IfExpr_test, QuotePMExprWithClNm(test, clnm, lb));
      ie.SetField(pos_AS_IfExpr_cons, QuotePMExprWithClNm(cons, clnm, lb));
      ie.SetField(pos_AS_IfExpr_elsif, newelsif);
      ie.SetField(pos_AS_IfExpr_altn, QuotePMExprWithClNm(altn, clnm, lb));
      return ie;
    }
    case TAG_TYPE_AS_CasesExpr: {
      const TYPE_AS_Expr & sel (pat.GetRecord(pos_AS_CasesExpr_sel));
      const SEQ<TYPE_AS_CaseAltn> & altns (pat.GetSequence(pos_AS_CasesExpr_altns));
      const Generic & Others (pat.GetField(pos_AS_CasesExpr_Others));
  
      size_t len_altns = altns.Length();
      SEQ<TYPE_AS_CaseAltn> newaltns;
      for (size_t idx = 1; idx <= len_altns; idx++) {
        const TYPE_AS_CaseAltn & ca (altns[idx]);
        const SEQ<TYPE_AS_Pattern> & match (ca.GetSequence(pos_AS_CaseAltn_match));
        const TYPE_AS_Expr & body (ca.GetRecord(pos_AS_CaseAltn_body));
        SET<TYPE_AS_Name> pnms;
        size_t len_match = match.Length();
        for (size_t i = 1; i <= len_match; i++) {
          SET<TYPE_AS_Name> p_nms (FindPatternId(match[i]).Dom());
          pnms.ImpUnion(p_nms);
        }
        TYPE_AS_CaseAltn new_ca (ca);
        new_ca.SetField(pos_AS_CaseAltn_body, QuotePMExprWithClNm(body, clnm, pnms));
        newaltns.ImpAppend(new_ca);
      }
  
      TYPE_AS_CasesExpr ce (pat);
      ce.SetField(pos_AS_CasesExpr_sel, QuotePMExprWithClNm(sel, clnm, lb));
      ce.SetField(pos_AS_CasesExpr_altns, newaltns); // 20070208
      ce.SetField(pos_AS_CasesExpr_Others, QuotePMExprWithClNm(Others, clnm, lb));
      return ce;
    }
    case TAG_TYPE_AS_PrefixExpr: {
      const TYPE_AS_Expr & arg (pat.GetRecord(pos_AS_PrefixExpr_arg));

      TYPE_AS_PrefixExpr pe (pat);
      pe.SetField(pos_AS_PrefixExpr_arg, QuotePMExprWithClNm(arg, clnm, lb));
      return pe;
    }
    case TAG_TYPE_AS_BinaryExpr: {
      TYPE_AS_BinaryExpr be (pat);
      be.SetField(pos_AS_BinaryExpr_left,
                  QuotePMExprWithClNm(pat.GetRecord(pos_AS_BinaryExpr_left), clnm, lb));
      be.SetField(pos_AS_BinaryExpr_right,
                  QuotePMExprWithClNm(pat.GetRecord(pos_AS_BinaryExpr_right), clnm, lb));
      return be;
    }
    case TAG_TYPE_AS_AllOrExistsExpr: {
      const SEQ<TYPE_AS_MultBind> & bind (pat.GetSequence(pos_AS_AllOrExistsExpr_bind));
      const TYPE_AS_Expr & pred (pat.GetRecord(pos_AS_AllOrExistsExpr_pred));
  
      SET<TYPE_AS_Name> local_bind (lb);
      SEQ<TYPE_AS_MultBind> new_bind_l;
      size_t len_bind = bind.Length(); 
      for (size_t idx = 1; idx <= len_bind; idx++) {
        const TYPE_AS_MultBind & mb (bind[idx]);
        switch(mb.GetTag()) {
          case TAG_TYPE_AS_MultSetBind: {
            const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSetBind_pat));
            const TYPE_AS_Expr & Set1 (mb.GetRecord(pos_AS_MultSetBind_Set));
  
            SET<TYPE_AS_Name> pnms;
            size_t len_pat_l = pat_l.Length();
            for (size_t i = 1; i <= len_pat_l; i++) {
              SET<TYPE_AS_Name> p_nms(FindPatternId(pat_l[i]).Dom());
              pnms.ImpUnion(p_nms);
            }
  
            local_bind.ImpUnion(pnms);

            TYPE_AS_MultSetBind msb (mb);
            msb.SetField(pos_AS_MultSetBind_Set, QuotePMExprWithClNm(Set1, clnm, lb));
            new_bind_l.ImpAppend(msb);
            break;
          }
          case TAG_TYPE_AS_MultTypeBind: {
            new_bind_l.ImpAppend(mb);
            break;
          }
          default:
            break;
        }
      }
  
      TYPE_AS_AllOrExistsExpr aoee (pat);
      aoee.SetField(pos_AS_AllOrExistsExpr_bind, new_bind_l);
      aoee.SetField(pos_AS_AllOrExistsExpr_pred, QuotePMExprWithClNm(pred, clnm, local_bind));
      return aoee;
    }
    case TAG_TYPE_AS_ExistsUniqueExpr: {
      const TYPE_AS_Bind & bind (pat.GetRecord(pos_AS_ExistsUniqueExpr_bind));
      const TYPE_AS_Expr & pred (pat.GetRecord(pos_AS_ExistsUniqueExpr_pred));
  
      SET<TYPE_AS_Name> local_bind(lb);
      TYPE_AS_Bind new_bind;
  
      switch(bind.GetTag()) {
        case TAG_TYPE_AS_TypeBind: {
          new_bind = bind;
          break;
        }
        case TAG_TYPE_AS_SetBind: {
          const TYPE_AS_Pattern & sb_pat (bind.GetRecord(pos_AS_SetBind_pat));
          const TYPE_AS_Expr & expr (bind.GetRecord(pos_AS_SetBind_Set));
          local_bind.ImpUnion(FindPatternId(sb_pat).Dom());

          TYPE_AS_SetBind sb (bind);
          sb.SetField(pos_AS_SetBind_Set, QuotePMExprWithClNm(expr, clnm, lb));
  
          new_bind = sb;
          break;
        }
        default:
          break;
      }
      TYPE_AS_ExistsUniqueExpr eue (pat);
      eue.SetField(pos_AS_ExistsUniqueExpr_bind, new_bind);
      eue.SetField(pos_AS_ExistsUniqueExpr_pred, QuotePMExprWithClNm(pred, clnm, local_bind));
      return eue;
    }
    case TAG_TYPE_AS_IotaExpr: {
      const TYPE_AS_Bind & bind (pat.GetRecord(pos_AS_IotaExpr_bind));
      const TYPE_AS_Expr & pred (pat.GetRecord(pos_AS_IotaExpr_pred));
  
      SET<TYPE_AS_Name> local_bind (lb);
      TYPE_AS_Bind new_bind;
  
      switch(bind.GetTag()) {
        case TAG_TYPE_AS_TypeBind: {
          new_bind = bind;
          break;
        }
        case TAG_TYPE_AS_SetBind: {
          const TYPE_AS_Pattern & sb_pat (bind.GetRecord(pos_AS_SetBind_pat));
          const TYPE_AS_Expr & expr (bind.GetRecord(pos_AS_SetBind_Set));
          local_bind.ImpUnion(FindPatternId(sb_pat).Dom());

          TYPE_AS_SetBind sb (bind);
          sb.SetField(pos_AS_SetBind_Set, QuotePMExprWithClNm(expr, clnm, lb));
  
          new_bind = sb;
          break;
        }
        default:
          break;
      }
      TYPE_AS_IotaExpr ie (pat);
      ie.SetField(pos_AS_IotaExpr_bind, new_bind);
      ie.SetField(pos_AS_IotaExpr_pred, QuotePMExprWithClNm(pred, clnm, local_bind));
      return ie;
    }
    case TAG_TYPE_AS_SetEnumerationExpr: {
      const SEQ<TYPE_AS_Expr> & els (pat.GetSequence(pos_AS_SetEnumerationExpr_els));
      SEQ<TYPE_AS_Expr> newels;
      size_t len_els = els.Length();
      for (size_t i = 1; i <= len_els; i++) {
        newels.ImpAppend(QuotePMExprWithClNm(els[i], clnm, lb));
      }
      TYPE_AS_SetEnumerationExpr see (pat);
      see.SetField(pos_AS_SetEnumerationExpr_els, newels);
      return see;
    }
    case TAG_TYPE_AS_SetComprehensionExpr: {
      const TYPE_AS_Expr & elem (pat.GetRecord(pos_AS_SetComprehensionExpr_elem));
      const SEQ<TYPE_AS_MultBind> & bind (pat.GetSequence(pos_AS_SetComprehensionExpr_bind));
      const Generic & pred (pat.GetField(pos_AS_SetComprehensionExpr_pred));
  
      SET<TYPE_AS_Name> local_bind(lb);
      SEQ<TYPE_AS_MultBind> new_bind_l;
      size_t len_bind = bind.Length();
      for (size_t idx = 1; idx <= len_bind; idx++) {
        const TYPE_AS_MultBind & mb (bind[idx]);
        switch(mb.GetTag()) {
          case TAG_TYPE_AS_MultSetBind: {
            const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSetBind_pat));
            const TYPE_AS_Expr & Set1 (mb.GetRecord(pos_AS_MultSetBind_Set));
            Set pnms;
            size_t len_pat_l = pat_l.Length(); 
            for (size_t i = 1; i <= len_pat_l; i++) {
              pnms.ImpUnion(FindPatternId(pat_l[i]).Dom());
            }
            local_bind.ImpUnion(pnms);
            TYPE_AS_MultSetBind msb (mb);
            msb.SetField(pos_AS_MultSetBind_Set, QuotePMExprWithClNm(Set1, clnm, lb));
            new_bind_l.ImpAppend(msb);
            break;
          }
          case TAG_TYPE_AS_MultTypeBind: {
            new_bind_l.ImpAppend(mb);
            break;
          }
        }
      }

      TYPE_AS_SetComprehensionExpr sce (pat);
      sce.SetField(pos_AS_SetComprehensionExpr_elem, QuotePMExprWithClNm(elem, clnm, local_bind));
      sce.SetField(pos_AS_SetComprehensionExpr_bind, new_bind_l);
      sce.SetField(pos_AS_SetComprehensionExpr_pred, QuotePMExprWithClNm(pred, clnm, local_bind));
      return sce;
    }
    case TAG_TYPE_AS_SetRangeExpr: {
      TYPE_AS_SetRangeExpr sre (pat);
      sre.SetField(pos_AS_SetRangeExpr_lb,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_SetRangeExpr_lb), clnm, lb));
      sre.SetField(pos_AS_SetRangeExpr_ub,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_SetRangeExpr_ub), clnm, lb));
      return sre;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      const SEQ<TYPE_AS_Expr> & els (pat.GetSequence(pos_AS_SeqEnumerationExpr_els));
      SEQ<TYPE_AS_Expr> newels;
      size_t len_els = els.Length();
      for (size_t i = 1; i <= len_els; i++) {
        newels.ImpAppend(QuotePMExprWithClNm(els[i], clnm, lb));
      }
      TYPE_AS_SeqEnumerationExpr see (pat);
      see.SetField(pos_AS_SeqEnumerationExpr_els, newels);
      return see;
    }
    case TAG_TYPE_AS_SeqComprehensionExpr : {
      const TYPE_AS_Expr & elem (pat.GetRecord(pos_AS_SeqComprehensionExpr_elem));
      const TYPE_AS_SetBind & bind (pat.GetRecord(pos_AS_SeqComprehensionExpr_bind));
      const Generic & pred (pat.GetField(pos_AS_SeqComprehensionExpr_pred));

      const TYPE_AS_Pattern & sb_pat (bind.GetRecord(pos_AS_SetBind_pat));
      const TYPE_AS_Expr & Set1 (bind.GetRecord(pos_AS_SetBind_Set));
  
      SET<TYPE_AS_Name> new_lb (lb);
      new_lb.ImpUnion(FindPatternId(sb_pat).Dom());
  
      TYPE_AS_SetBind new_bind (bind);
      new_bind.SetField(pos_AS_SetBind_Set, QuotePMExprWithClNm(Set1, clnm, lb));

      TYPE_AS_SeqComprehensionExpr sce (pat);
      sce.SetField(pos_AS_SeqComprehensionExpr_elem, QuotePMExprWithClNm(elem, clnm, new_lb));
      sce.SetField(pos_AS_SeqComprehensionExpr_bind, new_bind);
      sce.SetField(pos_AS_SeqComprehensionExpr_pred, QuotePMExprWithClNm(pred, clnm, new_lb));
      return sce;
    }
    case TAG_TYPE_AS_SubSequenceExpr: {
      TYPE_AS_SubSequenceExpr sse (pat);
      sse.SetField(pos_AS_SubSequenceExpr_sequence,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_SubSequenceExpr_sequence), clnm, lb));
      sse.SetField(pos_AS_SubSequenceExpr_frompos,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_SubSequenceExpr_frompos), clnm, lb));
      sse.SetField(pos_AS_SubSequenceExpr_topos,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_SubSequenceExpr_topos), clnm, lb));
      return sse;
    }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: {
      TYPE_AS_SeqModifyMapOverrideExpr smmoe (pat);
      smmoe.SetField(pos_AS_SeqModifyMapOverrideExpr_seqmap,
                     QuotePMExprWithClNm(pat.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap), clnm, lb));
      smmoe.SetField(pos_AS_SeqModifyMapOverrideExpr_mapexp,
                     QuotePMExprWithClNm(pat.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp), clnm, lb));
      return smmoe;
    }
    case TAG_TYPE_AS_MapEnumerationExpr : {
      const SEQ<TYPE_AS_Maplet> & els (pat.GetSequence(pos_AS_MapEnumerationExpr_els));
      SEQ<TYPE_AS_Maplet> newels;
      size_t len_els = els.Length();
      for (size_t i = 1; i <= len_els; i++) {
        const TYPE_AS_Maplet & ml (els[i]);
        TYPE_AS_Maplet new_ml (ml);
        new_ml.SetField(pos_AS_Maplet_mapdom,
                        QuotePMExprWithClNm(ml.GetRecord(pos_AS_Maplet_mapdom), clnm, lb));
        new_ml.SetField(pos_AS_Maplet_maprng,
                        QuotePMExprWithClNm(ml.GetRecord(pos_AS_Maplet_maprng), clnm, lb));
        newels.ImpAppend(new_ml);
      }
      TYPE_AS_MapEnumerationExpr mee (pat);
      mee.SetField(pos_AS_MapEnumerationExpr_els, newels);
      return mee;
    }
    case TAG_TYPE_AS_MapComprehensionExpr: {
      const TYPE_AS_Maplet & elem (pat.GetRecord(pos_AS_MapComprehensionExpr_elem));
      const SEQ<TYPE_AS_MultBind> & bind (pat.GetSequence(pos_AS_MapComprehensionExpr_bind));
      const Generic & pred (pat.GetField(pos_AS_MapComprehensionExpr_pred));

      const TYPE_AS_Expr & md (elem.GetRecord(pos_AS_Maplet_mapdom));
      const TYPE_AS_Expr & mr (elem.GetRecord(pos_AS_Maplet_maprng));
  
      SET<TYPE_AS_Name> local_bind (lb);
      SEQ<TYPE_AS_MultBind> new_bind_l;
      size_t len_bind = bind.Length(); 
      for (size_t idx = 1; idx <= len_bind; idx++) {
        const TYPE_AS_MultBind & mb (bind[idx]);
        switch(mb.GetTag()) {
          case TAG_TYPE_AS_MultSetBind: {
            const SEQ<TYPE_AS_Pattern> & pat_l (mb.GetSequence(pos_AS_MultSetBind_pat));
            const TYPE_AS_Expr & Set1 (mb.GetRecord(pos_AS_MultSetBind_Set));
  
            SET<TYPE_AS_Name> pnms;
            size_t len_pat_l = pat_l.Length();
            for (size_t i = 1; i <= len_pat_l; i++) {
              pnms.ImpUnion(FindPatternId(pat_l[i]).Dom());
            }
            local_bind.ImpUnion(pnms);

            TYPE_AS_MultSetBind msb (mb);
            msb.SetField(pos_AS_MultSetBind_Set, QuotePMExprWithClNm(Set1, clnm, lb));
            new_bind_l.ImpAppend(msb);
            break;
          }
          case TAG_TYPE_AS_MultTypeBind: {
            new_bind_l.ImpAppend(mb);
            break;
          }
          default: {
            break;
          }
        }
      }

      TYPE_AS_Maplet new_elem (elem);
      new_elem.SetField(pos_AS_Maplet_mapdom, QuotePMExprWithClNm(md, clnm, local_bind));
      new_elem.SetField(pos_AS_Maplet_maprng, QuotePMExprWithClNm(mr, clnm, local_bind));

      TYPE_AS_MapComprehensionExpr mce (pat);
      mce.SetField(pos_AS_MapComprehensionExpr_elem, new_elem);
      mce.SetField(pos_AS_MapComprehensionExpr_bind, new_bind_l);
      mce.SetField(pos_AS_MapComprehensionExpr_pred, QuotePMExprWithClNm(pred, clnm, local_bind));
      return mce;
    }
    case TAG_TYPE_AS_TupleConstructorExpr: {
      const SEQ<TYPE_AS_Expr> & fields (pat.GetSequence(pos_AS_TupleConstructorExpr_fields));
  
      SEQ<TYPE_AS_Expr> newfields;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++) {
        newfields.ImpAppend(QuotePMExprWithClNm(fields[i], clnm, lb));
      }

      TYPE_AS_TupleConstructorExpr tce (pat);
      tce.SetField(pos_AS_TupleConstructorExpr_fields, newfields);
      return tce;
    }
    case TAG_TYPE_AS_TokenConstructorExpr: {
      const TYPE_AS_Expr & field (pat.GetRecord(pos_AS_TokenConstructorExpr_field));

      TYPE_AS_TokenConstructorExpr tce (pat);
      tce.SetField(pos_AS_TokenConstructorExpr_field, QuotePMExprWithClNm(field, clnm, lb));
      return tce;
    }
    case TAG_TYPE_AS_RecordConstructorExpr: {
      const SEQ<TYPE_AS_Expr> & fields (pat.GetSequence(pos_AS_RecordConstructorExpr_fields));
      SEQ<TYPE_AS_Expr> newfields;
      size_t len_fields = fields.Length();
      for (size_t i = 1; i <= len_fields; i++) {
        newfields.ImpAppend(QuotePMExprWithClNm(fields[i], clnm, lb));
      }
      TYPE_AS_RecordConstructorExpr rce (pat);
      rce.SetField(pos_AS_RecordConstructorExpr_tag,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_RecordConstructorExpr_tag), clnm, Set()));
      rce.SetField(pos_AS_RecordConstructorExpr_fields, newfields);
      return rce;
    }
    case TAG_TYPE_AS_RecordModifierExpr: {
      const TYPE_AS_Expr & rec(pat.GetRecord(pos_AS_RecordModifierExpr_rec));
      const SEQ<TYPE_AS_RecordModification> & modifiers (pat.GetSequence(pos_AS_RecordModifierExpr_modifiers));
  
      SEQ<TYPE_AS_RecordModification> newmodifiers;
      size_t len_modifiers = modifiers.Length();
      for (size_t i = 1; i <= len_modifiers; i++) {
        TYPE_AS_RecordModification rm (modifiers[i]);
        const TYPE_AS_Expr & nexpr (rm.GetRecord(pos_AS_RecordModification_newexpr));

        rm.SetField(pos_AS_RecordModification_newexpr, QuotePMExprWithClNm(nexpr, clnm, lb));
        newmodifiers.ImpAppend(rm);
      }

      TYPE_AS_RecordModifierExpr rme (pat);
      rme.SetField(pos_AS_RecordModifierExpr_rec, QuotePMExprWithClNm(rec, clnm, lb));
      rme.SetField(pos_AS_RecordModifierExpr_modifiers, newmodifiers);
      return rme;
    }
    case TAG_TYPE_AS_FieldSelectExpr : {
      TYPE_AS_FieldSelectExpr fse (pat);
      fse.SetField(pos_AS_FieldSelectExpr_rec,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_FieldSelectExpr_rec), clnm, lb));
      return fse;
    }
    case TAG_TYPE_AS_FctTypeInstExpr : {
      TYPE_AS_FctTypeInstExpr ftie (pat);
      ftie.SetField(pos_AS_FctTypeInstExpr_polyfct,
                    QuotePMExprWithClNm(pat.GetRecord(pos_AS_FctTypeInstExpr_polyfct), clnm, lb));
      return ftie;
    }
    case TAG_TYPE_AS_LambdaExpr : {
      TYPE_AS_LambdaExpr le (pat);
      le.SetField(pos_AS_LambdaExpr_body,
                  QuotePMExprWithClNm(pat.GetRecord(pos_AS_LambdaExpr_body), clnm, lb));
      return le;
    }
    case TAG_TYPE_AS_ApplyExpr : {
      const SEQ<TYPE_AS_Expr> & arg (pat.GetSequence(pos_AS_ApplyExpr_arg));
      SEQ<TYPE_AS_Expr> new_arg;
      size_t len_arg = arg.Length(); 
      for (size_t i = 1; i <= len_arg; i++) {
        new_arg.ImpAppend(QuotePMExprWithClNm(arg[i], clnm, lb));
      }
      TYPE_AS_ApplyExpr ae (pat);
      ae.SetField(pos_AS_ApplyExpr_fct,
                  QuotePMExprWithClNm(pat.GetRecord(pos_AS_ApplyExpr_fct), clnm, lb));
      ae.SetField(pos_AS_ApplyExpr_arg, new_arg);
      return ae;
    }
    case TAG_TYPE_AS_IsExpr: {
      const Generic & type (pat.GetField(pos_AS_IsExpr_type));
      const TYPE_AS_Expr & arg (pat.GetRecord(pos_AS_IsExpr_arg));
  
      TYPE_AS_IsExpr ie (pat);
      if (type.Is(TAG_TYPE_AS_Name)) {
        ie.SetField(pos_AS_IsExpr_type, QuotePMExprWithClNm(type, clnm, lb));
      }
      ie.SetField(pos_AS_IsExpr_arg, QuotePMExprWithClNm(arg, clnm, lb));
      return ie;
    }
    case TAG_TYPE_AS_UndefinedExpr: {
      return pat;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr:
    case TAG_TYPE_AS_NewExpr: {
      return pat;
    }
    case TAG_TYPE_AS_IsOfClassExpr: {
      TYPE_AS_IsOfClassExpr ioce (pat);
      ioce.SetField(pos_AS_IsOfClassExpr_arg,
                    QuotePMExprWithClNm(pat.GetRecord(pos_AS_IsOfClassExpr_arg), clnm, lb));
      return ioce;
    }
    case TAG_TYPE_AS_IsOfBaseClassExpr: {
      TYPE_AS_IsOfBaseClassExpr iobce (pat);
      iobce.SetField(pos_AS_IsOfBaseClassExpr_arg,
                     QuotePMExprWithClNm(pat.GetRecord(pos_AS_IsOfBaseClassExpr_arg), clnm, lb));
      return iobce;
    }
    case TAG_TYPE_AS_SameBaseClassExpr : {
      TYPE_AS_SameBaseClassExpr sbce (pat);
      sbce.SetField(pos_AS_SameBaseClassExpr_expr1,
                    QuotePMExprWithClNm(pat.GetRecord(pos_AS_SameBaseClassExpr_expr1), clnm, lb));
      sbce.SetField(pos_AS_SameBaseClassExpr_expr2,
                    QuotePMExprWithClNm(pat.GetRecord(pos_AS_SameBaseClassExpr_expr2), clnm, lb));
      return sbce;
    }
    case TAG_TYPE_AS_SameClassExpr : {
      TYPE_AS_SameClassExpr sce (pat);
      sce.SetField(pos_AS_SameClassExpr_expr1,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_SameClassExpr_expr1), clnm, lb));
      sce.SetField(pos_AS_SameClassExpr_expr2,
                   QuotePMExprWithClNm(pat.GetRecord(pos_AS_SameClassExpr_expr2), clnm, lb));
      return sce;
    }
    case TAG_TYPE_AS_ActExpr :
    case TAG_TYPE_AS_FinExpr :
    case TAG_TYPE_AS_ActiveExpr :
    case TAG_TYPE_AS_WaitingExpr :
    case TAG_TYPE_AS_ReqExpr : {
      return pat;
    }
#endif //VDMPP
    case TAG_TYPE_AS_BoolLit :
    case TAG_TYPE_AS_NilLit :
    case TAG_TYPE_AS_RealLit :
    case TAG_TYPE_AS_NumLit :
    case TAG_TYPE_AS_CharLit :
    case TAG_TYPE_AS_TextLit :
    case TAG_TYPE_AS_QuoteLit : {
      return pat;
    }
    case TAG_TYPE_AS_Name : {
      if (pat.GetSequence(pos_AS_Name_ids).Length() == 2 || lb.InSet(pat)) {
        return pat;
      }
      else {
        return ASTAUX::Combine2Names(clnm, pat);
      }
    }
    default : {
      return pat;
    }
  }
}

// QuotePMExprWithTemp
// pat : (AS`Pattern | [AS`Expr])
// clnm : AS`Name
// lb : set of AS`Name
// ==> [AS`Pattern | AS`Expr]
Generic vdmcg::QuotePMExprWithTemp(const Generic & patg, const TYPE_AS_Name & clnm, const SET<TYPE_AS_Name> & lb)
{
  if (patg.IsNil()) {
    return patg;
  }

  Record pat (patg);
  switch (pat.GetTag()) {
    case TAG_TYPE_AS_PatternName: {
      TYPE_AS_PatternName pm (pat);
      const Generic & gnm (pm.GetField(pos_AS_PatternName_nm) );  // [Name] | OldName
      if (gnm.IsNil()) {
        return pat;
      }
      Record nm(gnm); 
      TYPE_AS_Ids ids (nm.GetField(1));
      TYPE_AS_Id id (ids[1]);
      TYPE_AS_Id newid (id.ImpConc(ASTAUX::MkId(L"temp")));
      TYPE_AS_Ids newids;
      newids.ImpAppend(newid);
      nm.SetField(1,newids);
      pm.set_nm(nm);
  //    pm.set_cid(cid);
      return pm;
    }
    case TAG_TYPE_AS_MatchVal: {
      return pat;
    }
    case TAG_TYPE_AS_SetEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & elems (pat.GetSequence(pos_AS_SetEnumPattern_Elems));
      SEQ<TYPE_AS_Pattern> newElems;
      size_t len_elems = elems.Length();
      for (size_t idx = 1; idx <= len_elems; idx++) {
        newElems.ImpAppend(QuotePMExprWithTemp(elems[idx], clnm, lb));
      }
      TYPE_AS_SetEnumPattern sep (pat);
      sep.SetField(pos_AS_SetEnumPattern_Elems, newElems);
      return sep;
    }
    case TAG_TYPE_AS_SetUnionPattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SetUnionPattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SetUnionPattern_rp));
      TYPE_AS_SetUnionPattern sup (pat);
      sup.SetField(pos_AS_SetUnionPattern_lp, QuotePMExprWithTemp(lp, clnm, lb));
      sup.SetField(pos_AS_SetUnionPattern_rp, QuotePMExprWithTemp(rp, clnm, lb));
      return sup;
    }
    case TAG_TYPE_AS_SeqEnumPattern: {
      const SEQ<TYPE_AS_Pattern> & els (pat.GetSequence(pos_AS_SeqEnumPattern_els));
      SEQ<TYPE_AS_Pattern> new_els;
      size_t len_els = els.Length();
      for (size_t idx = 1; idx <= len_els; idx++) {
        new_els.ImpAppend(QuotePMExprWithTemp(els[idx], clnm, lb));
      }
      TYPE_AS_SeqEnumPattern sep(pat);
      sep.SetField(pos_AS_SeqEnumPattern_els, new_els);
      return sep;
    }
    case TAG_TYPE_AS_SeqConcPattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_SeqConcPattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_SeqConcPattern_rp));
      TYPE_AS_SeqConcPattern scp (pat);
      scp.SetField(pos_AS_SeqConcPattern_lp, QuotePMExprWithTemp(lp, clnm, lb));
      scp.SetField(pos_AS_SeqConcPattern_rp, QuotePMExprWithTemp(rp, clnm, lb));
      return scp;
    }
    case TAG_TYPE_AS_MapEnumPattern: {
      const SEQ<TYPE_AS_MapletPattern> & mls (pat.GetSequence(pos_AS_MapEnumPattern_mls));
      SEQ<TYPE_AS_MapletPattern> new_mls;
      size_t len_mls = mls.Length();
      for (size_t idx = 1; idx <= len_mls; idx++) {
        const TYPE_AS_MapletPattern & mp (mls[idx]);
        const TYPE_AS_Pattern & dp (mp.GetRecord(pos_AS_MapletPattern_dp));
        const TYPE_AS_Pattern & rp (mp.GetRecord(pos_AS_MapletPattern_rp));
        TYPE_AS_MapletPattern new_mp (mp);
        new_mp.SetField(pos_AS_MapletPattern_dp, QuotePMExprWithTemp(dp, clnm, lb));
        new_mp.SetField(pos_AS_MapletPattern_rp, QuotePMExprWithTemp(rp, clnm, lb));
        new_mls.ImpAppend(new_mp);
      }
      TYPE_AS_MapEnumPattern mep(pat);
      mep.SetField(pos_AS_MapEnumPattern_mls, new_mls);
      return mep;
    }
    case TAG_TYPE_AS_MapMergePattern: {
      const TYPE_AS_Pattern & lp (pat.GetRecord(pos_AS_MapMergePattern_lp));
      const TYPE_AS_Pattern & rp (pat.GetRecord(pos_AS_MapMergePattern_rp));
      TYPE_AS_MapMergePattern mmp (pat);
      mmp.SetField(pos_AS_MapMergePattern_lp, QuotePMExprWithTemp(lp, clnm, lb));
      mmp.SetField(pos_AS_MapMergePattern_rp, QuotePMExprWithTemp(rp, clnm, lb));
      return mmp;
    }
    case TAG_TYPE_AS_TuplePattern: {
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_TuplePattern_fields));
      SEQ<TYPE_AS_Pattern> newfields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        newfields.ImpAppend(QuotePMExprWithTemp(fields[idx], clnm, lb));
      }
      TYPE_AS_TuplePattern tp (pat);
      tp.SetField(pos_AS_TuplePattern_fields, newfields);
      return tp;
    }
    case TAG_TYPE_AS_RecordPattern: {
      const TYPE_AS_Name & nm (pat.GetRecord(pos_AS_RecordPattern_nm));
      const SEQ<TYPE_AS_Pattern> & fields (pat.GetSequence(pos_AS_RecordPattern_fields));
      SEQ<TYPE_AS_Pattern> newfields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        newfields.ImpAppend(QuotePMExprWithTemp(fields[idx], clnm, lb));
      }
      TYPE_AS_RecordPattern rp (pat);
      rp.SetField(pos_AS_RecordPattern_nm, QuotePMExprWithTemp(nm, clnm, lb));
      rp.SetField(pos_AS_RecordPattern_fields, newfields);
      return rp;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_ObjectPattern: {
      const SEQ<TYPE_AS_FieldPattern> & fields (pat.GetSequence(pos_AS_ObjectPattern_fields));
      SEQ<TYPE_AS_FieldPattern> newfields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        const TYPE_AS_FieldPattern & fp (fields[idx]);
        const TYPE_AS_Pattern & p (fp.GetRecord(pos_AS_FieldPattern_pat));
        TYPE_AS_FieldPattern new_fp (fp);
        new_fp.SetField(pos_AS_FieldPattern_pat, QuotePMExprWithTemp(p, clnm, lb));
        newfields.ImpAppend(new_fp);
      }
      TYPE_AS_ObjectPattern rp (pat);
      rp.SetField(pos_AS_RecordPattern_fields, newfields);
      return rp;
    }
#endif // VDMPP
    case TAG_TYPE_AS_BracketedExpr: {
      TYPE_AS_BracketedExpr be (pat);
      be.set_expr(QuotePMExprWithTemp(pat.GetRecord(pos_AS_BracketedExpr_expr), clnm, lb));
      return be;
    }
    case TAG_TYPE_AS_DefExpr: {
      const type_dd2PL & def_l (pat.GetSequence(pos_AS_DefExpr_Def));
      const TYPE_AS_Expr & expr (pat.GetRecord(pos_AS_DefExpr_In));
  
      SET<TYPE_AS_Name> localbind (lb);
      type_dd2PL local_def;
      size_t len_def_l = def_l.Length();
      for (size_t idx = 1; idx <= len_def_l; idx++) {
        type_dd2P as_def (def_l[idx]);
        const TYPE_AS_PatternBind & p (as_def.GetField(1));
        const TYPE_AS_Expr & e (as_def.GetField(2));
        SET<TYPE_AS_Name> pnms (FindPatternId(p).Dom());
        as_def.SetField(2, QuotePMExprWithTemp(e, clnm, localbind));
        local_def.ImpAppend(as_def);
        localbind.ImpUnion(pnms);
      }
      TYPE_AS_DefExpr de (pat);
      de.SetField(pos_AS_DefExpr_Def, local_def);
      de.SetField(pos_AS_DefExpr_In, QuotePMExprWithTemp(expr, clnm, localbind));
      return de;
    }
    case TAG_TYPE_AS_LetExpr: {
      const SEQ<TYPE_AS_LocalDef> & locdef (pat.GetSequence(pos_AS_LetExpr_localdef));
      const TYPE_AS_Expr & body (pat.GetRecord(pos_AS_LetExpr_body));
  
      SET<TYPE_AS_Name> localbind (lb);
      SEQ<TYPE_AS_LocalDef> local_def;
      size_t len_locdef = locdef.Length();
      for (size_t idx = 1; idx <= len_locdef; idx++) {
        const TYPE_AS_LocalDef & as_localdef (locdef[idx]);
        switch(as_localdef.GetTag()) {
          case TAG_TYPE_AS_ExplFnDef:
          case TAG_TYPE_AS_ImplFnDef:
          case TAG_TYPE_AS_ExtExplFnDef: {
            local_def.ImpAppend(as_localdef);
            break;
          }
          case TAG_TYPE_AS_ValueDef: {
            const TYPE_AS_Pattern & vdpat (as_localdef.GetRecord(pos_AS_ValueDef_pat));
            const TYPE_AS_Expr & expr (as_localdef.GetRecord(pos_AS_ValueDef_val));
            SET<TYPE_AS_Name> pnms (FindPatternId(vdpat).Dom());
            TYPE_AS_ValueDef vd (as_localdef);
            vd.set_val(QuotePMExprWithTemp(expr, clnm, localbind));
            local_def.ImpAppend(vd);
            localbind.ImpUnion(pnms);
            break;
          }
        }
      }
      TYPE_AS_LetExpr le (pat);
      le.SetField(pos_AS_LetExpr_localdef, local_def);
      le.SetField(pos_AS_LetExpr_body, QuotePMExprWithTemp(body, clnm, localbind));
      return le;
    }
    case TAG_TYPE_AS_LetBeSTExpr: {
      const SEQ<TYPE_AS_MultBind> & bind (pat.GetSequence(pos_AS_LetBeSTExpr_lhs));
      const Generic & St        (pat.GetField(pos_AS_LetBeSTExpr_St));
      const TYPE_AS_Expr & In   (pat.GetRecord(pos_AS_LetBeSTExpr_In));
  
      SET<TYPE_AS_Name> localbind (lb);
      SEQ<TYPE_AS_MultBind> newbind;
      size_t len_bind = bind.Length();
      for (size_t idx = 1; idx <= len_bind; idx++) {
        const TYPE_AS_MultBind & b (bind[idx]);
        switch(b.GetTag()) {
          case TAG_TYPE_AS_MultSetBind: {
            const SEQ<TYPE_AS_Pattern> & pat_l (b.GetSequence(pos_AS_MultSetBind_pat));
            const TYPE_AS_Expr & Set1 (b.GetRecord(pos_AS_MultSetBind_Set));
  
            SET<TYPE_AS_Name> pnms;
            size_t len_pat_l = pat_l.Length();
            for (size_t i = 1; i <= len_pat_l; i++) {
              pnms.ImpUnion(FindPatternId(pat_l[i]).Dom());
            } 
            localbind.ImpUnion(pnms);

            TYPE_AS_MultSetBind msb (b);
            msb.SetField(pos_AS_MultSetBind_Set, QuotePMExprWithTemp(Set1, clnm, lb));
            newbind.ImpAppend(msb);
            break;
          }
          case TAG_TYPE_AS_MultTypeBind: {
            newbind.ImpAppend(b);
            break;
          }
        }
      }
      TYPE_AS_LetBeSTExpr lbe (pat);
      lbe.SetField(pos_AS_LetBeSTExpr_lhs, newbind);
      lbe.SetField(pos_AS_LetBeSTExpr_St, QuotePMExprWithTemp(St, clnm, localbind));
      lbe.SetField(pos_AS_LetBeSTExpr_In, QuotePMExprWithTemp(In, clnm, localbind));
      return lbe;
    }
    case TAG_TYPE_AS_IfExpr: {
      const TYPE_AS_Expr & test (pat.GetRecord(pos_AS_IfExpr_test));
      const TYPE_AS_Expr & cons (pat.GetRecord(pos_AS_IfExpr_cons));
      const SEQ<TYPE_AS_ElseifExpr> & elsif (pat.GetSequence(pos_AS_IfExpr_elsif));
      const TYPE_AS_Expr & altn (pat.GetRecord(pos_AS_IfExpr_altn));
  
      SEQ<TYPE_AS_ElseifExpr> newelsif;
      size_t len_elsif = elsif.Length();
      for (size_t idx = 1; idx <= len_elsif; idx++) {
        TYPE_AS_ElseifExpr eie (elsif[idx]);
        const TYPE_AS_Expr & else_test (eie.GetRecord(pos_AS_ElseifExpr_test));
        const TYPE_AS_Expr & else_cons (eie.GetRecord(pos_AS_ElseifExpr_cons));
        eie.SetField(pos_AS_ElseifExpr_test, QuotePMExprWithTemp(else_test, clnm, lb));
        eie.SetField(pos_AS_ElseifExpr_cons, QuotePMExprWithTemp(else_cons, clnm, lb));
        newelsif.ImpAppend(eie);
      }
      TYPE_AS_IfExpr ie (pat);
      ie.SetField(pos_AS_IfExpr_test, QuotePMExprWithTemp(test, clnm, lb));
      ie.SetField(pos_AS_IfExpr_cons, QuotePMExprWithTemp(cons, clnm, lb));
      ie.SetField(pos_AS_IfExpr_elsif, newelsif);
      ie.SetField(pos_AS_IfExpr_altn, QuotePMExprWithTemp(altn, clnm, lb));
      return ie;
    }
    case TAG_TYPE_AS_CasesExpr: {
      const TYPE_AS_Expr & sel            (pat.GetRecord(pos_AS_CasesExpr_sel));
      const SEQ<TYPE_AS_CaseAltn> & altns (pat.GetSequence(pos_AS_CasesExpr_altns));
      const Generic & Others              (pat.GetField(pos_AS_CasesExpr_Others));
  
      SEQ<TYPE_AS_CaseAltn> newaltns;
      size_t len_altns = altns.Length();
      for (size_t idx = 1; idx <= len_altns; idx++) {
        TYPE_AS_CaseAltn ca (altns[idx]);
        const SEQ<TYPE_AS_Pattern> & match (ca.GetSequence(pos_AS_CaseAltn_match));
        const TYPE_AS_Expr & body (ca.GetRecord(pos_AS_CaseAltn_body));
        Set pnms;
        size_t len_match = match.Length();
        for (size_t i = 1; i <= len_match; i++) {
          SET<TYPE_AS_Name> p_nms (FindPatternId(match[i]).Dom());
          pnms.ImpUnion(p_nms);
        }
        ca.SetField(pos_AS_CaseAltn_body, QuotePMExprWithTemp(body, clnm, pnms));
        newaltns.ImpAppend(ca);
      }
      TYPE_AS_CasesExpr ce (pat);
      ce.SetField(pos_AS_CasesExpr_sel, QuotePMExprWithTemp(sel, clnm, lb));
      ce.SetField(pos_AS_CasesExpr_altns, newaltns);
      ce.SetField(pos_AS_CasesExpr_Others, QuotePMExprWithTemp(Others, clnm, lb));
      return ce;
    }
    case TAG_TYPE_AS_PrefixExpr: {
      TYPE_AS_PrefixExpr pe (pat);
      const TYPE_AS_Expr & arg (pat.GetRecord(pos_AS_PrefixExpr_arg));
      pe.SetField(pos_AS_PrefixExpr_arg, QuotePMExprWithTemp(arg, clnm, lb));
      return pe;
    }
    case TAG_TYPE_AS_BinaryExpr: {
      TYPE_AS_BinaryExpr be (pat);
      be.set_left(QuotePMExprWithTemp(pat.GetRecord(pos_AS_BinaryExpr_left), clnm, lb));
      be.set_right(QuotePMExprWithTemp(pat.GetRecord(pos_AS_BinaryExpr_right), clnm, lb));
      return be;
    }
    case TAG_TYPE_AS_AllOrExistsExpr: {
      const SEQ<TYPE_AS_MultBind> & bind (pat.GetSequence(pos_AS_AllOrExistsExpr_bind));
      const TYPE_AS_Expr & pred (pat.GetRecord(pos_AS_AllOrExistsExpr_pred));
  
      SET<TYPE_AS_Name> local_bind (lb);
      SEQ<TYPE_AS_MultBind> new_bind_l;
      size_t len_bind = bind.Length();
      for (size_t idx = 1; idx <= len_bind; idx++) {
        const TYPE_AS_MultBind & b (bind[idx]);
        switch(b.GetTag()) {
          case TAG_TYPE_AS_MultSetBind: {
            const SEQ<TYPE_AS_Pattern> & pat_l (b.GetSequence(pos_AS_MultSetBind_pat));
            const TYPE_AS_Expr & Set1 (b.GetRecord(pos_AS_MultSetBind_Set));
  
            SET<TYPE_AS_Name> pnms;
            size_t len_pat_l = pat_l.Length();
            for (size_t i = 1; i <= len_pat_l; i++) {
              pnms.ImpUnion(FindPatternId(pat_l[i]).Dom());
            } 
            local_bind.ImpUnion(pnms);

            TYPE_AS_MultSetBind msb (b);
            msb.SetField(pos_AS_MultSetBind_Set, QuotePMExprWithTemp(Set1, clnm, lb));
            new_bind_l.ImpAppend(msb);
            break;
          }
          case TAG_TYPE_AS_MultTypeBind: {
            new_bind_l.ImpAppend(b);
            break;
          }
        }
      }
  
      TYPE_AS_AllOrExistsExpr aoee (pat);
      aoee.SetField(pos_AS_AllOrExistsExpr_bind, new_bind_l);
      aoee.SetField(pos_AS_AllOrExistsExpr_pred, QuotePMExprWithTemp(pred, clnm, local_bind));
      return aoee;
    }
    case TAG_TYPE_AS_ExistsUniqueExpr: {
      const TYPE_AS_Bind & bind (pat.GetRecord(pos_AS_ExistsUniqueExpr_bind));
      const TYPE_AS_Expr & pred (pat.GetRecord(pos_AS_ExistsUniqueExpr_pred));
  
      SET<TYPE_AS_Name> local_bind (lb);
      TYPE_AS_Bind new_bind;
  
      switch(bind.GetTag()) {
        case TAG_TYPE_AS_TypeBind: {
          new_bind = bind;
          break;
        }
        case TAG_TYPE_AS_SetBind: {
          const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SetBind_pat));
          const TYPE_AS_Expr & expr   (bind.GetRecord(pos_AS_SetBind_Set));
          local_bind.ImpUnion(FindPatternId(pat).Dom());
          TYPE_AS_SetBind sb (bind);
          sb.SetField(pos_AS_SetBind_Set, QuotePMExprWithTemp(expr, clnm, lb));
          new_bind = sb;
          break;
        }
        default: {
          break;
        }
      }
      TYPE_AS_ExistsUniqueExpr eue (pat);
      eue.SetField(pos_AS_ExistsUniqueExpr_bind, new_bind);
      eue.SetField(pos_AS_ExistsUniqueExpr_pred, QuotePMExprWithTemp(pred, clnm, local_bind));
      return eue;
    }
    case TAG_TYPE_AS_IotaExpr: {
      const TYPE_AS_Bind & bind (pat.GetRecord(pos_AS_IotaExpr_bind));
      const TYPE_AS_Expr & pred (pat.GetRecord(pos_AS_IotaExpr_pred));
  
      SET<TYPE_AS_Name> local_bind(lb);
      TYPE_AS_Bind new_bind;
  
      switch(bind.GetTag()) {
        case TAG_TYPE_AS_TypeBind: {
          new_bind = bind;
          break;
        }
        case TAG_TYPE_AS_SetBind: {
          const TYPE_AS_Pattern & pat (bind.GetRecord(pos_AS_SetBind_pat));
          const TYPE_AS_Expr & expr   (bind.GetRecord(pos_AS_SetBind_Set));

          local_bind.ImpUnion(FindPatternId(pat).Dom());
          TYPE_AS_SetBind sb (bind);
          sb.SetField(pos_AS_SetBind_Set, QuotePMExprWithTemp(expr, clnm, lb));
          new_bind = sb;
          break;
        }
        default: {
          break;
        }
      }
      TYPE_AS_IotaExpr ie (pat);
      ie.SetField(pos_AS_IotaExpr_bind, new_bind);
      ie.SetField(pos_AS_IotaExpr_pred, QuotePMExprWithTemp(pred, clnm, local_bind));
      return ie;
    }
    case TAG_TYPE_AS_SetEnumerationExpr: {
      const SEQ<TYPE_AS_Expr> & els (pat.GetSequence(pos_AS_SetEnumerationExpr_els));
  
      SEQ<TYPE_AS_Expr> newels;
      size_t len_els = els.Length();
      for (size_t idx = 1; idx <= len_els; idx++) {
        newels.ImpAppend(QuotePMExprWithTemp(els[idx], clnm, lb));
      } 
      TYPE_AS_SetEnumerationExpr see (pat);
      see.SetField(pos_AS_SetEnumerationExpr_els, newels);
      return see;
    }
    case TAG_TYPE_AS_SetComprehensionExpr: {
      const TYPE_AS_Expr & elem          (pat.GetRecord(pos_AS_SetComprehensionExpr_elem));
      const SEQ<TYPE_AS_MultBind> & bind (pat.GetSequence(pos_AS_SetComprehensionExpr_bind));
      const Generic & pred               (pat.GetField(pos_AS_SetComprehensionExpr_pred));
  
      SET<TYPE_AS_Name> local_bind(lb);
      SEQ<TYPE_AS_MultBind> new_bind_l;
      size_t len_bind = bind.Length();
      for (size_t idx = 1; idx <= len_bind; idx++) {
        const TYPE_AS_MultBind & b (bind[idx]);
        switch(b.GetTag()) {
          case TAG_TYPE_AS_MultSetBind: {
            const SEQ<TYPE_AS_Pattern> & pat_l (b.GetSequence(pos_AS_MultSetBind_pat));
            const TYPE_AS_Expr & Set1          (b.GetRecord(pos_AS_MultSetBind_Set));
  
            SET<TYPE_AS_Name> pnms;
            size_t len_pat_l = pat_l.Length();
            for (size_t i = 1; i <= len_pat_l; i++) {
              pnms.ImpUnion(FindPatternId(pat_l[i]).Dom());
            }
            local_bind.ImpUnion(pnms);
            TYPE_AS_MultSetBind msb (b);
            msb.SetField(pos_AS_MultSetBind_Set, QuotePMExprWithTemp(Set1, clnm, lb));
            new_bind_l.ImpAppend(msb);
            break;
          }
          case TAG_TYPE_AS_MultTypeBind: {
            new_bind_l.ImpAppend(b);
            break;
          }
        }
      }
      TYPE_AS_SetComprehensionExpr sce (pat);
      sce.SetField(pos_AS_SetComprehensionExpr_elem, QuotePMExprWithTemp(elem, clnm, local_bind));
      sce.SetField(pos_AS_SetComprehensionExpr_bind, new_bind_l);
      sce.SetField(pos_AS_SetComprehensionExpr_pred, QuotePMExprWithTemp(pred, clnm, local_bind));
      return sce;
    }
    case TAG_TYPE_AS_SetRangeExpr: {
      TYPE_AS_SetRangeExpr sre (pat);
      sre.SetField(pos_AS_SetRangeExpr_lb, QuotePMExprWithTemp(sre.GetRecord(pos_AS_SetRangeExpr_lb), clnm, lb));
      sre.SetField(pos_AS_SetRangeExpr_ub, QuotePMExprWithTemp(sre.GetRecord(pos_AS_SetRangeExpr_ub), clnm, lb));
      return sre;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      const SEQ<TYPE_AS_Expr> & els (pat.GetSequence(pos_AS_SeqEnumerationExpr_els));
      SEQ<TYPE_AS_Expr> newels;
      size_t len_els = els.Length();
      for (size_t idx = 1; idx <= len_els; idx++) {
        newels.ImpAppend(QuotePMExprWithTemp(els[idx], clnm, lb));
      } 
      TYPE_AS_SeqEnumerationExpr see (pat);
      see.SetField(pos_AS_SeqEnumerationExpr_els, newels);
      return see;
    }
    case TAG_TYPE_AS_SeqComprehensionExpr : {
      const TYPE_AS_Expr & elem    (pat.GetRecord(pos_AS_SeqComprehensionExpr_elem));
      const TYPE_AS_SetBind & bind (pat.GetRecord(pos_AS_SeqComprehensionExpr_bind));
      const Generic & pred         (pat.GetField(pos_AS_SeqComprehensionExpr_pred));

      const TYPE_AS_Pattern & lpat (bind.GetRecord(pos_AS_SetBind_pat));
      const TYPE_AS_Expr & Set1   (bind.GetRecord(pos_AS_SetBind_Set));
  
      SET<TYPE_AS_Name> new_lb (lb);
      new_lb.ImpUnion(FindPatternId(lpat).Dom());
  
      TYPE_AS_SetBind newbind (bind);
      newbind.set_Set(QuotePMExprWithTemp(Set1, clnm, lb));

      TYPE_AS_SeqComprehensionExpr sce (pat);
      sce.SetField(pos_AS_SeqComprehensionExpr_elem, QuotePMExprWithTemp(elem, clnm, new_lb));
      sce.SetField(pos_AS_SeqComprehensionExpr_bind, newbind);
      sce.SetField(pos_AS_SeqComprehensionExpr_pred, QuotePMExprWithTemp(pred, clnm, new_lb));
      return sce;
    }
    case TAG_TYPE_AS_SubSequenceExpr: {
      TYPE_AS_SubSequenceExpr sse (pat);
      sse.SetField(pos_AS_SubSequenceExpr_sequence,
                   QuotePMExprWithTemp(pat.GetRecord(pos_AS_SubSequenceExpr_sequence), clnm, lb));
      sse.SetField(pos_AS_SubSequenceExpr_frompos,
                   QuotePMExprWithTemp(pat.GetRecord(pos_AS_SubSequenceExpr_frompos), clnm, lb));
      sse.SetField(pos_AS_SubSequenceExpr_topos,
                   QuotePMExprWithTemp(pat.GetRecord(pos_AS_SubSequenceExpr_topos), clnm, lb));
      return sse;
    }
    case TAG_TYPE_AS_SeqModifyMapOverrideExpr: {
      TYPE_AS_SeqModifyMapOverrideExpr smmoe (pat);
      smmoe.SetField(pos_AS_SeqModifyMapOverrideExpr_seqmap,
                     QuotePMExprWithTemp(pat.GetRecord(pos_AS_SeqModifyMapOverrideExpr_seqmap), clnm, lb));
      smmoe.SetField(pos_AS_SeqModifyMapOverrideExpr_mapexp,
                     QuotePMExprWithTemp(pat.GetRecord(pos_AS_SeqModifyMapOverrideExpr_mapexp), clnm, lb));
      return smmoe;
    }
    case TAG_TYPE_AS_MapEnumerationExpr : {
      const SEQ<TYPE_AS_Maplet> & els (pat.GetSequence(pos_AS_MapEnumerationExpr_els));
  
      SEQ<TYPE_AS_Maplet> newels;
      size_t len_els = els.Length();
      for (size_t idx = 1; idx <= len_els; idx++) {
        TYPE_AS_Maplet ml (els[idx]);
        ml.SetField(pos_AS_Maplet_mapdom, QuotePMExprWithTemp(ml.GetRecord(pos_AS_Maplet_mapdom), clnm, lb));
        ml.SetField(pos_AS_Maplet_maprng, QuotePMExprWithTemp(ml.GetRecord(pos_AS_Maplet_maprng), clnm, lb));
        newels.ImpAppend(ml);
      }
      TYPE_AS_MapEnumerationExpr mee (pat);
      mee.SetField(pos_AS_MapEnumerationExpr_els, newels);
      return mee;
    }
    case TAG_TYPE_AS_MapComprehensionExpr: {
      const TYPE_AS_Maplet & elem        (pat.GetRecord(pos_AS_MapComprehensionExpr_elem));
      const SEQ<TYPE_AS_MultBind> & bind (pat.GetSequence(pos_AS_MapComprehensionExpr_bind));
      const Generic & pred               (pat.GetField(pos_AS_MapComprehensionExpr_pred));

      const TYPE_AS_Expr & md (elem.GetRecord(pos_AS_Maplet_mapdom));
      const TYPE_AS_Expr & mr (elem.GetRecord(pos_AS_Maplet_maprng));
  
      SET<TYPE_AS_Name> local_bind(lb);
      SEQ<TYPE_AS_MultBind> new_bind_l;
      size_t len_bind = bind.Length(); 
      for (size_t idx = 1; idx <= len_bind; idx++) {
        const TYPE_AS_MultBind & b (bind[idx]);
        switch(b.GetTag()) {
          case TAG_TYPE_AS_MultSetBind: {
            const SEQ<TYPE_AS_Pattern> & pat_l (b.GetSequence(pos_AS_MultSetBind_pat));
            const TYPE_AS_Expr & Set1          (b.GetRecord(pos_AS_MultSetBind_Set));
  
            SET<TYPE_AS_Name> pnms;
            size_t len_pat_l = pat_l.Length();
            for (size_t i = 1; i <= len_pat_l; i++) {
              pnms.ImpUnion(FindPatternId(pat_l[i]).Dom());
            }
            local_bind.ImpUnion(pnms);
            TYPE_AS_MultSetBind msb (b);
            msb.SetField(pos_AS_MultSetBind_Set, QuotePMExprWithTemp(Set1, clnm, lb));
            new_bind_l.ImpAppend(msb);
            break;
          }
          case TAG_TYPE_AS_MultTypeBind: {
            new_bind_l.ImpAppend(b);
            break;
          }
        }
      }
      TYPE_AS_Maplet newelem (elem);
      newelem.SetField(pos_AS_Maplet_mapdom, QuotePMExprWithTemp(md, clnm, local_bind));
      newelem.SetField(pos_AS_Maplet_maprng, QuotePMExprWithTemp(mr, clnm, local_bind));
      TYPE_AS_MapComprehensionExpr mce (pat);
      mce.SetField(pos_AS_MapComprehensionExpr_elem, newelem);
      mce.SetField(pos_AS_MapComprehensionExpr_bind, new_bind_l);
      mce.SetField(pos_AS_MapComprehensionExpr_pred, QuotePMExprWithTemp(pred, clnm, local_bind));
  
      return mce;
    }
    case TAG_TYPE_AS_TupleConstructorExpr : {
      const SEQ<TYPE_AS_Expr> & fields (pat.GetSequence(pos_AS_TupleConstructorExpr_fields));
  
      SEQ<TYPE_AS_Expr> newfields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        newfields.ImpAppend(QuotePMExprWithTemp(fields[idx], clnm, lb));
      }
      TYPE_AS_TupleConstructorExpr tce (pat);
      tce.SetField(pos_AS_TupleConstructorExpr_fields, newfields);
      return tce;
    }
    case TAG_TYPE_AS_TokenConstructorExpr: {
      TYPE_AS_TokenConstructorExpr tce (pat);
      const TYPE_AS_Expr & field (pat.GetRecord(pos_AS_TokenConstructorExpr_field));
      tce.SetField(pos_AS_TokenConstructorExpr_field, QuotePMExprWithTemp(field, clnm, lb));
      return tce;
    }
    case TAG_TYPE_AS_RecordConstructorExpr: {
      const SEQ<TYPE_AS_Expr> & fields (pat.GetSequence(pos_AS_RecordConstructorExpr_fields));
      SEQ<TYPE_AS_Expr> newfields;
      size_t len_fields = fields.Length();
      for (size_t idx = 1; idx <= len_fields; idx++) {
        newfields.ImpAppend(QuotePMExprWithTemp(fields[idx], clnm, lb));
      }
      TYPE_AS_RecordConstructorExpr rce (pat);
      rce.SetField(pos_AS_RecordConstructorExpr_tag,
                   QuotePMExprWithTemp(pat.GetRecord(pos_AS_RecordConstructorExpr_tag), clnm, Set()));
      rce.SetField(pos_AS_RecordConstructorExpr_fields, newfields);
      return rce;
    }
    case TAG_TYPE_AS_RecordModifierExpr: {
      const TYPE_AS_Expr & rec (pat.GetRecord(pos_AS_RecordModifierExpr_rec));
      const SEQ<TYPE_AS_RecordModification> & modifiers (pat.GetSequence(pos_AS_RecordModifierExpr_modifiers));
  
  
      SEQ<TYPE_AS_RecordModification> newmodifiers;
      size_t len_modifiers = modifiers.Length();
      for (size_t idx = 1; idx <= len_modifiers; idx++) {
        const TYPE_AS_RecordModification & rm (modifiers[idx]);
        const TYPE_AS_Expr & nexpr (rm.GetRecord(pos_AS_RecordModification_newexpr));
        TYPE_AS_RecordModification newrm (rm);
        newrm.SetField(pos_AS_RecordModification_newexpr, QuotePMExprWithTemp(nexpr, clnm, lb));
        newmodifiers.ImpAppend(newrm);
      }
      TYPE_AS_RecordModifierExpr rme (pat);
      rme.SetField(pos_AS_RecordModifierExpr_rec, QuotePMExprWithTemp(rec, clnm, lb));
      rme.SetField(pos_AS_RecordModifierExpr_modifiers, newmodifiers);
      return rme;
    }
    case TAG_TYPE_AS_FieldSelectExpr : {
      TYPE_AS_FieldSelectExpr fse (pat);
      fse.SetField(pos_AS_FieldSelectExpr_rec,
                   QuotePMExprWithTemp(pat.GetRecord(pos_AS_FieldSelectExpr_rec), clnm, lb));
      return fse;
    }
    case TAG_TYPE_AS_FctTypeInstExpr : {
      TYPE_AS_FctTypeInstExpr ftie (pat);
      ftie.SetField(pos_AS_FctTypeInstExpr_polyfct,
                    QuotePMExprWithTemp(pat.GetRecord(pos_AS_FctTypeInstExpr_polyfct), clnm, lb));
      return ftie;
    }
    case TAG_TYPE_AS_LambdaExpr : {
      TYPE_AS_LambdaExpr le (pat);
      le.SetField(pos_AS_LambdaExpr_body,
                  QuotePMExprWithTemp(pat.GetRecord(pos_AS_LambdaExpr_body), clnm, lb));
      return le;
    }
    case TAG_TYPE_AS_ApplyExpr : {
      const SEQ<TYPE_AS_Expr> & arg (pat.GetSequence(pos_AS_ApplyExpr_arg));
      SEQ<TYPE_AS_Expr> new_arg;
      size_t len_arg = arg.Length();
      for (size_t idx = 1; idx <= len_arg; idx++) {
        new_arg.ImpAppend(QuotePMExprWithTemp(arg[idx], clnm, lb));
      }
      TYPE_AS_ApplyExpr ae (pat);
      ae.SetField(pos_AS_ApplyExpr_fct, QuotePMExprWithTemp(pat.GetRecord(pos_AS_ApplyExpr_fct), clnm, lb));
      ae.SetField(pos_AS_ApplyExpr_arg, new_arg);
      return ae;
    }
    case TAG_TYPE_AS_IsExpr: {
      const Generic & type (pat.GetField(pos_AS_IsExpr_type));
      const TYPE_AS_Expr & arg (pat.GetRecord(pos_AS_IsExpr_arg));
  
      TYPE_AS_IsExpr ie (pat);
      if (type.Is(TAG_TYPE_AS_Name)) {
        ie.SetField(pos_AS_IsExpr_type, QuotePMExprWithTemp(type, clnm, lb));
      }
      ie.SetField(pos_AS_IsExpr_arg, QuotePMExprWithTemp(arg, clnm, lb));
      return ie;
    }
    case TAG_TYPE_AS_UndefinedExpr: return pat;
#ifdef VDMPP
    case TAG_TYPE_AS_SelfExpr:
    case TAG_TYPE_AS_NewExpr:  return pat;

    case TAG_TYPE_AS_IsOfClassExpr: {
      TYPE_AS_IsOfClassExpr ioce (pat);
      ioce.SetField(pos_AS_IsOfClassExpr_arg,
                    QuotePMExprWithTemp(pat.GetRecord(pos_AS_IsOfClassExpr_arg), clnm, lb));
      return ioce;
    }
    case TAG_TYPE_AS_IsOfBaseClassExpr: {
      TYPE_AS_IsOfBaseClassExpr iobce (pat);
      iobce.SetField(pos_AS_IsOfBaseClassExpr_arg,
                     QuotePMExprWithTemp(pat.GetRecord(pos_AS_IsOfBaseClassExpr_arg), clnm, lb));
      return iobce;
    }
    case TAG_TYPE_AS_SameBaseClassExpr: {
      TYPE_AS_SameBaseClassExpr sbce (pat);
      sbce.SetField(pos_AS_SameBaseClassExpr_expr1,
                    QuotePMExprWithTemp(pat.GetRecord(pos_AS_SameBaseClassExpr_expr1), clnm, lb));
      sbce.SetField(pos_AS_SameBaseClassExpr_expr2,
                    QuotePMExprWithTemp(pat.GetRecord(pos_AS_SameBaseClassExpr_expr2), clnm, lb));
      return sbce;
    }
    case TAG_TYPE_AS_SameClassExpr: {
      TYPE_AS_SameClassExpr sce(pat);
      sce.SetField(pos_AS_SameClassExpr_expr1,
                   QuotePMExprWithTemp(pat.GetRecord(pos_AS_SameClassExpr_expr1), clnm, lb));
      sce.SetField(pos_AS_SameClassExpr_expr2,
                   QuotePMExprWithTemp(pat.GetRecord(pos_AS_SameClassExpr_expr2), clnm, lb));
      return sce;
    }
    case TAG_TYPE_AS_ActExpr :
    case TAG_TYPE_AS_FinExpr :
    case TAG_TYPE_AS_ActiveExpr :
    case TAG_TYPE_AS_WaitingExpr :
    case TAG_TYPE_AS_ReqExpr : {
      return pat;
    }
#ifdef VICE
    case TAG_TYPE_AS_CurTimeExpr : {
      return pat;
    }
#endif // VICE
#endif // VDMPP
    case TAG_TYPE_AS_BoolLit :
    case TAG_TYPE_AS_NilLit :
    case TAG_TYPE_AS_RealLit :
    case TAG_TYPE_AS_NumLit :
    case TAG_TYPE_AS_CharLit :
    case TAG_TYPE_AS_TextLit :
    case TAG_TYPE_AS_QuoteLit : {
      return pat;
    }
    case TAG_TYPE_AS_Name : {
      TYPE_AS_Name nm (pat);
      if (nm.get_ids().Length() == 2 || lb.InSet(nm)) {
        return pat;
      }
      else {
        return ASTAUX::Combine2Names(clnm, nm);
      }
    }
    default : {
      return pat;
    }
  }
}

// StringNumber
// i : nat
// -> seq of char
TYPE_AS_Id vdmcg::StringNumber(const Int & i)
{
  return ASTAUX::MkId(i.ascii());
}

// FindAllNamesInExpr
// e : AS`Expr
// -> set of AS`Name
SET<TYPE_AS_Name> vdmcg::FindAllNamesInExpr(const TYPE_AS_Expr & e)
{
  switch (e.GetTag()) {
    case TAG_TYPE_AS_BracketedExpr: {
      const TYPE_AS_Expr & e1 (e.GetRecord(pos_AS_BracketedExpr_expr));
      return FindAllNamesInExpr(e1);
    }
    case TAG_TYPE_AS_PrefixExpr: {
      const TYPE_AS_Expr & expr (e.GetRecord(pos_AS_PrefixExpr_arg));
      return FindAllNamesInExpr(expr);
    }
    case TAG_TYPE_AS_BinaryExpr: {
      const TYPE_AS_Expr & e1 (e.GetRecord(pos_AS_BinaryExpr_left));
      const TYPE_AS_Expr & e2 (e.GetRecord(pos_AS_BinaryExpr_right));
      SET<TYPE_AS_Name> res (FindAllNamesInExpr(e1));
      res.ImpUnion(FindAllNamesInExpr(e2));
      return res;
    }
    case TAG_TYPE_AS_TupleConstructorExpr: {
      const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_TupleConstructorExpr_fields));
      SET<TYPE_AS_Name> res;
      size_t len_e_l = e_l.Length();
      for (size_t i = 1; i <= len_e_l; i++)
        res.ImpUnion(FindAllNamesInExpr(e_l[i]));
      return res;
    }
    case TAG_TYPE_AS_RecordConstructorExpr: {
      const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_RecordConstructorExpr_fields));
      SET<TYPE_AS_Name> res;
      size_t len_e_l = e_l.Length();
      for (size_t i = 1; i <= len_e_l; i++)
        res.ImpUnion(FindAllNamesInExpr(e_l[i]));
      return res;
    }
    case TAG_TYPE_AS_SeqEnumerationExpr: {
      const SEQ<TYPE_AS_Expr> & els_l (e.GetSequence(pos_AS_SeqEnumerationExpr_els));
      SET<TYPE_AS_Name> res;
      size_t len_els_l = els_l.Length();
      for (size_t i = 1; i <= len_els_l; i++)
        res.ImpUnion(FindAllNamesInExpr(els_l[i]));
      return res;
    }
    case TAG_TYPE_AS_SetEnumerationExpr: {
      const SEQ<TYPE_AS_Expr> & els_l (e.GetSequence(pos_AS_SetEnumerationExpr_els));
      SET<TYPE_AS_Name> res;
      size_t len_els_l = els_l.Length();
      for (size_t i = 1; i <= len_els_l; i++)
        res.ImpUnion(FindAllNamesInExpr(els_l[i]));
      return res;
    }
    case TAG_TYPE_AS_ApplyExpr: {
      const TYPE_AS_Expr & f (e.GetRecord(pos_AS_ApplyExpr_fct));
      const SEQ<TYPE_AS_Expr> & e_l (e.GetSequence(pos_AS_ApplyExpr_arg));
      SET<TYPE_AS_Name> res(FindAllNamesInExpr(f));
      size_t len_e_l = e_l.Length();
      for (size_t i = 1; i <= len_e_l; i++)
        res.ImpUnion(FindAllNamesInExpr(e_l[i]));
      return res;
    }
    case TAG_TYPE_AS_FieldSelectExpr: {
      const TYPE_AS_Expr & rec (e.GetRecord(pos_AS_FieldSelectExpr_rec));
      const Record & nm (e.GetRecord(pos_AS_FieldSelectExpr_nm));
      SET<TYPE_AS_Name> res (FindAllNamesInExpr(rec));
      res.ImpUnion(FindAllNamesInExpr(nm));
      return res;
    }
    case TAG_TYPE_AS_Name: {
      SET<TYPE_AS_Name> res;
      res.Insert(e);
      return res;
    }
    case TAG_TYPE_AS_OldName: {
      TYPE_AS_OldName onm (e);
      SET<TYPE_AS_Name> res;
      res.Insert(TYPE_AS_Name().Init(onm.get_ids(), onm.get_cid()));
      return res;
    }
#ifdef VDMPP
    case TAG_TYPE_AS_IsOfClassExpr: {
      const TYPE_AS_Name & nm (e.GetRecord(pos_AS_IsOfClassExpr_cls));
      const TYPE_AS_Expr & expr (e.GetRecord(pos_AS_IsOfClassExpr_arg));
      SET<TYPE_AS_Name> res (FindAllNamesInExpr(nm));
      res.ImpUnion(FindAllNamesInExpr(expr));
      return res;
    }
    case TAG_TYPE_AS_IsOfBaseClassExpr: {
      const TYPE_AS_Name & nm (e.GetRecord(pos_AS_IsOfBaseClassExpr_cls));
      const TYPE_AS_Expr & expr (e.GetRecord(pos_AS_IsOfBaseClassExpr_arg));
      SET<TYPE_AS_Name> res (FindAllNamesInExpr(nm));
      res.ImpUnion(FindAllNamesInExpr(expr));
      return res;
    }
    case TAG_TYPE_AS_SameBaseClassExpr: {
      const TYPE_AS_Expr & e1 (e.GetRecord(pos_AS_SameBaseClassExpr_expr1));
      const TYPE_AS_Expr & e2 (e.GetRecord(pos_AS_SameBaseClassExpr_expr2));
      SET<TYPE_AS_Name> res (FindAllNamesInExpr(e1));
      res.ImpUnion(FindAllNamesInExpr(e2));
      return res;
    }
    case TAG_TYPE_AS_SameClassExpr: {
      const TYPE_AS_Expr & e1 (e.GetRecord(pos_AS_SameClassExpr_expr1));
      const TYPE_AS_Expr & e2 (e.GetRecord(pos_AS_SameClassExpr_expr2));
      SET<TYPE_AS_Name> res (FindAllNamesInExpr(e1));
      res.ImpUnion(FindAllNamesInExpr(e2));
      return res;
    }
#endif //VDMPP
    default: {
      return SET<TYPE_AS_Name>();
    }
  }
}

// FindAllNamesInPatternBind
// pb : AS`PatternBind
// -> set of AS`Name
SET<TYPE_AS_Name> vdmcg::FindAllNamesInPatternBind(const TYPE_AS_PatternBind & pb)
{
  switch (pb.GetTag()) {
    case TAG_TYPE_AS_SetBind: {
      const TYPE_AS_Pattern & pat (pb.GetRecord(pos_AS_SetBind_pat));
      const TYPE_AS_Expr & expr (pb.GetRecord(pos_AS_SetBind_Set));
      SET<TYPE_AS_Name> res (FindPatternId(pat).Dom());
      res.ImpUnion(FindAllNamesInExpr(expr));
      return res;
    }
    case TAG_TYPE_AS_TypeBind: {
      const TYPE_AS_Pattern & pat (pb.GetRecord(pos_AS_TypeBind_pat));
      return FindPatternId(pat).Dom();
    }
    default: {
      return FindPatternId(pb).Dom();
    }
  }
}

// InitState_CGAUX (InitState)
// nm : [AS`Name]
// ==> ()
void vdmcg::InitState_CGAUX(const Generic & nm)
{
  this->quotes_s.Clear();
  this->import_s.Clear();
  if (nm.IsNil()) {
    this->abstractMethods       = Map();
    this->nonAbstractMethods    = Map();
    this->allNonAbstractMethods = Map();
    this->superClasses          = Map();
    this->orderedSupers         = Map();
    this->instanceVariables     = Map();
    this->classVisitOrder       = SEQ<TYPE_AS_Name>();
    this->methodNames           = Map();
    this->unhiddenNames         = Map();
    this->packageNames          = Map();
#ifdef VDMPP
    InitJavaPrimitiveType();
#endif // VDMPP
  } else {
    this->cc                         = nm;
    this->auxfct                     = false;
    this->isabstract                 = false;
    this->c_s                        = SET<TYPE_AS_Name>();
    this->nametab                    = Sequence().ImpAppend(Map());
    this->globalenv                  = Map();
    this->val                        = false;
    this->throwException             = false;
    this->throwNotSupported          = false;
    this->var_in_expr                = SET<TYPE_AS_Name>();
    this->isConstr                   = false;
    this->isAtomic                   = false;
    this->hasInv                     = false;
    this->alwaysLevel                = 0;
    this->classHasDefaultConstructor = false;
    this->usesSuper                  = false;
    this->isStatic                   = false;
    this->usesThis                   = false;
    this->cause                      = Sequence().ImpAppend(Nil()); // 20121217 
  }
}

// InitEnv_CGAUX (InitEnv)
// ==> ()
void vdmcg::InitEnv_CGAUX()
{
  this->nametab = Sequence().ImpAppend(Map());
}

// CurrentEnv_CGAUX (CurrentEnv)
// ==> BlkEnv
Map vdmcg::CurrentEnv_CGAUX()
{
  return this->nametab.Hd();
}

// PushEnv_CGAUX (PushEnv)
// ==> ()
void vdmcg::PushEnv_CGAUX()
{
  this->nametab.ImpPrepend(Map());
}

// PopEnv_CGAUX (PopEnv)
// ==> ()
void vdmcg::PopEnv_CGAUX()
{
  this->nametab.ImpTl();
}

// PushThisEnv_CGAUX (PushThisEnv)
// env : BlkEnv
// ==> bool
bool vdmcg::PushThisEnv_CGAUX(const Map& env)
{
  this->nametab.ImpPrepend(env);
  return true;
}

// PopLastEnv_CGAUX (PopLastEnv)
// ==> bool
bool vdmcg::PopLastEnv_CGAUX()
{
  this->nametab.ImpTl();
  return true;
}

// InsertNamesinEnv_CGAUX (InsertNamesinEnv)
// nms : set of (AS`Name | AS`OldName)
// ==> bool
bool vdmcg::InsertNamesinEnv_CGAUX(const Set & nms)
{
  Set nms_q(nms);
  Generic elm;
  for (bool bb = nms_q.First(elm); bb; bb = nms_q.Next(elm)) {
    InsertName_CGAUX(elm);
  }
  return true;
}

// InsertNamesinEnv2_CGAUX (InsertNamesinEnv2)
// nms : set of (AS`Name | AS`OldName)
// ==> ()
void vdmcg::InsertNamesinEnv2_CGAUX(const Set & nms)
{
  SET<TYPE_AS_Name> nms_q(nms);
  Generic elm;
  for (bool bb = nms_q.First(elm); bb; bb = nms_q.Next(elm)) {
    InsertName_CGAUX(elm);
  }
}

// InsertName_CGAUX (InsertName)
// nm : (AS`Name | AS`OldName)
// ==> (AS`Name | AS`OldName)
TYPE_AS_Name vdmcg::InsertName_CGAUX(const Record & nm)
{
  Map blk (this->nametab.Hd());

  Tuple fn (FindName(nm)); // bool * int * nat
  bool exist (fn.GetBoolValue(1));
  int level (fn.GetIntValue(2));
  int lastnum (fn.GetIntValue(3));

  if (exist) {
    if (level != 0) {
      blk.Insert(nm, mk_(nm, Int(lastnum + 1)));
    }
  }
  else {
    blk.Insert(nm, mk_(nm, Int(0)));
  }
  this->nametab.ImpTl();
  this->nametab.ImpPrepend(blk);
  return nm;
}

// InsertGlobalName
// nm : AS`Name
// ==> AS`Name
TYPE_AS_Name vdmcg::InsertGlobalName(const TYPE_AS_Name & nm)
{
  this->globalenv.Insert(nm, mk_(nm, Int(0))); // map AS`Name to (AS`Name * nat)
  return nm;
}

// FindName
// nm : (AS`Name | AS`OldName)
// ==> bool * int * nat
Tuple vdmcg::FindName(const Record & nm)
{
  int level = 0;
  size_t len_nametab = this->nametab.Length();
  for (size_t idx = 1; idx <= len_nametab; idx++) {
    Map blkenv (this->nametab[idx]);
    if (blkenv.DomExists(nm)) {      // map AS`Name to (AS`Name * nat)
      Tuple blk (blkenv[nm]);        // (AS`Name * nat)
//      const TYPE_AS_Name & name (blk.GetRecord(1));
      const Int & num (blk.GetInt(2));
      return mk_(Bool(true), Int(level), num);
    }
    else {
      level = level + 1;
    }
  }
  if (this->globalenv.DomExists(nm)) {     // map AS`Name to (AS`Name * nat)
    Tuple glb (this->globalenv[nm]);       // (AS`Name * nat)
//    const TYPE_AS_Name & name (glb.GetRecord(1));   
    const Int & num (glb.GetInt(2));
    return mk_(Bool(true), Int(-1), num);
  }
  else {
    return mk_(Bool(false), Int(-1), Int(0));
  }
}

// FindScope
// nm : (AS`Name | AS`OldName)
// ==> int
Int vdmcg::FindScope(const Record & nm)
{
  size_t len_nametab = this->nametab.Length();
  for (size_t idx = 1; idx <= len_nametab; idx++) {
    Map blkenv (this->nametab[idx]);        // map AS`Name to (AS`Name * nat)
    if (blkenv.DomExists(nm)) {
      Tuple blk (blkenv[nm]);   // (AS`Name * nat)
      return (blk.GetInt(2)); 
    }
  }
  if (this->globalenv.DomExists(nm)) {  // map AS`Name to (AS`Name * nat)
    Tuple glb (this->globalenv[nm]);   // (AS`Name * nat)
    return (glb.GetInt(2));
  }
  else {
    return Int(-2);
  }
}

#ifdef VDMPP
// IncludeClass
// nm : AS`Name
// ==> ()
void vdmcg::IncludeClass(const TYPE_AS_Name& nm)
{
  this->c_s.Insert(nm);
}

// GenClassIncls
// ==> CPP`Preprocessors
SEQ<TYPE_CPP_Preprocessor> vdmcg::GenClassIncls()
{
  Set nm_s (this->c_s);
  if (nm_s.InSet (this->cc)) {
    nm_s.RemElem(this->cc);
  }
  SEQ<TYPE_CPP_Preprocessor> pp;
  Generic nm;
  for (bool bb = nm_s.First(nm); bb; bb = nm_s.Next(nm)) {
#ifdef _MSC_VER
    // TODO: "MATH" must be renamed for Windows
    TYPE_AS_Id clnm (GiveLastName(nm));
    TYPE_AS_Id id ((clnm == ASTAUX::MkId(L"MATH")) ? ASTAUX::MkId(L"vdm_MATH.h") : clnm.ImpConc(ASTAUX::MkId(L".h")));
#else
    TYPE_AS_Id id (GiveLastName(nm).ImpConc(ASTAUX::MkId(L".h")));
#endif // _MSC_VER
    pp.ImpAppend( vdm_BC_GenInclusion( id ) );
  }
  return pp;
}

// StoreClasses
// nm_s : set of AS`Name
// ==> ()
void vdmcg::StoreClasses(const SET<TYPE_AS_Name> & nm_s)
{
  this->c_s.ImpUnion(nm_s);
}
#endif // VDMPP

// GiveCurCName
// ==> AS`Id
TYPE_AS_Id vdmcg::GiveCurCName()
{
  return this->GiveLastName(this->cc);
}

// GiveCurCASName
// ==> AS`Name
TYPE_AS_Name vdmcg::GiveCurCASName()
{
  return this->cc;
}

// GiveCurCName
// ==> AS`Id * nat
Tuple vdmcg::GiveCurMName()
{
  this->sc++;
  return mk_(GiveLastName(this->cm), Int(this->sc));
}

// GiveCurMASName
// ==> AS`Name
TYPE_AS_Name vdmcg::GiveCurMASName()
{
  return this->cm;
}

// CurrentMethod
// nm : AS`Name
// ==> ()
void vdmcg::CurrentMethod(const TYPE_AS_Name& nm)
{
  this->cm = nm;
  this->sc = 0;
}

SEQ<Char> vdmcg::GiveOrgnFileName() {
  if (this->cc != TYPE_AS_Name())
  {
    Tuple flcp (GetCI().GetFileLineColPos(this->cc.GetInt(pos_AS_Name_cid)));
    if (!flcp.GetField(1).IsNil())
      return flcp.GetSequence(1);
  }
  return SEQ<Char>();
}

#ifdef VDMPP
// UseAuxFct
// ==> ()
void vdmcg::UseAuxFct()
{
  this->auxfct = true;
}

// IsAbstract
// ==> bool
bool vdmcg::IsAbstract ()
{
  return this->isabstract || HasAbstractMethods() ;
}

// ThrowsException
// ==> bool
bool vdmcg::ThrowsException () const
{
  return this->throwException;
}

// ThrowsNotSupportedException
// ==> bool
bool vdmcg::ThrowsNotSupportedException () const
{
  return this->throwNotSupported;
}

// VariablesinValDef
// ==> set of AS`Name
SET<TYPE_AS_Name> vdmcg::VariablesinValDef () const
{
  return this->var_in_expr;
}

// UsedQuotes
// ==> set of seq of char
SET<TYPE_AS_Id> vdmcg::UsedQuotes () const
{
  return this->quotes_s;
}

// IsSuperClass
// p_base : AS`Name
// p_super : AS`Name
// ==> bool
bool vdmcg::IsSuperClass(const TYPE_AS_Name & p_base, const TYPE_AS_Name & p_super) const
{
  return Set(this->superClasses[p_base]).InSet(p_super);
}

// DefineClassAsAbstract
// ==> ()
void vdmcg::DefineClassAsAbstract ()
{
  this->isabstract = true;
}

// SetException
// b : bool
// ==> ()
void vdmcg::SetException (bool b)
{
  this->throwException = b;
  if (!b) {
    SetNotSupportedException(false);
  }
}

// SetNotSupportedException
// b : bool
void vdmcg::SetNotSupportedException (bool b)
{
  this->throwNotSupported = b;
}

// InitVariables
// ==> ()
void vdmcg::InitVariables ()
{
  this->var_in_expr = SET<TYPE_AS_Name>();
}

// InitQuotes
// ==> ()
void vdmcg::InitQuotes ()
{
  this->quotes_s = SET<TYPE_AS_Id>();
}

// InsertVariable
// nm : AS`Name
// ==> ()
void vdmcg::InsertVariable (const TYPE_AS_Name & nm)
{
  this->var_in_expr.Insert(nm);
}

// InsertQuote
// ch_l : seq of char
// ==> seq of char
void vdmcg::InsertQuote (const TYPE_AS_Id & ch_l)
{
  this->quotes_s.Insert(ch_l);
}

// InsertImport
// ch_l : seq of char
// ==> ()
void vdmcg::InsertImport(const SEQ<Char> & ch_l)
{
  this->import_s.Insert(ch_l);
}

// InsertJDKImport
// ch_l : seq of char
// ==> ()
void vdmcg::InsertJDKImport(const wstring & ch_l)
{
  TYPE_AS_Name jdknm (ASTAUX::MkNameFromId(ASTAUX::MkId(L"JDK_" + ch_l), NilContextId)); 
  if (packageNames.DomExists(jdknm)) {
    SEQ<Char> pnm (packageNames[jdknm]);
    InsertImport(pnm.ImpConc(ASTAUX::MkId(L"." + ch_l)));
    return;
  }
  bool exists = false;
  int index = 0;
  while (!java_util[index].empty() && !exists) {
    exists = (java_util[index] == ch_l);
    index++;
  }
  if (exists) {
    InsertImport(SEQ<Char>(wstring(L"java.util.") + ch_l));
    return;
  }
  index = 0;
  while (!java_sql[index].empty() && !exists) {
    exists = (java_sql[index] == ch_l);
    index++;
  }
  if (exists) {
    InsertImport(SEQ<Char>(wstring(L"java.sql.") + ch_l));
    return;
  }
  index = 0;
  while (!java_io[index].empty() && !exists) {
    exists = (java_io[index] == ch_l);
    index++;
  }
  if (exists) {
    InsertImport(SEQ<Char>(wstring(L"java.io.") + ch_l));
    return;
  }
  index = 0;
  while (!java_net[index].empty() && !exists) {
    exists = (java_net[index] == ch_l);
    index++;
  }
  if (exists) {
    InsertImport(SEQ<Char>(wstring(L"java.net.") + ch_l));
    return;
  }
  index = 0;
  while (!java_security[index].empty() && !exists) {
    exists = (java_security[index] == ch_l);
    index++;
  }
  if (exists) {
    InsertImport(SEQ<Char>(wstring(L"java.security.") + ch_l));
    return;
  }
  index = 0;
  while (!java_security_cert[index].empty() && !exists) {
    exists = (java_security_cert[index] == ch_l);
    index++;
  }
  if (exists) {
    InsertImport(SEQ<Char>(wstring(L"java.security.cert.") + ch_l));
    return;
  }
  index = 0;
  while (!java_math[index].empty() && !exists) {
    exists = (java_math[index] == ch_l);
    index++;
  }
  if (exists) {
    InsertImport(SEQ<Char>(wstring(L"java.math.") + ch_l));
    return;
  }
  index = 0;
  while (!java_nio[index].empty() && !exists) {
    exists = (java_nio[index] == ch_l);
    index++;
  }
  if (exists) {
    InsertImport(SEQ<Char>(wstring(L"java.nio.") + ch_l));
    return;
  }
}

// GetValState
// ==> bool
bool vdmcg::GetValState() const
{
  return this->val;
}

// SetValState
// ==> ()
void vdmcg::SetValState()
{
  this->val = true;
}

// UnsetValState
// ==> ()
void vdmcg::UnsetValState()
{
  this->val = false;
}
#endif // VDMPP

// CurrentRType
// tp : REP`TypeRep
// ==> ()
void vdmcg::CurrentRType(const TYPE_REP_TypeRep & tp)
{
  this->r_tp = tp;
}

// GiveCurrentRType
// REP`TypeRep
TYPE_REP_TypeRep vdmcg::GiveCurrentRType() const
{
  return this->r_tp;
}

// UpdateSortAuxFct
// - : SortFct
// ==> ()
void vdmcg::UpdateSortAuxFct(const Record &)
{
  this->auxfct = true;
}

// UpdatePermAuxFct
// - : set of ( CPP`Name * REP`SeqTypeRep )
// ==> ()
void vdmcg::UpdatePermAuxFct(const Set &)
{
  this->auxfct = true;
}

// GiveFlatUnionType
// type_s : set of REP`TypeRep
// ==> set of REP`TypeRep
SET<TYPE_REP_TypeRep> vdmcg::GiveFlatUnionType(const SET<TYPE_REP_TypeRep>& type_s)
{
  SET<TYPE_REP_TypeRep> type_s_q (type_s);
  SET<TYPE_REP_TypeRep> res_s;
  Generic tp;
  for (bool bb = type_s_q.First(tp); bb; bb = type_s_q.Next(tp))
    res_s.Insert(CleanFlatType(tp));
  return res_s;
}

// FindSeqElemType
// ti : REP`TypeRep
// ==> [REP`TypeRep]
Generic vdmcg::FindSeqElemType(const TYPE_REP_TypeRep& ti_)
{
  TYPE_REP_TypeRep ti (RemoveInvType(ti_));
  switch (ti.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: {
      return ti;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ti_s (ti.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> tti_s;
      Generic g;
      for (bool bb = ti_s.First(g); bb; bb = ti_s.Next(g)) {
        TYPE_REP_TypeRep ttp (CleanFlatType(g));
        if (IsSeqType(ttp))
          //tti_s.Insert(ttp);
          tti_s.Insert(FindSeqElemType(ttp));
      }
      //return CombineSeqTypeRep(tti_s).GetField(pos_REP_SeqTypeRep_elemtp);
      switch (tti_s.Card()) {
        case 0: { 
          vdm_err << L"Type must be sequense." << endl;;
          ReportError(L"FindSeqElemType");
          return Nil(); // To avoid warnings
        }
        case 1: {
          return tti_s.GetElem();
        }
        default: { 
          return mk_REP_UnionTypeRep(tti_s);
        }
      }
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return CleanFlatType(ti.GetField(pos_REP_SeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      //return Nil();
      return CleanFlatType(ti.GetField(pos_REP_EmptySeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return FindSeqElemType(ti.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
//      vdm_err << L"Type must be sequence." << endl;;
//      ReportError(L"FindSeqElemType");
//      return Nil(); // To avoid warnings
      return mk_REP_AllTypeRep();
    }
  }
}

// FindSetElemType
// ti : REP`TypeRep
// ==> [REP`TypeRep]
Generic vdmcg::FindSetElemType(const TYPE_REP_TypeRep & ti_)
{
  TYPE_REP_TypeRep ti (RemoveInvType(ti_));
  switch (ti.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: {
      return ti;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ti_s (ti.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> tti_s;
      Generic g;
      for (bool bb = ti_s.First(g); bb; bb = ti_s.Next(g)) {
        TYPE_REP_TypeRep tp (CleanFlatType(g));
        if (IsSetType(tp))
          //tti_s.Insert(tp);
          tti_s.Insert(FindSetElemType(tp));
      }
      //return CombineSetTypeRep(tti_s).GetField(pos_REP_SetTypeRep_elemtp);
      switch (tti_s.Card()) {
        case 0: { 
//          vdm_err << L"Type must be set." << endl;
//          ReportError(L"FindSetqElemType");
//          return Nil(); // To avoid warnings
          return mk_REP_AllTypeRep();
        }
        case 1: {
          return tti_s.GetElem();
        }
        default: { 
          return mk_REP_UnionTypeRep(tti_s);
        }
      }
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return CleanFlatType(ti.GetRecord(pos_REP_SetTypeRep_elemtp));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      //return Nil();
      return CleanFlatType(ti.GetRecord(pos_REP_EmptySetTypeRep_elemtp));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return FindSetElemType(ti.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
//      vdm_err << L"Type must be set." << endl;;
//      ReportError(L"FindSetElemType");
//      return Nil(); // To avoid warnings
      return mk_REP_AllTypeRep();
    }
  }
}

// FindMapDomType
// ti : REP`TypeRep
// ==> [REP`TypeRep]
Generic vdmcg::FindMapDomType(const TYPE_REP_TypeRep & ti_)
{
  TYPE_REP_TypeRep ti (RemoveInvType(ti_));
  switch (ti.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: {
      return ti;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ti_s (ti.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> tti_s;
      Generic g;
      for (bool bb = ti_s.First(g); bb; bb = ti_s.Next(g)) {
        TYPE_REP_TypeRep tp (CleanFlatType(g));
        if (IsMapType(tp))
          tti_s.Insert(FindMapDomType(tp));
      }
      switch (tti_s.Card()) {
        case 0: { 
//          vdm_err << L"Type must be map." << endl;;
//          ReportError(L"FindMapDomType");
//          return Nil(); // To avoid warnings
          return mk_REP_AllTypeRep();
        }
        case 1: {
          return tti_s.GetElem();
        }
        default: { 
          return mk_REP_UnionTypeRep(tti_s);
        }
      }
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return CleanFlatType(ti.GetRecord(pos_REP_GeneralMapTypeRep_mapdom));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return CleanFlatType(ti.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return CleanFlatType(ti.GetRecord(pos_REP_EmptyMapTypeRep_mapdom));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return FindMapDomType(ti.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
//      vdm_err << L"Type must be map." << endl;;
//      ReportError(L"FindMapDomType");
//      return Nil(); // To avoid warnings
      return mk_REP_AllTypeRep();
    }
  }
}

// FindMapRngType
// ti : REP`TypeRep
// ==> [REP`TypeRep]
Generic vdmcg::FindMapRngType(const TYPE_REP_TypeRep & ti_)
{
  TYPE_REP_TypeRep ti (RemoveInvType(ti_));
  switch (ti.GetTag()) {
    case TAG_TYPE_REP_AllTypeRep: {
      return ti;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ti_s (ti.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> tti_s;
      Generic g;
      for (bool bb = ti_s.First(g); bb; bb = ti_s.Next(g)) {
        TYPE_REP_TypeRep tp (CleanFlatType(g));
        if (IsMapType(tp))
          tti_s.Insert(FindMapRngType(tp));
      }
      switch (tti_s.Card()) {
        case 0: { 
//          vdm_err << L"Type must be map." << endl;;
//          ReportError(L"FindMapDomType");
//          return Nil(); // To avoid warnings
          return mk_REP_AllTypeRep();
        }
        case 1: {
          return tti_s.GetElem();
        }
        default: { 
          return mk_REP_UnionTypeRep(tti_s);
        }
      }
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return CleanFlatType(ti.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return CleanFlatType(ti.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return CleanFlatType(ti.GetRecord(pos_REP_EmptyMapTypeRep_maprng));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return FindMapDomType(ti.GetRecord(pos_REP_InvTypeRep_shape));
    }
    default: {
//      vdm_err << L"Type must be map." << endl;;
//      ReportError(L"FindMapRngType");
//      return Nil(); // To avoid warnings
      return mk_REP_AllTypeRep();
    }
  }
}

// FindProductElemType
// ti : REP`TypeRep
// ==> [seq of REP`TypeRep]
Generic vdmcg::FindProductElemType(const TYPE_REP_TypeRep & ti_, size_t len)
{
  TYPE_REP_TypeRep ti (RemoveInvType(ti_));
  switch (ti.GetTag()) {
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ti_s (ti.GetSet(pos_REP_UnionTypeRep_tps));
      Sequence tti_l; // seq of seq of REP`TypeRep
      Generic g;
      for (bool bb = ti_s.First(g); bb; bb = ti_s.Next(g)) {
        TYPE_REP_TypeRep tp (CleanFlatType(g));
        if (IsProductType(tp))
        {
          Generic gtpl(FindProductElemType(tp, len));
          if (!gtpl.IsNil())
            tti_l.ImpAppend(gtpl);
        }
      }
      switch (tti_l.Length()) {
        case 0: { 
          vdm_err << L"Type must be product type." << endl;;
          ReportError(L"FindMapDomType");
          return Nil(); // To avoid warnings
        }
        case 1: {
          return tti_l[1];
        }
        default: { 
          Sequence tsl; // seq of set of REP`TypeRep
          for (size_t i = 1; i <= len; i++)
          {
            SET<TYPE_REP_TypeRep> tp_s;
            size_t len_tti_l = tti_l.Length();
            for (size_t idx = 1; idx <= len_tti_l; idx++)
            {
              const SEQ<TYPE_REP_TypeRep> & tp_l (tti_l[idx]);
              tp_s.Insert(tp_l[i]);
            }
            tsl.ImpAppend(tp_s);
          }
          SEQ<TYPE_REP_TypeRep> tp_l;
          for (size_t j = 1; j <= len; j++)
          {
            SET<TYPE_REP_TypeRep> t_s (tsl[j]);
            switch (t_s.Card()) {
              case 0:  { tp_l.ImpAppend(mk_REP_AllTypeRep()); break; } // must not occur
              case 1:  { tp_l.ImpAppend(t_s.GetElem()); break;}
              default: { tp_l.ImpAppend(mk_REP_UnionTypeRep(t_s)); break; }
            }
          }
          return tp_l;
        }
      }
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      SEQ<TYPE_REP_TypeRep> tp_l (ti.GetSequence(pos_REP_ProductTypeRep_tps));
      if ((size_t)tp_l.Length() == len)
        return tp_l;
      else
        return Nil();
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return FindProductElemType(ti.GetRecord(pos_REP_InvTypeRep_shape), len);
    }
// 20140711 -->
    case TAG_TYPE_REP_AllTypeRep: {
      SEQ<TYPE_REP_TypeRep> tp_l;
      for (size_t i = 1; i <= len; i++) {
        tp_l.ImpAppend(mk_REP_AllTypeRep());
      }
      return tp_l;
    }
// <-- 20140711
    default: {
      //vdm_err << L"Type must be product type." << endl;;
      //ReportError(L"FindProductElemType");
      return Nil(); // To avoid warnings
    }
  }
}

// posFieldApplyTypes
// type : REP`TypeRep
// sel : AS`Name
// ==> REP`TypeRep
TYPE_REP_TypeRep vdmcg::posFieldApplyTypes(const TYPE_REP_TypeRep& type, const TYPE_AS_Name& sel)
{
  TYPE_REP_TypeRep tp (CleanFlatType(RemoveInvType(type)));
  switch(tp.GetTag()) {
    case TAG_TYPE_REP_CompositeTypeRep: {
      SEQ<TYPE_REP_FieldRep> fr_l (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      Generic fr_g;
      for (bool bb = fr_l.First(fr_g); bb; bb = fr_l.Next(fr_g))
      {
        TYPE_REP_FieldRep fr (fr_g);
        Generic sel1 (fr.get_sel());
        if (sel1 == sel)
          return CleanFlatType(fr.get_tp());
      }
      ReportError(L"posFieldApplyTypes");
      break;
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tpf_s (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> tp_rec;
      Generic tp_g;
      for (bool bb = tpf_s.First(tp_g); bb; bb = tpf_s.Next(tp_g))
      {
        TYPE_REP_TypeRep ttp (tp_g);
        if (ttp.Is(TAG_TYPE_REP_CompositeTypeRep))
        {
          SEQ<TYPE_REP_FieldRep> fr_l (ttp.GetSequence(pos_REP_CompositeTypeRep_fields));
          Generic fr_g;
          for (bool dd = fr_l.First(fr_g); dd; dd = fr_l.Next(fr_g))
          {
            TYPE_REP_FieldRep fr (fr_g);
            Generic sel1 (fr.get_sel());
            if (sel1 == sel) {
              tp_rec.Insert(fr.get_tp());
            }
          }
        }
      }
      if (tp_rec.Card() == 1 ) {
        Generic tp1 (tp_rec.GetElem());
        return CleanFlatType(tp1);
      } else {
        return CleanFlatType(mk_REP_UnionTypeRep(tp_rec));
      }
      break;
    }
#ifdef VDMPP
    case TAG_TYPE_REP_ObjRefTypeRep: {
      TYPE_AS_Name nm (tp.GetRecord(pos_REP_ObjRefTypeRep_nm));
      return GetInstanceVarTp(nm, sel);
      break;
    }
#endif //VDMPP
    default:
      break;
  }
  return Record(0,0); //to avoid warning.
}

// posRngMapOrIndsSeqType
// type : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep vdmcg::posRngMapOrIndsSeqType(const TYPE_REP_TypeRep& type)
{
  TYPE_REP_TypeRep tp (CleanFlatType(RemoveInvType(type)));
  switch(tp.GetTag()) {
    case TAG_TYPE_REP_GeneralMapTypeRep:
      return CleanFlatType(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
    case TAG_TYPE_REP_InjectiveMapTypeRep:
      return CleanFlatType(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
    case TAG_TYPE_REP_SeqTypeRep:
      return CleanFlatType(tp.GetRecord(pos_REP_SeqTypeRep_elemtp));

    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> pos_tps;
      Generic tp_g;
      for (bool bb = tps.First(tp_g); bb; bb = tps.Next(tp_g)) {
        if ((IsMapType(tp_g) || IsSeqType(tp_g)) &&
            (!tp_g.Is(TAG_TYPE_REP_EmptySeqTypeRep) &&
             !tp_g.Is(TAG_TYPE_REP_EmptyMapTypeRep) ))
          pos_tps.Insert(posRngMapOrIndsSeqType(tp_g));
      }

      if (pos_tps.Card() == 1)
        return pos_tps.GetElem();
      else
        return CleanFlatType(mk_REP_UnionTypeRep(pos_tps));
    }
    default:
      ReportError(L"posRngmapOrIndsSeqType");
  }
  return TYPE_REP_TypeRep(); // To avoid warnings
}

// LOThelp (not in spec)
// tp : REP`TypeRep
// dones_ : set of REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep vdmcg::LOThelp (const TYPE_REP_TypeRep& tp, const SET<TYPE_REP_TypeRep> & dones_)
{
  if (!tp.Is(TAG_TYPE_REP_TypeNameRep))
    return tp;
  else if (dones_.InSet (tp))
  {
    GetStatSem().GenErr(tp.GetRecord(pos_REP_TypeNameRep_nm), StatSem::ERR, 358, Sequence());
    return TYPE_REP_AllTypeRep();
  }
  else
  {
    SET<TYPE_REP_TypeRep> dones (dones_);
    dones.Insert (tp);
    TYPE_AS_Name tn (tp.GetRecord(pos_REP_TypeNameRep_nm));

#ifdef VDMSL
    Generic g (GetStatSem().PublicLookUpTypeName (tn, GiveCurCASName()));
#endif // VDMSL
#ifdef VDMPP
    Tuple t (GetStatSem().PublicLookUpTypeName (tn, GiveCurCASName()));
    Generic g (t.GetField(1));

// 20100519 -->
    if (!g.IsNil() && !t.GetField(2).IsNil())
    {
      if (Record(g).Is(TAG_TYPE_REP_CompositeTypeRep))
      {
        TYPE_AS_Name cls (t.GetField(2));
        TYPE_REP_CompositeTypeRep ctr (g);
        TYPE_AS_Name tag (ctr.GetRecord(pos_REP_CompositeTypeRep_nm));
        if (tag.GetSequence(pos_AS_Name_ids).Length() == 1)
        {
           TYPE_AS_Name new_tag (ASTAUX::Combine2Names(cls, tag));
           ctr.SetField(pos_REP_CompositeTypeRep_nm, new_tag);
           g = ctr;
        }
      }
    }
// <-- 20100519
#endif // VDMPP

    if (g.IsNil ())
    {
      GetStatSem().GenErr(tn, StatSem::WRN2, 359, mk_sequence(SEQ<Char>(ASTAUX::ASName2String(tn))));
      return TYPE_REP_AllTypeRep();
    }
    return LOThelp (g, dones);
  }
//  return TYPE_REP_TypeRep(0,0); // To avoid warnings
}

// LOT
// tp_q : REP`TypeRep
// ==> REP`TypeRep
TYPE_REP_TypeRep vdmcg::LOT (const Generic & tp_q)
{
  if (!tp_q.Is(TAG_TYPE_REP_TypeNameRep))
    return tp_q;

  TYPE_REP_TypeRep t (LOThelp (tp_q, Set ()));

#ifdef VDMSL
  // for renamed composite type
  TYPE_AS_Name nm (((TYPE_REP_TypeNameRep)tp_q).get_nm());
  TYPE_AS_Name qnm (GenQName(nm));
  Map known_types (get_known_types());
  if(known_types.DomExists(qnm))
  {
    Generic tp (known_types[qnm]);
// 20120912 -->
//    if(!tp.IsNil() && tp.Is(TAG_TYPE_AS_CompositeType) &&
//       (qnm == Record(tp).GetRecord(pos_AS_CompositeType_name)))
    if(tp.Is(TAG_TYPE_AS_CompositeType))
// <-- 20120912
    {
      TYPE_REP_TypeRep tr (FromAS2RepType(tp));

      if (t.Is(TAG_TYPE_REP_InvTypeRep))
      { 
        TYPE_REP_InvTypeRep itr (t);
        itr.SetField(pos_REP_InvTypeRep_shape, tr);
        return itr;
      }
      else

        return tr;
    }
  }
#endif // VDMSL

  return t;
}

// LOTset
// tps : set of REP`TypeRep
// ==> set of REP`TypeRep
SET<TYPE_REP_TypeRep> vdmcg::LOTset (const SET<TYPE_REP_TypeRep> & tps)
{
  SET<TYPE_REP_TypeRep> tps_q (tps);
  SET<TYPE_REP_TypeRep> res;
  Generic tp;
  for (bool bb = tps_q.First(tp); bb; bb = tps_q.Next(tp))
    res.Insert (LOT (tp));
  return res;
}

// SetUsesSuper
// ==> ()
void vdmcg::SetUsesSuper()
{
  this->usesSuper = true;
}

// ResetUsesSuper
// ==> ()
void vdmcg::ResetUsesSuper()
{
  this->usesSuper = false;
}

// UsesSuper
// ==> bool
bool vdmcg::UsesSuper() const
{
  return this->usesSuper;
}

// SetUsesThis
// ==> ()
void vdmcg::SetUsesThis()
{
  this->usesThis = true;
}

// ResetUsesThis
// ==> ()
void vdmcg::ResetUsesThis()
{
  this->usesThis = false;
}

// UsesThis
// ==> bool
bool vdmcg::UsesThis() const
{
  return this->usesThis;
}

// SetIsStatic
// ==> ()
void vdmcg::SetIsStatic()
{
  this->isStatic = true;
}

// ResetIsStatic
// ==> ()
void vdmcg::ResetIsStatic()
{
  this->isStatic = false;
}

// IsStatic
// ==> bool
bool vdmcg::IsStatic() const
{
  return this->isStatic;
}

// SetAtomic
// ==> ()
void vdmcg::SetAtomic()
{
  this->isAtomic = true;
}

// UnsetAtomic
// ==> ()
void vdmcg::UnsetAtomic()
{
  this->isAtomic = false;
}

// GetAtomic
// ==> bool
bool vdmcg::GetAtomic() const
{
  return this->isAtomic;
}

void vdmcg::UpAlwaysLevel()
{
  this->alwaysLevel++;
}

void vdmcg::DownAlwaysLevel()
{
  this->alwaysLevel--;
}

bool vdmcg::IsInAlways() const
{
  return this->alwaysLevel > 0;
}

void vdmcg::PushCause(const Generic & c)
{
  this->cause.ImpPrepend(c);
}

void vdmcg::PopCause()
{
  this->cause.ImpTl();
}

Generic vdmcg::GetCause() const
{
  return this->cause.Hd();
}

#ifdef VDMPP
// AddSuperClasses
// supers : seq of AS`Name
// ==> ()
void vdmcg::AddSuperClasses(const SEQ<TYPE_AS_Name> & supers)
{
  this->superClasses.ImpModify(cc, supers.Elems());
}

// AddAbstractMethods
// cl : AS`Class
// ==> ()
void vdmcg::AddAbstractMethods(const TYPE_AS_Class &cl)
{
  if (!cl.get_defs().IsNil())
  {
    const TYPE_AS_Definitions & defs (cl.GetRecord(pos_AS_Class_defs));
    const MAP<TYPE_AS_Name, TYPE_AS_FnDef> & fctDefs (defs.GetMap(pos_AS_Definitions_fnm));
    const MAP<TYPE_AS_Name, TYPE_AS_OpDef> & opDefs (defs.GetMap(pos_AS_Definitions_opm));

    SET<TYPE_AS_Ids> absFctDefs, nonAbsFctDefs;
    Set dom_fctDefs (fctDefs.Dom());
    Generic g;
    for (bool bb = dom_fctDefs.First(g); bb; bb = dom_fctDefs.Next(g)) {
      TYPE_AS_Name fnm (g);
// 20120717 -->
//      if (IsAbstractFunction(fctDefs[fnm]))
//        absFctDefs.Insert(fnm.get_ids());
//      else
//        nonAbsFctDefs.Insert(fnm.get_ids());
      TYPE_AS_FnDef fd (fctDefs[fnm]);
      TYPE_AS_Name mangledNm (MANGLE::IsMangled(fnm) ? fnm : MANGLE::Mangle(fnm, GetFnParms(fd)));
      if (IsAbstractFunction(fctDefs[fnm]))
        absFctDefs.Insert(mangledNm.get_ids());
      else
        nonAbsFctDefs.Insert(mangledNm.get_ids());
// <-- 20120717
    }

    Set dom_opDefs (opDefs.Dom());
    SET<TYPE_AS_Ids> absOpDefs, nonAbsOpDefs;
    for (bool dd = dom_opDefs.First(g); dd; dd = dom_opDefs.Next(g)) {
      TYPE_AS_Name opnm (g);
// 20120717 -->
//      if (IsAbstractOperation(opDefs[opnm]))
//        absOpDefs.Insert(opnm.get_ids());
//      else
//        nonAbsOpDefs.Insert(opnm.get_ids());
      TYPE_AS_OpDef od (opDefs[opnm]);
      TYPE_AS_Name mangledNm (MANGLE::IsMangled(opnm) ? opnm : MANGLE::Mangle(opnm, GetOpParms(od)));
      if (IsAbstractOperation(od))
        absOpDefs.Insert(mangledNm.get_ids());
      else
        nonAbsOpDefs.Insert(mangledNm.get_ids());
// <-- 20120717
    }

    SET<TYPE_AS_Ids> thisAbstractMethods (absFctDefs), thisNonAbstractMethods (nonAbsFctDefs);
    thisAbstractMethods.ImpUnion(absOpDefs);
    thisNonAbstractMethods.ImpUnion(nonAbsOpDefs);

    TYPE_AS_Name nm (cl.get_nm());
    this->abstractMethods.Insert(nm, thisAbstractMethods);
    this->nonAbstractMethods.Insert(nm, thisNonAbstractMethods);

    Set methNames;
    for (bool ee = dom_fctDefs.First(g); ee; ee = dom_fctDefs.Next(g))
    {
      TYPE_AS_FnDef fd (fctDefs[g]);
      TYPE_AS_Access access (ASTAUX::GetFnAccess(fd));
      if ( access == Int(PUBLIC_AS) || access == Int(PROTECTED_AS))
      {
        TYPE_AS_Name l_nm (g);
        TYPE_AS_Name mangledNm (MANGLE::IsMangled(l_nm) ? l_nm : MANGLE::Mangle(l_nm, GetFnParms(fd)));
        methNames.Insert(mk_(QualiName(cl.get_nm(), mangledNm), access, GetFnRestype(fd)));
      }
    }

    for (bool ff = dom_opDefs.First(g); ff; ff = dom_opDefs.Next(g))
    {
      TYPE_AS_OpDef od (opDefs[g]);
      TYPE_AS_Access access (ASTAUX::GetOpAccess(od));
      if ( access == Int(PUBLIC_AS) || access == Int(PROTECTED_AS))
      {
        TYPE_AS_Name l_nm (g);
        TYPE_AS_Name mangledNm (MANGLE::IsMangled(l_nm) ? l_nm : MANGLE::Mangle(l_nm, GetOpParms(od)));
        methNames.Insert(mk_(QualiName(cl.get_nm(), mangledNm), access, GetOpRestype(od)));
      }
    }
    this->methodNames.Insert(cl.get_nm(), methNames);
  }
}

// IsAbstractFunction
// fndef : AS`FnDef
// -> bool
bool vdmcg::IsAbstractFunction(const TYPE_AS_FnDef & fndef)
{
  switch(fndef.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:
    case TAG_TYPE_AS_ExtExplFnDef:
      return ((Int) SUBRESP == ASTAUX::GetFnBody(fndef).get_body());
    case TAG_TYPE_AS_ImplFnDef:
    default:
      return false;
  }
}

// IsAbstractOperation
// opdef : AS`OpDef
// -> bool
bool vdmcg::IsAbstractOperation(const TYPE_AS_OpDef & opdef)
{
  switch(opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:
    case TAG_TYPE_AS_ExtExplOpDef:
      return ((Int) SUBRESP == ASTAUX::GetOpBody(opdef).get_body());
    case TAG_TYPE_AS_ImplOpDef:
    default:
      return false;
  }
}

// IsNotYetSpecifiedOperation
// opdef : AS`OpDef
// -> bool
bool vdmcg::IsNotYetSpecifiedOperation(const TYPE_AS_OpDef& opdef)
{
  switch(opdef.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef:
    case TAG_TYPE_AS_ExtExplOpDef:
      return ((Int) NOTYETSPEC == ASTAUX::GetOpBody(opdef).get_body());
    case TAG_TYPE_AS_ImplOpDef:
    default:
      return false;
  }
}

// ProcessAbstractMethods
// ==> ()
void vdmcg::ProcessAbstractMethods()
{
  size_t len_classVisitOrder = this->classVisitOrder.Length();
  for(size_t idx = 1; idx <= len_classVisitOrder; idx++)
  {
    const TYPE_AS_Name & cl (this->classVisitOrder[idx]);

    Set inheritedAbstractMethods, inheritedNonAbstractMethods;

    SET<TYPE_AS_Name> thisSuperClasses (this->superClasses[cl]);

    Generic superCl;
    for(bool dd = thisSuperClasses.First(superCl); dd; dd = thisSuperClasses.Next(superCl))
    {
      inheritedAbstractMethods.ImpUnion(this->abstractMethods[superCl]);
      inheritedNonAbstractMethods.ImpUnion(this->allNonAbstractMethods[superCl]);
    }
    Set thisAllNonAbstractMethods (this->nonAbstractMethods[cl]);
    thisAllNonAbstractMethods.ImpUnion(inheritedNonAbstractMethods);
    this->allNonAbstractMethods.ImpModify(cl, thisAllNonAbstractMethods);

    Set thisAbstractMethods(this->abstractMethods[cl]);
    thisAbstractMethods.ImpUnion(inheritedAbstractMethods);
    thisAbstractMethods.ImpDiff(this->allNonAbstractMethods[cl]);
    this->abstractMethods.ImpModify(cl, thisAbstractMethods);
  }
}

// GenerateOrderedSupers
// ==> ()
void vdmcg::GenerateOrderedSupers()
{
  SET<TYPE_AS_Name> visited;
  SET<TYPE_AS_Name> to_visit (this->superClasses.Dom());

  while(!to_visit.IsEmpty()) {
    bool exists = false;
    TYPE_AS_Name cl;
    Generic g;
    for (bool bb = to_visit.First(g); bb && !exists; bb = to_visit.Next(g))
    {
      exists = Set(this->superClasses[g]).SubSet(visited);
      if (exists)
        cl = g;
    }
    if (exists)
    {
      SEQ<TYPE_AS_Name> seqSupers (Set(this->superClasses[cl]).ToSequence()); // [] or [x]
      SEQ<TYPE_AS_Name> orderedSupers_cl (seqSupers);

      Generic sups;
      for (bool dd = seqSupers.First(sups); dd; dd = seqSupers.Next(sups))
      {
        if (this->orderedSupers.DomExists(sups))
          orderedSupers_cl.ImpConc(this->orderedSupers[sups]);
      }

      this->orderedSupers.ImpModify(cl, orderedSupers_cl); //map AS`Name to seq of AS`Name
      visited.Insert(cl);
      to_visit.RemElem(cl);
      this->classVisitOrder.ImpAppend(cl);
    }
    else
    {
      SET<TYPE_AS_Name> s;
      Generic cl_g;
      for (bool dd = to_visit.First(cl_g); dd; dd = to_visit.Next(cl_g))
      {
        s.ImpUnion(this->superClasses[cl_g]);
        if (!this->orderedSupers.DomExists(cl_g))
//          this->orderedSupers.ImpModify(cl_g, UTIL::set2seq(this->superClasses[cl_g]));
          this->orderedSupers.ImpModify(cl_g, Set(this->superClasses[cl_g]).ToSequence());
      }
      s.ImpDiff(visited);
      for (bool ee = s.First(cl_g); ee; ee = s.Next(cl_g))
      {
        vdm_err << L"Class \'" + ASTAUX::ASName2String(cl_g) + L"\' is't defined." << endl;;
//        ReportError (L"GenerateOrderedSupers");
        if (!this->orderedSupers.DomExists(cl_g))
          this->orderedSupers.ImpModify(cl_g, Sequence());
      }
      to_visit.Clear();
    }
  }
}

// ProcessMethodNames
// ==> ()
void vdmcg::ProcessMethodNames()
{
  size_t len_classVisitOrder = this->classVisitOrder.Length();
  for (size_t idx = 1; idx <= len_classVisitOrder; idx++)
  {
    const TYPE_AS_Name & b_cl (this->classVisitOrder[idx]);
    Set l_theseNames (this->methodNames[b_cl]);
                 // set of (AS`Name * AS`Access * [AS`Type])

    Set l_superNames; // set of (AS`Name * AS`Access * [AS`Type])
    SET<TYPE_AS_Name> theseSuperClasses (this->superClasses[b_cl]);

    Generic b_sup;
    for (bool dd = theseSuperClasses.First(b_sup); dd; dd = theseSuperClasses.Next(b_sup))
      l_superNames.ImpUnion(this->unhiddenNames[b_sup]);

    Set l_theseUnmangled, l_theseUnquali;
    Generic b_nmG;
    for (bool ee = l_theseNames.First(b_nmG); ee;
              ee = l_theseNames.Next(b_nmG))
    {
      Tuple b_nm (b_nmG);  // (AS`Name * AS`Access * [AS`Type])
      l_theseUnmangled.Insert(UnqualiName(MANGLE::GetUnmangledName(b_nm.GetField(1))));

      l_theseUnquali.Insert(mk_(UnqualiName(b_nm.GetField(1)), b_nm.GetField(3)));
    }

    Set l_hidden;           // set of (AS`Name * AS`Access * [AS`Type])
    for (bool ff = l_superNames.First(b_nmG); ff; ff = l_superNames.Next(b_nmG))
    {
      Tuple b_nm (b_nmG);   // (AS`Name * AS`Access * [AS`Type])
      if (l_theseUnmangled.InSet(UnqualiName(MANGLE::GetUnmangledName(b_nm.GetField(1)))))
        l_hidden.Insert(b_nm);
    }

    Set l_hiddenWithoutReps; // set of (AS`Name * AS`Access * [AS`Type])
    for (bool gg = l_hidden.First(b_nmG); gg; gg = l_hidden.Next(b_nmG))
    {
      Tuple b_nm (b_nmG);   // (AS`Name * AS`Access * [AS`Type])
      Tuple t (mk_(UnqualiName(b_nm.GetField(1)), b_nm.GetField(3)));
      if (!l_theseUnquali.InSet(t))
        l_hiddenWithoutReps.Insert(b_nm);
    }

    this->unhiddenNames.Insert(b_cl, l_theseNames.ImpUnion(l_hiddenWithoutReps));
  }
}

// GetUnhiddenNames
// p_cl : AS`Name
// ==> set of (AS`Name * AS`Access * [AS`Type])
Set vdmcg::GetUnhiddenNames(const TYPE_AS_Name& p_cl)
{
  return this->unhiddenNames[p_cl];
}

// IsInClassNamespace
// p_cl : AS`Name
// p_methNm : AS`Name
// ==> bool
bool vdmcg::IsInClassNamespace(const TYPE_AS_Name& p_cl, const TYPE_AS_Name& p_methNm) const
{
//  Set l_meths;
  Set l_unhiddenNms (this->unhiddenNames[p_cl]);
  Generic g;
  for (bool bb = l_unhiddenNms.First(g); bb; bb = l_unhiddenNms.Next(g))
  {
    TYPE_AS_Name l_nm (Tuple(g).GetField(1));
    TYPE_AS_Name l_unmangled (l_nm);
    if (MANGLE::IsMangled(l_nm))
      l_unmangled = MANGLE::GetUnmangledName(l_nm);
    if (l_unmangled == p_methNm)
      return true;
  }

  Set l_otherNms (this->abstractMethods[p_cl]);
  l_otherNms.ImpUnion(this->nonAbstractMethods[p_cl]);
  for (bool dd = l_otherNms.First(g); dd; dd = l_otherNms.Next(g))
  {
    TYPE_AS_Ids ids (g);
    TYPE_AS_Name l_nm (ASTAUX::MkNameFromIds(ids, NilContextId));
// 20120717 -->
//    if (p_methNm == l_nm)
//      return true;
    TYPE_AS_Name l_unmangled (l_nm);
    if (MANGLE::IsMangled(l_nm))
      l_unmangled = MANGLE::GetUnmangledName(l_nm);
    if (l_unmangled == p_methNm)
      return true;
// <-- 20120717
  }

  return false;
}

// HasAbstractMethods
// ==> bool
bool vdmcg::HasAbstractMethods() {
  //  if(!this->abstractMethods.Dom().InSet(cc)) return false;
  return (!Set(this->abstractMethods[cc]).IsEmpty());
}

// IsAbstractClass
// nm : AS`Name
// ==> bool
bool vdmcg::IsAbstractClass(const TYPE_AS_Name & nm)
{
  //  if(!this->abstractMethods.Dom().InSet(nm)) return false;
  return (!Set(this->abstractMethods[nm]).IsEmpty());
}

// SetConstr
// ==> ()
void vdmcg::SetConstr()
{
  this->isConstr = true;
}

// UnsetConstr
// ==> ()
void vdmcg::UnsetConstr()
{
  this->isConstr = false;
}

// GetConstr
// ==> bool
bool vdmcg::GetConstr() const
{
  return this->isConstr;
}

// SetHasInv
// ==> ()
void vdmcg::SetHasInv()
{
  this->hasInv = true;
}

// UnsetHasInv
// ==> ()
void vdmcg::UnsetHasInv()
{
  this->hasInv = false;
}

// GetHasInv
// ==> bool
bool vdmcg::GetHasInv() const
{
  return this->hasInv;
}

// SetHasDefaultConstr
// ==> ()
void vdmcg::SetHasDefaultConstr()
{
  this->classHasDefaultConstructor = true;
}

// GetHasDefaultConstr
// ==> bool
bool vdmcg::GetHasDefaultConstr() const
{
  return this->classHasDefaultConstructor;
}

// IsClass
// pName : AS`Name
// ==> bool
bool vdmcg::IsClass(const TYPE_AS_Name& pName)
{
  return this->classVisitOrder.Elems().InSet(pName);
}

// GetAllClasses
// ==> set of AS`Name
SET<TYPE_AS_Name> vdmcg::GetAllClasses()
{
  return this->classVisitOrder.Elems();
}
#endif //VDMPP

#ifdef VDMPP
// AddInstanceVars
// nm : AS`Name
// instvars : seq of AS`InstanceVarDef
// ==> ()
void vdmcg::AddInstanceVars(const TYPE_AS_Name & nm, const SEQ<TYPE_AS_InstanceVarDef> & instvars)
{
  MAP<TYPE_AS_Name, TYPE_REP_TypeRep> thisClass;
  size_t len_instvars = instvars.Length();
  for (size_t idx = 1; idx <= len_instvars; idx++)
  {
    const TYPE_AS_InstanceVarDef & iv (instvars[idx]);
    switch(iv.GetTag()) {
      case TAG_TYPE_AS_InstAssignDef: {
        const TYPE_AS_AssignDef & ad (iv.GetRecord(pos_AS_InstAssignDef_ad));
        const TYPE_AS_Name & varNm (ad.GetRecord(pos_AS_AssignDef_var));
        const TYPE_AS_Type & tp (ad.GetRecord(pos_AS_AssignDef_tp));
        TYPE_REP_TypeRep repType (FromAS2RepType(tp));

        // not in spec -->
        if (repType.Is(TAG_TYPE_REP_CompositeTypeRep)) {
//          TYPE_REP_CompositeTypeRep ctr (repType);
//          if (ctr.get_nm().get_ids().Length() == 1) {
//            ctr.set_nm(UTIL::QualiName(nm, ctr.get_nm()));
//            repType = ctr;
//          }
          const TYPE_AS_Name & tag (repType.GetRecord(pos_REP_CompositeTypeRep_nm));
          if (tag.GetSequence(pos_AS_Name_ids).Length() == 1)
          {
            repType.SetField(pos_REP_CompositeTypeRep_nm, QualiName(nm, tag));
          }
        }
        // <--

        thisClass.ImpModify(varNm, repType);
        break;
      }
      default: {
        break;
      }
    }
  }
  this->instanceVariables.ImpModify(nm, thisClass);
}

// GetInstanceVarTp
// className : AS`Name
// varName : AS`Name
// ==> [REP`TypeRep]
Generic vdmcg::GetInstanceVarTp(const TYPE_AS_Name & className,
                                const TYPE_AS_Name & varName)
{
  SEQ<TYPE_AS_Name> searchClasses;
  searchClasses.ImpAppend(className);
  searchClasses.ImpConc(this->orderedSupers[className]);

  size_t len_searchClasses = searchClasses.Length();
  for (size_t idx = 1; idx <= len_searchClasses; idx++)
  {
    const TYPE_AS_Name & cl (searchClasses[idx]);
    Map m (this->instanceVariables[cl]);
    if (m.DomExists(varName))
      return m[varName];
  }
  return Nil();
}

// GetOrderedSupers
// clnm : AS`Name
// ==> seq of AS`Name
SEQ<TYPE_AS_Name> vdmcg::GetOrderedSupers(const TYPE_AS_Name & clnm)
{
  return this->orderedSupers[clnm];
}
#endif //VDMPP
///
/// Remark this function differs from the specification:
/// The extension of the cc file is dependent of the platform
/// and the environment variable VDMCGEXT.
/// Default extension for Unix is : .cc
///                   for NT      : .cpp

// GenFileExt
// fnm : CPP`FileName
// -> CPP`FileName
TYPE_AS_Id vdmcg::GenFileExt(const TYPE_AS_Id & fnm)
{
  TYPE_AS_Id filename (fnm);
  return filename.ImpConc(ASTAUX::MkId(TBUTILS::GetCGExt()));
}

#ifdef VDMPP
// GenJavaQuoteFiles
// ==> set of CPP`File
SET<TYPE_CPP_File> vdmcg::GenJavaQuoteFiles()
{
  SET<TYPE_CPP_File> files;
  TYPE_CPP_PackageAndImportDeclarations decl (
    vdm_BC_GenPackageAndImportDeclarations(
      vdm_BC_GenPackageDeclaration(vdm_BC_GenSimplePackageName(ASTAUX::MkId(L"quotes"))),
      SEQ<TYPE_CPP_ImportDeclaration>()));

  SET<TYPE_AS_Id> quotes (UsedQuotes());
  Generic g;
  for (bool bb = quotes.First(g); bb; bb = quotes.Next(g))
  {
    TYPE_AS_Id quotename (g);
    type_dL pub_ml;
    pub_ml.ImpAppend(GenHashMember());
    pub_ml.ImpAppend(GenQuoteConstructor(quotename));
    pub_ml.ImpAppend(GenQuoteHashcodeMethod(quotename));
    pub_ml.ImpAppend(GenQuoteEqualsMethod(quotename));
    pub_ml.ImpAppend(GenQuoteToStringMethod(quotename));

    SEQ<TYPE_CPP_Annotation> annos;
    SEQ<TYPE_CPP_Modifier> mods;
    mods.ImpAppend(vdm_BC_GenModifier(quote_PUBLIC));
    TYPE_AS_Id quoteSeq (quotename);
    TYPE_CPP_ClassHead ch (vdm_BC_GenJavaClassHead(annos, mods, vdm_BC_GenIdentifier(quoteSeq),
                                                   type_dL(), Sequence(), Nil()));
    TYPE_CPP_IdentDeclaration cl (vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
                                    type_dL().ImpAppend(vdm_BC_GenTypeClassSpecifier(ch, pub_ml)),
                                    Nil()));

    TYPE_AS_Ids package (PackageToDir(GenPackageName(TYPE_AS_Ids().ImpAppend(ASTAUX::MkId(L"quotes")))));
    files.Insert(
      vdm_BC_GenFile(package,
                     quoteSeq.ImpConc(ASTAUX::MkId(L".java")),
                     decl,
                     TYPE_CPP_CPPAS().ImpAppend(cl)));
  }
  return files;
}

// GenTypeImportDeclarations
// ==> seq of CPP`SingleTypeImportDeclaration
SEQ<TYPE_CPP_SingleTypeImportDeclaration> vdmcg::GenTypeImportDeclarations()
{
  SEQ<TYPE_CPP_SingleTypeImportDeclaration> decls;
  Generic e;
  for (bool bb = this->import_s.First(e); bb; bb = this->import_s.Next(e))
  {
    decls.ImpAppend(vdm_BC_GenSingleTypeImportDeclaration(vdm_BC_GenIdentifier(e)));
  }
  return decls;
}

// GenHashMember
// -> CPP`MemberDeclaration
TYPE_CPP_MemberDeclaration vdmcg::GenHashMember()
{
  SEQ<TYPE_CPP_DeclSpecifier> mods;
  mods.ImpAppend(vdm_BC_GenModifier(quote_STATIC));
  mods.ImpAppend(vdm_BC_GenModifier(quote_PRIVATE));
  mods.ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int"))));

  return vdm_BC_GenIdentDeclaration(SEQ<TYPE_CPP_Annotation>(),
           mods,
           SEQ<TYPE_CPP_InitDeclarator>().ImpAppend(
             vdm_BC_GenInitDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"hc")),
                                vdm_BC_GenAsgnInit(vdm_BC_GenIntegerLit(0)))));
}

// GenQuoteHashcodeMethod
// ch_l : seq of char
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenQuoteHashcodeMethod(const TYPE_AS_Id & ch_l)
{
  SEQ<TYPE_CPP_Stmt> stmtl;
  stmtl.ImpAppend(vdm_BC_GenReturnStmt(vdm_BC_GenIdentifier(ASTAUX::MkId(L"hc"))));

  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ASTAUX::MkId(L"hashCode")), type_dL()));

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
           SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
           type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"int")))),
           decl,
           Nil(),
           vdm_BC_GenBlock(stmtl));
}

// GenQuoteConstructor
// ch_l : seq of char
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenQuoteConstructor(const TYPE_AS_Id & ch_l)
{
  TYPE_CPP_Expr iftest (vdm_BC_GenEq(vdm_BC_GenIdentifier(ASTAUX::MkId(L"hc")), vdm_BC_GenIntegerLit(0)));

  TYPE_CPP_Stmt returnstmt (vdm_BC_GenAsgnStmt(
                              vdm_BC_GenIdentifier(ASTAUX::MkId(L"hc")),
                              vdm_BC_GenFctCallObjMemAcc(GenSuper(), ASTAUX::MkId(L"hashCode"), SEQ<TYPE_CPP_Expr>())));

  TYPE_CPP_Stmt ifstmt (vdm_BC_GenIfStmt(iftest, returnstmt, Nil()));

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(),
           SEQ<TYPE_CPP_Modifier>().ImpAppend(vdm_BC_GenModifier(quote_PUBLIC)),
           type_dL(),
           vdm_BC_GenFctDecl(vdm_BC_GenIdentifier(ch_l), SEQ<TYPE_CPP_ArgumentDeclaration>()),
           Nil(),
           vdm_BC_GenBlock(SEQ<TYPE_CPP_Stmt>().ImpAppend(ifstmt)));
}

// GenQuoteEqualsMethod
// ch_l : seq of char
// ==> CPP`FunctionDefinition
TYPE_CPP_FunctionDefinition vdmcg::GenQuoteEqualsMethod(const TYPE_AS_Id & ch_l)
{

  TYPE_CPP_Identifier lhs (vdm_BC_GenIdentifier(ASTAUX::MkId(L"obj")));
  TYPE_CPP_TypeSpecifier rhs (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier( ch_l)));
  SEQ<TYPE_CPP_Stmt> stmtl;
  stmtl.ImpAppend(vdm_BC_GenReturnStmt(vdm_BC_GenTypeComp(rhs,lhs)));

  TYPE_CPP_ArgDecl arg (vdm_BC_GenArgDecl(
                          type_dL().ImpAppend(vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"Object")))),
                          vdm_BC_GenIdentifier(ASTAUX::MkId(L"obj"))));

  TYPE_CPP_FctDecl decl (vdm_BC_GenFctDecl(
                           vdm_BC_GenIdentifier(ASTAUX::MkId(L"equals")),
                           SEQ<TYPE_CPP_ArgumentDeclaration>().ImpAppend(arg)));

  SEQ<TYPE_CPP_Modifier> modifiers;
  modifiers.ImpAppend(vdm_BC_GenModifier(quote_PUBLIC));

  TYPE_CPP_TypeSpecifier obj (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"boolean"))));
  SEQ<TYPE_CPP_DeclSpecifier> obj_l;
  obj_l.ImpAppend(obj);

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(), modifiers, obj_l, decl, Nil(), vdm_BC_GenBlock(stmtl));
}

// GenQuoteToStringMethod
// ch_l : seq of char
// ==> CPP`FunctionDefinition
//TYPE_CPP_FunctionDefinition vdmcg::GenQuoteToStringMethod(const Token & ch_l)
TYPE_CPP_FunctionDefinition vdmcg::GenQuoteToStringMethod(const TYPE_AS_Id & ch_l)
{
  SEQ<TYPE_CPP_Stmt> stmtl;
  stmtl.ImpAppend(vdm_BC_GenReturnStmt(vdm_BC_GenStringLit(ASTAUX::MkId(L"<").ImpConc(ch_l).ImpConc(ASTAUX::MkId(L">")))));

  TYPE_CPP_FctDecl decl(vdm_BC_GenFctDecl(
                          vdm_BC_GenIdentifier(ASTAUX::MkId(L"toString")),
                          SEQ<TYPE_CPP_ArgumentDeclaration>()));

  SEQ<TYPE_CPP_Modifier> modifiers;
  modifiers.ImpAppend(vdm_BC_GenModifier(quote_PUBLIC));

  TYPE_CPP_TypeSpecifier obj (vdm_BC_GenTypeSpecifier(vdm_BC_GenIdentifier(ASTAUX::MkId(L"String"))));

  SEQ<TYPE_CPP_DeclSpecifier> obj_l;
  obj_l.ImpAppend(obj);

  return vdm_BC_GenJavaFctDef(SEQ<TYPE_CPP_Annotation>(), modifiers, obj_l, decl, Nil(), vdm_BC_GenBlock(stmtl));
}

// GenPackageName
// dirs : seq1 of seq of char
// -> CPP`PackageName
TYPE_CPP_PackageName vdmcg::GenPackageName(const TYPE_AS_Ids & dirs)
{
  if (dirs.Length() == 1)
    return vdm_BC_GenSimplePackageName(dirs.Hd());
  else
  {
    TYPE_AS_Id lastDir (dirs[dirs.Length()]);
    TYPE_AS_Ids frontDirs;
    size_t len_dirs = dirs.Length();
    for (size_t i = 1; i < len_dirs; i++)
      frontDirs.ImpAppend(dirs[i]);

    return vdm_BC_GenQualifiedPackageName(GenPackageName(frontDirs), lastDir);
  }
}

// PackageToDir
// package : [CPP`PackageName]
// -> [seq of seq1 of char]
Generic vdmcg::PackageToDir(const Generic & gpackage)
{
  if (gpackage.IsNil())
    return Nil();

  TYPE_CPP_PackageName package (gpackage);
  switch(package.GetTag()) {
    case TAG_TYPE_CPP_SimplePackageName: {
      TYPE_CPP_SimplePackageName spn (package);
      TYPE_AS_Ids ids;
      ids.ImpAppend(spn.get_id().get_id());
      return ids;
    }
    case TAG_TYPE_CPP_QualifiedPackageName: {
      TYPE_CPP_QualifiedPackageName qpn (package);
      TYPE_AS_Ids dirs (PackageToDir(qpn.get_pn()));
      dirs.ImpAppend(qpn.get_id().get_id());
      return dirs;
    }
    default:
      return Nil();
  }
}
#endif // VDMPP

// RemoveInvType
// tp : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::RemoveInvType(const TYPE_REP_TypeRep & tp)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_InvTypeRep: {
      return RemoveInvType(tp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (tp.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> new_tps;
      Generic t;
      for (bool bb = tps.First(t); bb; bb = tps.Next(t)) {
        new_tps.Insert(RemoveInvType(t));
      }
      if (new_tps.Card() == 1) {
        return new_tps.GetElem();
      }
      else {
        return mk_REP_UnionTypeRep(new_tps);
      }
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return mk_REP_SetTypeRep(RemoveInvType(tp.GetRecord(pos_REP_SetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return mk_REP_EmptySetTypeRep(RemoveInvType(tp.GetRecord(pos_REP_EmptySetTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return mk_REP_SeqTypeRep(RemoveInvType(tp.GetRecord(pos_REP_SeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return mk_REP_EmptySeqTypeRep(RemoveInvType(tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp)));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return mk_REP_GeneralMapTypeRep(RemoveInvType(tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)),
                                      RemoveInvType(tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return mk_REP_InjectiveMapTypeRep(RemoveInvType(tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)),
                                        RemoveInvType(tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return mk_REP_EmptyMapTypeRep(RemoveInvType(tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom)),
                                    RemoveInvType(tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng)));
    }
    case TAG_TYPE_REP_ProductTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & fl (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      SEQ<TYPE_REP_TypeRep> fl_new;
      size_t len_fl = fl.Length();
      for (size_t idx = 1; idx <= len_fl; idx++) {
        fl_new.ImpAppend(RemoveInvType(fl[idx]));
      }
      return mk_REP_ProductTypeRep(fl_new);
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> new_fndom;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        new_fndom.ImpAppend(RemoveInvType(fndom[idx]));
      return mk_REP_PartialFnTypeRep(new_fndom, RemoveInvType(tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_TotalFnTypeRep : {
      const SEQ<TYPE_REP_TypeRep> & fndom (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      SEQ<TYPE_REP_TypeRep> new_fndom;
      size_t len_fndom = fndom.Length();
      for (size_t idx = 1; idx <= len_fndom; idx++)
        new_fndom.ImpAppend(RemoveInvType(fndom[idx]));
      return mk_REP_TotalFnTypeRep(new_fndom, RemoveInvType(tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng)));
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & opdom (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      SEQ<TYPE_REP_TypeRep> new_opdom;
      size_t len_opdom = opdom.Length();
      for (size_t idx = 1; idx <= len_opdom; idx++)
        new_opdom.ImpAppend(RemoveInvType(opdom[idx]));
      return mk_REP_OpTypeRep(new_opdom, RemoveInvType(tp.GetRecord(pos_REP_OpTypeRep_Rng)));
    }
    default: {
      return tp;
    }
  }
}

// RemoveNil
// type : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::RemoveNil(const TYPE_REP_TypeRep & type)
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (type.Is(TAG_TYPE_REP_UnionTypeRep)) {
      SET<TYPE_REP_TypeRep> tps (type.GetSet(pos_REP_UnionTypeRep_tps));
      tps.ImpDiff(mk_set(mk_REP_NilTypeRep()));
      switch (tps.Card()) {
        case 0:  { return type; } // must not occur
        case 1:  { return tps.GetElem(); }
        default: { return type; }
      }
    }
  }
#endif //VDMPP
  return type;
}

// RemoveExitType
// type : REP`TypeRep
// -> REP`TypeRep
TYPE_REP_TypeRep vdmcg::RemoveExitType(const TYPE_REP_TypeRep & type)
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (type.GetSet(pos_REP_UnionTypeRep_tps));
      SET<TYPE_REP_TypeRep> ntps;
      Generic tp;
      for ( bool bb = tps.First(tp); bb; bb = tps.Next(tp) ) {
        if (!tp.Is(TAG_TYPE_REP_ExitTypeRep))
          ntps.Insert(tp);
      }
      switch (ntps.Card()) {
        case 0:  { return type; } // dummy : not occur
        case 1:  { return ntps.GetElem();}
        default: { return mk_REP_UnionTypeRep(ntps); }
      }
      break;
    }
    default: {
      return type;
      break;
    }
  }
}

// NoReturnType
// type : REP`TypeRep
// -> bool
bool vdmcg::NoReturnValue(const TYPE_REP_TypeRep & type)
{
  switch (type.GetTag()) {
    case TAG_TYPE_REP_UnitTypeRep:
    case TAG_TYPE_REP_RetTypeRep: { return true; }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> tps (type.GetSet(pos_REP_UnionTypeRep_tps));
      bool forall = true;
      Generic tp;
      for ( bool bb = tps.First(tp); bb && forall; bb = tps.Next(tp) ) {
        forall = NoReturnValue(tp);
      }
      return forall;
    }
    default: { return false; }
  }
}

#ifdef VDMPP
// IsInstVar
// nm : AS`Name
// ==> bool
Bool vdmcg::IsInstVar(const TYPE_AS_Name & nm)
{
  return (GetStatSem().IsInstanceVar(nm) && !GetStatSem().IsStaticInstanceVar(nm));
}

// ExpandClassName
// nm : AS`Name
// clnm : AS`Name
// nm_s : set of AS`Name
// ==> bool * AS`Name
Tuple vdmcg::ExpandClassName(const TYPE_AS_Name & nm, const TYPE_AS_Name & clnm, const SET<TYPE_AS_Name> & nm_s)
{
  if (nm_s.InSet(nm))
    return mk_(Bool(false), nm);
  if (IsClass(nm))
    return mk_(Bool(true), nm);
  else
  {
    TYPE_AS_Name clsnm;
    TYPE_AS_Name tpnm;
    if (nm.GetSequence(pos_AS_Name_ids).Length() == 2)
    {
      clsnm = ASTAUX::GetFirstName(nm);
      tpnm = ASTAUX::GetSecondName(nm);
    }
    else
    {
      clsnm = clnm;
      tpnm = nm;
    }
    Tuple t (GetStatSem().PublicLookUpTypeName (nm, clnm));
    TYPE_REP_TypeRep tr (t.GetRecord(1));
    if (tr.Is(TAG_TYPE_REP_TypeNameRep))
    {
      return ExpandClassName(tr.GetRecord(pos_REP_TypeNameRep_nm), clnm, Set(nm_s).Insert(nm));
    }
  }
  return mk_(Bool(false), nm);
}

// AddPackageName
// nm : AS`Name
// vals : seq of AS`ValueDef
void vdmcg::AddPackageName(const TYPE_AS_Name & clnm, const SEQ<TYPE_AS_ValueDef> & vals)
{
  bool controll_found = false;
  size_t len_vals = vals.Length();
  for (size_t idx = 1; idx <= len_vals && !controll_found; idx++)
  {
    const TYPE_AS_ValueDef & vd (vals[idx]);
    const TYPE_AS_Pattern & p (vd.GetRecord(pos_AS_ValueDef_pat));
    if (p.Is(TAG_TYPE_AS_PatternName) && !p.GetField(pos_AS_PatternName_nm).IsNil())
    {
      const TYPE_AS_Name & var (p.GetField(pos_AS_PatternName_nm));
      if (var == ASTAUX::MkNameFromId(ASTAUX::MkId(L"JCGControl_package"), NilContextId))
      {
        const TYPE_AS_Expr & val (vd.GetRecord(pos_AS_ValueDef_val));
        if (val.Is(TAG_TYPE_AS_TextLit))
        {
          const SEQ<Char> & ps (val.GetSequence(pos_AS_TextLit_val));
          this->packageNames.Insert(clnm, ps);
          controll_found = true;
        }
      }
    }
  }
}

void vdmcg::InitJavaPrimitiveType()
{
  // boolean
  for (int i = 0; boolT[i].classname != NULL; i++)
  {
    AddJavaPrimitiveBoolType(ASTAUX::MkId(boolT[i].classname), ASTAUX::MkId(boolT[i].methodname));
  }
  // int
  for (int i = 0; intT[i].classname != NULL; i++)
  {
    AddJavaPrimitiveIntType(ASTAUX::MkId(intT[i].classname), ASTAUX::MkId(intT[i].methodname));
  }
  // double
  for (int i = 0; realT[i].classname != NULL; i++)
  {
    AddJavaPrimitiveRealType(ASTAUX::MkId(realT[i].classname), ASTAUX::MkId(realT[i].methodname));
  }
  SET<TYPE_AS_Name> bnm_s (Settings.GetJCGBoolean());
  Generic nm;
  for (bool bb = bnm_s.First(nm); bb; bb = bnm_s.Next(nm))
  {
     TYPE_AS_Ids ids (Record(nm).GetSequence(pos_AS_Name_ids));
     TYPE_AS_Id cnm ((ids.Length() == 1) ? ASTAUX::MkId(L"*") : ids[1]);
     TYPE_AS_Id mnm ((ids.Length() == 1) ? ids[1] : ids[2]);
     AddJavaPrimitiveBoolType(cnm, mnm);
  }
  SET<TYPE_AS_Name> inm_s (Settings.GetJCGInteger());
  for (bool bb = inm_s.First(nm); bb; bb = inm_s.Next(nm))
  {
     TYPE_AS_Ids ids (Record(nm).GetSequence(pos_AS_Name_ids));
     TYPE_AS_Id cnm ((ids.Length() == 1) ? ASTAUX::MkId(L"*") : ids[1]);
     TYPE_AS_Id mnm ((ids.Length() == 1) ? ids[1] : ids[2]);
     AddJavaPrimitiveIntType(cnm, mnm);
  }
  SET<TYPE_AS_Name> rnm_s (Settings.GetJCGDouble());
  for (bool bb = rnm_s.First(nm); bb; bb = rnm_s.Next(nm))
  {
     TYPE_AS_Ids ids (Record(nm).GetSequence(pos_AS_Name_ids));
     TYPE_AS_Id cnm ((ids.Length() == 1) ? ASTAUX::MkId(L"*") : ids[1]);
     TYPE_AS_Id mnm ((ids.Length() == 1) ? ids[1] : ids[2]);
     AddJavaPrimitiveRealType(cnm, mnm);
  }
}

void vdmcg::AddJavaPrimitiveBoolType(const SEQ<Char> & cnm, const SEQ<Char> & mnm)
{
  AddJavaPrimitiveType(cnm, mnm, GenBoolType().get_tp());
}

void vdmcg::AddJavaPrimitiveIntType(const SEQ<Char> & cnm, const SEQ<Char> & mnm)
{
  AddJavaPrimitiveType(cnm, mnm, GenImplIntType().get_tp());
}

void vdmcg::AddJavaPrimitiveRealType(const SEQ<Char> & cnm, const SEQ<Char> & mnm)
{
  AddJavaPrimitiveType(cnm, mnm, GenImplRealType().get_tp());
}

void vdmcg::AddJavaPrimitiveType(const SEQ<Char> & cnm, const SEQ<Char> & mnm, const TYPE_CPP_Identifier & id)
{
  if ((id == GenBoolType().get_tp()) ||
      (id == GenImplIntType().get_tp()) ||
      (id == GenImplRealType().get_tp()))
  {
    if (cnm == SEQ<Char>(L"*"))
      this->primitiveType.ImpModify(mnm, id);
    else
      this->primitiveType.ImpModify(mk_(cnm, mnm), id);
  }
}

TYPE_CPP_Expr vdmcg::CheckJavaPrimitiveType(const TYPE_CPP_Expr & e, const SEQ<Char> & cnm, const SEQ<Char> & mnm)
{
  SEQ<Char> cnm_q (cnm);
  if (cnm.SubSequence(1,4) == SEQ<Char>(L"JDK_"))
    cnm_q = cnm.SubSequence(5, cnm.Length());
  if (this->primitiveType.DomExists(mk_(cnm_q, mnm)))
    return vdm_BC_GenClassInstanceCreationExpr(this->primitiveType[mk_(cnm_q, mnm)], type_dL().ImpAppend(e));
  else if (this->primitiveType.DomExists(mnm))
    return vdm_BC_GenClassInstanceCreationExpr(this->primitiveType[mnm], type_dL().ImpAppend(e));

  return e;
}
#endif //VDMPP

// AddNoCheckSeqApply
// s: set of (CPP`Expr * CPP`Expr)
// ==> ()
void vdmcg::AddNoCheckSeqApply(const Set & s)
{
  this->noCheckSeqApply.ImpUnion(s);
}

// AddNoCheckSeqApply
// s : set of (CPP`Expr * CPP`Expr)
// ==> ()
void vdmcg::RemNoCheckSeqApply(const Set & s)
{
  this->noCheckSeqApply.ImpDiff(s);
}

// CheckSeqApply
// e : CPP`Expr * CPP`Expr
// ==> bool
bool vdmcg::CheckSeqApply(const Tuple & e)
{
  return !this->noCheckSeqApply.InSet(e);
}

// IsSeqIndicesSet
// e : AS`Expr
// ==> bool
bool vdmcg::IsSeqIndicesSet(const TYPE_AS_Expr & e)
{
  if (e.Is(TAG_TYPE_AS_PrefixExpr) && (e.GetField(pos_AS_PrefixExpr_opr) == Int(SEQINDICES))) {
    const TYPE_AS_Expr & arg (e.GetRecord(pos_AS_PrefixExpr_arg));
    if (arg.Is(TAG_TYPE_AS_Name)) {
      Generic tp (FindType(arg));
      if (!tp.IsNil()) {
        return IsSeqType(tp);
      }
    } 
  }
  return false;
}

// IsBoolExpr
// expr : CPP`Expr
// ==> bool
bool vdmcg::IsBoolExpr(const TYPE_CPP_Expr & expr) 
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return (expr.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr) &&
            (expr.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenBoolType().get_tp()));
  }
  else
#endif // VDMPP
  {
    return (expr.Is(TAG_TYPE_CPP_FctCall) &&
            (expr.GetRecord(pos_CPP_FctCall_fct) == GenBoolType().get_tp()));
  }
}

// IsIntExpr
// expr : CPP`Expr
// ==> bool
bool vdmcg::IsIntExpr(const TYPE_CPP_Expr & expr) 
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    if (expr.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr)) {
      return (expr.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenImplIntType().get_tp());
    }
    else {
      if (expr.Is(TAG_TYPE_CPP_FctCall)) {
        const TYPE_CPP_Expr & fct(expr.GetRecord(pos_CPP_FctCall_fct));
        return (fct.Is(TAG_TYPE_CPP_ObjectMemberAccess) &&
            (fct.GetRecord(pos_CPP_ObjectMemberAccess_object) == GenImplIntType().get_tp()) &&
            (fct.GetRecord(pos_CPP_ObjectMemberAccess_name) == vdm_BC_GenIdentifier(ASTAUX::MkId(L"valueOf"))));
      }
    }
    return false;
  }
  else
#endif // VDMPP
  {
    return (expr.Is(TAG_TYPE_CPP_FctCall) &&
            (expr.GetRecord(pos_CPP_FctCall_fct) == GenImplIntType().get_tp()));
  }
}

// IsRealExpr
// expr : CPP`Expr
// ==> bool
bool vdmcg::IsRealExpr(const TYPE_CPP_Expr & expr) 
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return (expr.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr) &&
            (expr.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenImplRealType().get_tp()));
  }
  else
#endif // VDMPP
  {
    return (expr.Is(TAG_TYPE_CPP_FctCall) &&
            (expr.GetRecord(pos_CPP_FctCall_fct) == GenImplRealType().get_tp()));
  }
}

// IsStringExpr
// expr : CPP`Expr
// ==> bool
bool vdmcg::IsStringExpr(const TYPE_CPP_Expr & expr) 
{
#ifdef VDMPP
  if (vdm_CPP_isJAVA()) {
    return (expr.Is(TAG_TYPE_CPP_ClassInstanceCreationExpr) &&
            (expr.GetRecord(pos_CPP_ClassInstanceCreationExpr_classtype) == GenStringType().get_tp()));
  }
  else
#endif // VDMPP
  {
    return (expr.Is(TAG_TYPE_CPP_FctCall) &&
            (expr.GetRecord(pos_CPP_FctCall_fct) == GenStringType().get_tp()));
  }
}

// StripBracketedExpr
// expr : CPP`Expr
// +> CPP`Expr
TYPE_CPP_Expr vdmcg::StripBracketedExpr (const TYPE_CPP_Expr & expr)
{
  if (expr.Is(TAG_TYPE_CPP_BracketedExpr))
    return StripBracketedExpr(expr.GetRecord(pos_CPP_BracketedExpr_expr));
  else
    return expr;
}

// StripCastExpr
// expr : CPP`Expr
// +> CPP`Expr
TYPE_CPP_Expr vdmcg::StripCastExpr (const TYPE_CPP_Expr & expr)
{
  if (expr.Is(TAG_TYPE_CPP_CastExpr))
    return StripCastExpr(expr.GetRecord(pos_CPP_CastExpr_expr));
  else
    return expr;
}

// StripBracketedAndCastExpr
// expr : CPP`Expr
// +> CPP`Expr
TYPE_CPP_Expr vdmcg::StripBracketedAndCastExpr (const TYPE_CPP_Expr & expr)
{
  switch (expr.GetTag()) {
    case TAG_TYPE_CPP_BracketedExpr: {
      return StripBracketedAndCastExpr(expr.GetRecord(pos_CPP_BracketedExpr_expr));
    }
    case TAG_TYPE_CPP_CastExpr: {
      return StripBracketedAndCastExpr(expr.GetRecord(pos_CPP_CastExpr_expr));
    }
    default: {
      return expr;
    }
  }
}

// ExpandCompoundStmt
// stmts : seq of CPP`Stmt
// -> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::ExpandCompoundStmt(const SEQ<TYPE_CPP_Stmt> & stmts)
{
  size_t len_stmts = stmts.Length();
  if ((len_stmts >= 2) && stmts[len_stmts - 1].Is(TAG_TYPE_CPP_CompoundStmt) &&
        stmts[len_stmts].Is(TAG_TYPE_CPP_Return)) {
    bool forall = true;
    for (size_t i = 1; (i < len_stmts - 1) && forall; i++) {
      const TYPE_CPP_Stmt & stmt (stmts[i]);
      forall = (stmt.Is(TAG_TYPE_CPP_DeclarationStmt) || stmt.Is(TAG_TYPE_CPP_ExpressionStmt));
    }
    if (forall) {
      SEQ<TYPE_CPP_Stmt> res (stmts.SubSequence(1,len_stmts - 2));
      res.ImpConc(stmts[len_stmts - 1].GetSequence(pos_CPP_CompoundStmt_stms));
      res.ImpAppend(stmts[len_stmts]);
      return res;
    }
  }
  else if ((len_stmts >= 1) && stmts[len_stmts].Is(TAG_TYPE_CPP_CompoundStmt)) {
    bool forall = true;
    for (size_t i = 1; (i < len_stmts) && forall; i++) {
      const TYPE_CPP_Stmt & stmt (stmts[i]);
      forall = (stmt.Is(TAG_TYPE_CPP_DeclarationStmt) || stmt.Is(TAG_TYPE_CPP_ExpressionStmt));
    }
    if (forall) {
      SEQ<TYPE_CPP_Stmt> res (stmts.SubSequence(1,len_stmts - 1));
      res.ImpConc(stmts[len_stmts].GetSequence(pos_CPP_CompoundStmt_stms));
      return res;
    }
  }
  return stmts;
}

// GetOpParms
// op : AS`OpDef
// -> seq of AS`Type
SEQ<TYPE_AS_Type> vdmcg::GetOpParms(const TYPE_AS_OpDef & op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef: {
      return op.GetRecord(pos_AS_ExplOpDef_tp).GetSequence(pos_AS_OpType_opdom);
    }
    case TAG_TYPE_AS_ImplOpDef: {
      return ConstructImplDomType(op.GetSequence(pos_AS_ImplOpDef_partps));
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      return ConstructImplDomType(op.GetSequence(pos_AS_ExtExplOpDef_partps));
    }
    default: {
      ReportError(L"GetOpParams");
      return SEQ<TYPE_AS_Type>();
    }
  }
}

// GetFnParms
// fn : AS`FnDef
// -> seq of AS`Type
SEQ<TYPE_AS_Type> vdmcg::GetFnParms(const TYPE_AS_FnDef & fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      return fn.GetRecord(pos_AS_ExplFnDef_tp).GetSequence(1);
    }
    case TAG_TYPE_AS_ImplFnDef: {
      return ConstructImplDomType(fn.GetSequence(pos_AS_ImplFnDef_partps));
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      return ConstructImplDomType(fn.GetSequence(pos_AS_ExtExplFnDef_partps));
    }
    default: {
      ReportError(L"GetFnParams");
      return SEQ<TYPE_AS_Type>();
    }
  }
}

// GetOpRestype
// op : AS`OpDef
// -> AS`Type
TYPE_AS_Type vdmcg::GetOpRestype(const TYPE_AS_OpDef& op)
{
  switch (op.GetTag()) {
    case TAG_TYPE_AS_ExplOpDef: {
      return op.GetRecord(pos_AS_ExplOpDef_tp).GetRecord(pos_AS_OpType_oprng);
    }
    case TAG_TYPE_AS_ImplOpDef: {
      return ConstructImplRngType(op.GetSequence(pos_AS_ImplOpDef_resnmtps));
    }
    case TAG_TYPE_AS_ExtExplOpDef: {
      return ConstructImplRngType(op.GetSequence(pos_AS_ExtExplOpDef_resnmtps));
    }
    default: {
      ReportError(L"GetOPRestype");
      return TYPE_AS_Type();
    }
  }
}

// GetFnRestype
// fn : AS`FnDef
// -> AS`Type
TYPE_AS_Type vdmcg::GetFnRestype(const TYPE_AS_FnDef& fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef: {
      return fn.GetRecord(pos_AS_ExplFnDef_tp).GetRecord(2);
    }
    case TAG_TYPE_AS_ImplFnDef: {
      return ConstructImplRngType(fn.GetSequence(pos_AS_ImplFnDef_resnmtps));
    }
    case TAG_TYPE_AS_ExtExplFnDef: {
      return ConstructImplRngType(fn.GetSequence(pos_AS_ExtExplFnDef_resnmtps));
    }
    default: {
      ReportError(L"GetFnRestype");
      return TYPE_AS_Type();
    }
  }
}

// ConstructImplDomType
// partps : seq of AS`PatTypePair
// -> seq of AS`Type
SEQ<TYPE_AS_Type> vdmcg::ConstructImplDomType(const SEQ<TYPE_AS_PatTypePair> & partps)
{
  SEQ<TYPE_AS_Type> result;
  size_t len_partps = partps.Length();
  for (size_t i = 1; i <= len_partps; i++)
  {
    const TYPE_AS_PatTypePair & ptp (partps[i]);
    const TYPE_AS_Type & tp (ptp.GetRecord(pos_AS_PatTypePair_tp));
    size_t len_ptp = ptp.GetSequence(pos_AS_PatTypePair_pats).Length();
    for (size_t j = 1; j <= len_ptp; j++)
      result.ImpAppend(tp);
  }
  return result;
}

// ConstructImplRngType
// restps : seq of AS`NameType
// -> [AS`Type]
TYPE_AS_Type vdmcg::ConstructImplRngType(const SEQ<TYPE_AS_NameType> & restps)
{
  switch(restps.Length()) {
    case 0:
      return TYPE_AS_VoidType().Init(NilContextId);
    case 1:
      return restps[1].GetRecord(pos_AS_NameType_tp);
    default: {
      SEQ<TYPE_AS_Type> tps;
      size_t len_restps = restps.Length();
      for (size_t idx = 1; idx <= len_restps; idx++)
        tps.ImpAppend(restps[idx].GetRecord(pos_AS_NameType_tp));
      return TYPE_AS_ProductType().Init(tps, NilContextId);
    }
  }
}

// GetFnMeasu
// fn : AS`FnDef
// -> [AS`Name]
Generic vdmcg::GetFnMeasu(const TYPE_AS_FnDef & fn)
{
  switch (fn.GetTag()) {
    case TAG_TYPE_AS_ExplFnDef:
      return fn.GetField(pos_AS_ExplFnDef_measu);
    case TAG_TYPE_AS_ImplFnDef:
    case TAG_TYPE_AS_ExtExplFnDef: {
      return Nil();
    }
    default: {
      ReportError(L"FindFnMeasu");
      return Nil();
    }
  }
}

// QualiName
// p_cl : AS`Name
// p_nm : AS`Name -> AS`Name
TYPE_AS_Name vdmcg::QualiName(const TYPE_AS_Name & p_cl, const TYPE_AS_Name & p_nm)
{
  return ((p_nm.GetSequence(pos_AS_Name_ids).Length() == 2)
                    ? p_nm
                    : ASTAUX::Combine2Names(p_cl, p_nm));
}

// UnqualiName
// p_nm : AS`Name
// -> AS`Name
TYPE_AS_Name vdmcg::UnqualiName(const TYPE_AS_Name& p_nm)
{
  return TYPE_AS_Name().Init(TYPE_AS_Ids().ImpAppend(ASTAUX::GetLastId(p_nm)),
                             p_nm.GetInt(pos_AS_Name_cid));
}

// AddClMod
// tp : AS`Type
// clmod : AS`Name
// allClasses : set of AS`Name
// -> AS`Type
TYPE_AS_Type vdmcg::AddClMod(const TYPE_AS_Type & tp, const TYPE_AS_Name & clmod,
                             const SET<TYPE_AS_Name> & allClasses)
{
  switch (tp.GetTag()) {
    case TAG_TYPE_AS_TypeName: {
      const TYPE_AS_Name & name (tp.GetRecord(pos_AS_TypeName_name));
      if (2 == name.GetSequence(pos_AS_Name_ids).Length()) {
        return tp;
      }
      else {
        if (allClasses.InSet(name)) {
          return tp;
        }
        else {
          TYPE_AS_TypeName type (tp);
          type.SetField(pos_AS_TypeName_name, ASTAUX::Combine2Names(clmod, name));
          return type;
        }
      }
    }
    case TAG_TYPE_AS_CompositeType: {
      const TYPE_AS_Name & name (tp.GetRecord(pos_AS_CompositeType_name));
      if (2 == name.GetSequence(pos_AS_Name_ids).Length()) {
        return tp;
      }
      else {
        TYPE_AS_CompositeType type (tp);
        type.SetField(pos_AS_CompositeType_name, ASTAUX::Combine2Names(clmod, name));
        return type;
      }
    }
    case TAG_TYPE_AS_UnionType: {
      const SEQ<TYPE_AS_Type> & tps (tp.GetSequence(pos_AS_UnionType_tps));
      SEQ<TYPE_AS_Type> newtps;
      size_t len_tps = tps.Length();
      for (size_t i = 1; i <= len_tps; i++) {
        newtps.ImpAppend(AddClMod(tps[i], clmod, allClasses));
      }
      TYPE_AS_UnionType type (tp);
      type.SetField(pos_AS_UnionType_tps, newtps);
      return type;
    }
    case TAG_TYPE_AS_ProductType: {
      const SEQ<TYPE_AS_Type> & tps (tp.GetSequence(pos_AS_ProductType_tps));
      SEQ<TYPE_AS_Type> newtps;
      size_t len_tps = tps.Length();
      for (size_t i = 1; i <= len_tps; i++) {
        newtps.ImpAppend(AddClMod(tps[i], clmod, allClasses));
      }
      TYPE_AS_ProductType type (tp);
      type.SetField(pos_AS_ProductType_tps, newtps);
      return type;
    }
    case TAG_TYPE_AS_Set0Type: {
      TYPE_AS_Set0Type type (tp);
      type.SetField(pos_AS_Set0Type_elemtp,
                    AddClMod(tp.GetRecord(pos_AS_Set0Type_elemtp), clmod, allClasses));
      return type;
    }
    case TAG_TYPE_AS_Set1Type: {
      TYPE_AS_Set1Type type (tp);
      type.SetField(pos_AS_Set1Type_elemtp,
                    AddClMod(tp.GetRecord(pos_AS_Set1Type_elemtp), clmod, allClasses));
      return type;
    }
    case TAG_TYPE_AS_Seq0Type: {
      TYPE_AS_Seq0Type type (tp);
      type.SetField(pos_AS_Seq0Type_elemtp,
                    AddClMod(tp.GetRecord(pos_AS_Seq0Type_elemtp), clmod, allClasses));
      return type;
    }
    case TAG_TYPE_AS_Seq1Type: {
      TYPE_AS_Seq1Type type (tp);
      type.SetField(pos_AS_Seq1Type_elemtp,
                    AddClMod(tp.GetRecord(pos_AS_Seq1Type_elemtp), clmod, allClasses));
      return type;
    }
    case TAG_TYPE_AS_GeneralMap0Type: {
      TYPE_AS_GeneralMap0Type type (tp);
      type.SetField(pos_AS_GeneralMap0Type_mapdom,
                    AddClMod(tp.GetRecord(pos_AS_GeneralMap0Type_mapdom), clmod, allClasses));
      return type;
    }
    case TAG_TYPE_AS_GeneralMap1Type: {
      TYPE_AS_GeneralMap1Type type (tp);
      type.SetField(pos_AS_GeneralMap1Type_mapdom,
                    AddClMod(tp.GetRecord(pos_AS_GeneralMap1Type_mapdom), clmod, allClasses));
      return type;
    }
    case TAG_TYPE_AS_InjectiveMap0Type: {
      TYPE_AS_InjectiveMap0Type type (tp);
      type.SetField(pos_AS_InjectiveMap0Type_mapdom,
                    AddClMod(tp.GetRecord(pos_AS_InjectiveMap0Type_mapdom), clmod, allClasses));
      return type;
    }
    case TAG_TYPE_AS_InjectiveMap1Type: {
      TYPE_AS_InjectiveMap1Type type (tp);
      type.SetField(pos_AS_InjectiveMap1Type_mapdom,
                    AddClMod(tp.GetRecord(pos_AS_InjectiveMap1Type_mapdom), clmod, allClasses));
      return type;
    }
    default: {
      return tp;
    }
  }
}

// MergeStmts
// decl : seq of CPP`Stmt
// pm : seq of CPP`Stmt
// ==> seq of CPP`Stmt
SEQ<TYPE_CPP_Stmt> vdmcg::MergeStmts (const SEQ<TYPE_CPP_Stmt> & decls, const SEQ<TYPE_CPP_Stmt> & pm) {
  Map id_m;
  size_t len_decls = decls.Length();
  for ( size_t i = 1; i <= len_decls; i++ ) {
    const TYPE_CPP_Stmt & stmt (decls[i]);
    if ( stmt.Is(TAG_TYPE_CPP_DeclarationStmt) &&
         stmt.GetRecord(pos_CPP_DeclarationStmt_decl).Is(TAG_TYPE_CPP_IdentDeclaration)) {
      const TYPE_CPP_IdentDeclaration & iDecl (stmt.GetRecord(pos_CPP_DeclarationStmt_decl));
      const SEQ<TYPE_CPP_InitDeclarator> & ids (iDecl.GetSequence(pos_CPP_IdentDeclaration_dl));
      if ( ids.Length() == 1 ) {
        const TYPE_CPP_Declarator & decl (ids[1].GetRecord(pos_CPP_InitDeclarator_decl));
        if ( decl.Is(TAG_TYPE_CPP_Identifier) ) {
          id_m.Insert( decl, stmt );
        }
      }
    }
  }
  SEQ<TYPE_CPP_Stmt> new_decls (decls);
  SEQ<TYPE_CPP_Stmt> stmts (pm);
  size_t len_pm = pm.Length();
  for  ( size_t index = 1; index <= len_pm; index++ ) {
    TYPE_CPP_Stmt stmt (pm[index]);
    if (stmt.Is(TAG_TYPE_CPP_ExpressionStmt) &&
        stmt.GetField(pos_CPP_ExpressionStmt_expr).Is(TAG_TYPE_CPP_AssignExpr) &&
        (stmt.GetRecord(pos_CPP_ExpressionStmt_expr).GetRecord(pos_CPP_AssignExpr_assignop)
                            .GetField(pos_CPP_AssignOp_op) == Quote(L"ASEQUAL"))) {
      const TYPE_CPP_Expr & unary (stmt.GetRecord(pos_CPP_ExpressionStmt_expr)
                                       .GetRecord(pos_CPP_AssignExpr_unary));
      if (id_m.DomExists( unary )) {
        const TYPE_CPP_Expr & assignexpr (stmt.GetRecord(pos_CPP_ExpressionStmt_expr)
                                              .GetRecord(pos_CPP_AssignExpr_assignexpr));
        TYPE_CPP_DeclarationStmt declstmt ( id_m[unary] );
        int64_t idx = new_decls.Find( declstmt );
        new_decls.RemElem( idx );

        TYPE_CPP_IdentDeclaration idecl (declstmt.GetRecord(pos_CPP_DeclarationStmt_decl));
        SEQ<TYPE_CPP_DeclSpecifier> ds (idecl.GetSequence(pos_CPP_IdentDeclaration_ds));
        TYPE_CPP_InitDeclarator inidec (idecl.GetSequence(pos_CPP_IdentDeclaration_dl)[1]);
        ds.ImpPrepend(vdm_BC_GenTypeSpecifier(quote_CONST));
        TYPE_CPP_Initializer initExpr;
#ifdef VDMPP
        if (vdm_CPP_isJAVA()) {
          initExpr = vdm_BC_GenAsgnInit(assignexpr);
        }
        else
#endif // VDMPP
        {
          if (assignexpr.Is(TAG_TYPE_CPP_FctCall) &&
              assignexpr.GetRecord(pos_CPP_FctCall_fct).Is(TAG_TYPE_CPP_Identifier) &&
              assignexpr.GetSequence(pos_CPP_FctCall_arg).IsEmpty()) {
            const SEQ<Char> & id (assignexpr.GetRecord(pos_CPP_FctCall_fct).GetSequence(pos_CPP_Identifier_id));
            if ( id.StartsWith( SEQ<Char>(L"vdm_")) ) {
              initExpr = vdm_BC_GenObjectInit(mk_sequence(assignexpr));
            }
            else {
              initExpr = vdm_BC_GenAsgnInit(assignexpr);
            }
          }
          else {
            initExpr = vdm_BC_GenObjectInit(mk_sequence(assignexpr));
          }
        }
        inidec.SetField(pos_CPP_InitDeclarator_i, initExpr);
        idecl.SetField(pos_CPP_IdentDeclaration_ds, ds);
        idecl.SetField(pos_CPP_IdentDeclaration_dl, mk_sequence(inidec));
        declstmt.SetField(pos_CPP_DeclarationStmt_decl, idecl);
        stmts.ImpModify(index, declstmt);
      }
    }
  }
  return new_decls.ImpConc(stmts);
}

// IsCyclicType
// tp : REP`TypeRep
// ==> bool
bool vdmcg::IsCyclicTypeRep(const TYPE_REP_TypeRep & tp) {
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      return FindTypeNameRep(tp, CleanFlatType(tp));
    }
    default: {
      return false;
    }
  }
}

// FindTypeNameRep
// tnr : REP`TypeNameRep
// tp : REP`TypeRep
// ==> bool
bool vdmcg::FindTypeNameRep(const TYPE_REP_TypeNameRep & tnr, const TYPE_REP_TypeRep & tp) {
  switch (tp.GetTag()) {
    case TAG_TYPE_REP_TypeNameRep: {
      return (tnr == tp);
    }
    case TAG_TYPE_REP_UnionTypeRep: {
      SET<TYPE_REP_TypeRep> ts (tp.GetSet(pos_REP_UnionTypeRep_tps));
      bool exists = false; 
      Generic t;
      for (bool bb = ts.First(t); bb && !exists; bb = ts.Next(t) ) {
        exists = FindTypeNameRep(tnr, t);
      }
      return exists;
    }
    case TAG_TYPE_REP_ProductTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & tpl (tp.GetSequence(pos_REP_ProductTypeRep_tps));
      bool exists = false; 
      size_t len_tpl = tpl.Length();
      for (size_t idx = 1; (idx <= len_tpl && !exists); idx++) {
        exists = FindTypeNameRep(tnr, tpl[idx]);
      }
      return exists;
    }
    case TAG_TYPE_REP_SetTypeRep: {
      return FindTypeNameRep(tnr, tp.GetRecord(pos_REP_SetTypeRep_elemtp));
    }
    case TAG_TYPE_REP_EmptySetTypeRep: {
      return FindTypeNameRep(tnr, tp.GetRecord(pos_REP_EmptySetTypeRep_elemtp));
    }
    case TAG_TYPE_REP_SeqTypeRep: {
      return FindTypeNameRep(tnr, tp.GetRecord(pos_REP_SeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_EmptySeqTypeRep: {
      return FindTypeNameRep(tnr, tp.GetRecord(pos_REP_EmptySeqTypeRep_elemtp));
    }
    case TAG_TYPE_REP_GeneralMapTypeRep: {
      return FindTypeNameRep(tnr, tp.GetRecord(pos_REP_GeneralMapTypeRep_mapdom)) ||
             FindTypeNameRep(tnr, tp.GetRecord(pos_REP_GeneralMapTypeRep_maprng));
    }
    case TAG_TYPE_REP_InjectiveMapTypeRep: {
      return FindTypeNameRep(tnr, tp.GetRecord(pos_REP_InjectiveMapTypeRep_mapdom)) ||
             FindTypeNameRep(tnr, tp.GetRecord(pos_REP_InjectiveMapTypeRep_maprng));
    }
    case TAG_TYPE_REP_EmptyMapTypeRep: {
      return FindTypeNameRep(tnr, tp.GetRecord(pos_REP_EmptyMapTypeRep_mapdom)) ||
             FindTypeNameRep(tnr, tp.GetRecord(pos_REP_EmptyMapTypeRep_maprng));
    }
    case TAG_TYPE_REP_InvTypeRep: {
      return FindTypeNameRep(tnr, tp.GetRecord(pos_REP_InvTypeRep_shape));
    }
    case TAG_TYPE_REP_CompositeTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & fields (tp.GetSequence(pos_REP_CompositeTypeRep_fields));
      bool exists = false; 
      size_t len_fields = fields.Length();
      for (size_t idx = 1; (idx <= len_fields && !exists); idx++) {
        const TYPE_REP_FieldRep & fr (fields[idx]);
        exists = FindTypeNameRep(tnr, fr.GetRecord(pos_REP_FieldRep_tp));
      }
      return exists;
    }
    case TAG_TYPE_REP_OpTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & dtpl (tp.GetSequence(pos_REP_OpTypeRep_Dom));
      bool exists = FindTypeNameRep(tnr, tp.GetRecord(pos_REP_OpTypeRep_Rng));
      size_t len_dtpl = dtpl.Length();
      for (size_t idx = 1; (idx <= len_dtpl && !exists); idx++) {
        exists = FindTypeNameRep(tnr,dtpl[idx]);
      }
      return exists;
    }
    case TAG_TYPE_REP_PartialFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & dtpl (tp.GetSequence(pos_REP_PartialFnTypeRep_fndom));
      bool exists = FindTypeNameRep(tnr, tp.GetRecord(pos_REP_PartialFnTypeRep_fnrng));
      size_t len_dtpl = dtpl.Length();
      for (size_t idx = 1; (idx <= len_dtpl && !exists); idx++) {
        exists = FindTypeNameRep(tnr,dtpl[idx]);
      }
      return exists;
    }
    case TAG_TYPE_REP_TotalFnTypeRep: {
      const SEQ<TYPE_REP_TypeRep> & dtpl (tp.GetSequence(pos_REP_TotalFnTypeRep_fndom));
      bool exists = FindTypeNameRep(tnr, tp.GetRecord(pos_REP_TotalFnTypeRep_fnrng));
      size_t len_dtpl = dtpl.Length();
      for (size_t idx = 1; (idx <= len_dtpl && !exists); idx++) {
        exists = FindTypeNameRep(tnr,dtpl[idx]);
      }
      return exists;
    }
    default: {
      return false;
    }
  }
}
