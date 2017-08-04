/***
*  * WHAT
*  *   Implementations of the classes ToolboxThread and CommandBuffer
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/toolboxthread.cc,v $
*  * VERSION
*  *    $Revision: 1.39 $
*  * DATE
*  *    $Date: 2006/06/13 07:02:12 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include "toolboxthread.h"

#if QT_VERSION < 0x040000
#include <qapplication.h>
#endif // QT_VERSION < 0x040000

#include "customevents.h"

extern void postGUIEvent(QEvent*);

#ifdef __APPLE_CC__
//#define REQUIRED_STACK_SIZE 1024*1024
#define REQUIRED_STACK_SIZE 2048*1024
#endif // __APPLE_CC__
//
// ToolboxThread::ToolboxThread
//
// Constructor for the ToolboxThread class. Initializes the instance
// variables and then starts the thread
//
// Arguments: None
// Returns: instance of ToolboxThread
//
ToolboxThread::ToolboxThread()
#ifdef __APPLE_CC__
#if QT_VERSION < 0x040000
  : QThread(REQUIRED_STACK_SIZE)
#endif // QT_VERSION < 0x040000
#endif // __APPLE_CC__
{
  mainLoopContinue = false;

#ifdef __APPLE_CC__
#if QT_VERSION >= 0x040000
  this->setStackSize(REQUIRED_STACK_SIZE);
#endif // QT_VERSION >= 0x040000
#endif // __APPLE_CC__

  QThread::start();
}

//
// ToolboxThread::~ToolboxThread
//
// Destructor for the ToolboxThread class. Currently does nothing
// since the objects pointed to by the pointers in the class are
// destroyed at their point of creation
//
// Arguments: None
// Returns: None
//
ToolboxThread::~ToolboxThread()
{
}

//
// ToolboxThread::canContinue
//
// Used by the GUI to determine whether the interpreter can continue
// or the end of a debug has been reached
//
// Arguments: None
// Returns: boolean representing whether the interpreter can continue
// (true) or not (false).
//
bool ToolboxThread::canContinue()
{
  //  return mainLoopContinue;

  // FIXME: This is a hack; a call back should be generated instead,
  // since I doubt QT can cope with a signal being emitted from
  // another thread to the one in which the catching slot is
  // running. Or can it?
  return true;
}

//
// ToolboxThread::run
//
// The run function of the thread is executed when the thread is
// started. It repeatedly takes fetches commands from cmdBuffer and
// passes them on to an auxiliary function. When there are no commands
// left, the function finishes.
//
// Arguments: None
// Returns: None
//
void ToolboxThread::run()
{
  while (!this->cmdBuffer.isEmpty()) {
    ToolboxCMD * command (this->cmdBuffer.getCommand());
    this->runCommand(*command);
    delete command;
  }
}

// runCommand
// command :
void ToolboxThread::runCommand(const ToolboxCMD & command)
{
  switch (command.get_cmd()) {
    case CMD_LoadProject: {
      disableGUI();
      loadProject((const LoadProjectCMD &)command);
      enableGUI();
      break;
    }
    case CMD_SyntaxCheck: {
      disableGUI();
      syntaxCheck((const SyntaxCheckCMD &)command);
      enableGUI();
      break;
    }
    case CMD_TypeCheck: {
      disableGUI();
      typeCheck((const TypeCheckCMD &)command);
      enableGUI();
      break;
    }
    case CMD_CodeGenerate: {
      disableGUI();
      codeGenerate((const CodeGenerateCMD &)command);
      enableGUI();
      break;
    }
    case CMD_JavaGenerate: {
      disableGUI();
      javaGenerate((const JavaGenerateCMD &)command);
      enableGUI();
      break;
    }
    case CMD_PrettyPrint: {
      disableGUI();
      prettyPrint((const PrettyPrintCMD &)command);
      enableGUI();
      break;
    }
#ifdef VDMPP
    case CMD_VDMGenerate: {
      disableGUI();
      VDMGenerate((const VDMGenerateCMD &)command);
      enableGUI();
      break;
    }
#endif // VDMPP
    case CMD_ExecuteCommand: {
      //const Sequence & comSeq (command.GetSequence(1));
      //wstring cmd (comSeq.GetString());
      QString cmd (((const ExecuteCommandCMD &)(command)).cmd());
      if (cmd != "init")
        enableStop();
      disableGUI();
      executeCommand((const ExecuteCommandCMD &)command);
      enableGUI();
      disableStop();
      break;
    }
    case CMD_AddFiles: {
      disableGUI();
      addFiles((const AddFilesCMD &)command);
      enableGUI();
      break;
    }
    case CMD_RemoveFiles: {
      disableGUI();
      removeFiles((const RemoveFilesCMD &)command);
      enableGUI();
      break;
    }
    case CMD_SaveProjectAs: {
      disableGUI();
      saveProjectAs((const SaveProjectAsCMD &)command);
      enableGUI();
      break;
    }
    case CMD_NewUnnamedProject: {
      disableGUI();
      newUnnamedProject((const NewUnnamedProjectCMD &)command);
      enableGUI();
      break;
    }
#ifdef VDMPP
    case CMD_ResetMapper: {
      resetMapper((const ResetMapperCMD &)command);
      break;
    }
    case CMD_SetModelName: {
      setModelName((const SetModelNameCMD &)command);
      break;
    }
    case CMD_UMLDiff: {
      UMLDiff((const UMLDiffCMD &)command);
      break;
    }
#endif// VDMPP
    case CMD_GenPOG: {
      disableGUI();
      GenPOG((const GenPOGCMD &)command);
      enableGUI();
      break;
    }
    case CMD_CallAPI: {
      apiPeriodic((const CallAPICMD &)command);
      break;
    }
    default: {
      logWrite(Qt2TB::wstring2qstring(L"Internal Error: unknown command in ToolboxThread"));
      break;
    }
  }
}

void ToolboxThread::enableGUI()
{
  postGUIEvent(new EnableInputEvent());
}

void ToolboxThread::disableGUI()
{
  postGUIEvent(new DisableInputEvent());
}

void ToolboxThread::enableStop()
{
  postGUIEvent(new EnableStopEvent());
}

void ToolboxThread::disableStop()
{
  postGUIEvent(new DisableStopEvent());
}

void ToolboxThread::logWrite(const QString & str)
{
  postGUIEvent(new LogWriteEvent(str));
}

void ToolboxThread::addCommand(ToolboxCMD * cmd)
{
  this->cmdBuffer.addCommand( cmd );
}

//
// Auxiliary function which instructs the specification manager to
// load a project, with name given by the command argument.
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the name of the project file to be opened
// Returns: None
//
void ToolboxThread::loadProject(const LoadProjectCMD & command)
{
  Qt2TB::LoadProject(command.project());

  postGUIEvent(new LoadProjectEvent());
}

//
// ToolboxThread::syntaxCheck
//
// Auxiliary function which instructs the specification manager to
// parse a list of files.
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the list of filenames to be parsed (these may be VDM
//   or Java files).
// Returns: None
//
void ToolboxThread::syntaxCheck(const SyntaxCheckCMD & command)
{
  QStringList files (command.files());
  QStringList vdmFiles, javaFiles;
  for (QStringList::const_iterator it = files.begin(); it != files.end(); ++it)
  {
    if (Qt2TB::isJavaFile(*it))
      javaFiles.append(*it);
    else
      vdmFiles.append(*it);
  }

  if (!vdmFiles.isEmpty())
    Qt2TB::SyntaxCheck(vdmFiles);

#ifdef VDMPP
  if (!javaFiles.isEmpty())
    Qt2TB::JavaSyntaxCheck(javaFiles);
#endif // VDMPP
}

//
// ToolboxThread::typeCheck
//
// Auxiliary function which instructs the specification manager to
// type check a list of modules
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the list of modules to be type checked (these may be VDM
//   or Java modules).
// Returns: None
//
void ToolboxThread::typeCheck(const TypeCheckCMD& command)
{
  const QStringList & modules (command.modules());
  QStringList vdmModules, javaModules;
  for (QStringList::const_iterator it = modules.begin(); it != modules.end(); ++it)
  {
    if ((*it) != "")
    {
      if (Qt2TB::isJavaModule(*it))
        javaModules.append(*it);
      else
        vdmModules.append(*it);
    }
  }
  if (!vdmModules.isEmpty())
    Qt2TB::TypeCheck(vdmModules);

  if (!javaModules.isEmpty())
    Qt2TB::JavaTypeCheck(javaModules);

  postGUIEvent(new CheckActionsEvent());
}

//
// ToolboxThread::codeGenerate
//
// Auxiliary function which instructs the specification manager to
// generate C++ for a list of modules
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the list of modules to be code generated.
//
// Returns: None
//
void ToolboxThread::codeGenerate(const CodeGenerateCMD& command)
{
  Qt2TB::CPPGenerate(command.modules());
}

//
// ToolboxThread::javaGenerate
//
// Auxiliary function which instructs the specification manager to
// generate Java for a list of modules
//
// Arguments:
//
// const Record& command: record with one field, which is a Sequence
//   containing the list of modules to be code generated.
//
// Returns: None
//
void ToolboxThread::javaGenerate(const JavaGenerateCMD & command)
{
  Qt2TB::JavaGenerate(command.modules());
}

//
// ToolboxThread::prettyPrint
//
// Auxiliary function which instructs the specification manager to
// pretty print a list of files.
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the list of filenames to be pretty printed
//
// Returns: None
//
void ToolboxThread::prettyPrint (const PrettyPrintCMD & command)
{
  Qt2TB::PrettyPrint(command.files());
}

#ifdef VDMPP
void ToolboxThread::VDMGenerate (const VDMGenerateCMD & command)
{
  Qt2TB::JavaGenerateVDM(command.modules(), command.stubOnly(), true, command.transforms());
}
#endif // VDMPP

//
// ToolboxThread::executeCommand
//
// Auxiliary function which instructs the specification manager to
// execute a command given by a string. This could either be an
// interpreter command corresponding to one of the interpreter buttons
// in the GUI, or a command entered by the user in the interpreter
// itself.
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the text of the command.
//
//
void ToolboxThread::executeCommand(const ExecuteCommandCMD & command)
{
  mainLoopContinue = Qt2TB::ExecuteCommand(command.cmd());
}

//
// ToolboxThread::addFiles
//
// Auxiliary function which instructs the specification manager to
// add a list of files to the current project.
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the list of filenames to be added to the project
//
// Returns: None
//
void ToolboxThread::addFiles(const AddFilesCMD & command)
{
  Qt2TB::AddFiles(command.files());
}

//
// ToolboxThread::removeFiles
//
// Auxiliary function which instructs the specification manager to
// remove a list of files from the current project.
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the list of filenames to be removed from the project
//
// Returns: None
//
void ToolboxThread::removeFiles(const RemoveFilesCMD & command)
{
  Qt2TB::RemoveFiles(command.files());
}

//
// ToolboxThread::saveProjectAs
//
// Auxiliary function which instructs the specification manager to
// save the current project using the file given as an argument
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the filename for the project to be saved under.
//
// Returns: None
//
void ToolboxThread::saveProjectAs(const SaveProjectAsCMD & command)
{
  Qt2TB::SaveProjectAs(command.project());

  postGUIEvent(new PostSaveProjectAsEvent());
}

//
// ToolboxThread::newUnnamedProject
//
// Auxiliary function which instructs the specification manager to
// create a new project without a name
//
// Arguments:
//   const Record&: not used
//
// Returns: None
//
void ToolboxThread::newUnnamedProject(const NewUnnamedProjectCMD &)
{
  Qt2TB::NewUnnamedProject();

  postGUIEvent(new NewUnnamedProjectEvent());
}

#ifdef VDMPP
//
// ToolboxThread::resetMapper
//
// Auxiliary function which instructs the specification manager to
// reset the UML mapper
//
// Arguments:
//   const Record&: not used
//
// Returns: None
//
void ToolboxThread::resetMapper(const ResetMapperCMD &)
{
  //FIXME: When is this used?
  Qt2TB::ResetMapper();
}

//
// ToolboxThread::setModelName
//
// Auxiliary function which instructs the specification manager to
// set the name of the UML model, as given by the argument.
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the filename for the project to be saved under.
//
// Returns: None
//
void ToolboxThread::setModelName(const SetModelNameCMD & command)
{
  //FIXME: When is this used?
  Qt2TB::SetModelNamesI(command.name(), QString(""));
}

//
// ToolboxThread::UMLDiff
//
// Auxiliary function which instructs the specification manager to
// perform a diff on the VDM++ and UML versions of the specified classes.
//
// Arguments:
//   const Record& command: record with one field, which is a Sequence
//   containing the list of classes to be compared.
//
// Returns: None
//
void ToolboxThread::UMLDiff(const UMLDiffCMD & command)
{
  Qt2TB::UMLDiff(command.classes());
}
#endif //VDMPP

void ToolboxThread::GenPOG(const GenPOGCMD & command)
{
  Qt2TB::PogGenerate(command.modules());
}

void ToolboxThread::apiPeriodic(const CallAPICMD & command)
{
  Qt2TB::api_Periodic();
}

///////////////////////////////////////////////////////////

//
// CommandBuffer::addCommand
//
// Adds a command to the command buffer. Has a mutex lock to ensure
// synchronized access to commandList
//
// Arguments:
//   const Record& command: command to add to buffer
//
// Returns: None
//
void CommandBuffer::addCommand(ToolboxCMD * command)
{
  this->mutex.lock();
  this->commandList.append(command);
  this->mutex.unlock();
}

//

// CommandBuffer::getCommand
//
// Fetches a command from the command buffer. Has a mutex lock to ensure
// synchronized access to commandList. Fetched in a FIFO fashion.
//
// Arguments: None
//
// Returns:
//   Oldest command in buffer
//
ToolboxCMD * CommandBuffer::getCommand()
{
  this->mutex.lock();
#if QT_VERSION >= 0x040000
  ToolboxCMD * cmd (this->commandList.takeFirst());
#else
  ToolboxCMD * cmd (this->commandList.take(0));
#endif // QT_VERSION >= 0x040000
  this->mutex.unlock();
  return cmd;
}

//
// CommandBuffer::isEmpty
//
// Checks to see whether the buffer is empty or not.
//
// Arguments: None
//
// Returns:
//   boolean indicating whether the buffer is empty (true) or not
//   (false).
//
bool CommandBuffer::isEmpty()
{
  this->mutex.lock();
#if QT_VERSION >= 0x040000
  bool b = this->commandList.empty();
#else
  bool b = this->commandList.isEmpty();
#endif // QT_VERSION >= 0x040000
  this->mutex.unlock();
  return b;
}

