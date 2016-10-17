/***
*  * WHAT
*  *    Command queue for the CORBA API calls (implementation)
*  * ID
*  *    $Id: cmd_queue.cc,v 
*  * PROJECT
*  *    Toolbox
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <assert.h>

#include "cmd_queue.h"

#include "tools.h"
#include "projectval.h"
#include "tbutils.h"
#include "tb_wstring.h"
#include "corba_api.h"
#include "BaseTools.h"

// initialize commandqueue member variables
APICommandQueue* APICommandQueue::_instance = (APICommandQueue*)NULL;

static APICommandQueue theCommandQueue;

/*
 * Constructor of UserInputThread
 */
UserInputThread::UserInputThread (APICommandQueue* queue): omni_thread (), _queue (queue) 
{
  start_undetached ();
}

/*
 * run_undetached
 * Command Line Input Loop on CORBA environment
 */
void* UserInputThread::run_undetached (void* arg)
{
  Bool cont (true);
  while (cont.GetValue ())
  {
    wstring input (TOOLS::ReadLine ());
    cont = _queue->EnqueueStringCommand (input);
  }
  return NULL;
}

/**
 * Initialize the queue
 */
APICommandQueue::APICommandQueue ()
{
  if (APICommandQueue::_instance == (APICommandQueue*)NULL)
  {
    // initialize the semaphores and locks
    this->_queueLock = new omni_mutex ();
    
    this->_commandLock = new omni_mutex ();
    this->_commandCondition = new omni_condition (_commandLock);

    this->_waitSemaphore = new omni_semaphore (0);
    
    // no command waiting at the moment, no result, no exception
    this->_currentCommand = (APICommand*)NULL;
    this->_currentStringCommand = (wstring*)NULL;
    this->_result = (Generic*)NULL;
    this->_exceptionType = NONE;

    this->_quit = false;
    this->_inputThread = (UserInputThread*)NULL;
  }
}

/**
 * Destructor for command queue
 */
APICommandQueue::~APICommandQueue ()
{
  if (this->_inputThread != (UserInputThread*)NULL)
  {
    this->_inputThread->join (0);
    delete _inputThread;
  }

  if ((this->_currentCommand != (APICommand*)NULL) || (this->_currentStringCommand != (wstring*)NULL))
  // oops - somebody closed the queue before the command was evaluated!
  {
    delete this->_currentCommand; 
    delete this->_currentStringCommand;
    vdm_err << L"PANIC! Somebody killed the CommandQueue before the command was executed!" << endl;
    this->_commandCondition->broadcast ();
  }

  delete this->_commandCondition;
  delete this->_commandLock;
  delete this->_queueLock;
  delete this->_waitSemaphore;

  if (this->_result != (Generic*)NULL)
    delete this->_result;
}

/**
 * Singleton Instance-Method
 */
APICommandQueue* APICommandQueue::GetInstance ()
{
  if (APICommandQueue::_instance == (APICommandQueue*)NULL)
    APICommandQueue::_instance = new APICommandQueue ();

  return APICommandQueue::_instance;
}

/**
 * Waits <sec> seconds plus <nsec> nanoseconds for
 * SignalCommand
 
bool APICommandQueue::TimedWaitForCommand (unsigned long sec, unsigned long nsec)
{
  unsigned long abs_sec, abs_nsec;

  omni_thread::get_time (&abs_sec, &abs_nsec, sec, nsec);
  if (_waitSemaphore->timedwait (abs_sec, abs_nsec) == ETIMEDOUT)
    return false;

  // let EnqueueCommand block before starting the evaluation
  omni_thread::yield (); 
  return true;
}
ETIMEDOUT isn't defined anywhere, so I can't use this */

/**
 * Waits an infinite amount of time for Signalcommand
 */
void APICommandQueue::WaitForCommand ()
{
  this->_waitSemaphore->wait ();

  // let EnqueueCommand block before starting the evaluation
  // this is a workaround, but the only way to make this work
  // on all systems, regardless of the thread scheduling mechanism
}

/**
 * Wakes up threads waiting for a command
 */
void APICommandQueue::SignalCommand ()
{
  this->_waitSemaphore->post ();
}

/**
 * Starts waiting for user input
 * should only be called once!
 * returns immediately
 */ 
void APICommandQueue::StartUserInputLoop ()
{
  if (this->_inputThread == (UserInputThread*)NULL)
    this->_inputThread = new UserInputThread (this);
}

/**
 * This command is used by the API to execute
 * commands in the main thread (so that the
 * GUI is not accessed from 2 different threads)
 */
Generic APICommandQueue::EnqueueCommand (APICommand& command) 
{
  // lock Queue so that other calls have to wait
  this->_queueLock->acquire ();

  assert (this->_currentCommand == (APICommand*)NULL);
  this->_currentCommand = new APICommand (command);

  this->_commandLock->acquire ();
  this->SignalCommand ();
  this->_commandCondition->wait (); // wait now until the command is processed
  this->_commandLock->release ();

  // rethrow exception
  if (this->_exceptionType != NONE)
  {
    // clean up !!!
    if (this->_result != (Generic*)NULL)
      delete this->_result;
    
    this->_queueLock->release ();
    
    if (this->_exceptionType == API)
      ThrowApiError (this->_exceptionStr.c_str (), L"APICommandQueue::EnqueueCommand");
    else
      throw VDM::VDMError (this->_exceptionNum);
  }

  Generic res;
  if (this->_result != (Generic*)NULL)
  {
    res = *_result;
    delete this->_result;
    this->_result = (Generic*)NULL;
  }

  // let other calls enqueue their commands again
  this->_queueLock->release ();
  return res;
}

/*
 * EnqueueStringCommand
 * cmd - command line input
 */
Bool APICommandQueue::EnqueueStringCommand (const wstring & cmd)
{
  // lock Queue so that other calls have to wait
  this->_queueLock->acquire ();
  
  assert (this->_currentStringCommand == (wstring*)NULL);
  this->_currentStringCommand = new wstring (cmd);

  this->_commandLock->acquire ();
  this->SignalCommand ();
  this->_commandCondition->wait (); // wait now until the command is processed
  this->_commandLock->release ();

  Bool res (false);
  if (this->_result != (Generic*)NULL)
  {
    res = Bool(*this->_result);
    delete this->_result;
    this->_result = (Generic*)NULL;
  }

  // let other calls enqueue their commands again
  this->_queueLock->release ();
  return res;
} 

/**
 * called from within the main thread to 
 * execute the queued command
 * NEVER CALL THIS FROM WITHIN THE CORBA-PROCESS!
 */
void APICommandQueue::ExecuteQueuedCommand ()
{
  if ((this->_currentCommand == (APICommand*)NULL) && (this->_currentStringCommand == (wstring*)NULL))
    return; // nothing to do...
  
  this->_commandLock->acquire ();

  if (this->_currentStringCommand != (wstring*)NULL)
  {
    Bool cont (ToolMediator::BTools()->vdm_ExecuteCommand(PTAUX::mk_ToolCommand (*this->_currentStringCommand)));
    vdm_log.flush ();
    vdm_err.flush ();

    this->_quit = !(cont.GetValue ());

    this->_result = new Generic (cont);
    delete _currentStringCommand;
    this->_currentStringCommand = (wstring*)NULL;
      
    this->_commandLock->release ();
    // signal semaphore
    this->_commandCondition->signal ();
    return;
  }

  this->_exceptionType = NONE;

  try
  {
    switch (this->_currentCommand->GetTag ()) {
      case VDM_PROJECT_NEW: {
        VDMProject_New ();
        break;
      }
      case VDM_PROJECT_OPEN: {
        VDMProject_Open (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_PROJECT_SAVEAS: {
        VDMProject_SaveAs (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_PROJECT_GETMODULES: {
        this->_result = VDMProject_GetModules ();
        break;
      }
      case VDM_PROJECT_GETFILES: {
        this->_result = VDMProject_GetFiles ();
        break;
      }
      case VDM_PROJECT_ADDFILE: {
        VDMProject_AddFile (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_PROJECT_REMOVEFILE: {
        VDMProject_RemoveFile (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_REPOSITORY_FILESOFMODULE: {
        this->_result = VDMModuleRepos_FilesOfModule (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_REPOSITORY_STATUS: {
        this->_result = VDMModuleRepos_Status (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_REPOSITORY_SUPERCLASSES: {
        this->_result = VDMModuleRepos_SuperClasses (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_REPOSITORY_SUBCLASSES: {
        this->_result = VDMModuleRepos_SubClasses (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_REPOSITORY_USES: {
        this->_result = VDMModuleRepos_Uses (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_REPOSITORY_USEDBY: {
        this->_result = VDMModuleRepos_UsedBy (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_REPOSITORY_GETCURRENTMOD: {
        this->_result = VDMModuleRepos_GetCurrentModule ();
        break;
      }
      case VDM_REPOSITORY_POPMODULE: {
        VDMModuleRepos_PopModule ();
        break;
      }
      case VDM_REPOSITORY_PUSHMODULE: {
        VDMModuleRepos_PushModule (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_INTERPRETER_INITIALIZE: {
        VDMInterpreter_Initialize ();
        break;
      }
      case VDM_INTERPRETER_DOEVAL: {
        this->_result = VDMInterpreter_DoEval (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_INTERPRETER_EVALCMD: {
        VDMInterpreter_EvalCmd (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_INTERPRETER_SETBREAKBYNAME: {
        this->_result = VDMInterpreter_SetBreakPointByName (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_INTERPRETER_SETBREAKBYPOS: {
        this->_result = VDMInterpreter_SetBreakPointByPos (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_INTERPRETER_DELETEBREAK: {
        VDMInterpreter_DeleteBreakPoint (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_INTERPRETER_PARSEANDDEBUG: {
        this->_result = VDMInterpreter_ParseAndDebug (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_INTERPRETER_DEBUGSTEP: {
        this->_result = VDMInterpreter_DebugStep ();
        break;
      }
      case VDM_INTERPRETER_DEBUGSTEPIN: {
        this->_result = VDMInterpreter_DebugStepIn ();
        break;
      }
      case VDM_INTERPRETER_DEBUGSINGLESTEP: {
        this->_result = VDMInterpreter_DebugSingleStep ();
        break;
      }
      case VDM_INTERPRETER_DEBUGCONTINUE: {
        this->_result = VDMInterpreter_DebugContinue ();
        break;
      }
      case VDM_CODEGENERATOR_CODEGENERATEJAVA: {
        this->_result = VDMCodeGenerator_CodeGenerate (this->_currentCommand->GetParams (), Quote(L"JAVA"));
        break;
      }
      case VDM_CODEGENERATOR_CODEGENERATECPP: {
        this->_result = VDMCodeGenerator_CodeGenerate (this->_currentCommand->GetParams (), Quote(L"CPP"));
        break;
      }
      case VDM_PARSER_PARSE: {
        this->_result = VDMParser_Parse (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_TYPECHECKER_TYPECHECK: {
        this->_result = VDMTypeChecker_TypeCheck (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_PRETTYPRINTER_PRETTYPRINT: {
        this->_result = VDMPrettyPrinter_PrettyPrint (this->_currentCommand->GetParams ());
        break;
      }
      case VDM_ERRORS_GETERRORS: {
        this->_result = VDMErrors_GetErrors ();
        break;
      }
      case VDM_ERRORS_GETWARNINGS: {
        this->_result = VDMErrors_GetWarnings ();
        break;
      }
      case VDM_ERRORS_GETSTATUS: {
        this->_result = VDMErrors_GetStatus ();
        break;
      }
      case VDM_ERRORS_SHOWMSG: {
        VDMErrors_ShowMsg ();
        break;
      }
      default: {
        this->_exceptionType = API;
        this->_exceptionStr = L"Unknown command tag found in command queue";
      }
    }
  }
  catch (ToolboxAPI::APIError &ex)
  {
    this->_exceptionType = API;
    this->_exceptionStr = TBWSTR::string2wstring(string(ex.msg));
  }
  catch (VDM::VDMError &ex)
  {
    this->_exceptionType = VDM;
    this->_exceptionNum = ex.err;
  }
  catch (...)
  {
    this->_exceptionType = API;
    this->_exceptionStr = L"Unknown exception occured during execution of API command";
  }

  delete this->_currentCommand;
  this->_currentCommand = (APICommand*)NULL;

  this->_commandLock->release ();
  this->_commandCondition->signal ();
}

/**
 * Following methods are used for execution of the queued commands
 */



/*******************************************************
 * Project methods
 ******************************************************/

/**
 * Create new project
 */
void APICommandQueue::VDMProject_New ()
{
  ToolMediator::NewUnnamedProject ();
}

/**
 * Open project
 * @param params Sequence with first element of type Record 
 * created by mk_FileName
 */
void APICommandQueue::VDMProject_Open (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMProject_Open");

  const Generic & fileName (params[1]);
  checkValidFile (fileName, wstring(L"VDMProject::Open ()"));

  ToolMediator::Open(fileName);
}

/**
 * Save the current project as ...
 * @param params Sequence with first element of type 
 * Record created by mk_FileName
 */
void APICommandQueue::VDMProject_SaveAs (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMProject_SaveAs");

  const Generic & fileName (params[1]);
  if (!fileName.Is(TAG_TYPE_ProjectTypes_FileName))
    ThrowApiError(INVALID_ARG, L"APICommandQueue::VDMProject_SaveAs");

  ToolMediator::SaveAs (fileName);
}

/**
 * Get the module-list from the repository
 * @returns a Set of modulenames
 */
Generic* APICommandQueue::VDMProject_GetModules ()
{
  return new Generic((ToolMediator::AllModules ()));
}

/**
 * Get the file-list from the repository
 * @returns a Set of filenames
 */
Generic* APICommandQueue::VDMProject_GetFiles ()
{  
  return new Generic(ToolMediator::Files ());
}

/**
 * Add a file to the repository
 * @param params Sequence with first element of type 
 * Record created by mk_FileName
 */
void APICommandQueue::VDMProject_AddFile (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMProject_AddFile");

  const Generic & fileName (params[1]);
  checkValidFile (fileName, wstring(L"VDMProject::AddFile ()"));

  ToolMediator::BTools()->vdm_AddFiles (params.Elems());
}

/**
 * Remove a file from the repository
 * @param params Sequence with first element of type 
 * Record created by mk_FileName
 */
void APICommandQueue::VDMProject_RemoveFile (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMProject_RemoveFile"); 

  const Generic & fileName (params[1]);
  if (!fileName.Is(TAG_TYPE_ProjectTypes_FileName))
    ThrowApiError (INVALID_ARG, L"APICommandQueue::VDMProject_RemoveFile"); 
  
  Set files (ToolMediator::Repos()->vdm_Files());
  if(!files.InSet(fileName))
  {
    wstring name (PTAUX::ExtractFileName (fileName));
    wstring message (wstring(L"VDMProject::RemoveFile() failed. File is not in current project: ") + name);
    ThrowApiError(message.c_str(), L"CommandQueue::VDMProject_RemoveFile");
  }

  ToolMediator::BTools()->vdm_RemoveFiles (params.Elems ());
}

/*******************************************************
 * Repository methods
 ******************************************************/

/**
 * Returns a list of files in a Module
 * @param params Sequence with first element of type 
 * Record created by mk_ModuleName
 */
Generic* APICommandQueue::VDMModuleRepos_FilesOfModule (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMModuleRepos_FilesOfModule"); 

  const Generic & moduleName (params[1]);
  checkValidModuleName (moduleName, wstring(L"VDMModuleRepos::FilesOfModule ()"));

  return new Generic (ToolMediator::Repos()->vdm_FileOfModule (moduleName));
}

/**
 * Returns the status of a Module
 * @param params Sequence with first element of type 
 * Record created by mk_ModuleName
 */
Generic* APICommandQueue::VDMModuleRepos_Status (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMModuleRepos_Status"); 

  const Generic & moduleName (params[1]);
  checkValidModuleName (moduleName, wstring(L"VDMModuleRepos::Status ()"));

  return new Generic (ToolMediator::Repos()->vdm_Status (moduleName));
}

/**
 * Returns the list of superclasses of a module
 * @param params Sequence with first element of type 
 * Record created by mk_ModuleName
 */
Generic* APICommandQueue::VDMModuleRepos_SuperClasses (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMModuleRepos_SuperClasses"); 

  const Generic & moduleName (params[1]);
  checkValidModuleName (moduleName, wstring(L"VDMModuleRepos::SuperClasses ()"));

  return new Generic (ToolMediator::Repos()->vdm_GetSuper (moduleName));
}

/**
 * Returns the list of subclasses of a module
 * @param params Sequence with first element of type 
 * Record created by mk_ModuleName
 */
Generic* APICommandQueue::VDMModuleRepos_SubClasses (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMModuleRepos_SubClasses"); 

  const Generic & moduleName (params[1]);
  checkValidModuleName (moduleName, wstring(L"VDMModuleRepos::SubClasses ()"));

  return new Generic (ToolMediator::Repos()->vdm_GetSub (moduleName));
}

/**
 * Returns the list of used classes 
 * @param params Sequence with first element of type 
 * Record created by mk_ModuleName
 */
Generic* APICommandQueue::VDMModuleRepos_Uses (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMModuleRepos_Uses"); 

  const Generic & moduleName (params[1]);
  checkValidModuleName (moduleName, wstring(L"VDMModuleRepos::Uses ()"));

  ToolMediator::BTools()->vdm_Depend ();

  return new Generic (ToolMediator::Repos()->vdm_GetUses (moduleName));
}

/**
 * Returns the list of classes the module is used by
 * @param params Sequence with first element of type 
 * Record created by mk_ModuleName
 */
Generic* APICommandQueue::VDMModuleRepos_UsedBy (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMModuleRepos_UsedBy"); 

  const Generic & moduleName (params[1]);
  checkValidModuleName (moduleName, wstring(L"VDMModuleRepos::UsedBy ()"));

  ToolMediator::BTools()->vdm_Depend ();
  return new Generic (ToolMediator::Repos()->vdm_GetUsedBy (moduleName));
}

/**
 * @returns the name of the module that is currently
 * on top of the stack
 * @exception APIError no specification is present
 */
Generic* APICommandQueue::VDMModuleRepos_GetCurrentModule ()
{
  Tuple res (ToolMediator::BTools()->vdm_GetCurrentModule ());
  if (res.GetBoolValue(1))
    return new Generic (res.GetField (2));
  else
    ThrowApiError (L"VDMModuleRepos::GetCurrentModule() failed",
                   L"APICommandQueue::VDMModuleRepos_GetCurrentModule");
  return new Generic (); // dummy
}

/**
 * pops a module from the module-stack
 * @exception APIError if stack is empty
 */
void APICommandQueue::VDMModuleRepos_PopModule ()
{
  Bool b (ToolMediator::BTools()->vdm_PopModule ());
  if (!b.GetValue ())
    ThrowApiError (L"VDMModuleRepos::PopModule () failed",
                   L"APICommandQueue::VDMModuleRepos_PopModule");
}

/**
 * pops a module from the module-stack
 * @exception APIError if stack is empty
 */
void APICommandQueue::VDMModuleRepos_PushModule (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMModuleRepos_PushModule"); 

  const Generic & moduleName (params[1]);
  checkValidModuleName (moduleName, wstring(L"VDMModuleRepos::PushModule ()"));

  Bool b (ToolMediator::BTools()->vdm_PushModule (moduleName));
  
  if (!b.GetValue ())
    ThrowApiError (L"VDMModuleRepos::PushModule() failed",
                   L"APICommandQueue::VDMModuleRepos_PushModule");
}

/*******************************************************
 * Interpreter methods
 ******************************************************/

/**
 * Initialize the Interpreter
 */
void APICommandQueue::VDMInterpreter_Initialize ()
{
  ToolMediator::BTools()->vdm_InitInterpreter ();
}

/**
 * Evaluate an expression
 */
Generic* APICommandQueue::VDMInterpreter_DoEval (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMInterpreter_DoEval"); 

  const Generic & expression (params[1]);
  if (!expression.Is(TAG_TYPE_ProjectTypes_ToolCommand))
    ThrowApiError (INVALID_ARG, L"APICommandQueue::VDMInterpreter_DoEval");

  return new Generic (ToolMediator::BTools()->vdm_ParseAndEvalExprs (expression));
}

/**
 * Executes a command
 */
void APICommandQueue::VDMInterpreter_EvalCmd (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMInterpreter_EvalCmd"); 

  const Generic & cmd (params[1]);
  if (!cmd.Is(TAG_TYPE_ProjectTypes_ToolCommand))
    ThrowApiError (INVALID_ARG, L"APICommandQueue::VDMInterpreter_EvalCmd");

  ToolMediator::BTools()->vdm_ExecuteCommand(cmd);
}

/**
 * Sets a breakpoint at specified position
 * @param params Sequence of params, holding
 * the FileName, line and position of the breakpoint
 */
Generic* APICommandQueue::VDMInterpreter_SetBreakPointByPos (const Sequence & params)
{
  if (params.Length() != 3)
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMInterpreter_SetBreakPointByPos"); 

  const Generic & fileName (params[1]);
  checkValidFile (fileName, wstring(L"VDMInterpreter::SetBreakPointByPos ()"));

  const Generic & line (params[2]);
  if (!line.IsInt())
    ThrowApiError (INVALID_ARG, L"APICommandQueue::VDMInterpreter_SetBreakPointByPos");

  const Generic & col (params[3]);
  if (!col.IsInt())
    ThrowApiError (INVALID_ARG, L"APICommandQueue::VDMInterpreter_SetBreakPointByPos");

  return new Generic (ToolMediator::BTools()->vdm_SetBreakOnPos (fileName, line, col));
}

/**
 * Sets a breakpoint at specified operation/function
 * @param params Sequence of params, holding
 * the module name and the operation/function name
 * (types: TYPE_ProjectTypes_ModuleName,
 * TYPE_ProjectTypes_Name)
 */
Generic* APICommandQueue::VDMInterpreter_SetBreakPointByName (const Sequence & params)
{
  if (params.Length() != 2)
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMInterpreter_SetBreakPointByName"); 

  const Generic & moduleName (params[1]);
  checkValidModuleName (moduleName, wstring(L"VDMInterpreter::SetBreakPointByName ()"));

  const Generic & nm (params[2]);
  if (!nm.Is(TAG_TYPE_ProjectTypes_Name))
    ThrowApiError (INVALID_ARG, L"APICommandQueue::VDMInterpreter_SetBreakPointByName");

  return new Generic (ToolMediator::BTools()->vdm_SetBreakOnName (moduleName, nm));
}

/**
 * Deletes a breakpoint
 * @param Sequence of params, containing the
 * number of the breakpoint as first element (of type Int)
 */
void APICommandQueue::VDMInterpreter_DeleteBreakPoint (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMInterpreter_DeleteBreakPoint"); 
  
  const Generic & num (params[1]);
  if (!num.IsInt ())
    ThrowApiError (INVALID_ARG, L"APICommandQueue::VDMInterpreter_DeleteBreakPoint");

  ToolMediator::BTools()->vdm_DeleteBreakPoint (num);
}

/**
 * Start debugging of an Expression
 */
Generic* APICommandQueue::VDMInterpreter_ParseAndDebug (const Sequence & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMInterpreter_ParseAndDebug"); 

  const Generic & expression (params[1]);
  if (!expression.Is(TAG_TYPE_ProjectTypes_ToolCommand))
    ThrowApiError (INVALID_ARG, L"APICommandQueue::VDMInterpreter_ParseAndDebug");

  try
  {
    return new Generic (ToolMediator::BTools()->vdm_ParseAndDebugExprs (expression));
  }
  catch (...) // alas, almost everything can happen in there :(
  {
    ThrowApiError (L"Error occured when debugging expression",
                   L"APICommandQueue::VDMInterpreter_ParseAndDebug");
  }
  return new Generic (); // dummy
}

/**
 * Perform a debugging step
 */
Generic* APICommandQueue::VDMInterpreter_DebugStep ()
{
  return new Generic (ToolMediator::BTools()->vdm_DebugStep ());
}

/**
 * Perform a debugging step in
 */
Generic* APICommandQueue::VDMInterpreter_DebugStepIn ()
{
  return new Generic (ToolMediator::BTools()->vdm_DebugStepIn ());
}

/**
 * Perform a debugging single step
 */
Generic* APICommandQueue::VDMInterpreter_DebugSingleStep ()
{
  return new Generic (ToolMediator::BTools()->vdm_DebugSingleStep ());
}

/**
 * Continue debugging
 */
Generic* APICommandQueue::VDMInterpreter_DebugContinue ()
{
  return new Generic (ToolMediator::BTools()->vdm_DebugContinue ());
}

/*******************************************************
 * CodeGenerator methods
 ******************************************************/

/**
 * Used to generate Code.
 * @param params List of modules 
 * @param targetLang target language, either L"CPP" or "JAVA"
 */
Generic* APICommandQueue::VDMCodeGenerator_CodeGenerate (const SEQ<TYPE_ProjectTypes_ModuleName> & params,
                                                         const Quote & targetLang)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMCodeGenerator_CodeGenerate"); 

  size_t len_params = params.Length();
  for (size_t idx = 1; idx <= len_params; idx++)
    checkValidModuleName (params[idx], wstring(L"VDMCodeGenerator::CodeGenerate[List] ()"));

  return new Generic (ToolMediator::BTools()->vdm_CodeGenerate(params, targetLang,
                                           false, true, false, false, Nil(), false, false));
}

/*******************************************************
 * Parser methods
 ******************************************************/

/**
 * Used to parse files.
 * @param params List of files (constructed with mk_FileName) 
 */
Generic* APICommandQueue::VDMParser_Parse (const SEQ<TYPE_ProjectTypes_FileName> & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMParser_Parse"); 

  size_t len_params = params.Length();
  for (size_t idx = 1; idx <= len_params; idx++)
    checkValidFile (params[idx], wstring(L"VDMParser::Parse ()"));

  return new Generic (ToolMediator::BTools()->vdm_SyntaxCheck (params));
}


/*******************************************************
 * TypeChecker methods
 ******************************************************/

/**
 * Used to typecheck Modules.
 * @param params List of modules 
 */
Generic* APICommandQueue::VDMTypeChecker_TypeCheck (const SEQ<TYPE_ProjectTypes_ModuleName> & params)
{

  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMTypeChecker_TypeCheck"); 

  size_t len_params = params.Length();
  for (size_t idx = 1; idx <= len_params; idx++)
    checkValidModuleName (params[idx], wstring(L"VDMTypeChecker::TypeCheck[List] ()"));

  return new Generic (ToolMediator::BTools()->vdm_TypeCheck (params));
}


/*******************************************************
 * PrettyPrinter methods
 ******************************************************/

/**
 * Used to typecheck Modules.
 * @param params List of modules 
 */
Generic* APICommandQueue::VDMPrettyPrinter_PrettyPrint (const SEQ<TYPE_ProjectTypes_FileName> & params)
{
  if (params.IsEmpty())
    ThrowApiError (WRONG_ARG_NUM, L"APICommandQueue::VDMPrettyPrinter_PrettyPrint"); 

  size_t len_params = params.Length();
  for (size_t idx = 1; idx <= len_params; idx++)
    checkValidFile (params[idx], wstring(L"VDMPrettyPrinter::PrettyPrint[List] ()"));

  return new Generic (ToolMediator::BTools()->vdm_PrettyPrint (params));
}


/*******************************************************
 * Error methods
 ******************************************************/

Generic* APICommandQueue::VDMErrors_GetStatus ()
{
  return new Generic (ToolMediator::Errs ()->vdm_GetStatus ());
}

void APICommandQueue::VDMErrors_ShowMsg ()
{
  ToolMediator::Errs()->vdm_ShowMsg ();
}

Generic* APICommandQueue::VDMErrors_GetErrors ()
{
  return new Generic (ToolMediator::Errs ()->vdm_GetErrors ());
}

Generic* APICommandQueue::VDMErrors_GetWarnings ()
{
  return new Generic (ToolMediator::Errs ()->vdm_GetWarnings ());
}

/**
 * Checks if file exists.
 * @param name Name of file
 * @param func Name of the function that called checkValidFile
 */
void APICommandQueue::checkValidFile (const Generic & fileName, const wstring & func)
{
  if (!fileName.Is(TAG_TYPE_ProjectTypes_FileName))
    ThrowApiError (INVALID_ARG, func.c_str());

  wstring file (PTAUX::ExtractFileName (fileName));
  wifstream wif (TBWSTR::wstring2fsstr(file).c_str());
  if(wif)
    wif.close();
  else {
    wstring s (func + wstring(L" failed. File does not exist: ") + file);
    ThrowApiError(s.c_str(), L"checkValidFile");
  }
}

/**
 * Checks if module exists in repository
 * @param name Name of module
 * @param func Name of the function that called checkValidModuleName
 */
void APICommandQueue::checkValidModuleName(const Generic & moduleName, const wstring & func)
  // Determines if the module 'name' is known to the toolbox.
{
  if (!moduleName.Is(TAG_TYPE_ProjectTypes_ModuleName))
    ThrowApiError (INVALID_ARG, func.c_str());

  //if(!((Set) ToolMediator::Repos()->vdm_AllModules()).InSet(moduleName))
  if(!(ToolMediator::Repos()->vdm_AllModules().InSet(moduleName)))
  {
    wstring name (PTAUX::ExtractModuleName(moduleName));
    wstring s (func + wstring(L" failed. Module does not exist: ") + name);
    ThrowApiError(s.c_str(), L"checkValidModuleName");
  }
}

