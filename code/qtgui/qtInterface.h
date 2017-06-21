/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/qtInterface.h,v $
*  * VERSION
*  *    $Revision: 1.12 $
*  * DATE
*  *    $Date: 2006/01/19 04:10:43 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#ifndef __qtInterface_h__
#define __qtInterface_h__

#if QTVER >= 4
#include <QtGui/QtGui>
#else
#include <qapplication.h>
#endif // QTVER >= 4

#include "tools.h"
#include "position.h"
#include "projectval.h"
#include "astaux.h"

//static bool MainLoopContinue;
extern void postGUIEvent(QEvent* ev);

class QtInterface : public vdm_Interface
{
private:
  Sequence bt_seq;
  bool meter_initialized;

public:
  QtInterface() : vdm_Interface(), meter_initialized(false) {};
  virtual ~QtInterface() { this->bt_seq.Clear(); };

  virtual Generic vdm_GetBtSeq() { return this->bt_seq; };
  virtual void vdm_SetBtSeq(const Generic & new_bt_seq) {
    if (new_bt_seq.IsSequence())
      this->bt_seq = new_bt_seq;
  };

  virtual void vdm_ChangedFile() {};

  virtual void vdm_Log(const type_cL &  msg) {
    wstring str (PTAUX::Seq2Str(msg));
    postGUIEvent(new LogWriteEvent(Qt2TB::wstring2qstring(str)));
  };

  virtual void vdm_InterpreterLog(const type_cL &  msg) {
    wstring str (PTAUX::Seq2Str(msg));
    postGUIEvent(new LogWriteEvent(Qt2TB::wstring2qstring(str)));
  };

  virtual void vdm_ChangedMod() {};

  virtual void vdm_Error(const type_cL& ) {
    //    vdm_log << L"TestInterface::vdm_Error is not yet implemented\n" << flush;
  }

  virtual void vdm_ExecuteMainLoop ()
  {
    postGUIEvent(new LogWriteEvent("ExecuteMainLoop"));
  }

  virtual void vdm_CallBack (const Record & cmd)
  {
    EventToCommands(cmd);
  }

  virtual void vdm_InitMeter(const type_cL & title, const type_cL & label)
  {
    // Initialize a meter widget with title L"title" and init text "label"
    this->meter_initialized = true;
    wstring titleStr, labelStr;
    title.GetString(titleStr);
    label.GetString(labelStr);
    postGUIEvent(new MeterInitEvent(Qt2TB::wstring2qstring(titleStr), Qt2TB::wstring2qstring(labelStr)));
  }

  virtual void vdm_IncrementMeter(const Generic& text)
  {
    if (this->meter_initialized) {
      if (text.IsSequence()) {
        Sequence textSeq (text);
        wstring textStr;
        textSeq.GetString(textStr);
        postGUIEvent(new MeterIncrementEvent(Qt2TB::wstring2qstring(textStr)));
      }
      else
        postGUIEvent(new MeterIncrementEvent(""));
    }
  }

  virtual void vdm_SetMeterTotal(const Real& pct)
  {
    if (this->meter_initialized)
      postGUIEvent(new MeterSetTotalEvent(pct.GetValue()));
  }

  virtual void vdm_UpdateMeter(const Real& pct, const type_cL& text)
  {
    if(this->meter_initialized){
    // Update the meter widget to have completed percentage L"pct" with
    // current text L"text"
      wstring labelStr;
      text.GetString(labelStr);
      postGUIEvent(new MeterUpdateEvent(pct.GetValue(), Qt2TB::wstring2qstring(labelStr)));
    }
  }

  // Destroy the meter widget
  virtual void vdm_DestroyMeter()
  {
    this->meter_initialized = false;
    postGUIEvent(new MeterDestroyEvent());
  }

  // Enable user input in the interpreter window
  virtual void vdm_EnableUserInput () {
    postGUIEvent(new EnableInputEvent());
  }

  // Disable user input in the interpreter window
  virtual void vdm_DisableUserInput () {
    postGUIEvent(new DisableInputEvent());
  }

  // Update all windows
  virtual void vdm_RefreshInterface() {
    postGUIEvent(new RefreshInterfaceEvent());
  }

  void EventToCommands(const TYPE_ProjectTypes_CallBackCommand & cmd)
  {
    switch (cmd.GetTag()) {
      case TAG_TYPE_ProjectTypes_ChangedProjName: {
        QString oldp (Qt2TB::wstring2qstring(PTAUX::ExtractOldProjName(cmd)));
        QString newp (Qt2TB::wstring2qstring(PTAUX::ExtractNewProjName(cmd)));
        postGUIEvent(new ChangeProjectNameEvent(oldp, newp));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_AddFiles: {
        SET<TYPE_ProjectTypes_FileName> fnm_s (cmd.GetSet(pos_AddFiles_nms));
        QStringList files;
        Generic fnm;
        for (bool bb = fnm_s.First(fnm); bb; bb = fnm_s.Next(fnm))
        {
          files.append(Qt2TB::wstring2qstring(PTAUX::ExtractFileName(fnm)));
        }
        postGUIEvent(new AddFileEvent(files));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_RemoveFiles: {
        SET<TYPE_ProjectTypes_FileName> fnm_s (cmd.GetSet(pos_RemoveFiles_nms));
        QStringList files;
        Generic fnm;
        for (bool bb = fnm_s.First(fnm); bb; bb = fnm_s.Next(fnm))
        {
          files.append(Qt2TB::wstring2qstring(PTAUX::ExtractFileName(fnm)));
        }
        postGUIEvent(new RemoveFilesEvent(files));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_AddModules: {
        SET<TYPE_ProjectTypes_ModuleName> mnm_s (cmd.GetSet(pos_AddModules_nms));
        Generic mnm;
        for (bool bb = mnm_s.First(mnm); bb; bb = mnm_s.Next(mnm))
        {
          QString module (Qt2TB::wstring2qstring(PTAUX::ExtractModuleName(mnm)));
          SET<TYPE_ProjectTypes_FileName> file_s (ToolMediator::Repos()->vdm_FileOfModule(mnm));
          QStringList files;
          Generic fn;
          for (bool bb = file_s.First(fn); bb; bb = file_s.Next(fn))
          {
            files.append(Qt2TB::wstring2qstring(PTAUX::ExtractFileName(fn)));
          }
          status_type st (PTAUX::ExtractStatus(ToolMediator::Repos()->vdm_Status(mnm)));
          postGUIEvent(new AddModuleAndStatusEvent(module, files,
                                                   Qt2TB::convertStatusType(st.syntax),
                                                   Qt2TB::convertStatusType(st.type),
                                                   Qt2TB::convertStatusType(st.cg),
                                                   Qt2TB::convertStatusType(st.jcg),
                                                   Qt2TB::convertStatusType(st.pp)));
        }
        break;
      }
  
      case TAG_TYPE_ProjectTypes_RemoveModules: {
        SET<TYPE_ProjectTypes_ModuleName> mnm_s (cmd.GetSet(pos_RemoveModules_nms));
        QStringList modules;
        Generic mnm;
        for (bool bb = mnm_s.First(mnm); bb; bb = mnm_s.Next(mnm))
        {
          modules.append(Qt2TB::wstring2qstring(PTAUX::ExtractModuleName(mnm)));
        }
        postGUIEvent(new RemoveModulesEvent(modules));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_ChangedModuleStatus: {
        SET<TYPE_ProjectTypes_ModuleName> mnm_s (cmd.GetSet(pos_ChangedModuleStatus_nms));
        Generic mnm;
        for (bool bb = mnm_s.First(mnm); bb; bb = mnm_s.Next(mnm))
        {
          QString module (Qt2TB::wstring2qstring(PTAUX::ExtractModuleName(mnm)));
          status_type st (PTAUX::ExtractStatus(ToolMediator::Repos()->vdm_Status(mnm)));
          postGUIEvent(new UpdateModuleStatusEvent(module,
                                                   Qt2TB::convertStatusType(st.syntax),
                                                   Qt2TB::convertStatusType(st.type),
                                                   Qt2TB::convertStatusType(st.cg),
                                                   Qt2TB::convertStatusType(st.jcg),
                                                   Qt2TB::convertStatusType(st.pp)));
        }
        break;
      }
  
      case TAG_TYPE_ProjectTypes_ChangedFileStatus: {
/*
        wstring s (PTAUX::ExtractFileName(cmd.GetRecord(pos_ChangedFileStatus_nm)));
        postGUIEvent(new ChangedFileStatusEvent(Qt2TB::wstring2qstring(s)));
        // If we are removing the file changed mark, this must mean that
        // the file has been reread by the Toolbox. Therefore we need to
        // ensure that the source window in the GUI is updated.
*/
        const TYPE_ProjectTypes_FileName & fname (cmd.GetRecord(pos_ChangedFileStatus_nm));
        SET<TYPE_ProjectTypes_ModuleName> mnm_s (ToolMediator::Repos()->vdm_ModulesInFile(fname));
        Generic mnm;
        for (bool bb = mnm_s.First(mnm); bb; bb = mnm_s.Next(mnm))
        {
          QString module (Qt2TB::wstring2qstring(PTAUX::ExtractModuleName(mnm)));
          status_type st (PTAUX::ExtractStatus(ToolMediator::Repos()->vdm_Status(mnm)));
          postGUIEvent(new UpdateModuleStatusEvent(module,
                                                   Qt2TB::convertStatusType(st.syntax),
                                                   Qt2TB::convertStatusType(st.type),
                                                   Qt2TB::convertStatusType(st.cg),
                                                   Qt2TB::convertStatusType(st.jcg),
                                                   Qt2TB::convertStatusType(st.pp)));
        }
        break;
      }
      case TAG_TYPE_ProjectTypes_RemoveFileChangedMark: {
        wstring s (PTAUX::ExtractFileName(cmd.GetRecord(pos_RemoveFileChangedMark_nm)));
        postGUIEvent(new ChangedFileStatusEvent(Qt2TB::wstring2qstring(s)));
        // If we are removing the file changed mark, this must mean that
        // the file has been reread by the Toolbox. Therefore we need to
        // ensure that the source window in the GUI is updated.
        wstring src (Qt2TB::GetSrcFileI(s));
        postGUIEvent(new RefreshSrcWindowEvent(Qt2TB::wstring2qstring(s), Qt2TB::wstring2qstring(src)));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_BreakNameSet: {
        Tuple t (PTAUX::ExtractBreakNameSet(cmd));
        wstring b (ASTAUX::ASName2String(t.GetRecord(1)));
        postGUIEvent(new AddBreakNameEvent(Qt2TB::wstring2qstring(b), t.GetIntValue(2)));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_BreakPosSet: {
        Tuple t (PTAUX::ExtractBreakPosSet(cmd));
        postGUIEvent(new AddBreakPosEvent(Qt2TB::wstring2qstring(ASTAUX::Id2String(ASTAUX::GetFirstId(t.GetRecord(1)))),
                                          t.GetIntValue(2), t.GetIntValue(3), t.GetIntValue(4)));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_BreakRemove: {
        int num = PTAUX::ExtractBreakRemove(cmd);
        postGUIEvent(new RemoveBreakEvent(num));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_BreakEnable: {
        int num = PTAUX::ExtractBreakEnable(cmd);
        postGUIEvent(new EnableBreakEvent(num));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_BreakDisable: {
        int num = PTAUX::ExtractBreakDisable(cmd);
        postGUIEvent(new DisableBreakEvent(num));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_BackTrace: {
        const Sequence & bt_infos (cmd.GetSequence(pos_BackTrace_info));
        //ToolMediator::Interf ()->vdm_SetBtSeq(bt_infos);
        vdm_SetBtSeq(bt_infos);
#if QT_VERSION >= 0x040000
        QList<BackTraceEntry> bts;
#else
        QValueList<BackTraceEntry> bts;
#endif // QT_VERSION >= 0x040000
        int len_bt_infos = bt_infos.Length();
        for (int i = 1; i <= len_bt_infos; i++)
        {
          const Tuple & info (bt_infos[i]); // seq of char * [seq of SEM`VAL] * bool
  
          // Get name field
          wstring nameStr;
          Sequence nameSeq (info.GetSequence (1));
          nameSeq.GetString(nameStr);
          QString nameString(Qt2TB::wstring2qstring(nameStr));
          nameString.replace(QRegExp("\n"), "");

          BackTraceEntry bte;
          bte.name = nameString;
  
          // Get args field
          Generic args (info.GetField (2));
          bte.args = (args.IsNil());

          // Is it a debug line?
          bool isDebugLine = info.GetBoolValue(3);
          bte.isDebug = isDebugLine;
          bts.append(bte);
        }
        postGUIEvent(new BackTraceEvent(bts));
        break;
      }

      case TAG_TYPE_ProjectTypes_BtGotoLevel: {
        int num = PTAUX::ExtractBtGotoLevel(cmd);
        postGUIEvent(new GotoBtLevelEvent(num));
        break;
      }
  
      case TAG_TYPE_ProjectTypes_DependInfo: {
        break;
      }

      case TAG_TYPE_ProjectTypes_DependUpdate: {
        break;
      }

      case TAG_TYPE_ProjectTypes_DrawInhTree: {
        break;
      }

      case TAG_TYPE_ProjectTypes_ClearInhTree: {
        break;
      }
  
      case TAG_TYPE_ProjectTypes_ClearDebugWindow: {
        postGUIEvent(new ClearDebugWindowEvent());
        break;
      }
      case TAG_TYPE_ProjectTypes_UpdateSelections: {
        postGUIEvent(new UpdateSelectionsEvent());
        break;
      }
      case TAG_TYPE_ProjectTypes_ClearAll: {
        postGUIEvent(new ClearAllEvent());
        break;
      }
      case TAG_TYPE_ProjectTypes_PogCleanUp: {
        SEQ<TYPE_ProjectTypes_ModuleName> nms (cmd.GetSet(pos_PogCleanUp_nms));
        QStringList modules;
        size_t len_nms = nms.Length();
        for (size_t idx = 1; idx <= len_nms; idx++)
        {
          modules.append(Qt2TB::wstring2qstring(PTAUX::ExtractModuleName(nms[idx])));
        }
        postGUIEvent(new PogCleanUpEvent(modules));
        break;
      }
      case TAG_TYPE_ProjectTypes_PogAdd: {
        postGUIEvent(new PogAddEvent(Qt2TB::wstring2qstring(cmd.GetSequence(pos_PogAdd_checked).GetString()),
                                     Qt2TB::wstring2qstring(cmd.GetSequence(pos_PogAdd_modnm).GetString()),
                                     Qt2TB::wstring2qstring(cmd.GetSequence(pos_PogAdd_memnm).GetString()),
                                     Qt2TB::wstring2qstring(cmd.GetSequence(pos_PogAdd_locclass).GetString()),
                                     Qt2TB::wstring2qstring(cmd.GetSequence(pos_PogAdd_kind).GetString()),
                                     cmd.GetIntValue(pos_PogAdd_no),
                                     Qt2TB::wstring2qstring(cmd.GetSequence(pos_PogAdd_file).GetString()),
                                     Qt2TB::wstring2qstring(cmd.GetSequence(pos_PogAdd_tmpfn).GetString()),
                                     cmd.GetIntValue(pos_PogAdd_line),
                                     cmd.GetIntValue(pos_PogAdd_column),
                                     cmd.GetIntValue(pos_PogAdd_length),
                                     Qt2TB::wstring2qstring(cmd.GetSequence(pos_PogAdd_po).GetString())
                                    ));
        break;
      }
      case TAG_TYPE_ProjectTypes_PogUpdateFilter: {
        postGUIEvent(new PogUpdateFilterEvent());
        break;
      }
    }
  }
};

class QtErrors : public vdm_Errors
{
private:
  int GetMsgPrt() { return this->vdm_msgPtr.GetValue(); }

public:
  QtErrors() : vdm_Errors() {};
  virtual ~QtErrors() {};

  void SetMsgPtr(int i) { this->vdm_msgPtr = Int(i); }

  virtual void vdm_SetFilePosition(const Record & msg)
  {
    int fileid = msg.GetInt (pos_ErrMsg_fid);
    int lineid = msg.GetInt (pos_ErrMsg_line);
    int columnid = msg.GetInt (pos_ErrMsg_col);

    int length = POSITION::GetLength(); // 20081201

    wstring file (Qt2TB::GetFileNameI(fileid));

    if (!file.empty())
    {
      wstring src (Qt2TB::GetSrcFileI(file));
      postGUIEvent(new OpenFileEvent(Qt2TB::wstring2qstring(src),
                                     Qt2TB::wstring2qstring(file),
                                     lineid,
                                     columnid,
                                     length));
    }
  }

  virtual void vdm_ShowMsg() {
    // Extract error message
    TYPE_ProjectTypes_Message msg (this->vdm_msgs[this->vdm_msgPtr]);

    // Set file position
    int fileid = msg.GetIntValue(pos_ErrMsg_fid);
    if (fileid == -10)
      return;

    wstring file (Qt2TB::GetFileNameI(fileid));

    vdm_SetFilePosition(msg);

    if (Int(CI_FID_CMD_LINE) == msg.GetInt (pos_ErrMsg_fid))
      file = wstring(L"command line");

    // Add message to error window
    int i = this->vdm_msgPtr.GetValue();
    wstring s (PTAUX::ExtractMessageAll(file, msg));
    postGUIEvent(new MessageShowEvent(i, Qt2TB::wstring2qstring(s)));
    //    m->showMessage(i, s.c_str());
  }

  virtual void vdm_EventAddMsg(const Int & I, const TYPE_ProjectTypes_Message & r)
  {
    int errnum = I.GetValue();
    int fileid = r.GetIntValue (pos_ErrMsg_fid);
    int line = r.GetIntValue (pos_ErrMsg_line);
    int col = r.GetIntValue (pos_ErrMsg_col);

    wstring file (Qt2TB::GetFileNameI(fileid));

    wstring file2 (file);
    if (CI_FID_CMD_LINE == fileid)
      file2 = wstring(L"command line");

    wstring s (PTAUX::ExtractMessageAll(file2, r));

    if (col != -1)
      vdm_SetFilePosition(r);

    wstring tmpfn (Qt2TB::GetSrcFileI(file));

    postGUIEvent(new MessageAddEvent(errnum,
                                     Qt2TB::wstring2qstring(file),
                                     line,
                                     col - 1,
                                     Qt2TB::wstring2qstring(s),
                                     Qt2TB::wstring2qstring(tmpfn)));
  }

  virtual void vdm_EventClearAll() {
    postGUIEvent(new MessageClearAllEvent());
  }

  virtual void vdm_EventAllDone() {
    postGUIEvent(new MessageAllDoneEvent());
  }

  // Go to the next message
  void vdm_EventNext()
  {
    postGUIEvent(new MessageNextEvent());
  }

  // Go to the previous message
  void vdm_EventPrev()
  {
    postGUIEvent(new MessagePrevEvent());
  }

  // Go to the first message
  void vdm_EventFirst()
  {
    postGUIEvent(new MessageFirstEvent());
  }

  // Go to the last message
  void vdm_EventLast()
  {
    postGUIEvent(new MessageLastEvent());
  }

  virtual void vdm_MakeStatus(const Int & NoErr, const Int & NoWarn)
  {
    if ((NoErr.GetValue() > 0) || (NoWarn.GetValue() > 0))
    {
      postGUIEvent(new ErrorStatusEvent(NoErr.GetValue(), NoWarn.GetValue()));
    }
  }
};

class QtExprErrors : public vdm_Errors
{
private:
  int GetMsgPrt() { return this->vdm_msgPtr.GetValue(); }

public:
  QtExprErrors() : vdm_Errors() {};
  virtual ~QtExprErrors() {};

  void SetMsgPtr(int i) { this->vdm_msgPtr = Int(i); }

  virtual void vdm_EventAddMsg(const Int & I, const Record & r)
  {
    int errnum = I.GetValue();
    int fileid = r.GetIntValue(pos_ErrMsg_fid);
    int line = r.GetIntValue(pos_ErrMsg_line);
    int col = r.GetIntValue(pos_ErrMsg_col);

    wstring file (Qt2TB::GetFileNameI(fileid));
    wstring s (PTAUX::ExtractMessageAll(file, r));
    wstring tmpfn (Qt2TB::GetSrcFileI(file));

    postGUIEvent(new MessageAddEvent(errnum,
                                     Qt2TB::wstring2qstring(file),
                                     line,
                                     col - 1,
                                     Qt2TB::wstring2qstring(s),
                                     Qt2TB::wstring2qstring(tmpfn)));
  }

  virtual void vdm_EventClearAll() {}
  virtual void vdm_EventAllDone() {}
  virtual void vdm_SetFilePosition(const Record & msg) {}
  virtual void vdm_ShowMsg() {}
  virtual void vdm_MakeStatus(const Int & NoErr, const Int & NoWarn) {}
};

#endif // __qtInterface_h__
