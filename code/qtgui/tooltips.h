/***
*  * WHAT
*  * FILE
*  *    $Source: /home/vdmtools/cvsroot/toolbox/code/qtgui/tooltips.h,v $
*  * VERSION
*  *    $Revision: 1.24 $
*  * DATE
*  *    $Date: 2006/06/29 08:37:52 $
*  * AUTHOR
*  *    $Author: vdmtools $
*  * COPYRIGHT
*  *    (C) Kyushu University
***/
#ifndef __tooltips_h__
#define __tooltips_h__

//const char * exitText =      "This operation exits the program";
//const char * aboutText =     "General information about the program";
//const char * aboutqtText =   "Information about the QT version used";
const char * whatsthisText = "Gives additional information about selected item";
const char * saveErrorText = "Warning: unable to save project file";
const char * confirmFileRemovalText = "Confirm removal of file(s) from project";
const char * okText = "&Ok";
const char * cancelText = "&Cancel";
const char * javaWinSelErr = "Please select classes from the Java class browser";
const char * saveProjectPrompt = "Project is modified: Save changes to project?";
const char * yesText = "&Yes";
const char * noText = "&No";
const char * confirmFileOverwriteText = "File already exists. Confirm overwrite of old file:";
//const char * selectAllText = "Select all items";
const char * recentProjectText = "Select recent project";

actionSpec projectActions[] = {
  { "&New Project" , Qt::CTRL + Qt::Key_N, "Creates a new project", "projectnew.png", true}, 
  { "&Load Project ...", Qt::CTRL + Qt::Key_O, "Loads an existing project", "load.png", true},
  { "&Save Project ...", Qt::CTRL + Qt::Key_S, "Saves current project", "projectsave.png", true},
  { "Save Project &As ...", Qt::CTRL + Qt::SHIFT + Qt::Key_S, "Saves current project under a new name",
    "projectsaveas.png", true},
  { "", -1, "", NULL, true},
  { "Add &File to Project ...",  Qt::CTRL + Qt::Key_R, "Adds selected files to project", "plus.png", true},
  { "&Remove File from Project", Qt::Key_Delete, "Removes selected files from project", "minus.png", true},
  { "", -1, "", NULL, true},
  { "Project &Options ...", 0, "Shows and edits current project options", "projectoptions.png", true},
  { "&Tool Options ...",    0, "Select tool options", "tooloptions.png", true},
  { "", -1, "", NULL, true},
  { 0, 0, 0, 0, false }
};

actionSpec fileActions[] = {
  { "New File" , Qt::CTRL + Qt::SHIFT + Qt::Key_N, "Creates a new file", "projectnew.png", true}, 
  { "&External Editor", Qt::CTRL + Qt::Key_E, "Opens selected files in the external editor",
                                              "externaleditor.png", true},
#if QT_VERSION >= 0x040000
  { "&Close file", Qt::CTRL + Qt::Key_L, "Closes selected file from Source Window", "fileclose.png", true},
#else
  { "&Close file", Qt::CTRL + Qt::Key_W, "Closes selected file from Source Window", "fileclose.png", true},
#endif // QT_VERSION >= 0x040000
  { "Close &All files", Qt::SHIFT + Qt::CTRL + Qt::Key_W, "Closes all files from Source Window",
    "filecloseall.png", true},
  { "", -1, "", NULL, false},
  { "&Select All", Qt::CTRL + Qt::Key_A, "Select all items", NULL, false},
  { 0, 0, 0, 0, false }
};

actionSpec windowActions[] = {
  { "&Manager", Qt::ALT + Qt::Key_M, "Allows managing of Project files,Browse and apply actions to classes",
    "browser.png", true},
  { "&Source Window", Qt::ALT + Qt::Key_S, "Allows user to view text", "source.png", true},
  { "&Log Window", Qt::ALT + Qt::Key_L, "Shows project log", "log.png", true},
  { "In&terpreter", Qt::ALT + Qt::Key_T, "Opens the VDM++ interpreter window", "interpreter.png", true},
  { "&Error List", Qt::ALT + Qt::Key_E, "Lists all error that ocurred", "error.png", true},
  { "&Integrity properties", 0, // Qt::ALT + Qt::Key_I, // conflicts with Interpreter
    "Shows integrity properties", "integrity.png", true},
  { "Search Identifier", 0, "Search identifier", "syntaxcheckdone.png", true},
  { 0, 0, 0, 0, false }
};

actionSpec roseAction = {
  "&UML Link", Qt::ALT + Qt::Key_U, "Opens the UML link window", "rose.png", true
};

actionSpec actionActions[] = {
  { "&Syntax Check", Qt::Key_F5, "Performs syntax check on selected classes", "syntaxcheck.png", true},
  { "&Type Check", Qt::Key_F6, "Performs type check on selected classes", "typecheck.png", true},
  { "&Integrity Check", Qt::CTRL + Qt::Key_F9, "Generates integrity properties for the selected classes",
    "integritycheck.png", true},
  { "&Generate C++", Qt::Key_F7, "Generates C++ code for selected classes", "cplusplus.png", true},
  { "Generate &Java", Qt::Key_F8, "Generates Java code for selected classes", "java.png", true},
  { "&Pretty Print", Qt::CTRL + Qt::Key_F7, "Pretty prints the selected class", "prettyprint.png", true},
  { "Java to &Vdm", Qt::CTRL + Qt::Key_F8, "Generates VDM++ classes for the selected Java classes",
    "java2vdm.png", true},
  { 0, 0, 0, 0, false }
};

actionSpec interpreterActions[] = {
  { "Init", Qt::SHIFT + Qt::Key_F9, "Initializes the interpreter", "runI.png", true},
  { "Step", Qt::Key_F10, "Perform a step", "stepI.png", true},
  { "Step In", Qt::ALT + Qt::Key_F10, "Step inside ...", "stepintoI.png", true},
  { "Single Step", Qt::CTRL + Qt::Key_F10, "Perform a single step", "singlestepI.png", true},
  { "Continue", Qt::Key_F9, "Continues the execution", "continueI.png", true},
  { "Stop", Qt::Key_Escape, "Stops the interpreter", "pauseI.png", true}, 
  { "Up", Qt::CTRL + Qt::Key_Up, "Jump to where the current function was called in function trace", "upI.png", true},
  { "Down", Qt::CTRL + Qt::Key_Down, "Jump to where the function called the current subfunction in function trace",
    "downI.png", true},
  { "Finish", Qt::SHIFT + Qt::Key_F12, "Finishes the execution", "stopI.png", true},
  { "Debug Expression", Qt::SHIFT + Qt::CTRL + Qt::Key_E, "Debug stored expression", "continueI.png", false},
  { "Start Call Log", Qt::SHIFT + Qt::CTRL + Qt::Key_L, "Function/operation call log", "continueI.png", false},
  { 0, 0, 0, 0, false }
};

actionSpec helpActions[] = {
  { "&About", 0, "General information about the program", "help.png", true},
  { "About &Qt", 0, "Information about the QT version used", "qt.png", true},
  { 0, 0, 0, 0, false }
};

actionSpec exitAction = {
  "E&xit", Qt::CTRL + Qt::Key_X, "This operation exits the program", 0, false
};

#endif // __tooltips_h__
