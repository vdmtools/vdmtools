/***
*  * WHAT
*  *    Command queue for the CORBA API calls (definition)
*  * ID
*  *    $Id: cmd_queue.h,v 
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __cmd_queue_h__
#define __cmd_queue_h__

#include <metaiv.h>
#include <omniconfig.h>
#include <omnithread.h>
#include "corba_api.h"
#include "metaiv_impl.h"

// define the tags for the commands

#define WRONG_ARG_NUM           L"Wrong number of arguments"
#define INVALID_ARG             L"Invalid argument"

#define VDM_PROJECT_BASE        100
#define VDM_PROJECT_NEW         VDM_PROJECT_BASE + 1
#define VDM_PROJECT_OPEN        VDM_PROJECT_BASE + 2
#define VDM_PROJECT_SAVE        VDM_PROJECT_BASE + 3
#define VDM_PROJECT_SAVEAS      VDM_PROJECT_BASE + 4
#define VDM_PROJECT_GETMODULES  VDM_PROJECT_BASE + 5
#define VDM_PROJECT_GETFILES    VDM_PROJECT_BASE + 6
#define VDM_PROJECT_ADDFILE     VDM_PROJECT_BASE + 7
#define VDM_PROJECT_REMOVEFILE  VDM_PROJECT_BASE + 8

#define VDM_REPOSITORY_BASE             200
#define VDM_REPOSITORY_FILESOFMODULE    VDM_REPOSITORY_BASE + 1
#define VDM_REPOSITORY_STATUS           VDM_REPOSITORY_BASE + 2
#define VDM_REPOSITORY_SUPERCLASSES     VDM_REPOSITORY_BASE + 3
#define VDM_REPOSITORY_SUBCLASSES       VDM_REPOSITORY_BASE + 4
#define VDM_REPOSITORY_USES             VDM_REPOSITORY_BASE + 5
#define VDM_REPOSITORY_USEDBY           VDM_REPOSITORY_BASE + 6
#define VDM_REPOSITORY_GETCURRENTMOD    VDM_REPOSITORY_BASE + 7
#define VDM_REPOSITORY_POPMODULE        VDM_REPOSITORY_BASE + 8
#define VDM_REPOSITORY_PUSHMODULE       VDM_REPOSITORY_BASE + 9

#define VDM_INTERPRETER_BASE            300
#define VDM_INTERPRETER_INITIALIZE      VDM_INTERPRETER_BASE + 1
#define VDM_INTERPRETER_DOEVAL          VDM_INTERPRETER_BASE + 2
#define VDM_INTERPRETER_EVALCMD         VDM_INTERPRETER_BASE + 3
#define VDM_INTERPRETER_SETBREAKBYNAME  VDM_INTERPRETER_BASE + 4
#define VDM_INTERPRETER_SETBREAKBYPOS   VDM_INTERPRETER_BASE + 5
#define VDM_INTERPRETER_DELETEBREAK     VDM_INTERPRETER_BASE + 6
#define VDM_INTERPRETER_PARSEANDDEBUG   VDM_INTERPRETER_BASE + 7
#define VDM_INTERPRETER_DEBUGSTEP       VDM_INTERPRETER_BASE + 8
#define VDM_INTERPRETER_DEBUGSTEPIN     VDM_INTERPRETER_BASE + 9
#define VDM_INTERPRETER_DEBUGSINGLESTEP VDM_INTERPRETER_BASE + 10
#define VDM_INTERPRETER_DEBUGCONTINUE   VDM_INTERPRETER_BASE + 11


#define VDM_CODEGENERATOR_BASE              400
#define VDM_CODEGENERATOR_CODEGENERATE      VDM_CODEGENERATOR_BASE + 1
#define VDM_CODEGENERATOR_CODEGENERATEJAVA  VDM_CODEGENERATOR_BASE + 2
#define VDM_CODEGENERATOR_CODEGENERATECPP   VDM_CODEGENERATOR_BASE + 3

#define VDM_PARSER_BASE                 500
#define VDM_PARSER_PARSE                VDM_PARSER_BASE + 1

#define VDM_TYPECHECKER_BASE            600
#define VDM_TYPECHECKER_TYPECHECK       VDM_TYPECHECKER_BASE + 1

#define VDM_PRETTYPRINTER_BASE          700
#define VDM_PRETTYPRINTER_PRETTYPRINT   VDM_PRETTYPRINTER_BASE + 1

#define VDM_ERRORS_BASE                 800
#define VDM_ERRORS_GETERRORS            VDM_ERRORS_BASE + 1
#define VDM_ERRORS_GETWARNINGS          VDM_ERRORS_BASE + 2
#define VDM_ERRORS_GETSTATUS            VDM_ERRORS_BASE + 3
#define VDM_ERRORS_SHOWMSG              VDM_ERRORS_BASE + 4

class APICommand
{
 private:
  unsigned int _tag;
  Sequence     _params;

 public:
  /**
   * Constructor for APICommand
   * @param tag Tag that is used to identify the command
   * @param params Parameters needed for execution
   */
  APICommand (unsigned int tag, Sequence params): _tag (tag), _params (params) { }

  /**
   * Copy Constructor
   */
  APICommand (APICommand& command): _tag (command.GetTag ()), _params (command.GetParams ()) { }
  
  /**
   * Selectors for private members
   */
  unsigned int GetTag () { return _tag; }
  Sequence GetParams () { return _params; }

};


class APICommandQueue;

class UserInputThread: public omni_thread
{
protected:
  APICommandQueue* _queue;
public:
  UserInputThread (APICommandQueue* queue);
  
private:
  void* run_undetached (void* arg);
};

/**
 * Class used to make delayed calls to the toolbox
 * possible (since the GUI cannot be accessed from
 * 2 different threads, the main thread has to
 * execute the queued command).
 * The queue has only one element.
 */
class APICommandQueue
{
 private:
  // contains the current command
  APICommand* _currentCommand;
  wstring*     _currentStringCommand;

  // this will be set to true when
  // a wstring command was L"quit"
  bool        _quit;

  // the user input thread
  UserInputThread* _inputThread;

  // _result contains the result of a command after execution
  Generic*    _result;

  // contains exception value
  wstring     _exceptionStr;
  int        _exceptionNum;

  // _exceptionType is NONE if no exception occured
  // when executing the command
  enum ExceptionType {NONE, VDM, API};
  ExceptionType  _exceptionType;
  
  // locks and semaphores for thread synchronisation
  omni_mutex*        _queueLock;
  omni_mutex*        _commandLock;
  omni_condition*    _commandCondition;

  omni_semaphore*    _waitSemaphore;
  
  static APICommandQueue*   _instance;

 public:
  /**
   * Constructor
   */
  APICommandQueue ();
  
  /**
   * Destructor
   */
  ~APICommandQueue ();

  /**
   * Singleton Instance-Method
   */
  static APICommandQueue* GetInstance ();

  /**
   * This method does not return until the command
   * has been executed by the main-thread.
   * @param command The command that shall be executed.
   */
  Generic EnqueueCommand (APICommand &command);

  /**
   * This method does not return until the 
   * command has been executed by the main thread.
   * @param cmd the command-wstring that should be executed
   */
  Bool EnqueueStringCommand (const wstring & cmd);

  /**
   * This method has to be called by the main thread
   * NEVER CALL THIS METHOD DIRECTLY FROM THE CORBA THREAD
   */
  void ExecuteQueuedCommand ();

  /**
   * TimedWaitForCommand blocks, until SignalCommand is called
   * @param sec max seconds to wait
   * @param nsec max nanoseconds to wait
   * @returns true if SignalCommand was called, false if 
   * wait timed out

   bool TimedWaitForCommand (unsigned long sec, unsigned long nsec);
  */
  
  /**
   * WaitForCommand blocks until SignalCommand is called.
   */
  void WaitForCommand ();

  void SignalCommand ();

  /**
   * Starts waiting for user input on
   * the command line
   */
  void StartUserInputLoop ();

  bool QuitCommandExecuted () { return _quit; }

 protected:
  /**
   * Methods called in ExecuteQueuedCommand
   */
  void VDMProject_New ();
  void VDMProject_Open (const Sequence & params);
  void VDMProject_SaveAs (const Sequence & params);
  Generic* VDMProject_GetModules ();
  Generic* VDMProject_GetFiles ();
  void VDMProject_AddFile (const Sequence & params);
  void VDMProject_RemoveFile (const Sequence & params);

  Generic* VDMModuleRepos_FilesOfModule (const Sequence & params);
  Generic* VDMModuleRepos_Status (const Sequence & params);
  Generic* VDMModuleRepos_SuperClasses (const Sequence & params);
  Generic* VDMModuleRepos_SubClasses (const Sequence & params);
  Generic* VDMModuleRepos_Uses (const Sequence & params);
  Generic* VDMModuleRepos_UsedBy (const Sequence & params);
  Generic* VDMModuleRepos_GetCurrentModule ();
  void VDMModuleRepos_PopModule ();
  void VDMModuleRepos_PushModule (const Sequence & params);

  void VDMInterpreter_Initialize ();
  Generic* VDMInterpreter_DoEval (const Sequence & params);
  void VDMInterpreter_EvalCmd (const Sequence & params);
  Generic* VDMInterpreter_SetBreakPointByName (const Sequence & params);
  Generic* VDMInterpreter_SetBreakPointByPos (const Sequence & params);
  void VDMInterpreter_DeleteBreakPoint (const Sequence & params);
  Generic* VDMInterpreter_ParseAndDebug (const Sequence & params);
  Generic* VDMInterpreter_DebugStep ();
  Generic* VDMInterpreter_DebugStepIn ();
  Generic* VDMInterpreter_DebugSingleStep ();
  Generic* VDMInterpreter_DebugContinue ();

  Generic* VDMCodeGenerator_CodeGenerate (const SEQ<TYPE_ProjectTypes_ModuleName> & params,
                                          const Quote & targetLang);

  Generic* VDMParser_Parse (const SEQ<TYPE_ProjectTypes_FileName> & params);

  Generic* VDMTypeChecker_TypeCheck (const SEQ<TYPE_ProjectTypes_ModuleName> & params);

  Generic* VDMPrettyPrinter_PrettyPrint (const SEQ<TYPE_ProjectTypes_FileName> & params);

  Generic* VDMErrors_GetErrors ();
  Generic* VDMErrors_GetWarnings ();
  void VDMErrors_ShowMsg ();
  Generic* VDMErrors_GetStatus ();

private:
  void checkValidFile (const Generic & fileName, const wstring & func);
  void checkValidModuleName(const Generic & moduleName, const wstring & func);
};

#endif // __cmd_queue_h__

