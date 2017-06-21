/***
*  * WHAT
*  *   Declarations of the classes ToolboxThread and CommandBuffer
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/toolboxthread.h,v $
*  * VERSION
*  *    $Revision: 1.9 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:43 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

//
// toolboxthread.h
//
// This header file defines the interfaces for the classes
// ToolboxThread and Command Buffer. 
//
// ToolboxThread is a class which uses a thread to communicate
// asynchronously between the QT GUI and the underlying Toolbox. In
// this way the GUI does not freeze while the Toolbox is busy.
//
// CommandBuffer is used by the GUI to communicate with
// ToolboxThread. An instance of CommandBuffer is shared between the
// two, and contains commands from the GUI to be executed by
// ToolboxThread. Once the CommandBuffer is empty, the ToolboxThread
// finishes. 
//

#ifndef __toolboxthread_h__
#define __toolboxthread_h__

#if QTVER >= 4
#include <QtGui/QtGui>
#else
#include <qthread.h>
#include <qstringlist.h>
#endif // QTVER >= 4

enum
{
  CMD_LoadProject       = 1,
  CMD_SyntaxCheck       = 2,
  CMD_TypeCheck         = 3,
  CMD_CodeGenerate      = 4,
  CMD_JavaGenerate      = 5,
  CMD_PrettyPrint       = 6,
  CMD_VDMGenerate       = 7,
  CMD_ExecuteCommand    = 8,
  CMD_AddFiles          = 9,
  CMD_RemoveFiles       = 10,
  CMD_SaveProjectAs     = 11,
  CMD_NewUnnamedProject = 12,
  CMD_ResetMapper       = 13,
  CMD_SetModelName      = 14,
  CMD_UMLDiff           = 15,
  CMD_GenPOG            = 16,
  CMD_CallAPI           = 17
};

class ToolboxCMD
{
public:
  ToolboxCMD(int cmd) : mCmd(cmd) {}
  int get_cmd() const { return this->mCmd; }
private:
  int mCmd;
};

class LoadProjectCMD : public ToolboxCMD
{
public:
  LoadProjectCMD(const QString & pnm) : ToolboxCMD(CMD_LoadProject), mProject(pnm) {};
  const QString & project() const { return this->mProject; };
private:
  QString mProject;
};

class SyntaxCheckCMD : public ToolboxCMD
{
public:
  SyntaxCheckCMD(const QStringList & files) : ToolboxCMD(CMD_SyntaxCheck), mFiles(files) {};
  const QStringList & files() const { return this->mFiles; };
private:
  QStringList mFiles;
};

class TypeCheckCMD : public ToolboxCMD
{
public:
  TypeCheckCMD(const QStringList & modules) : ToolboxCMD(CMD_TypeCheck), mModules(modules) {};
  const QStringList & modules() const { return this->mModules; };
private:
  QStringList mModules;
};

class CodeGenerateCMD : public ToolboxCMD
{
public:
  CodeGenerateCMD(const QStringList & modules) : ToolboxCMD(CMD_CodeGenerate), mModules(modules) {};
  const QStringList & modules() const { return this->mModules; };
private:
  QStringList mModules;
};

class JavaGenerateCMD : public ToolboxCMD
{
public:
  JavaGenerateCMD(const QStringList & modules) : ToolboxCMD(CMD_JavaGenerate), mModules(modules) {};
  const QStringList & modules() const { return this->mModules; };
private:
  QStringList mModules;
};

class PrettyPrintCMD : public ToolboxCMD
{
public:
  PrettyPrintCMD(const QStringList & files) : ToolboxCMD(CMD_PrettyPrint), mFiles(files) {};
  const QStringList & files() const { return this->mFiles; };
private:
  QStringList mFiles;
};

#ifdef VDMPP
class VDMGenerateCMD : public ToolboxCMD
{
public:
  VDMGenerateCMD(const QStringList & modules, bool stubOnly, bool autoRenaming, bool transforms)
   : ToolboxCMD(CMD_VDMGenerate), mModules(modules), mStubOnly(stubOnly), mAutoRenaming(autoRenaming),
     mTransforms(transforms) {};
  const QStringList & modules() const { return this->mModules; };
  bool stubOnly() const { return this->mStubOnly; };
  bool autoRenaming() const { return this->mAutoRenaming; };
  bool transforms() const { return this->mTransforms; };
private:
  QStringList mModules;
  bool mStubOnly;
  bool mAutoRenaming;
  bool mTransforms;
};
#endif // VDMPP

class ExecuteCommandCMD : public ToolboxCMD
{
public:
  ExecuteCommandCMD(const QString & cmd) : ToolboxCMD(CMD_ExecuteCommand), mCmd(cmd) {};
  const QString & cmd() const { return this->mCmd; };
private:
  QString mCmd;
};

class AddFilesCMD : public ToolboxCMD
{
public:
  AddFilesCMD(const QStringList & files) : ToolboxCMD(CMD_AddFiles), mFiles(files) {};
  const QStringList & files() const { return this->mFiles; };
private:
  QStringList mFiles;
};

class RemoveFilesCMD : public ToolboxCMD
{
public:
  RemoveFilesCMD(const QStringList & files) : ToolboxCMD(CMD_RemoveFiles), mFiles(files) {};
  const QStringList & files() const { return this->mFiles; };
private:
  QStringList mFiles;
};

class SaveProjectAsCMD : public ToolboxCMD
{
public:
  SaveProjectAsCMD(const QString & pnm) : ToolboxCMD(CMD_SaveProjectAs), mProject(pnm) {};
  const QString & project() const { return this->mProject; };
private:
  QString mProject;
};

class NewUnnamedProjectCMD : public ToolboxCMD
{
public:
  NewUnnamedProjectCMD() : ToolboxCMD(CMD_NewUnnamedProject) {};
};

#ifdef VDMPP
class ResetMapperCMD : public ToolboxCMD
{
public:
  ResetMapperCMD() : ToolboxCMD(CMD_ResetMapper) {};
};

class SetModelNameCMD : public ToolboxCMD
{
public:
  SetModelNameCMD(const QString & nm) : ToolboxCMD(CMD_SetModelName), mName(nm) {};
  const QString & name() const { return this->mName; };
private:
  QString mName;
};

class UMLDiffCMD : public ToolboxCMD
{
public:
  UMLDiffCMD(const QStringList & classes) : ToolboxCMD(CMD_UMLDiff), mClasses(classes) {};
  const QStringList & classes() const { return this->mClasses; };
private:
  QStringList mClasses;
};
#endif // VDMPP

class GenPOGCMD : public ToolboxCMD
{
public:
  GenPOGCMD(const QStringList & modules) : ToolboxCMD(CMD_GenPOG), mModules(modules) {};
  const QStringList & modules() const { return this->mModules; };
private:
  QStringList mModules;
};

class CallAPICMD : public ToolboxCMD
{
public:
  CallAPICMD() : ToolboxCMD(CMD_CallAPI) {};
};

/*
class TYPE_CMD_LoadProject : public Record {
public:
  TYPE_CMD_LoadProject () : Record(TAG_TYPE_CMD_LoadProject, 1) {}
  TYPE_CMD_LoadProject &Init (const Sequence &p)
  { SetField(1, p); return *this; };
  TYPE_CMD_LoadProject (const Generic &c) : Record(c) {}

  const wchar_t * GetTypeName () const {
    return L"TYPE_CMD_LoadProject";
  }
  Sequence get_filenm () const { return GetSequence(1); };
  void set_filenm (const Sequence &p) { SetField(1, p); };
};
*/

// 
// The CommandBuffer class
//

class CommandBuffer 
{
public:
  void addCommand(ToolboxCMD *);
  ToolboxCMD * getCommand();
  bool isEmpty();
  CommandBuffer(){};
  ~CommandBuffer(){};

private:
#if QTVER >= 4
  QList<ToolboxCMD *> commandList;
#else
  QPtrList<ToolboxCMD> commandList;
#endif // QTVER >= 4
  QWaitCondition waitCond;
  QMutex mutex;
};

//
// The ToolboxThread class
//
class ToolboxThread: public QThread
{
public:  // functions
  virtual void run();
  void runCommand(const ToolboxCMD&);
  ToolboxThread();
  ~ToolboxThread();
  bool canContinue();
  void addCommand(ToolboxCMD *);
private: // functions
  void enableGUI();
  void disableGUI();
  void enableStop();
  void disableStop();
  void logWrite(const QString & str );

  void loadProject(const LoadProjectCMD&);
  void syntaxCheck (const SyntaxCheckCMD&);
  void typeCheck (const TypeCheckCMD&);
  void codeGenerate(const CodeGenerateCMD&);
  void javaGenerate(const JavaGenerateCMD&);
  void prettyPrint (const PrettyPrintCMD&);
#ifdef VDMPP
  void VDMGenerate (const VDMGenerateCMD&);
#endif // VDMPP
  void executeCommand(const ExecuteCommandCMD&);
  void addFiles(const AddFilesCMD&);
  void removeFiles(const RemoveFilesCMD&);
  void saveProjectAs(const SaveProjectAsCMD&);
  void newUnnamedProject(const NewUnnamedProjectCMD&);
#ifdef VDMPP
  void resetMapper(const ResetMapperCMD&);
  void setModelName(const SetModelNameCMD&);
  void UMLDiff(const UMLDiffCMD&);
#endif // VDMPP
  void GenPOG(const GenPOGCMD&);
  void apiPeriodic(const CallAPICMD&);

private: // member variables
  CommandBuffer cmdBuffer;
  bool mainLoopContinue;
};

#endif // __toolboxthread_h__
