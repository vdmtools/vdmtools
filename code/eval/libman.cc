/***
*  * WHAT
*  *    Library management. Translation from AST to \Sigma.
*  * Id
*  *    $Id: libman.cc,v 1.339 2006/09/23 10:58:19 vdmtools Exp $
*  *
***/

#include "metaiv.h"
#include "libman.h"

LibraryManager * LibraryManager::libman = NULL;

void LibraryManager::Init()
{
  if ( LibraryManager::libman != NULL )
    delete LibraryManager::libman;
  LibraryManager::libman = new LibraryManager();
}

void LibraryManager::Finish()
{
  if ( LibraryManager::libman != NULL )
    delete LibraryManager::libman;
  LibraryManager::libman = NULL;
}

//////////////////////////////////////////////////
EvalDebugger & LibraryManager::GetDebugger ()
{
  if (this->the_EvalDebugger == NULL) {
    this->the_EvalDebugger = new EvalDebugger();
  }
  return * this->the_EvalDebugger;
}

StackEval & LibraryManager::GetStackMachine ()
{
  if (this->the_StackMachine == NULL) {
    this->the_StackMachine = new StackEval();
  }
  return * this->the_StackMachine;
}

StackCompiler & LibraryManager::GetCompiler ()
{
  if (this->the_Compiler == NULL)
    this->the_Compiler = new StackCompiler;
  return * this->the_Compiler;
}

EvalState & LibraryManager::GetState ()
{
  if (this->the_State == NULL)
    this->the_State = new EvalState();
  return * this->the_State;
}

#ifdef VDMPP
SCHD & LibraryManager::GetScheduler ()
{
  if (this->the_Scheduler == NULL)
    this->the_Scheduler = new SCHD();
  return * this->the_Scheduler;
}

SYSTEM & LibraryManager::GetSystem ()
{
  if (this->the_System == NULL)
    this->the_System = new SYSTEM();
  return * this->the_System;
}
#endif // VDMPP

LibraryManager::~LibraryManager()
{
  // clean up 
  if (this->the_StackMachine != NULL)
    this->the_StackMachine->User_Init(TYPE_AS_Document(), true);

  if (this->the_EvalDebugger != NULL) delete this->the_EvalDebugger;
  if (this->the_StackMachine != NULL) delete this->the_StackMachine;
  if (this->the_Compiler != NULL) delete this->the_Compiler;
#ifdef VDMPP
  if (this->the_Scheduler != NULL) delete this->the_Scheduler;
  if (this->the_System != NULL) delete this->the_System;
#endif // VDMPP
  if (this->the_State != NULL) delete this->the_State;
}

EvalDebugger & theDebugger()
{
  return LibraryManager::libman->GetDebugger();
}

StackEval & theStackMachine ()
{
  return LibraryManager::libman->GetStackMachine();
}

StackCompiler & theCompiler ()
{
  return LibraryManager::libman->GetCompiler();
}

EvalState & theState ()
{
  return LibraryManager::libman->GetState();
}

#ifdef VDMPP
SCHD & theScheduler ()
{
  return LibraryManager::libman->GetScheduler();
}

SYSTEM & theSystem ()
{
  return LibraryManager::libman->GetSystem();
}
#endif // VDMPP

void StackEval::DeleteMe()
{
}
