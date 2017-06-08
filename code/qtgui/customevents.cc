/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/customevents.cc,v $
*  * VERSION
*  *    $Revision: 1.13 $
*  * DATE
*  *    $Date: 2006/04/21 04:33:23 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#include "customevents.h"

AddFileEvent::AddFileEvent(const QStringList& pFiles)
  : QEvent((QEvent::Type)GUIEvent::EV_ADD_FILES), mFiles(pFiles) {}

RemoveFilesEvent::RemoveFilesEvent(const QStringList& pFiles)
  : QEvent((QEvent::Type)GUIEvent::EV_REMOVE_FILES), mFiles(pFiles) {}

AddModulesEvent::AddModulesEvent(const QStringList& pModules)
  : QEvent((QEvent::Type)GUIEvent::EV_ADD_MODULES), mModules(pModules) {}

RemoveModulesEvent::RemoveModulesEvent(const QStringList& pModules)
  : QEvent((QEvent::Type)GUIEvent::EV_REMOVE_MODULES), mModules(pModules) {}

ChangeModulesStatusEvent::ChangeModulesStatusEvent(const QStringList& pModules)
  : QEvent((QEvent::Type)GUIEvent::EV_CHANGE_MODULES_STATUS), mModules(pModules) {}

ChangedFileStatusEvent::ChangedFileStatusEvent(const QString & pFile)
  : QEvent((QEvent::Type)GUIEvent::EV_CHANGED_FILE_STATUS), mFile(pFile) {}

AddBreakNameEvent::AddBreakNameEvent(const QString & pName, int pNum)
  : QEvent((QEvent::Type)GUIEvent::EV_ADD_BREAK_NAME), mName(pName), mNum(pNum) {}

AddBreakPosEvent::AddBreakPosEvent(const QString & pName, int pLine, int pCol, int pNum)
  : QEvent((QEvent::Type)GUIEvent::EV_ADD_BREAK_POS), mName(pName), mLine(pLine), mCol(pCol), mNum(pNum) {}

RemoveBreakEvent::RemoveBreakEvent(int pNum)
  : QEvent((QEvent::Type)GUIEvent::EV_REMOVE_BREAK), mNum(pNum) {}

EnableBreakEvent::EnableBreakEvent(int pNum)
  : QEvent((QEvent::Type)GUIEvent::EV_ENABLE_BREAK), mNum(pNum) {}

DisableBreakEvent::DisableBreakEvent(int pNum)
  : QEvent((QEvent::Type)GUIEvent::EV_DISABLE_BREAK), mNum(pNum) {}

BackTraceEvent::BackTraceEvent(const QLIST<BackTraceEntry>& pBts)
  : QEvent((QEvent::Type)GUIEvent::EV_BACK_TRACE), mBts(pBts) {}

GotoBtLevelEvent::GotoBtLevelEvent(int pLevel)
  : QEvent((QEvent::Type)GUIEvent::EV_GOTO_BT_LEVEL), mLevel(pLevel) {}

///////////////////////////////////////////////////////////////

OpenFileEvent::OpenFileEvent(const QString & pFilename, const QString & pTitle,
			     int pLine, int pColumn, int pLength)
  : QEvent((QEvent::Type)GUIEvent::EV_OPENFILE),
    mFilename(pFilename), mTitle(pTitle), mLine(pLine), mColumn(pColumn), mLength(pLength) {}

ChangeProjectNameEvent::ChangeProjectNameEvent(const QString& pOld, const QString& pNew)
  : QEvent((QEvent::Type)GUIEvent::EV_CHANGE_PROJECT_NAME), mOldName(pOld), mNewName(pNew) {}

RoseDiffEvent::RoseDiffEvent(const QString& pText)
  : QEvent((QEvent::Type)GUIEvent::EV_ROSE_DIFF), mText(pText) {}

InterpreterWriteEvent::InterpreterWriteEvent(const QString& pText)
  : QEvent((QEvent::Type)GUIEvent::EV_INTERPRETER_WRITE), mText(pText) {}

LogWriteEvent::LogWriteEvent(const QString & pText)
  : QEvent((QEvent::Type)GUIEvent::EV_LOG_WRITE), mText(pText) {}

RoseErrorEvent::RoseErrorEvent(const QString& pText)
  : QEvent((QEvent::Type)GUIEvent::EV_ROSE_ERROR), mText(pText) {}

MessageShowEvent::MessageShowEvent(int pIndex, const QString & pMessage)
  : QEvent((QEvent::Type)GUIEvent::EV_MESSAGE_SHOW), mIndex(pIndex), mMessage(pMessage) {}

MessageAddEvent::MessageAddEvent(int pErrnum, const QString & pFilename,
				 int pLine, int pColumn,
                                 const QString & pMessage,
				 const QString & pTempFilename)
  : QEvent((QEvent::Type)GUIEvent::EV_MESSAGE_ADD),
    mErrnum(pErrnum), mFilename(pFilename), mLine(pLine), mColumn(pColumn), mMessage(pMessage),
    mTempFilename(pTempFilename) {}

ErrorStatusEvent::ErrorStatusEvent(int pErrors, int pWarnings)
  : QEvent((QEvent::Type)GUIEvent::EV_ERROR_STATUS), mErrors(pErrors), mWarnings(pWarnings) {}

PogAddEvent::PogAddEvent(
            const QString & aCheckMark,
            const QString & aModName,
            const QString & aMemberName,
            const QString & aLocationType,
            const QString & aPogType,
            int aPogNum,
            const QString & aFilename,
            const QString & aTmpFilename,
            int aLine,
            int aCol,
            int aLength,
            const QString & aPogDesc)
  : QEvent((QEvent::Type)GUIEvent::EV_POG_ADD),
    mCheckMark(aCheckMark),
    mModName(aModName),
    mMemberName(aMemberName),
    mLocationType(aLocationType),
    mPogType(aPogType),
    mPogNum(aPogNum),
    mFilename(aFilename),
    mTmpFilename(aTmpFilename),
    mLine(aLine),
    mCol(aCol),
    mLength(aLength),
    mPogDesc(aPogDesc) {}

PogCleanUpEvent::PogCleanUpEvent(const QStringList& aModList)
  : QEvent((QEvent::Type)GUIEvent::EV_POG_CLEAN), mModList(aModList) {}

MeterInitEvent::MeterInitEvent(const QString & pTitle, const QString & pLabel)
  : QEvent((QEvent::Type)GUIEvent::EV_METER_INIT), mTitle(pTitle), mLabel(pLabel) {}

MeterUpdateEvent::MeterUpdateEvent(float pPercentage, const QString & pLabel)
  : QEvent((QEvent::Type)GUIEvent::EV_METER_UPDATE), mPercentage(pPercentage), mLabel(pLabel) {}
  
MeterSetTotalEvent::MeterSetTotalEvent(float pTotal)
  : QEvent((QEvent::Type)GUIEvent::EV_METER_SET_TOTAL), mTotal(pTotal) {}
  
MeterIncrementEvent::MeterIncrementEvent(const QString & pLabel)
  : QEvent((QEvent::Type)GUIEvent::EV_METER_INCREMENT), mLabel (pLabel) {}

RefreshSrcWindowEvent::RefreshSrcWindowEvent(const QString & pTitle, const QString & pFilename)
  : QEvent((QEvent::Type)GUIEvent::EV_REFRESH_SRC_WINDOW), mTitle(pTitle), mFilename(pFilename) {}

AddModuleAndStatusEvent::AddModuleAndStatusEvent(const QString & pModule, const QStringList & pFiles,
                                                 int pSyntax, int pType, int pCg, int pJcg, int pPp)
  : QEvent((QEvent::Type)GUIEvent::EV_ADD_MODULE_AND_STATUS), 
    mModule(pModule), mFiles(pFiles), mSyntax(pSyntax), mType(pType), mCg(pCg), mJcg(pJcg), mPp(pPp) {}

UpdateModuleStatusEvent::UpdateModuleStatusEvent(const QString & pModule,
                                                 int pSyntax, int pType, int pCg, int pJcg, int pPp)
  : QEvent((QEvent::Type)GUIEvent::EV_UPDATE_MODULE_STATUS), 
    mModule(pModule), mSyntax(pSyntax), mType(pType), mCg(pCg), mJcg(pJcg), mPp(pPp) {}

