import os, re, sys, time
import setup, util
true, false = 1,0

#-----------------------------------------------------------------------------
# Setup start date
#-----------------------------------------------------------------------------
startDateStr = time.strftime("%Y-%m-%d-%H-%M-%S", time.localtime(time.time()))

def StartDate():
  return startDateStr

#-----------------------------------------------------------------------------
#             Class to contain parameters
#-----------------------------------------------------------------------------
class Parameter:
  # Attributes:
  # name    - The name of the parameter
  # arg     - The argument for this key as it stands in the input file.
  # value   - The value of the parameter after the input text has been translated.
  # where   - Indicates from where the value has been set, possibilities are:
  #           'default', 'cmdline', 'conffile'
  # lineno  - the line number of the parameter
  # used    - boolean indicating whether the given parameter has been used.
  # wildCardOrig - name of wildCard this one was expanded from.
  def __init__(self, arg, where, lineno):
    self.arg = arg
    self.where = where
    self.lineno = lineno
    if (where != 'default' and where != 'cmdline' and where != 'conffile'):
      raise 'where should be one of default, cmdline, or conffile'
    self.used = false
    self.value = None
    self.wildCardOrig = None
    

  def __str__(self):
    #return "mk_Parameter(" + `self.arg` + "," + `self.value` + "," + `self.where` + ")"
    return "mk_Parameter('" + self.arg + "','" + self.value + "','" + self.where + "')"


#-----------------------------------------------------------------------------
# Read the parameters from the command line, and from the configuration
# file.  
#-----------------------------------------------------------------------------
def Init(argv):
  global parameters, cmdLineError, testCases

  # Initialize the variable parameters
  # This is a map from parameter name to instance of the class Parameter
  parameters = { }
  cmdLineError = 0
  testCases = []

  # default value, may be changed on the CmdLine.
  InsertKeyVal("conf-file", ".powertest-setup", 'default')
  
  ReadCmdLine(argv)
  ReadConfFile(parameters['conf-file'].arg)
  VerifyAndTranslateKeys()
  VerifyTestCases()

  if (cmdLineError):
    sys.exit(-1)
  return cmdLineError


#-----------------------------------------------------------------------------
# Parse the command line
#-----------------------------------------------------------------------------
def ReadCmdLine(args):
  global testCases, cmdLine
  cmdLine = args
  for arg in args[1:]:
    # is this a KEY=VALUE 
    index = arg.find("=")
    if index == -1:
      testCases.append(arg)
    else:
      key = arg[:index]
      val = arg[index+1:]
      InsertKeyVal(key,val, 'cmdline')

#-----------------------------------------------------------------------------
# Read the configuration file
#-----------------------------------------------------------------------------
def ReadConfFile(filenm):
  # Open the file.
  if not os.path.exists(filenm):
    print ("File '" + filenm + "' does not exist.")
    print ("This file is required to run powertest.")
    print ("You should probably copy " + setup.BaseDir + "/default-setup to " + filenm + " and customize it to your setup.")
    print ("Have fun!")
    sys.exit(-1)
  else:
    FP = open(filenm)

  # Read the content of the file
  lineno = 0
  for line in FP.readlines():
    lineno = lineno + 1
    line = line[:-1]
    line = RemoveComments(line)

    if (line.strip() != ""):
      ExtractKey(line, lineno)

def RemoveComments(line):
  # At the moment we only remove everything after a #, later this may be changed
  # if we need a # in the value part.
  index = line.find("#")
  if (index != -1):
    return line[:index]
  else:
    return line

#-----------------------------------------------------------------------------
# Extract the key/value pair from 'line'. This line is located in the
# configuration file on line 'lineno'.
#-----------------------------------------------------------------------------
def ExtractKey(line, lineno):
  index = line.find("=")
  if index == -1:
    Error("Error on line " + str(lineno) + ": '" + line + "' didn't match Keyword=Value", None)
  else:
    key = line[:index]
    val  = line[index+1:]
    InsertKeyVal(key, val, 'conffile', lineno)

#-----------------------------------------------------------------------------
# Insert the key 'key' into the parameters table with value 'value'
# This key is located in 'where' on line 'lineno' (see the class Parameters
# for description of this.
#-----------------------------------------------------------------------------
def InsertKeyVal(key,val, where, lineno = None):
  global parameters

  key = key.strip().lower()
  origKey = key
  val = val.strip()

  if key in setup.keywords:
    # The key is one of the non-wildcard keys.
    if (key in parameters and parameters[key].where == 'conffile'):
      Error("Key '" + key + "' defined multiple times in the input file", None)
    elif (not key in parameters or where != 'conffile'):
      # Ensure that we do not override key from the commandline
      parameters[key] = Parameter(val, where, lineno)

  else:
    # Now see if the key should be one of the wildcard keys.
    which = None
    for elm in setup.wildCardKeywords:
      if key[:len(elm)] == elm:
        which = elm
        break
    
    if which == None:
      # I know nothing about this keyword!
      if where == 'cmdline':
        Error("Unknown keyword on command line: '" + key + "'", None)
      else:
        Error("Unknow keyword on line " + str(lineno) + ": '" + key + "'", None)
      return

    # Ok at this possition we know that the keyword is a wildcard keyword.
    if setup.wildCardExecutable.count(which):
      # Verify that the key points to an executable
      filenm = os.path.expandvars(os.path.expanduser(val.split()[0]))
      if not os.path.isfile(filenm):
        ErrorPos("binary '" + filenm + "' does not exist", where, lineno)
        return
      else:
        value = filenm + " " + "".join(val.split()[1:])

      
    keys = [which]
    key = key[len(which):]

    (key, keys) = BuildKeys(key, keys, setup.availableTestenv)
    (key, keys) = BuildKeys(key, keys, setup.availableLanguages)
    (key, keys) = BuildKeys(key, keys, ['impl', 'spec'])
    (key, keys) = BuildKeys(key, keys, ['pos', 'def', 'dtcon', 'dtcoff'])

    if (key != ''):
      ErrorPos("Key '" + origKey + "' contains non wildcard text: '"+ key + "'", where, lineno)
      return

    for key in keys:
      parameters[key] = Parameter(val, where, lineno)
      parameters[key].value = val
      parameters[key].origWildCard = origKey

def BuildKeys(key, keys, possible):
  found = None
  for elm in possible:
    index = key.find("-" + elm)
    if index != -1:
      found = elm
      key = key[:index] + key[index+len(elm)+1:]
      break

  if found:
    vals = [found]
  else:
    vals = possible

  newKeys = []

  # This loop creates four of duplicates, namely the situations where val = pos,
  # val = def, val = dtcoff, val = dtcoff, but the key is not for tc or ip.
  # as the sequence is used as a set it doesn't matter.
  # furthemore it creates invalid keys like parser-tc-sl-impl (the problem
  # is that this key should either end in 'pos' or 'def'). This is however
  # not a problem as these key are never accessed. It is keep the way it
  # is, to obtain simplicity.
  for val in vals:
    for k in keys:
      append = infer(val == 'pos' or val == 'def', k.find('tc') != -1) and \
               infer(val == 'dtcon' or val == 'dtcoff', k.find('ip') != -1)
      if append:
        newKeys.append(k+"-"+val)
      else:
        newKeys.append(k)
  return (key, newKeys)

# This is the function x => y
def infer(x,y):
  return (x and y) or not x
        


#-----------------------------------------------------------------------------
# Run through all the keys and (1) verify that their value are ok
# and (2) translate the key to a more suitable format
#-----------------------------------------------------------------------------
def VerifyAndTranslateKeys():


  #------------------------------------------------------------
  #                   OVERALL TRANSLATION
  #------------------------------------------------------------

  # What-To-Run - this must be first so we ensure that we do not check extra things.
  if not "what-to-run" in parameters:
    print ("Fatal error: Must specify what-to-run in configuration file")
    sys.exit(-1);

  VerifyAndTranslateSeq('what-to-run', setup.availableTestenv, 
                        "unknow test environment: %1")

  # Verify that all options has been specified.
  for keyword in setup.keywords.keys():
    if not keyword in parameters and NeedKeyWord(keyword):
      Error("Keyword '" + keyword + 
            "' not found in configuration file or on command line", None)

  # Ensure that directories exists
  for name in setup.directories:
    parameters[name].value = []
    for dirnm in parameters[name].arg.split(","):
      filenm = os.path.expanduser(dirnm.strip())
      if (NeedKeyWord(name) and not os.path.isdir(filenm)):
        Error("directory '" + filenm + "' does not exist", name)
      parameters[name].value.append(dirnm.strip())


  #------------------------------------------------------------
  #                       GENERAL OPTIONS
  #------------------------------------------------------------

  # Language
  VerifyAndTranslateSeq('language', setup.availableLanguages,
                        "Unknow language %1 should either be " 
                        + util.join(setup.availableLanguages, ", "))

  # Run-Type
  VerifyAndTranslateSeq('run-type', ['spec', 'impl'],
                        "Unknown run type: %1, should either be SPEC or IMPL")

  # Debug-Level
  VerifyAndTranslateInt('debug-level', "debug-level must be a number. %1 is not a number")

  # Spec-Job-Size
  VerifyAndTranslateInt('spec-job-size', "spec-job-size must be a numer. %1 is not a number")

  # Keep
  VerifyAndTranslateBool('keep', "keep was %1. It should either be YES or NO")
  
  # Keep-When-Error
  VerifyAndTranslateBool('keep-when-error', "keep-when-error was %1. It should either be YES or NO")

  # Show-Errors-In-Progress
  VerifyAndTranslateBool('show-errors-in-progress', "show-errors-in-progress was %1. It should either be YES or NO")

  # Enable-Profile
  VerifyAndTranslateBool('enable-profiling', "Enable-Profiling was %1, It should be YES or NO")
  
  #------------------------------------------------------------
  #              TYPE CHECKER OPTIONS
  #------------------------------------------------------------
  # TC-Type
  VerifyAndTranslateSeq('tc-type', ['pos','def'],
                        "Unknow type checker test type: %1, should either be DEF or POS")


  # TC-spec-dir
  EnsureDirIsSpecDir('tc-spec-dir', "type checker")

  #------------------------------------------------------------
  #              INTERPRETER OPTIONS
  #------------------------------------------------------------
  # IP-spec-dir
  EnsureDirIsSpecDir('ip-spec-dir', "interpreter")

  # IP-Type
  VerifyAndTranslateSeq('ip-type', ['dtcon','dtcoff'],
                        "Unknow interpreter test type: %1, should either be DTCON or DTCOFF")
  
  #------------------------------------------------------------
  #              C++ OPTIONS
  #------------------------------------------------------------
  # CPP-spec-dir
  EnsureDirIsSpecDir('CPP-spec-dir', "C++ code generator")


  #------------------------------------------------------------
  # Now move the values from arg to value for those keys, 
  # which has not been translated.
  #------------------------------------------------------------
  if not cmdLineError:
    for keyword in setup.keywords.keys():
      if NeedKeyWord(keyword) and parameters[keyword].value == None:
        parameters[keyword].value = parameters[keyword].arg

#-----------------------------------------------------------------------------
# This function translate the values given to the key 'name' to a sequence, and 
# verifies that the arguments is valid (that is in the sequence okSeq).
# If an argument is not valid then an error message is signaled. This error message
# is combined by replacing '%1' in the errMsg with the name of the wrong argument.
#-----------------------------------------------------------------------------
def VerifyAndTranslateSeq(name, okSeq, errMsg):
  if not name in parameters or not NeedKeyWord(name):
    return # The key has already been reported as missing

  if parameters[name].arg.strip() == '':
    raise "no arguments given to keyword '" + name + "'"
  else:
    # first setup the map.
    parameters[name].value = {}
    for elm in okSeq:
      parameters[name].value[elm] = 0
  
    for elm in parameters[name].arg.split(","):
      elm = elm.strip().lower()
      if okSeq.count(elm) == 0:
        msg = errMsg.replace("%1", "'" + elm + "'")
        Error(msg, name)
      else:
        parameters[name].value[elm] = 1


def VerifyAndTranslateInt(name, errMsg):
  if not name in parameters or not NeedKeyWord(name):
    return # The key has already been reported as missing

  elm = parameters[name].arg
  try:
    parameters[name].value = int(elm)
  except ValueError:
    msg = errMsg.replace("%1", "'" + elm + "'")
    Error(msg, name)

def VerifyAndTranslateBool(name, errMsg):
  if not name in parameters or not NeedKeyWord(name):
    return # The key has already been reported as missing

  elm = parameters[name].arg.strip().lower()
  if (elm == "yes" or elm == "true"):
    parameters[name].value = 1
  elif (elm == "no" or elm == "false"):
    parameters[name].value = 0
  else:
    msg = errMsg.replace("%1", "'" + elm + "'")
    Error(msg, name)
    

def EnsureDirIsSpecDir(name, dirMsg):
  if not name in parameters or not NeedKeyWord(name):
    return # The key has already been reported as missing

  dirnm = os.path.expanduser(parameters[name].arg)
  if not os.path.exists(dirnm):
    Error(dirMsg + " specification directory '" + dirnm + "' does not exist.", name)
  elif (not os.path.isdir(dirnm)):
    Error(dirMsg + " specification directory '" + dirnm + "' is not a directory.", name)
  elif (not os.path.isfile(dirnm+"/Makefile")):
    Error("No Makefile exist in " + dirMsg + " specification directory '" + name + "'", name)
  else:
    parameters[name].value = dirnm

#-----------------------------------------------------------------------------
# This function prints out error messages while reading the command line or
# when reading the configuration file.
# It should not be used for other kind of error handling
# The second parameter 'key' is used to print out possition information. If
# key is None, the no position information is printed.
#-----------------------------------------------------------------------------
def Error(msg, key):
  global cmdLineError
  cmdLineError = 1
  if key:
    ErrorPos(msg, parameters[key].where, parameters[key].lineno)
  else:
    print (msg)

def ErrorPos(msg, where, lineno):
  global cmdLineError
  cmdLineError = 1
  if where == 'cmdline':
    print ("Error on commandline: " + msg)
  else:
    print ("Error on line " + str(lineno) + ": " + msg)
  
#-----------------------------------------------------------------------------
# This function return true of env is one of the testenvironment which
# should be executed. env must be one of the allowed test environments from
# setup.availableTestenv or the word 'general'
#-----------------------------------------------------------------------------
def RunTestEnv(env):
  if env == 'general':
    return 1
  else:
    return parameters['what-to-run'].value[env]

#-----------------------------------------------------------------------------
# This function returns true of the given keyword is required for the
# testrun. The keyword may not be needed if it belongs to a test
# environment which is not executed during this testrun
#-----------------------------------------------------------------------------
def NeedKeyWord(keyword):
  return RunTestEnv(setup.keywords[keyword])

#-----------------------------------------------------------------------------
# Verifies that every test case / test directory specified on the command line
# exists.
#-----------------------------------------------------------------------------
def VerifyTestCases():
  global testCases

  for testCase in testCases:
    if (not os.path.exists(testCase)):
      Error("Test case or test test directory '" + testCase + "' does not exists",None)
      
#-----------------------------------------------------------------------------
# Return a list for test cases for the given test environment.
# 'env' should be one of the envs in setup.availableTestenv 
#-----------------------------------------------------------------------------
def GetTestCases(env):
  if testCases != []:
    return testCases
  else:
    return parameters[env.lower()+"-test-case-dir"].value


#-----------------------------------------------------------------------------
# LookUp returns the value of the key in the global map 'parameters'
#-----------------------------------------------------------------------------
def LookUp(key):
  return parameters[key.lower()].value

def GetCmdLine():
  return util.join(cmdLine," ")

#-----------------------------------------------------------------------------
# returns a specific VDM-SL or VDM++ interpreter
# env - test environment (TC, IP, CPP, ...)
# lang - language (SL, PP, RT)
# type - (IMPL, SPEC)
# which - name of purpose (PARSER, INTERPRETER, COMPARE, ...)
# extraType - specific for the different test envs:
#   TC - either POS or DEF
#   IP - eitehr DTCON or DTCOFF
#----------------------------------------------------------------------------- 
def LookUpWildCard(env, lang, type, which, extraType = None):
  key = which+"-"+env+"-"+lang+"-"+type
  if lang == 'tc' or lang == 'ip':
    key = key +"-"+extraType
  key = key.lower()

  parameters[key].used = true
  return parameters[key].value

def GetUsedWildCards():
  global parameters
  res = []
  for key in parameters.keys():
    if parameters[key].used:
      res.append((key, parameters[key].origWildCard))
  return res
