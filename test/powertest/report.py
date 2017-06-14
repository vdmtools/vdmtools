true, false = 1,0

import sys, string, time, getpass, socket, cmdline,os
import cmdline, util

#--------------------------------------------------------------------------------
#                             Global Variables
#--------------------------------------------------------------------------------
errStream = None
currentTestEnv = ''
currentRunType=''
currentLanguage = ''
currentTestCaseName = ''
currentTypeCheckType = ''
currentIPType = ''
startSec = None

ignoreFilesList = []
counters = {}
errorCases = {}

firstMessage = true

#--------------------------------------------------------------------------------
# Open error and report files
#--------------------------------------------------------------------------------
def Init():
  global errStream, reportStream, startSec

  ### The different log files
  dateStr = cmdline.StartDate()
  errorFile = 'errors-'+dateStr
  reportFile = 'report-'+dateStr
  
  ### Open the log files once and for all
  errStream = open(errorFile,"w")
  reportStream = open(reportFile,"w")

  util.SymLink(errorFile,"errors",true)
  util.SymLink(reportFile,"report",true)

  startSec = time.time()


#--------------------------------------------------------------------------------
# This function prints out progress information on standard output
# LEVEL is used to structure the output and may be set in the configuration
# file, to hide some of this progress information.
# MSG is the message printed.
#
# The following levels should be used:
# 1 - Start of new test environment / new language / new type
# 2 - Overall test case progress (handling case 1-10)
# 3 - detailed test case progress (preparing xxx, runnig xxx)
# 4 - detail execution progress (converting xxx to SL)
# 5 - print out output from all command executed.
#--------------------------------------------------------------------------------
def Progress(level,msg):  
  if (cmdline.LookUp('debug-level') >= level or cmdline.LookUp('debug-level') == -1):
    print (" " * 2 * (level-1) + msg)


#--------------------------------------------------------------------------------
# Print out output from a system run.
# Standard output is printed in blue, and standard error is printed i red.
#--------------------------------------------------------------------------------
def PrintCmdOutput(stdoutFile, stderrFile):
  if (cmdline.LookUp('debug-level') >= 5 or cmdline.LookUp('debug-level') == -1):
    if os.path.exists(stdoutFile):
      data = util.ReadFile(stdoutFile)
      if data:
        print ("[07m[34m" + data + "[30m[0m")
    if os.path.exists(stderrFile):
      data = util.ReadFile(stderrFile)
      if data:
        print ("[07m[31m" + data + "[30m[0m")
  
  
#--------------------------------------------------------------------------------
# Print an error message to the error file.
# msg - the actual error message
# diagnosis - a potentially diagnosis message
# stdout - the output on stdout ( used when reporting an error from a test
#                                 execution) 
# stderr - the output on stderr.
# return - Nothing
#--------------------------------------------------------------------------------

def Header():
  global errStream, firstMessage

  if firstMessage == false:
    os._exit(-1)
  

  l = (78 - len(currentTestCaseName)) /2
  errStream.write("*" * 80 + "\n")
  errStream.write(" " * l + currentTestCaseName + "\n")
  errStream.write("*" * 80 + "\n")
  errStream.write("Test environment: " + currentTestEnv + "\n")
  errStream.write("Run type: " + currentRunType + "\n")
  errStream.write("Language: " + currentLanguage + "\n")
  if (currentTestEnv == 'tc'):
    errStream.write("Type check type: " + currentTypeCheckType + "\n")
  if (currentTestEnv == 'ip'):
    errStream.write("Ip type : " + currentIPType + "\n")
  errStream.write("-" * 10 + "\n")

def Error(msg, diagnosis=None, stdout=None, stderr=None):
  global firstMessage, errStream
  if firstMessage == true:
    Header()
    firstMessage = false
    
  errStream.write("Message:\n  ")
  errStream.write(msg+"\n")

  if cmdline.LookUp('show-errors-in-progress'):
    Progress(1,"*** " + msg + " ***")
  
  if diagnosis:
    errStream.write("Possible diagnosis:\n")
    for line in diagnosis.split("\n"):
      errStream.write("  " + line + "\n")

  if stdout:
    errStream.write("Standard output:\n  ")
    for line in stdout.split("\n"):
      errStream.write("  " + line + "\n")

  if stderr:
    errStream.write("Standard error:\n  ")
    for line in stderr.split("\n"):
      errStream.write("  " + line + "\n")

  errStream.write("\n\n\n")
  errStream.flush()

  if currentTestEnv == 'tc':
    SetValueInMap(errorCases, [currentTestEnv,currentRunType,currentLanguage, currentTypeCheckType], [], lambda x: x + [currentTestCaseName])
  elif currentTestEnv == 'ip':
    SetValueInMap(errorCases, [currentTestEnv,currentRunType,currentLanguage, currentIPType], [], lambda x: x + [currentTestCaseName])
  else:
    SetValueInMap(errorCases, [currentTestEnv,currentRunType,currentLanguage], [], lambda x: x + [currentTestCaseName])


#--------------------------------------------------------------------------------
# Reports that the file 'file' will be ignored due to an .ignore file.
#--------------------------------------------------------------------------------
def IgnoreFile(file):
  global ignoreFilesList
  if cmdline.LookUp('show-errors-in-progress'):
    Progress(1, "*** Ignoring file: " + file + " ***")
  if ignoreFilesList.count(file) == 0:
    ignoreFilesList.append(file)

#--------------------------------------------------------------------------------
# Sets the name for the test case being executed. This is used when
# reporting error messages
#--------------------------------------------------------------------------------
def SetTestEnvName(name): # TC, IP, CPP, JAVA, ...
  global currentTestEnv
  currentTestEnv = name

def SetRunType(name): # SPEC, IMPL
  global currentRunType
  currentRunType = name

def SetLanguage(name): # SL, PP, RT
  global currentLanguage
  currentLanguage = name

def setTypeTestType(name): # POS, DEF
  global currentTypeCheckType
  currentTypeCheckType = name

def SetIPType(type): # DTC-ON, DTC-OFF
  global currentIPType
  currentIPType = type

def setTestCaseName(name): 
  global currentTestCaseName, firstMessage
  currentTestCaseName = name
  firstMessage = true
  

def IncrTestcaseCounter():
  global counters

  if currentTestEnv == 'tc':
    SetValueInMap(counters, [currentTestEnv,currentRunType,currentLanguage, currentTypeCheckType], 0, lambda x:x+1)
  elif currentTestEnv == 'ip':
    SetValueInMap(counters, [currentTestEnv,currentRunType,currentLanguage, currentIPType], 0, lambda x:x+1)
  else:
    SetValueInMap(counters, [currentTestEnv,currentRunType,currentLanguage], 0, lambda x:x+1)

def SetValueInMap(base,names, init, func):
  val = base
  for name in names[:-1]:
    if not val.has_key(name):
      val[name] = {}
    val = val[name]
  lastElm = names[-1]
  if not val.has_key(lastElm):
    val[lastElm] = init
  val[lastElm] = func(val[lastElm])

#--------------------------------------------------------------------------------
# Generate report of all test runs.
#--------------------------------------------------------------------------------
def genReport():
  global reportStream

  ## Print out the data
  reportStream.write(time.strftime("Report by powertest - %c\n\n",time.localtime(time.time())))
  
  reportStream.write("User: " + getpass.getuser()+"\n")
  reportStream.write("Host: " + socket.gethostname()+"\n")

  tot = int(time.time() - startSec);
  reportStream.write("Total time: " + PrintTime(tot) +"\n")

  reportStream.write("Cmd line: " + cmdline.GetCmdLine()+"\n")

  reportError()
  reportIgnores()
  reportTotal()
  reportUsedWildCards()

def PrintTime(tot):
  hour = tot/3600
  min  = (tot%3600) / 60
  sec  = (tot%3600) % 60

  if hour < 10:
    res = '0' + str(hour) + ":"
  else:
    res = str(hour)+":"

  if min < 10:
    res = res + "0" + str(min) +":"
  else:
    res = res + str(min) +":"

  if sec < 10:
    return res + "0" + str(sec)
  else:
    return res + str(sec)

#--------------------------------------------------------------------------------
#
#--------------------------------------------------------------------------------
def reportError():
  printHeader("Errors")
  for testEnv in errorCases.keys():
    for runType in errorCases[testEnv].keys():
      for language in errorCases[testEnv][runType].keys():
        if testEnv == 'tc':
          for tctype in errorCases[testEnv][runType][language].keys():
            for err in errorCases[testEnv][runType][language][tctype]:
              PrintData(testEnv, runType, language, tctype, err)
        elif testEnv == 'ip':
          for iptype in errorCases[testEnv][runType][language].keys():
            for err in errorCases[testEnv][runType][language][iptype]:
              PrintData(testEnv, runType, language, iptype, err)
        else:
          for err in errorCases[testEnv][runType][language]:
            PrintData(testEnv, runType, language, None, err)

#--------------------------------------------------------------------------------
# Report which files has been ignored
#--------------------------------------------------------------------------------
def reportIgnores():
  global ignoreFilesList

  printHeader("Files ignored")
  if len(ignoreFilesList) == 0:
    reportStream.write("No files ignored\n")
  else:
    for file in ignoreFilesList:
      reportStream.write(file+"\n")

#--------------------------------------------------------------------------------
# Print out total numbers of test cases and errors.
#--------------------------------------------------------------------------------
def reportTotal():
  global counters
  printHeader("Totals")
  for testEnv in counters.keys():
    for runType in counters[testEnv].keys():
      for language in counters[testEnv][runType].keys():
        if testEnv == 'tc':
          for tctype in counters[testEnv][runType][language].keys():
            PrintData(testEnv, runType, language, tctype, str(counters[testEnv][runType][language][tctype]))
        elif testEnv == 'ip':
          for iptype in counters[testEnv][runType][language].keys():
            PrintData(testEnv, runType, language, iptype, str(counters[testEnv][runType][language][iptype]))
        else:
          PrintData(testEnv, runType, language, None, str(counters[testEnv][runType][language]))

def reportUsedWildCards():
  printHeader("Used Wildcards")
  l = cmdline.GetUsedWildCards()
  for (name, orig) in l:
    reportStream.write(name +" ("+orig+")\n")

def PrintData(testenv, runtype, language, tciptype, txt):
  reportStream.write(testenv + "," +
                     runtype+","+
                     language + ",")
  if tciptype != None:
    reportStream.write(tciptype+",")
  reportStream.write(txt+"\n")

def printHeader(txt):
  reportStream.write("\n\n\n" + "-"*80+"\n")
  w = (80-len(txt)) / 2
  reportStream.write(" "*w + txt + "\n")
  reportStream.write("-"*80+"\n")

