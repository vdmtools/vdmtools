/***
*  * WHAT
*  *    Settings
*  *
*  * FILE
*  *    $Source: $
*  * VERSION
*  *    $Revision: $
*  * DATE
*  *    $Date: $
*  * FORMAT
*  *    $State: Exp $
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    OS + $Author: $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __settings_h__
#define __settings_h__

#include "metaiv.h"
#include "asquotes.h"
#include "ProjectTypes.h"
#ifdef VDMPP
#include "SCHDTP.h"
#endif // VDMPP

class JitterModeType
{
public:
  // Which type of CallStack item is it
  enum JMType {JM_Early, JM_Random, JM_Late};
};

class SETTINGS
{
public:
  SETTINGS();
  void InitSettings();

#ifdef VDMPP
  int GetMaxInstr () const;
  void SetMaxInstr(int);
  int GetTaskSwitch () const;
  void SetTaskSwitch(int);
//#ifdef VICE
  int GetTimeSlice () const;
  void SetTimeSlice(int);
  int GetTimeFactor () const;
  void SetTimeFactor(int);
  int GetDefaultCapacity () const;
  void SetDefaultCapacity(int);
  Generic GetVirtualCPUCapacity() const; 
  void SetVirtualCPUCapacity(const Generic &);
  void SetVirtualCPUCapacityInfinite();
  bool IsVirtualCPUCapacityInfinite();
  wstring GetVirtualCPUCapacityStr() const; 
  void SetVirtualCPUCapacityStr(const wstring &);
  int GetStepSize() const;
  void SetStepSize(int);
  JitterModeType::JMType GetJitterMode () const;
  void SetJitterMode(JitterModeType::JMType);
  wstring GetJitterModeStr () const;
  void SetJitterModeStr(const wstring &);
//#endif // VICE
  TYPE_SCHDTP_PrimarySchedulerAlgorithm GetPrimaryAlgorithm ();
  wstring GetPrimaryAlgorithmStr();
  void SetPrimaryAlgorithm(const TYPE_SCHDTP_PrimarySchedulerAlgorithm &);
  bool SetPrimaryAlgorithmStr(const wstring &);
  void SetPureCooperative();
  void SetInstrnumSlice();
#ifdef VICE
  void SetTimeSlice();
#endif // VICE
  void PriorityBasedOn ();
  void PriorityBasedOff ();
  bool PriorityBased() const;

  Generic GetLogArgs() const;
  void SetLogArgsAll();
  void SetLogArgs(const Set &);
  wstring GetLogArgsStr() const;
  void SetLogArgsStr(const wstring &);
 
#endif // VDMPP

  void VDM10On();
  void VDM10Off();
  bool VDM10() const;
  void DtcOn();
  void DtcOff();
  bool DTC() const;
  void PreOn();
  void PreOff();
  bool PreCheck() const;
  void PostOn();
  void PostOff();
  bool PostCheck() const;
  void InvOn();
  void InvOff();
  bool INV() const;
  void AssertOn();
  void AssertOff();
  bool Assertion() const;
  void PrintFormatOn();
  void PrintFormatOff();
  bool PrintFormat() const;
  void RandomOn(int);
  void RandomOff();
  int Random() const;
  void ContextOn();
  void ContextOff();
  bool Context() const;
  void RTErrExceptionOn();
  void RTErrExceptionOff();
  bool RTErrException() const;
  void SetExpression(const Generic & g) { this->expression = g; };
  Generic GetExpression() const { return this->expression; };

// Type Check Options
public:
  void VDMSLmodeOn();
  void VDMSLmodeOff();
  bool VDMSLmode() const;
  void DefOn();
  void DefOff();
  bool IsDEF() const;
  int TypeCheck() const;
  void SepOn();
  void SepOff();
  bool SEP() const;
  void ErrorLevelWRN1();
  void ErrorLevelPRF();
  void SetErrorLevel(int errorlevel);
  int ErrorLevel() const;
  bool IsErrorLevelPRF() const;

public:
  void SetCorbaOn();
  void SetCorbaOff();
  bool UsesCorba() const;

public:
  void SetCGRTI(bool rti) { this->cg_rti = rti; };
  bool GetCGRTI() const { return this->cg_rti; };
  void SetCGCheckPrePost(bool c) { this->cg_checkprepost = c; };
  bool GetCGCheckPrePost() const { return this->cg_checkprepost; };
  void SetCFlag(int f) { this->c_flag = f; };
  int GetCFlag() const { return this->c_flag; };
  void SetIndex(int i) { this->index = i; };
  int GetIndex() const { return this->index; };
  void SetPrettyPrintRTI(bool rti) { this->prettyprint_rti = rti; };
  bool GetPrettyPrintRTI() const { return this->prettyprint_rti; };

  void SetJCGSkel(bool b) { this->jcg_skel = b; };
  bool GetJCGSkel() const { return this->jcg_skel; };
  void SetJCGGenPrePost(bool b) { this->jcg_genprepost = b; };
  bool GetJCGGenPrePost() const { return this->jcg_genprepost; };
  void SetJCGTypes(bool b) { this->jcg_types = b; };
  bool GetJCGTypes() const { return this->jcg_types; };
  void SetJCGSmallTypes(bool b) { this->jcg_smalltypes = b; };
  bool GetJCGSmallTypes() const { return this->jcg_smalltypes; };
  void SetJCGLongs(bool b) { this->jcg_longs = b; };
  bool GetJCGLongs() const { return this->jcg_longs; };
  void SetJCGPackage(const Generic & g) { this->jcg_package = g; };
  Generic GetJCGPackage() const { return this->jcg_package; };
  void SetJCGConcur(bool b) { this->jcg_concur = b; };
  bool GetJCGConcur() const { return this->jcg_concur; };
  void SetJCGCheckPrePost(bool b) { this->jcg_checkprepost = b; };
  bool GetJCGCheckPrePost() const { return this->jcg_checkprepost; };
  void SetJCGInterfaces(const SET<TYPE_ProjectTypes_ModuleName> & s) { this->jcg_interfaces = s; };
  SET<TYPE_ProjectTypes_ModuleName> GetJCGInterfaces() const { return this->jcg_interfaces; };
  bool GetJCGVDMPrefix() const { return this->jcg_vdmprefix; } ;
  void SetJCGVDMPrefix(bool b) { this->jcg_vdmprefix = b; };
  bool GetJCGUseCodeDir() const { return this->jcg_usecodedir; };
  void SetJCGUseCodeDir(bool b) { this->jcg_usecodedir = b; };
  Generic GetJCGCodeDir() const { return this->jcg_codedir; };
  void SetJCGCodeDir(const Generic & g) { this->jcg_codedir = g; };
  bool GetJCGNeedBackup() const { return this->jcg_needbackup; };
  void SetJCGNeedBackup(bool b) { this->jcg_needbackup = b; };
  bool GetJCGHackParser() const { return this->jcg_hackparser; };
  void SetJCGHackParser(bool b) { this->jcg_hackparser = b; };
  void AddJCGBoolean(const TYPE_AS_Name & nm) { this->jcg_boolean.Insert(nm); };
  void SetJCGBoolean(const SET<TYPE_AS_Name> & nm_s) { this->jcg_boolean = nm_s; };
  SET<TYPE_AS_Name> GetJCGBoolean() const { return this->jcg_boolean; }
  void AddJCGInteger(const TYPE_AS_Name & nm) { this->jcg_integer.Insert(nm); };
  void SetJCGInteger(const SET<TYPE_AS_Name> & nm_s) { this->jcg_integer = nm_s; };
  SET<TYPE_AS_Name> GetJCGInteger() const { return this->jcg_integer; }
  void AddJCGDouble(const TYPE_AS_Name & nm) { this->jcg_double.Insert(nm); };
  void SetJCGDouble(const SET<TYPE_AS_Name> & nm_s) { this->jcg_double = nm_s; };
  SET<TYPE_AS_Name> GetJCGDouble() const { return this->jcg_double; }
  
public:
  bool isDLModuleEnabled() const;
  bool canCG() const;
  bool canJavaCG() const;
  bool canJ2V() const;
  void SetName(const SEQ<Char> & name);

  bool Profile() const;
  void ProfileOn();
  void ProfileOff();

  bool UseTranslation() const { return this->translation; };
  void SetTranslation() { this->translation = true; };
  void UnsetTranslation() { this->translation = false; };

  bool CasesCheck() const { return this->casescheck; };
  void CasesOn() { this->casescheck = true; };
  void CasesOff() { this->casescheck = false; };

  bool Measure() const { return this->measure; };
  void MeasureOn() { this->measure = true; };
  void MeasureOff() { this->measure = false; };

  bool ElapsedTime() const { return this->elapsedtime; };
  void ElapsedTimeOn() { this->elapsedtime = true; };
  void ElapsedTimeOff() { this->elapsedtime = false; };

  bool SetOne() const { return this->setone; };
  void SetOneOn() { this->setone = true; };
  void SetOneOff() { this->setone = false; };

public:
  SET<TYPE_AS_Name> GetNoCheck() const;
  void AddNoCheck(const TYPE_AS_Name & nm);
  void RemoveNoCheck(const TYPE_AS_Name & nm);
  bool DoCheck() const;
  void DoCheckOn() { this->docheck_enabled = true; };
  void DoCheckOff() { this->docheck_enabled = false; };

  SET<TYPE_AS_Name> GetAllTypeAllowed() { return this->allTypeAllowed; };
  void AddAllTypeAllowed(const TYPE_AS_Name & nm);
  bool IsAllTypeAllowed(const TYPE_AS_Name & nm);

  void CallLogOn() { this->calllog = true; };
  void CallLogOff() { this->calllog = false; };
  bool CallLog() { return this->calllog; };
  void SetCallLogFileName(const wstring & fnm) { this->calllogfnm = fnm; };
  wstring GetCallLogFileName() { return this->calllogfnm; };

private:
  bool vdm10;
// Eval Option
  bool dtc;            // dynamic typechecking
  bool precheck;       // pre-cond checking
  bool postcheck;      // post-cond checking
  bool invar;          // invariant checking
  bool assertion;      // assertioni checking
  bool print_format;   //pretty print format for vdmsl values on.
                       //Off is -1

  int random;          // Seed for random generator.

#ifdef VDMPP
  int maxinstr;        //Number of instructions to execute
                       //in each thread, before swapping to another thread.
  int maxTime;
  int defaultcapacity;
  Generic virtualcpucapacity;
  int stepsize;
  JitterModeType::JMType jittermode;

  int priorityBased;    //Priority based scheduling for VDM++
  int taskSwitch;      //Task switching constant for VDM++ (VICE)
  TYPE_SCHDTP_PrimarySchedulerAlgorithm primaryAlgorithm;
                       //Primary scheduling algorithm
  int timeFactor;
  Generic logargs;
#endif //VDMPP

  bool context;        // Turn off context info in interpreter
                       //-purely for speed!

  bool rterr_exception;
  Generic expression;

/////////////////
// Type Checker Option
  bool vdmslmode;      //typechecking in restricted vdm-sl mode off
  bool def;            //Definitely well-formedness for typechecking off.
  bool sep;            // Seperation of warnings and errors from typecheker
  int errlevel;        //Restricted number of warning and
                       //errors from typechecker
                       //given. To see all errors and warnings
                       //set errlevel

// Pretty Printer Option
  int index;           //Index for latex generator set to no index.
                       //Can be either 0 (no index),
                       //1  (index for definitions) or
                       //2 (index for definitions and used definitions)
  bool prettyprint_rti; //Coloring coveration for latex off.

// C++ Code generator options
  bool cg_rti;         //Run time position information for code
                       //generation off
  bool cg_checkprepost; // Check of pre and post conditions

  int c_flag;         // Output consistency check to file if > 0.

// Java code generator options

  bool jcg_skel;
  bool jcg_genprepost;
  bool jcg_types;
  bool jcg_smalltypes;
  bool jcg_longs;
  Generic jcg_package;
  bool jcg_concur;
  bool jcg_checkprepost;
  SET<TYPE_ProjectTypes_ModuleName> jcg_interfaces;
  bool jcg_vdmprefix;
  bool jcg_usecodedir;
  Generic jcg_codedir;
  bool jcg_needbackup;
  bool jcg_hackparser;
  SET<TYPE_AS_Name> jcg_boolean;
  SET<TYPE_AS_Name> jcg_integer;
  SET<TYPE_AS_Name> jcg_double;

  bool uses_corba;

  bool uses_dlmodule;
  bool uses_cg;
  bool uses_javacg;
  bool uses_j2v;
  SEQ<Char> myname;

  bool profile;

  bool translation;

  bool collection;

  // 
  SET<TYPE_AS_Name> nocheck;
  bool docheck_enabled;

  SET<TYPE_AS_Name> allTypeAllowed;

  bool casescheck;
  bool measure;
  bool elapsedtime;
  bool setone;

  bool calllog;
  wstring calllogfnm;
};

extern SETTINGS Settings;

#endif // __settings_h__
