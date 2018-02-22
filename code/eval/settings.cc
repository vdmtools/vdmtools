/***
*  * WHAT
*  *    Settings
*  *
*  * FILE
*  *    $Source: /mnt/cvsroot/toolbox/code/eval/settings.cc,v $
*  * VERSION
*  *    $Revision: 1.5 $
*  * DATE
*  *    $Date: 2006/09/22 11:14:06 $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    OS + $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "settings.h"
#include "libman.h"
#include "stackeval.h"
#include <stdlib.h>
#ifdef VICE
#include "TIMETRACE.h"
#endif // VICE
#include "statsem.h"
#include "astaux.h"
#include "tb_version.h"
#include "tb_wstring.h"

SETTINGS Settings;

SETTINGS::SETTINGS()
{
  this->InitSettings();
  this->SetJCGHackParser(false);
}

void SETTINGS::InitSettings()
{
  this->VDM10Off();
  this->OldReverseOff();
//The following is the definition of the standard settings which
// can be set by the 'set' command.
// 20100409 -->
/*
  this->DtcOff();              // dynamic typechecking off
  this->PreOff();              // pre-cond checking off
  this->PostOff();             // post-cond checking off
  this->InvOff();              // invariant checking off
*/
  this->DtcOn();              // dynamic typechecking on
  this->PreOn();              // pre-cond checking on
  this->PostOn();             // post-cond checking on
  this->InvOn();              // invariant checking on
  this->AssertOff();
// <-- 20100409

  this->PrintFormatOn();       // pretty print format for vdmsl values on.

  this->RandomOff();

#ifdef VDMPP
  this->SetMaxInstr(1000);     // Number of instructions to execute
                               // in each thread, before swapping to another thread.

  this->SetTimeSlice(1000);
  this->SetDefaultCapacity(1000000);
  this->SetVirtualCPUCapacity(Quote(L"INFINITE"));
  this->SetStepSize(100);
  this->SetJitterMode(JitterModeType::JM_Early);//

  this->PriorityBasedOff();   // Priority based scheduling for VDM++

#ifndef VICE
  this->SetTaskSwitch(0);     // Task switching constant for VDM++
  this->SetPrimaryAlgorithm(TYPE_SCHDTP_InstrnumSlice());
                              // Primary scheduling algorithm
#else
  this->SetTaskSwitch(2);     // Task switching constant for VICE
  this->SetPrimaryAlgorithm(TYPE_SCHDTP_PureCooperative());
                              // Primary scheduling algorithm
#endif // VICE

  this->SetTimeFactor(1);

  this->logargs = Set();
#endif // VDMPP

  this->ContextOff();       // Turn off context info in interpreter
                            // -purely for speed!
///////////////////////////////////////
  this->VDMSLmodeOff();     //typechecking in restricted vdm-sl mode off
  this->ErrorLevelWRN1();   //Restricted number of warning and
                            //errors from typechecker
                            //given. To see all errors and warnings
                            //set errlevel
                            //to ERR2 (see statsem.h)
  this->DefOff();           //Definitely well-formedness for typechecking off.
  this->SepOn();            //Seperation of warnings and errors
                            //from typecheker on


//  this->print_format = 0;   //pretty print format for vdmsl values on.
                            //Off is -1
  this->SetCGRTI(false);         //Run time position information for code
                            //generation off
  this->SetCGCheckPrePost(1); // Check of pre and post conditions

  this->SetIndex(0);        //Index for latex generator set to no index.
                              //Can be either 0 (no index),
                              //1  (index for definitions) or
                              //2 (index for definitions and used definitions)
  this->SetPrettyPrintRTI(false);   //Coloring coveration for latex off.

  this->SetCFlag(0);         // Output consistency check to file if > 0.

  this->SetJCGSkel(false);
  this->SetJCGGenPrePost(true);
  this->SetJCGTypes(false);
  this->SetJCGSmallTypes(false);
  this->SetJCGLongs(false);
  this->SetJCGPackage(Nil());
  this->SetJCGConcur(true);
  this->SetJCGCheckPrePost(true);
  this->SetJCGInterfaces(SET<TYPE_ProjectTypes_ModuleName>());
  this->SetJCGVDMPrefix(true);
  this->SetJCGUseCodeDir(false);
  this->SetJCGCodeDir(Nil());
  this->SetJCGNeedBackup(true);

  this->SetJCGBoolean(SET<TYPE_AS_Name>());
  this->SetJCGInteger(SET<TYPE_AS_Name>());
  this->SetJCGDouble(SET<TYPE_AS_Name>());

//  this->seed_nondetstmt = -1;
  this->SetCorbaOn();

  this->ProfileOff();
  //this->ProfileOn();

  this->SetTranslation();

  this->nocheck.Clear();
  this->DoCheckOff();

  this->RTErrExceptionOff();
//  this->RTErrExceptionOn();

  this->SetExpression(Nil());
//  this->SetExpression(Sequence(L"\"Hello VDM World!!\""));

  //this->CasesOn();
  this->CasesOff();

  //this->MeasureOn();
  this->MeasureOff();

  this->ElapsedTimeOff();

  this->MapOneOff();

  this->AddAllTypeAllowed(ASTAUX::MkName(L"IO"));
  this->AddAllTypeAllowed(ASTAUX::MkName(L"VDMUtil"));
  this->AddAllTypeAllowed(ASTAUX::MkName(L"JDK_PrintStream"));

  this->CallLogOff();
  this->SetCallLogFileName(wstring(L"calllog.txt"));
}

void SETTINGS::VDM10On()
{
  this->vdm10 = true;
}

void SETTINGS::VDM10Off()
{
  this->vdm10 = false;
}

bool SETTINGS::VDM10() const
{
  return this->vdm10;
}

void SETTINGS::OldReverseOn()
{
  this->oldreverse = true;
}

void SETTINGS::OldReverseOff()
{
  this->oldreverse = false;
}

bool SETTINGS::OldReverse() const
{
  return this->oldreverse;
}

void SETTINGS::DtcOn()
{
  this->dtc = true;
}

void SETTINGS::DtcOff()
{
  this->dtc = false;
}

bool SETTINGS::DTC() const
{
#ifdef VDMSL
  return this->dtc;
#endif // VDMSL
#ifdef VDMPP
  return this->dtc && DoCheck();
#endif // VDMPP
}

void SETTINGS::PreOn()
{
  this->precheck = true;
}

void SETTINGS::PreOff()
{
  this->precheck = false;
}

bool SETTINGS::PreCheck() const
{ 
#ifdef VDMSL
  return this->precheck;
#endif // VDMSL
#ifdef VDMPP
  return this->precheck && DoCheck();
#endif // VDMPP
}

void SETTINGS::PostOn()
{
  this->postcheck = true;
}

void SETTINGS::PostOff()
{
  this->postcheck = false;
}

bool SETTINGS::PostCheck() const
{
#ifdef VDMSL
  return this->postcheck;
#endif // VDMSL
#ifdef VDMPP
  return this->postcheck && DoCheck();
#endif // VDMPP
}

void SETTINGS::InvOn()
{
  this->invar = true;
}

void SETTINGS::InvOff()
{
  this->invar = false;
}

bool SETTINGS::INV() const
{
#ifdef VDMSL
  return this->invar;
#endif // VDMSL
#ifdef VDMPP
  return this->invar && DoCheck();
#endif // VDMPP
}

void SETTINGS::AssertOn()
{
  this->assertion = true;
}

void SETTINGS::AssertOff()
{
  this->assertion = false;
}

bool SETTINGS::Assertion() const
{
#ifdef VDMSL
  return this->assertion;
#endif // VDMSL
#ifdef VDMPP
  return this->assertion && DoCheck();
#endif // VDMPP
}

void SETTINGS::PrintFormatOn()
{
  this->print_format = true;
}

void SETTINGS::PrintFormatOff()
{
  this->print_format = false;
}

bool SETTINGS::PrintFormat() const
{
  return this->print_format;
}

void SETTINGS::RandomOn(int n)
{
  this->random = n;
}

void SETTINGS::RandomOff()
{
  this->random = -1;
}

int SETTINGS::Random() const
{
  return this->random;
}

#ifdef VDMPP
int SETTINGS::GetMaxInstr () const
{
  return this->maxinstr;
}

void SETTINGS::SetMaxInstr(int no)
{
  this->maxinstr = no;
}

int SETTINGS::GetTaskSwitch () const
{
  return this->taskSwitch;
}

void SETTINGS::SetTaskSwitch(int newTaskSwitch)
{
  this->taskSwitch = newTaskSwitch;
}

//#ifdef VICE
int SETTINGS::GetTimeSlice () const
{
  return this->maxTime;
}

void SETTINGS::SetTimeSlice(int newMaxTime)
{
  this->maxTime = newMaxTime;
}

int SETTINGS::GetTimeFactor () const
{
  return this->timeFactor;
}

void SETTINGS::SetTimeFactor(int newTimeFactor)
{
  this->timeFactor = newTimeFactor;
}

int SETTINGS::GetDefaultCapacity () const
{
  return this->defaultcapacity;
}

void SETTINGS::SetDefaultCapacity(int capacity)
{
  this->defaultcapacity = capacity;
}

Generic SETTINGS::GetVirtualCPUCapacity() const
{
  return this->virtualcpucapacity;
}

void SETTINGS::SetVirtualCPUCapacity(const Generic & capacity)
{
  this->virtualcpucapacity = capacity;
}

void SETTINGS::SetVirtualCPUCapacityInfinite()
{
  this->virtualcpucapacity = Quote(L"INFINITE");
}

bool SETTINGS::IsVirtualCPUCapacityInfinite()
{
  return this->virtualcpucapacity.IsQuote();
}

wstring SETTINGS::GetVirtualCPUCapacityStr() const
{
//  if( this->virtualcpucapacity.IsQuote() )
//    return L"INFINITE";
//  else 
//    return Int(this->virtualcpucapacity).ascii();
  return this->virtualcpucapacity.ascii();
}

void SETTINGS::SetVirtualCPUCapacityStr(const wstring & capacity)
{
  if(( capacity == L"INFINITE" ) || ( capacity == L"<INFINITE>" ))
    this->virtualcpucapacity = Quote(L"INFINITE");
  else
    this->virtualcpucapacity = Int(atol(TBWSTR::wstring2string(capacity).c_str()));
}

int SETTINGS::GetStepSize() const
{
  return this->stepsize;
}

void SETTINGS::SetStepSize(int step)
{
  this->stepsize = step;
}

JitterModeType::JMType SETTINGS::GetJitterMode () const
{
  return this->jittermode;
}

void SETTINGS::SetJitterMode(JitterModeType::JMType newjittermode)
{
  this->jittermode = newjittermode;
}

wstring SETTINGS::GetJitterModeStr() const
{
  if( this->jittermode == JitterModeType::JM_Early )
    return L"Early";
  else if( this->jittermode == JitterModeType::JM_Random )
    return L"Random";
  else if( this->jittermode == JitterModeType::JM_Late )
    return L"Late";
  else
    return L"Early";
}

void SETTINGS::SetJitterModeStr(const wstring & mode)
{
  if( mode == L"Early" )
    this->jittermode = JitterModeType::JM_Early;
  else if( mode == L"Random" )
    this->jittermode = JitterModeType::JM_Random;
  else if( mode == L"Late" )
    this->jittermode = JitterModeType::JM_Late;
  else
    this->jittermode = JitterModeType::JM_Early;
  
}
//#endif //VICE

TYPE_SCHDTP_PrimarySchedulerAlgorithm SETTINGS::GetPrimaryAlgorithm ()
{
//#ifdef VICE
//  return theSystem().GetPrimaryAlgorithm(Nil());
//#else
  return this->primaryAlgorithm;
//#endif // VICE
}

wstring SETTINGS::GetPrimaryAlgorithmStr()
{
  switch( this->primaryAlgorithm.GetTag() )
  {
    case TAG_TYPE_SCHDTP_PureCooperative:
      return L"pure_cooperative";
      break;
    case TAG_TYPE_SCHDTP_InstrnumSlice:
      return L"instruction_number_slice";
      break;
#ifdef VICE
    case TAG_TYPE_SCHDTP_TimeSlice:
      return L"timeslice";
      break;
#endif // VICE
    default:
      return L"Unknown primary scheduling algorithm";
  }
}

void SETTINGS::SetPrimaryAlgorithm(const TYPE_SCHDTP_PrimarySchedulerAlgorithm & newPrimaryAlgorithm)
{
//#ifdef VICE
//  theSystem().SetPrimaryAlgorithm(newPrimaryAlgorithm, Nil());
//#endif // VICE
  this->primaryAlgorithm = newPrimaryAlgorithm;
}

bool SETTINGS::SetPrimaryAlgorithmStr(const wstring & algorithm)
{
  if ((algorithm == L"pure_cooperative") || (algorithm == L"pc"))
  {
    this->SetPureCooperative();
    return true;
  }
#ifdef VICE
  else if ((algorithm == L"timeslice") || (algorithm == L"ts"))
  {
    this->SetTimeSlice();
    return true;
  }
#endif //VICE
  else if ((algorithm == L"instruction_number_slice") || (algorithm == L"in"))
  {
    this->SetInstrnumSlice();
    return true;
  }
  else
    return false;
}

void SETTINGS::SetPureCooperative()
{
  this->primaryAlgorithm = TYPE_SCHDTP_PureCooperative();
}

void SETTINGS::SetInstrnumSlice()
{
  this->primaryAlgorithm = TYPE_SCHDTP_InstrnumSlice();
}

#ifdef VICE
void SETTINGS::SetTimeSlice()
{
  this->primaryAlgorithm = TYPE_SCHDTP_TimeSlice();
}
#endif // VICE

void SETTINGS::PriorityBasedOn ()
{
  this->priorityBased = true;
}

void SETTINGS::PriorityBasedOff ()
{
  this->priorityBased = false;
}

bool SETTINGS::PriorityBased() const
{
  return this->priorityBased;
}

Generic SETTINGS::GetLogArgs() const
{
#ifdef VICE
  return TIMETRACE::GetArgLog();
#else
  return this->logargs;
#endif // VICE
}

void SETTINGS::SetLogArgsAll()
{
#ifdef VICE
  TIMETRACE::AddArgLog(Quote(L"ALL"));
#else
  this->logargs = Quote(L"ALL");
#endif // VICE
}

void SETTINGS::SetLogArgs(const Set & s)
{
#ifdef VICE
  TIMETRACE::ResetArgLog();
  TIMETRACE::AddArgLog(s);
#else
  this->logargs = s;
#endif // VICE
}

wstring SETTINGS::GetLogArgsStr() const
{
  Generic largs (this->GetLogArgs());
  if( largs.IsQuote() )
    return L"<ALL>";
  else
  {
    wstring ret;
    Set s (largs);
    int count = 0;
    Generic g;
    for(bool bb = s.First(g); bb; bb = s.Next(g))
    {
      if( count > 0 )
        ret += L",";
      ret += ASTAUX::ASName2String(g);
      count++;
    }
    return ret;
  }
}

TYPE_AS_Name wstring2asname(const wstring & name)
{
  if( name.length() == 0 )
    return ASTAUX::MkNameFromVoid();

  wstring clsnm;
  wstring opnm;
  int index = 0;
  while( (name[index] != L'`') && index < (int)name.length() )
  {
    clsnm += name[index];
    index++;
  }
  index++; 
  while( index < (int)name.length() )
  {
    opnm += name[index];
    index++;
  }
  Sequence cls_s (clsnm);
  Sequence op_s (opnm);
  TYPE_AS_Ids ids;
  ids.ImpAppend((Generic)cls_s);
  ids.ImpAppend((Generic)op_s);
  return ASTAUX::MkNameFromIds(ids, NilContextId); 
}

void SETTINGS::SetLogArgsStr(const wstring & str)
{
  if( str == L"<ALL>" )
    this->SetLogArgsAll();
  else
  {
    Set s;
    wstring fullopnm = L"";
    int index = 0;
    while( index < (int)str.length() )
    {
      if( str[index] == L',' )
      {
        TYPE_AS_Name asnm (wstring2asname(fullopnm));
        s.Insert(asnm);
        fullopnm = L"";
      }
      else
        fullopnm += str[index];
      index++; 
    }
    if(fullopnm.length() > 0 )
    {
      TYPE_AS_Name asnm (wstring2asname(fullopnm));
      s.Insert(asnm);
    }
    this->SetLogArgs(s);
  }
}

#endif // VDMPP

void SETTINGS::VDMSLmodeOn()
{
 this->vdmslmode = true;
}

void SETTINGS::VDMSLmodeOff()
{
 this->vdmslmode = false;
}

bool SETTINGS::VDMSLmode() const
{
  return this->vdmslmode;
}

void SETTINGS::DefOff()
{
  this->def = false;
}

void SETTINGS::DefOn()
{
  this->def = true;
}

bool SETTINGS::IsDEF() const
{
  return this->def;
}

int SETTINGS::TypeCheck() const
{
  return (this->IsDEF() ? DEF : POS);
}

void SETTINGS::SepOn()
{
 this->sep = true;
}

void SETTINGS::SepOff()
{
 this->sep = false;
}

bool SETTINGS::SEP() const
{
  return this->sep;
}

void SETTINGS::ErrorLevelWRN1()
{
  this->errlevel = StatSem::WRN1;
}

void SETTINGS::ErrorLevelPRF()
{
  this->errlevel = StatSem::PRF;
}

void SETTINGS::SetErrorLevel(int errorlevel)
{
  this->errlevel = errorlevel;
}

int SETTINGS::ErrorLevel() const
{
  return this->errlevel;
}

bool SETTINGS::IsErrorLevelPRF() const
{
  return (this->errlevel == StatSem::PRF);
}

void SETTINGS::ContextOn()
{
  this->context = true;
}

void SETTINGS::ContextOff()
{
  this->context = false;
}

bool SETTINGS::Context() const
{
  return this->context;
}

void SETTINGS::RTErrExceptionOn()
{
  this->rterr_exception = true;
}

void SETTINGS::RTErrExceptionOff()
{
  this->rterr_exception = false;
}

bool SETTINGS::RTErrException() const
{
  return this->rterr_exception;
}

void SETTINGS::SetCorbaOn()
{
  this->uses_corba = true;
}

void SETTINGS::SetCorbaOff()
{
  this->uses_corba = false;
}

bool SETTINGS::UsesCorba() const
{
  return this->uses_corba;
}

void SETTINGS::SetName(const SEQ<Char> & name)
{
  this->myname = name;
}

bool SETTINGS::Profile() const
{
  return this->profile;
}

void SETTINGS::ProfileOn()
{
  this->profile = true;
}

void SETTINGS::ProfileOff()
{
  this->profile = false;
}

SET<TYPE_AS_Name> SETTINGS::GetNoCheck() const
{
  return this->nocheck;
}

void SETTINGS::AddNoCheck(const TYPE_AS_Name & nm)
{
#ifdef VDMPP
  Set nm_s (theState().GetClasses().Dom());
  if (nm_s.InSet(nm))
    this->nocheck.Insert(nm);
#endif // VDMPP
}

void SETTINGS::RemoveNoCheck(const TYPE_AS_Name & nm)
{
#ifdef VDMPP
  if (this->nocheck.InSet(nm))
    this->nocheck.RemElem(nm);
#endif // VDMPP
}

bool SETTINGS::DoCheck() const
{
#ifdef VDMPP
  if (this->docheck_enabled)
  {
    TYPE_GLOBAL_OrigCl ocl (theStackMachine().GetCurCl());
    if (ocl.Is(TAG_TYPE_AS_Name))
    {
      return !this->nocheck.InSet(ocl);
    }
  }
#endif // VDMPP
  return true;
}

void SETTINGS::AddAllTypeAllowed(const TYPE_AS_Name & nm)
{
  this->allTypeAllowed.Insert(nm);
}

bool SETTINGS::IsAllTypeAllowed(const TYPE_AS_Name & nm)
{
  return this->allTypeAllowed.InSet(nm);
}
