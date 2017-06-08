/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/customevents.h,v $
*  * VERSION
*  *    $Revision: 1.14 $
*  * DATE
*  *    $Date: 2006/04/21 04:33:23 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __customevents_h__
#define __customevents_h__

#ifdef QT4
#include <QtGui/QtGui>
#define QLIST QList
#else
#include <qevent.h>
#include <qstringlist.h>
#define QLIST QValueList
#endif // QT4

#include "interface2TB.h"

class GUIEvent
{
public:
enum
{
  EV_OPENFILE             = QEvent::User + 1,
  EV_LOAD_PROJECT         = QEvent::User + 2,
  EV_NEW_UNNAMEDPROJECT   = QEvent::User + 3,
  EV_ADD_FILES            = QEvent::User + 4,
  EV_REMOVE_FILES         = QEvent::User + 5,
  EV_ADD_MODULES          = QEvent::User + 6,
  EV_REMOVE_MODULES       = QEvent::User + 7,
  EV_CHANGE_MODULES_STATUS = QEvent::User + 8,
  EV_CHANGED_FILE_STATUS  = QEvent::User + 9,
  EV_ADD_BREAK_NAME       = QEvent::User + 10,
  EV_ADD_BREAK_POS        = QEvent::User + 11,
  EV_REMOVE_BREAK         = QEvent::User + 12,
  EV_ENABLE_BREAK         = QEvent::User + 13,
  EV_DISABLE_BREAK        = QEvent::User + 14,
  EV_BACK_TRACE           = QEvent::User + 15,
  EV_GOTO_BT_LEVEL        = QEvent::User + 16,
  EV_CLEAR_DEBUG_WINDOW   = QEvent::User + 17,
  EV_UPDATE_SELECTIONS    = QEvent::User + 18,
  EV_CLEAR_ALL            = QEvent::User + 19,
  EV_CHANGE_PROJECT_NAME  = QEvent::User + 20,
  EV_ROSE_DIFF            = QEvent::User + 21,
  EV_INTERPRETER_WRITE    = QEvent::User + 22,
  EV_LOG_WRITE            = QEvent::User + 23,
  EV_ROSE_ERROR           = QEvent::User + 24,
  EV_MESSAGE_SHOW         = QEvent::User + 25,
  EV_MESSAGE_ADD          = QEvent::User + 26,
  EV_MESSAGE_CLEAR_ALL    = QEvent::User + 27,
  EV_MESSAGE_ALL_DONE     = QEvent::User + 28,
  EV_MESSAGE_NEXT         = QEvent::User + 29,
  EV_MESSAGE_PREV         = QEvent::User + 30,
  EV_MESSAGE_FIRST        = QEvent::User + 31,
  EV_MESSAGE_LAST         = QEvent::User + 32,
  EV_ERROR_STATUS         = QEvent::User + 33,
  EV_ENABLE_INPUT         = QEvent::User + 34,
  EV_DISABLE_INPUT        = QEvent::User + 35,
  EV_REFRESH_INTERFACE    = QEvent::User + 36,
  EV_METER_INIT           = QEvent::User + 37,
  EV_METER_UPDATE         = QEvent::User + 38,
  EV_METER_DESTROY        = QEvent::User + 39,
  EV_METER_INCREMENT      = QEvent::User + 40,
  EV_METER_SET_TOTAL      = QEvent::User + 41,
  EV_REFRESH_SRC_WINDOW   = QEvent::User + 42,
  EV_POG_ADD              = QEvent::User + 43,
  EV_POG_CLEAN            = QEvent::User + 44,
  EV_POG_UPDATE_FILTER    = QEvent::User + 45,
  EV_CHECK_ACTIONS        = QEvent::User + 46,
  EV_ENABLE_STOP          = QEvent::User + 47,
  EV_DISABLE_STOP         = QEvent::User + 48,
  EV_POST_SAVE_PROJECT_AS = QEvent::User + 49,
  EV_ADD_MODULE_AND_STATUS = QEvent::User + 50,
  EV_UPDATE_MODULE_STATUS = QEvent::User + 51,
};
};

class AddFileEvent : public QEvent
{
public: 
  AddFileEvent(const QStringList & pFiles);
  const QStringList & files() const {return this->mFiles; };

private:
  QStringList mFiles;
};

class RemoveFilesEvent : public QEvent
{
public: 
  RemoveFilesEvent(const QStringList& pFiles);
  const QStringList & files() const { return this->mFiles; };

private:
  QStringList mFiles;
};

class AddModulesEvent : public QEvent
{
public:
  AddModulesEvent(const QStringList& pModules);
  const QStringList & modules() const { return this->mModules; };

private:
  QStringList mModules;
};

class RemoveModulesEvent : public QEvent
{
public:
  RemoveModulesEvent(const QStringList& pModules);
  const QStringList & modules() const { return this->mModules; };

private:
  QStringList mModules;
};

class ChangeModulesStatusEvent : public QEvent
{
public:
  ChangeModulesStatusEvent(const QStringList& pModules);
  const QStringList & modules() const { return this->mModules; };

private:
  QStringList mModules;
};

class ChangedFileStatusEvent : public QEvent
{
public:
  ChangedFileStatusEvent(const QString & pFile);
  const QString & file() const { return this->mFile; };

private:
  QString mFile;
};

class AddBreakNameEvent : public QEvent
{
public:
  AddBreakNameEvent(const QString &, int);
  const QString & breakName() const { return this->mName; };
  int breakNum() const { return this->mNum; };

private:
  QString mName;
  int mNum;
};

class AddBreakPosEvent : public QEvent
{
public:
  AddBreakPosEvent(const QString &, int pLine, int pCol, int pNum);
  const QString & breakName() const { return this->mName;};
  int breakLine() const { return this->mLine; };
  int breakCol() const { return this->mCol; };
  int breakNum() const { return this->mNum; };

private:
  QString mName;
  int mLine;
  int mCol;
  int mNum;
};

class RemoveBreakEvent : public QEvent
{
public:
  RemoveBreakEvent(int);
  int breakNum() const { return this->mNum; };

private:
  int mNum;
};

class EnableBreakEvent : public QEvent
{
public:
  EnableBreakEvent(int);
  int breakNum() const { return this->mNum; };

private:
  int mNum;
};

class DisableBreakEvent : public QEvent
{
public:
  DisableBreakEvent(int);
  int breakNum() const { return this->mNum; };

private:
  int mNum;
};


class BackTraceEvent: public QEvent
{
public:
  BackTraceEvent(const QLIST<BackTraceEntry> & bts);
  const QLIST<BackTraceEntry> & bts() { return this->mBts; };

private:
  QLIST<BackTraceEntry> mBts;
};


class GotoBtLevelEvent: public QEvent
{
public:
  GotoBtLevelEvent(int);
  int level() const { return this->mLevel;};

private:
  int mLevel;
};

class OpenFileEvent : public QEvent
{
public:
  OpenFileEvent(const QString &, const QString &, int, int, int);
  const QString & filename() const { return this->mFilename; };
  const QString & title() const { return this->mTitle; };
  int line() const { return this->mLine; };
  int column() const { return this->mColumn; };
  int length() const { return this->mLength; };

private:
  QString mFilename;
  QString mTitle;
  int mLine;
  int mColumn;
  int mLength;
};

class ChangeProjectNameEvent : public QEvent
{
private:
  QString mOldName;
  QString mNewName;

public:
  ChangeProjectNameEvent(const QString &, const QString &);
  const QString & oldName() const { return this->mOldName; };
  const QString & newName() const { return this->mNewName; };
};

class RoseDiffEvent : public QEvent
{
public:
  RoseDiffEvent(const QString &);
  const QString & text() const { return this->mText; };

private:
  QString mText;
};

class InterpreterWriteEvent : public QEvent
{
public:
  InterpreterWriteEvent(const QString &);
  const QString & text() const { return this->mText; };

private:
  QString mText;
};

class LogWriteEvent : public QEvent
{
public:
  LogWriteEvent(const QString &);
  const QString & text() const { return this->mText; };

private:
  QString mText;
};

class RoseErrorEvent : public QEvent
{
public:
  RoseErrorEvent(const QString&);
  const QString & text() const { return this->mText; };

private:
  QString mText;
};

class MessageShowEvent  : public QEvent
{
public:
  MessageShowEvent(int, const QString &);
  int index() const {return this->mIndex; };
  const QString & message() const {return this->mMessage; };

private:
  int mIndex;
  QString mMessage;
};

class MessageAddEvent : public QEvent
{
public:
  MessageAddEvent(int, const QString &, int, int, const QString &, const QString &);
  int errnum()const { return this->mErrnum; };
  const QString & filename() const { return this->mFilename; };
  int line() const { return this->mLine; };
  int column() const { return this->mColumn; };
  const QString & message() const { return this->mMessage; };
  const QString & tempFilename() const { return this->mTempFilename; };

private:
  int mErrnum;
  QString mFilename;
  int mLine;
  int mColumn;
  QString mMessage;
  QString mTempFilename;
};

class ErrorStatusEvent : public QEvent
{
public:
  ErrorStatusEvent(int, int);
  int errors() const { return this->mErrors; };
  int warnings() const { return this->mWarnings; };

private:
  int mErrors;
  int mWarnings;
};

class MeterInitEvent : public QEvent
{
public:
  MeterInitEvent(const QString &, const QString &);
  const QString & title() const { return this->mTitle; };
  const QString & label() const { return this->mLabel; }

private:
  QString mTitle;
  QString mLabel;
};

class MeterUpdateEvent : public QEvent
{
public:
  MeterUpdateEvent(float, const QString &);
  float percentage() const { return this->mPercentage; };
  const QString & label() const { return this->mLabel; };

private:
  float mPercentage;
  QString mLabel;
};

class MeterSetTotalEvent : public QEvent
{
public:
  MeterSetTotalEvent(float);
  float total() const { return this->mTotal; };

private:
  float mTotal;
};

class MeterIncrementEvent : public QEvent
{
public:
  MeterIncrementEvent(const QString &);
  const QString & label() const { return this->mLabel; };

private:
  QString mLabel;
};

class RefreshSrcWindowEvent : public QEvent
{
public:
  RefreshSrcWindowEvent(const QString &, const QString &);
  const QString & title() const { return this->mTitle; };
  const QString & filename() const { return this->mFilename; };

private:
  QString mTitle;
  QString mFilename;
};

class PogAddEvent : public QEvent
{
public:
  PogAddEvent(const QString & aCheckMark,
              const QString & aModName,
              const QString & aMemberName,
              const QString & aLocationType,
              const QString & aPogType,
              int aPogNum,
              const QString & aFilename,
              const QString & aTmpFileName,
              int aLine,
              int aCol,
              int aLength,
              const QString & aPogDesc);

  const QString & checkMark() const { return this->mCheckMark; };
  const QString & modName() const { return this->mModName; };
  const QString & memberName() const { return this->mMemberName; };
  const QString & locationType() const { return this->mLocationType; };
  const QString & pogType() const { return this->mPogType; };
  int   pogNum() const { return this->mPogNum; };
  const QString & filename() const { return this->mFilename; };
  const QString & tmpFilename() const { return this->mTmpFilename; };
  int   line() const { return this->mLine; };
  int   col() const { return this->mCol; };
  int   length() const { return this->mLength; };
  const QString & pogDesc() const { return this->mPogDesc; };

private:
  QString mCheckMark;
  QString mModName;
  QString mMemberName;
  QString mLocationType;
  QString mPogType;
  int     mPogNum;
  QString mFilename;
  QString mTmpFilename;
  int     mLine;
  int     mCol;
  int     mLength;
  QString mPogDesc;
};

class PogCleanUpEvent : public QEvent
{
public:
  PogCleanUpEvent(const QStringList & aModList);
  const QStringList & modList() const { return this->mModList; };

private:
  QStringList mModList;
};

class LoadProjectEvent : public QEvent
{
public:
  LoadProjectEvent() : QEvent((QEvent::Type)GUIEvent::EV_LOAD_PROJECT) {}
};

class CheckActionsEvent : public QEvent
{
public:
  CheckActionsEvent() : QEvent((QEvent::Type)GUIEvent::EV_CHECK_ACTIONS) {}
};

class PostSaveProjectAsEvent : public QEvent
{
public:
  PostSaveProjectAsEvent() : QEvent((QEvent::Type)GUIEvent::EV_POST_SAVE_PROJECT_AS) {}
};

class NewUnnamedProjectEvent : public QEvent
{
public:
  NewUnnamedProjectEvent() : QEvent((QEvent::Type)GUIEvent::EV_NEW_UNNAMEDPROJECT) {}
};

class PogUpdateFilterEvent : public QEvent
{
public:
  PogUpdateFilterEvent() : QEvent((QEvent::Type)GUIEvent::EV_POG_UPDATE_FILTER) {}
};

class EnableInputEvent : public QEvent
{
public:
  EnableInputEvent() : QEvent((QEvent::Type)GUIEvent::EV_ENABLE_INPUT) {}
};

class DisableInputEvent : public QEvent
{
public:
  DisableInputEvent() : QEvent((QEvent::Type)GUIEvent::EV_DISABLE_INPUT) {}
};

class EnableStopEvent : public QEvent
{
public:
  EnableStopEvent() : QEvent((QEvent::Type)GUIEvent::EV_ENABLE_STOP) {}
};

class DisableStopEvent : public QEvent
{
public:
  DisableStopEvent() : QEvent((QEvent::Type)GUIEvent::EV_DISABLE_STOP) {}
};

class RefreshInterfaceEvent : public QEvent
{
public:
  RefreshInterfaceEvent() : QEvent((QEvent::Type)GUIEvent::EV_REFRESH_INTERFACE) {}
};

class MeterDestroyEvent : public QEvent
{
public:
  MeterDestroyEvent() : QEvent((QEvent::Type)GUIEvent::EV_METER_DESTROY) {}
};

class ClearDebugWindowEvent : public QEvent
{
public:
  ClearDebugWindowEvent() : QEvent((QEvent::Type)GUIEvent::EV_CLEAR_DEBUG_WINDOW) {}
};

class UpdateSelectionsEvent : public QEvent
{
public:
  UpdateSelectionsEvent() : QEvent((QEvent::Type)GUIEvent::EV_UPDATE_SELECTIONS) {}
};

class ClearAllEvent : public QEvent
{
public:
  ClearAllEvent() : QEvent((QEvent::Type)GUIEvent::EV_CLEAR_ALL) {}
};

class MessageClearAllEvent : public QEvent
{
public:
  MessageClearAllEvent() : QEvent((QEvent::Type)GUIEvent::EV_MESSAGE_CLEAR_ALL) {}
};

class MessageAllDoneEvent : public QEvent
{
public:
  MessageAllDoneEvent() : QEvent((QEvent::Type)GUIEvent::EV_MESSAGE_ALL_DONE) {}
};

class MessageNextEvent : public QEvent
{
public:
  MessageNextEvent() : QEvent((QEvent::Type)GUIEvent::EV_MESSAGE_NEXT) {}
};

class MessagePrevEvent : public QEvent
{
public:
  MessagePrevEvent() : QEvent((QEvent::Type)GUIEvent::EV_MESSAGE_PREV) {}
};

class MessageFirstEvent : public QEvent
{
public:
  MessageFirstEvent() : QEvent((QEvent::Type)GUIEvent::EV_MESSAGE_FIRST) {}
};

class MessageLastEvent : public QEvent
{
public:
  MessageLastEvent() : QEvent((QEvent::Type)GUIEvent::EV_MESSAGE_LAST) {}
};

class AddModuleAndStatusEvent : public QEvent
{
public: 
  AddModuleAndStatusEvent(const QString & pModule, const QStringList & pFiles,
                          int pSyntax, int pType, int pCg, int pJcg, int pPp);
  const QString & module() const { return this->mModule; };
  const QStringList & files() const { return this->mFiles; };
  int syntax() const { return this->mSyntax; }
  int type() const { return this->mType; }
  int cg() const { return this->mCg; }
  int jcg() const { return this->mJcg; }
  int pp() const { return this->mPp; }

private:
  QString mModule;
  QStringList mFiles;
  int mSyntax;
  int mType;
  int mCg;
  int mJcg;
  int mPp;
};

class UpdateModuleStatusEvent : public QEvent
{
public: 
  UpdateModuleStatusEvent(const QString & pModule, int pSyntax, int pType, int pCg, int pJcg, int pPp);
  const QString & module() const { return this->mModule; };
  int syntax() const { return this->mSyntax; }
  int type() const { return this->mType; }
  int cg() const { return this->mCg; }
  int jcg() const { return this->mJcg; }
  int pp() const { return this->mPp; }

private:
  QString mModule;
  int mSyntax;
  int mType;
  int mCg;
  int mJcg;
  int mPp;
};
#endif // __customevents_h__

