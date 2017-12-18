import gentestcases, cmdline, util, setup, report, convert, resfile, time
import os, re, sys, locale
from posix import environ
from omniORB import CORBA
#from Fnorb.orb import CORBA

import corba_api_idl
import internal_api_idl
import metaiv_idl_idl

import VDM
import ToolboxAPI

import VDM__POA
import ToolboxAPI__POA

true, false = 1, 0

#--------------------------------------------------------------------------------
# Execute CORBA API test environment.
# lang - language to use (SL, PP, RT)
# type - type of test (either spec or impl)
# return - Nothing.
#--------------------------------------------------------------------------------

def execute(lang, testType):
  expSet = resfile.MakeStdExpansionSet('api', lang, testType)
  resfile.RegisterExpansionSet(expSet)
  util.SetProfileBaseName("gmon-api-"+lang+"-"+testType+"-"+cmdline.StartDate())
  
  if lang == 'sl':
    vdmServer = os.path.expanduser(cmdline.LookUp ('api-sl-server'))
  else:
    vdmServer = os.path.expanduser(cmdline.LookUp ('api-pp-server'))


  start_toolbox = cmdline.LookUp ('api-run-start-toolbox')

  server_args = cmdline.LookUp ('server-init-args').split(" ")
   
  # corba-init-args is used to pass arguments to the orb
  client_args = cmdline.LookUp ('corba-init-args').split()
  orb = CORBA.ORB_init (client_args, CORBA.ORB_ID)

  if start_toolbox == 'yes':
    pid = os.fork ()
    
    if pid == 0:
      report.Progress(3, "Trying to start "+lang+"-toolbox: "+vdmServer)
      cmd = vdmServer + " " + "".join (server_args) + " > tb_output.tmp 2>&1"
      os.system (cmd)
      report.Error ("Startup of Toolbox failed!")
      return false
      _exit (-1)
    
    waittime = 5 
    print ("Waiting " + str(waittime) + " seconds for " + lang + "-server to start up...")
    time.sleep (waittime)
    
  if "VDM_OBJECT_LOCATION" in environ:
    location = environ["VDM_OBJECT_LOCATION"]
  else:
    if "HOME" in environ:
      location = environ["HOME"]
    else:
      location = "~"

  if lang == 'sl':
    location = location + '/vdmref.ior'
  else:
    location = location + '/vppref.ior'

  try:
    stringified_ior = open (os.path.expanduser(location)).read()
  except IOError:
    report.Error ("Could not find IOR file " + location + "! Please start " + lang + "-Toolbox!")
    if start_toolbox == 'yes':
      os.kill (pid, 9)
    os.system("tset")
    return false

  try:
    vdmApp = orb.string_to_object (stringified_ior)
  except CORBA.COMM_FAILURE:
    _, ex, _ = sys.exc_info()
    print ("CORBA.COMM_FAILUR")
    print (ex)
    vdmApp = None
  except CORBA.MARSHAL:
    _, ex, _ = sys.exc_info()
    print ("CORBA.MARSHAL")
    print (ex) #
    vdmApp = None
  
  if vdmApp is None:
    report.Error ("Nil object reference!");
    if start_toolbox == 'yes':
      os.kill (pid, 9)
    return false

  # check if vdmApp has correct type
  try:
    if not vdmApp._is_a ('IDL:ToolboxAPI/VDMApplication:1.0'):
      report.Error ("This is not a 'VDMApplication'-server!")
      if start_toolbox == 'yes':
        os.kill (pid, 9)
      return false
  except CORBA.COMM_FAILURE:
    report.Error ("No connection to 'VDMApplication'-object!")
    if start_toolbox == 'yes':
      os.kill (pid, 9)
    return false

  # narrow vdmApp and get typecode
  vdmApp = vdmApp._narrow (ToolboxAPI.VDMApplication);  
  tc = CORBA.TypeCode ('IDL:ToolboxAPI/VDMApplication:1.0');

  # check if server is the one we want (SL or PP)
  try:
    toolType = vdmApp._get_Tool ()

    if (lang == 'sl' and toolType == ToolboxAPI.PP_TOOLBOX) or \
       (lang == 'pp' and toolType == ToolboxAPI.SL_TOOLBOX):
      report.Error (vdmServer + " is not a " + lang + "-server")
      if start_toolbox == 'yes':
        os.kill (pid, 9)
      return false
    else:
      print (vdmServer + " is a " + lang + "-server")

    clientID = vdmApp.Register ()

    # run the testcases
    if not RunTestCases (lang, vdmApp, clientID):
      vdmApp.Unregister (clientID)
      if start_toolbox == 'yes':
        os.kill (pid, 9)
      return false

  except CORBA.COMM_FAILURE:
    report.Error ("CORBA Communication error")
    vdmApp.Unregister (clientID)
    if start_toolbox == 'yes':
      os.kill (pid, 9)
    return false

  # clean up
  time.sleep (1)
  vdmApp.Unregister (clientID)
  if start_toolbox == 'yes':
    os.kill (pid, 9)

  os.system("tset")
  return true

class Syntax:
  def __init__ (self, msg):
    self.message = msg

  def GetMessage (self):
    return self.message
  

class ToolMediator:
  def __init__ (self, app, clientID):
    self.clientID         = clientID
    self.vdmApplication   = app
    self.vdmProject       = app.GetProject ()
    self.vdmInterpreter   = app.GetInterpreter ()
    self.vdmCodeGenerator = app.GetCodeGenerator ()
    self.vdmParser        = app.GetParser ()
    self.vdmTypeChecker   = app.GetTypeChecker ()
    self.vdmPrettyPrinter = app.GetPrettyPrinter ()
    self.vdmErrorHandler  = app.GetErrorHandler ()
    self.vdmModuleRepos   = app.GetModuleRepos ()
    self.vdmFactory       = app.GetVDMFactory ()
    self.vdmErrors        = app.GetErrorHandler ()

  def ParseCommand (self, cmd, name, line):
    ################# General
    m = re.compile ('^_print\((?P<str>.*)\)$').match (cmd)
    if m != None:
      return m.group ('str') + "\n"

    m = re.compile ('^_exists file\((?P<filename>.*)\)').match (cmd)
    if m != None:
      if os.path.exists (m.group ('filename')):
        return "true\n"
      return "false\n"

    m = re.compile ('^_delete file\((?P<filename>.*)\)').match (cmd)
    if m != None:
      try:
        os.unlink (m.group ('filename'))
      except:
        pass
      return ""

    ################# VDMProject
    
    # VDMProject.New () = new project
    if re.compile ('^new project$').match (cmd) != None:
      self.vdmProject.New ()
      return ""

    # VDMProject.Open () = open project (filename)
    m = re.compile ('^open project\((?P<filename>.*)\)$').match (cmd)
    if m != None:
      self.vdmProject.Open (m.group ('filename'))
      return ""

    # VDMProject.SaveAs (filename) = save as (filename)
    m = re.compile ('^save$').match (cmd)
    if m != None:
      self.vdmProject.Save ()
      return ""


    # VDMProject.SaveAs (filename) = save as (filename)
    m = re.compile ('^save as\((?P<filename>.*)\)$').match (cmd)
    if m != None:
      self.vdmProject.SaveAs (m.group ('filename'))
      return ""

    # VDMProject.GetModules () = get modules
    if re.compile ('^get modules$').match (cmd) != None:
      (res, modules) = self.vdmProject.GetModules ()
      return util.join (modules, "\n") + "\n"

    # VDMProject.GetFiles () = get files
    if re.compile ('^get files$').match (cmd) != None:
      (res, files) = self.vdmProject.GetFiles ()
      return util.join (files, "\n") + "\n"

    # VDMProject.AddFile = add file (filename)
    m = re.compile ('^add file\((?P<filename>.*)\)$').match (cmd)
    if m != None:
      self.vdmProject.AddFile (m.group ('filename'))
      return ""

    # VDMProject.RemoveFile = remove file (filename)
    m = re.compile ('^remove file\((?P<filename>.*)\)$').match (cmd)
    if m != None:
      self.vdmProject.RemoveFile (m.group ('filename'))
      return ""

    ############## VDMModuleRepos

    # VDMModuleRepos.FilesOfModule = files of module (modulename)
    m = re.compile ('^files of module\((?P<modulename>.*)\)$').match (cmd)
    if m != None:
      (res, flist) = self.vdmModuleRepos.FilesOfModule (m.group ('modulename'))
      return util.join (flist, "\n") + "\n"
    
    # VDMModuleRepos.GetCurrentModule = current module
    if re.compile ('^current module$').match (cmd) != None:
      res = self.vdmModuleRepos.GetCurrentModule () + "\n"
      return res

    # VDMModuleRepos.GetCurrentModule = pop module
    if re.compile ('^pop module$').match (cmd) != None:
      self.vdmModuleRepos.PopModule ()
      return ""

    # VDMModuleRepos.PushModule = push module (modulename)
    m = re.compile ('^push module\((?P<modulename>.*)\)$').match (cmd)
    if m != None:
      self.vdmModuleRepos.PushModule (m.group ('modulename'))
      return ""

    # VDMModuleRepos.Status = status (modulename)
    m = re.compile ('^status\((?P<modulename>.*)\)$').match (cmd)
    if m != None:
      status = self.vdmModuleRepos.Status (m.group ('modulename'))
      resStr = ""
      if status.SyntaxChecked:
        resStr = resStr + "S"
      else:
        resStr = resStr + "-" 
      if status.TypeChecked:
        resStr = resStr + "T"
      else:
        resStr = resStr + "-" 
      if status.CodeGenerated:
        resStr = resStr + "C"
      else:
        resStr = resStr + "-" 
      if status.PrettyPrinted:
        resStr = resStr + "P"
      else:
        resStr = resStr + "-" 
      return resStr + "\n"

    # VDMModuleRepos.SuperClasses = superclasses (classname)
    m = re.compile ('^superclasses\((?P<classname>.*)\)$').match (cmd)
    if m != None:
      (res, clist) = self.vdmModuleRepos.SuperClasses (m.group ('classname'))
      return util.join (clist, "\n") + "\n"

    # VDMModuleRepos.SubClasses = subclasses (classname)
    m = re.compile ('^subclasses\((?P<classname>.*)\)$').match (cmd)
    if m != None:
      (res, clist) = self.vdmModuleRepos.SubClasses (m.group ('classname'))
      return util.join (clist, "\n") + "\n"

    # VDMModuleRepos.UsesOf = usesof (classname)
    m = re.compile ('^usesof\((?P<classname>.*)\)$').match (cmd)
    if m != None:
      (res, clist) = self.vdmModuleRepos.UsesOf (m.group ('classname'))
      return util.join (clist, "\n") + "\n"

    # VDMModuleRepos.UsedBy = usedby (classname)
    m = re.compile ('^usedby\((?P<classname>.*)\)$').match (cmd)
    if m != None:
      (res, clist) = self.vdmModuleRepos.UsedBy (m.group ('classname'))
      return util.join (clist, "\n") + "\n"

    ########### VDMInterpreter
    # VDMInterpreter.DynTypeCheck = set dtc [on|off]
    m = re.compile ('^set dtc ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmInterpreter._set_DynTypeCheck (true)
      else:
        self.vdmInterpreter._set_DynTypeCheck (false)
      return ""

    # VDMInterpreter.DynInvCheck = set inv [on|off]
    m = re.compile ('^set inv ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmInterpreter._set_DynInvCheck (true)
      else:
        self.vdmInterpreter._set_DynInvCheck (false)
      return ""

    # VDMInterpreter.DynPreCheck = set pre [on|off]
    m = re.compile ('^set pre ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmInterpreter._set_DynPreCheck (true)
      else:
        self.vdmInterpreter._set_DynPreCheck (false)
      return ""

    # VDMInterpreter.DynPostCheck = set post [on|off]
    m = re.compile ('^set post ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmInterpreter._set_DynPostCheck (true)
      else:
        self.vdmInterpreter._set_DynPostCheck (false)
      return ""

    # VDMInterpreter.PPOfValues = set ppr [on|off]
    m = re.compile ('^set ppr ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmInterpreter._set_PPOfValues (true)
      else:
        self.vdmInterpreter._set_PPOfValues (false)
      return ""

    # VDMInterpreter.Verbose = set verb [on|off]
    m = re.compile ('^set verb ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmInterpreter._set_Verbose (true)
      else:
        self.vdmInterpreter._set_Verbose (false)
      return ""

    # VDMInterpreter.Debug = set dbg [on|off]
    m = re.compile ('^set ppr ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmInterpreter._set_Debug (true)
      else:
        self.vdmInterpreter._set_Debug (false)
      return ""

    # VDMInterpreter.Initialize = init
    if re.compile ('^init$').match (cmd) != None:
      self.vdmInterpreter.Initialize ()
      return ""

    # VDMInterpreter.EvalExpression = eval (expression)
    m = re.compile ('^eval\((?P<expression>.*)\)$').match (cmd)
    if m != None:
      res = self.vdmInterpreter.EvalExpression (self.clientID, m.group ('expression'))
      return res.ToAscii () + "\n"

    # VDMInterpreter.EvalExpression = apply (expression) to [args]
    m = re.compile ('^apply\((?P<expression>.*)\)to\[(?P<arg>.*)\]$').match (cmd)
    if m != None:
      arg_str  =  "["+m.group ('arg')+"]"
      arg_eval = self.vdmInterpreter.EvalExpression (self.clientID,arg_str)
      expr_str = m.group ('expression')
      res = self.vdmInterpreter.Apply (self.clientID, expr_str, arg_eval)
      return res.ToAscii () + "\n"

    # VDMInterpreter.EvalCmd = evalcmd (command)
    m = re.compile ('^evalcmd\((?P<command>.*)\)$').match (cmd)
    if m != None:
      self.vdmInterpreter.EvalCmd (m.group ('command'))
      return ""

    # VDMInterpreter.SetBreakPointByPos = break (filename, line, col)
    m = re.compile ('^break\((?P<filename>.*?),(?P<line>\d+),(?P<col>\d+)\)$').match (cmd)
    if m != None:
      num = self.vdmInterpreter.SetBreakPointByPos (m.group ('filename'), \
                                                    locale.atoi (m.group ('line')), \
                                                    locale.atoi (m.group ('col')))
      return str(num) + "\n"
    
    # VDMInterpreter.SetBreakPointByName = break (modulename, func)
    m = re.compile ('^break\((?P<modulename>.*?),(?P<func>[a-zA-Z][\w`]*)\)$').match (cmd)
    if m != None:
      num = self.vdmInterpreter.SetBreakPointByName (m.group ('modulename'), \
                                                     m.group ('func'))
      return str(num) + "\n"

    # VDMInterpreter.DeleteBreakPoint = delete (number)
    m = re.compile ('^delete\((?P<number>\d+)\)$').match (cmd)
    if m != None:
      self.vdmInterpreter.DeleteBreakPoint (locale.atoi (m.group ('number')))
      return ""

    # VDMInterpreter.StartDebugging = debug (expression)
    m = re.compile ('^debug\((?P<expression>.*)\)$').match (cmd)
    if m != None:
      res = self.vdmInterpreter.StartDebugging (self.clientID, m.group ('expression'))
      return res.ToAscii () + "\n"

    # VDMInterpreter.DebugStep = step
    m = re.compile ('^step$').match (cmd)
    if m != None:
      res = self.vdmInterpreter.DebugStep (self.clientID)
      return res.ToAscii () + "\n"

    # VDMInterpreter.DebugStepIn = stepin
    m = re.compile ('^stepin$').match (cmd)
    if m != None:
      res = self.vdmInterpreter.DebugStepIn (self.clientID)
      return res.ToAscii () + "\n"

    # VDMInterpreter.DebugSingleStep = singlestep
    m = re.compile ('^singlestep$').match (cmd)
    if m != None:
      res = self.vdmInterpreter.DebugSingleStep (self.clientID)
      return res.ToAscii () + "\n"

    # VDMInterpreter.DebugContinue = cont
    m = re.compile ('^cont$').match (cmd)
    if m != None:
      res = self.vdmInterpreter.DebugStep (self.clientID)
      return res.ToAscii () + "\n"

    ########## VDMCodeGenerator

    # VDMCodeGenerator.GeneratePosInfo = set genposinfo [on|off]
    m = re.compile ('^set genposinfo ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmCodeGenerator._set_GeneratePosInfo (true)
      else:
        self.vdmCodeGenerator._set_GeneratePosInfo (false)
      return ""    

    # VDMCodeGenerator.CodeGenerateList = codegen [java|cpp] ([filename, filename, ...])
    m = re.compile ('^codegen (?P<lang>(java)|(cpp))\(\[(?P<modulenames>.*)\]\)$').match (cmd)
    if m != None:
      mlist = m.group ('modulenames').split(",")
      if m.group ('lang') == "java":
        res = self.vdmCodeGenerator.GenerateCodeList (mlist, ToolboxAPI.VDMCodeGenerator.JAVA)
      else:
        res = self.vdmCodeGenerator.GenerateCodeList (mlist, ToolboxAPI.VDMCodeGenerator.CPP)
      if res:
        return "true\n"
      return "false\n"
    
    # VDMCodeGenerator.CodeGenerate = codegen [java|cpp] (filename)
    m = re.compile ('^codegen (?P<lang>(java)|(cpp))\((?P<modulename>.*)\)$').match (cmd)
    if m != None:
      if m.group ('lang') == "java":
        res = self.vdmCodeGenerator.GenerateCode (m.group ('modulename'), ToolboxAPI.VDMCodeGenerator.JAVA)
      else:
        res = self.vdmCodeGenerator.GenerateCode (m.group ('modulename'), ToolboxAPI.VDMCodeGenerator.CPP)
      if res:
        return "true\n"
      return "false\n"    

    ########## VDMParser

    # VDMParser.ParseList = parse ([filename, filename, ...])
    m = re.compile ('^parse\(\[(?P<filenames>.*)\]\)$').match (cmd)
    if m != None:
      flist = m.group ('filenames').split(",")
      res = self.vdmParser.ParseList (flist)
      if res:
        return "true\n"
      return "false\n"


    # VDMParser.Parse = parse (filename)
    m = re.compile ('^parse\((?P<filename>.*)\)$').match (cmd)
    if m != None:
      parse_arg = m.group ('filename')
      res = self.vdmParser.Parse (parse_arg)
      if res:
        return "true\n"
      else:
        return "false\n"

 
    # VDMTypeChecker
    
    # VDMTypeChecker.TypeCheckList = typecheck [pos|def] ([modulename, modulename, ...])
    m = re.compile ('^typecheck (?P<posdef>(pos)|(def))\(\[(?P<modulenames>.*)\]\)$').match (cmd)
    if m != None:
      mlist = m.group ('modulenames').split(",")
      if m.group ('posdef') == "pos":
        self.vdmTypeChecker._set_DefTypeCheck (false)
      else:
        self.vdmTypeChecker._set_DefTypeCheck (true)
  
      res = self.vdmTypeChecker.TypeCheckList (mlist)
      if res:
        return "true\n"
      return "false\n"

    # VDMTypeChecker.TypeCheck = typecheck [pos|def] (filename)
    m = re.compile ('^typecheck (?P<posdef>(pos)|(def))\((?P<modulename>.*)\)$').match (cmd)
    if m != None:
      if m.group ('posdef') == "pos":
        self.vdmTypeChecker._set_DefTypeCheck (false)
      else:
        self.vdmTypeChecker._set_DefTypeCheck (true)
  
      res = self.vdmTypeChecker.TypeCheck (m.group ('modulename'))
      if res:
        return "true\n"
      return "false\n"

    # VDMTypeChecker.ExtendedTypeCheck = set full [on|off]
    m = re.compile ('^set full ((on)|(off))$').match (cmd)
    if m != None:
      if m.group (1) == 'on':
        self.vdmTypeChecker._set_ExtendedTypeCheck (true)
      else:
        self.vdmTypeChecker._set_ExtendedTypeCheck (false)
      return ""

    ############# VDMPrettyPrinter

    # VDMPrettyPrinter.PrettyPrintList = prettyprint ([filename, filename, ...])
    m = re.compile ('^prettyprint\(\[(?P<filenames>.*)\]\)$').match (cmd)
    if m != None:
      flist = m.group ('filenames').split(",")
      res = self.vdmPrettyPrinter.PrettyPrintList (flist)
      if res:
        return "true\n"
      return "false\n"
    
    # VDMPrettyPrinter.PrettyPrint = prettyprint (filename)
    m = re.compile ('^prettyprint\((?P<filename>.*)\)$').match (cmd)
    if m != None:
      res = self.vdmPrettyPrinter.PrettyPrint (m.group ('filename'))
      if res:
        return "true\n"
      else:
        return "false\n"

    ############ VDMError

    # VDMErrors.GetErrors = get errors
    if re.compile ('^get errors$').match (cmd) != None:
      (n, errList) = self.vdmErrors.GetErrors ()
      res = ""
      for err in errList:
        res = res + err.fname + " ["+ str(err.line) + "," + str(err.col) + "] " + err.msg + "\n"
      return res + "Errors: " + str(n) + "\n"

    # VDMErrors.GetWarnings = get warnings
    if re.compile ('^get warnings$').match (cmd) != None:
      (n, errList) = self.vdmErrors.GetWarnings ()
      res = ""
      for err in errList:
        res = res + err.fname + " ["+ str(err.line) + "," + str(err.col) + "] " + err.msg + "\n"
      return res + "Warnings: " + str(n) + "\n"
      
    
    raise Syntax ("Unknown command") 
      
    
def RunTestCases (lang, vdmApp, clientID):
  toolMediator = ToolMediator (vdmApp, clientID)

  total = 1

  # jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('api', lang, 'impl')
  name = gentestcases.NextTestCase()

  while (name != None):
    report.setTestCaseName(name)
    if (total % jobSize) == 1:
      report.Progress(2, "Handling test cases " + str(total) + "..." + str(total + jobSize-1))
    report.Progress(3, "Running " + name)

    ok = RunApiTestCase(name, lang, toolMediator)

    if util.CleanFile(ok):
      bn = util.ExtractName(name)
      util.DeleteFiles([bn+".vdm"])
    name = gentestcases.NextTestCase()
    total = total +1
  
  return true

def RunApiTestCase (name, lang, toolMediator):
  data = util.ReadFile (name)
  baseName = util.ExtractName (name)
  neutralName = re.sub ('.*/', '', name) # path neutralized!
  result = baseName + ":\n"
  last = ""
  symTable = {}
  symTable['GENPATH'] = os.path.expanduser(cmdline.LookUp ('api-gen-path'))
  symTable['SOURCEPATH'] = os.path.expanduser(cmdline.LookUp ('api-source-path'))
  print (symTable['GENPATH'])
  print (symTable['SOURCEPATH'])
  commands = data.split("\n")
  line, test = 0, 0
  empty = re.compile ('^\s*(#.*)|(\s*)$')

  interactive_mode = cmdline.LookUp ('api-run-interactive')

  for cmd in commands:
    print ("Command: " + cmd)

    if interactive_mode == 'yes':
      print ("Press <return>")
      raw_input ()
    line = line + 1
    # don't process empty lines
    if not empty.match (cmd):
      test = test + 1
      try:        
        cmd = re.sub ('\$\$', last, cmd) # replace $$ by last result
        cmd = re.sub ('\s+', ' ', cmd).strip() # replace multiple spaces by one
        cmd = re.sub ('\s*\(\s*', '(', cmd) # delete spaces around parentheses
        cmd = re.sub ('\s*\)\s*', ')', cmd)
        cmd = re.sub ('\s*\[\s*', '[', cmd) # delete spaces around parentheses
        cmd = re.sub ('\s*\]\s*', ']', cmd)
        cmd = re.sub ('\s*,\s*', ',', cmd) # delete spaces around commas
        cmd = re.sub ('\s*=\s*', '=', cmd) # delete spaces around =

        # find variable assignment
        m = re.compile ('^\$(?P<lval>\w+)=(?P<rval>.*)$').match (cmd)
        if m != None:
          lval = m.group ('lval')
          cmd  = m.group ('rval')
        else:
          lval = ""

        # find variable usages and replace them by value
        m = re.compile ('^.*\$(?P<id>\w+).*$').match (cmd)
        if m != None:
          while m != None:
            var  = "\$" + m.group ('id')
            try:
              repl =  symTable[m.group('id')]
            except KeyError:
              raise Syntax ("Unknown variable name $"+var)
            cmd = re.sub (var, repl, cmd)
            m = re.compile ('^.*\$(?P<id>\w+).*$').match (cmd)
          
        result = result + "-- " + str(test) + "(in line "+ str(line) +"):\n"

        # print (cmd)
        # parse and execute the command
        last  = toolMediator.ParseCommand (cmd, name, line)
        lines = last.split('\n')
        pathexp = re.compile ('^.*/(.*\.(vdm|vpp|api))$')
        for l in lines:
          # neutralize path
          result = result + pathexp.sub (r'\1', l) + "\n"
        last   = re.sub ('\n$', '', last)
        last   = re.sub ('\n', ',', last).strip()
        
        if lval != "":
          symTable[lval] = last

      except CORBA.COMM_FAILURE:
        raise CORBA.COMM_FAILURE
      except CORBA.BAD_PARAM:
        result = result + "CORBA.BAD_PARAM in " + neutralName + ", line " + str(line) + "\n"
      except ToolboxAPI.APIError:
        _, e, _ = sys.exc_info()
        result = result + "APIError in " + neutralName + ", line " + str(line) + ": " + e.msg + "\n"
      except VDM.VDMError:
        _, e, _ = sys.exc_info()
        result = result +  "VDMError in " + neutralName + ", line " + str(line) + ": " + str(e.err) + "\n"
      except Syntax:
        _, e, _ = sys.exc_info()
        result = result + "Syntax error in " + neutralName + ", line " + str(line) + ": " + e.GetMessage () + "\n"
      except CORBA.UserException:
        result = result + "CORBA.UserException: in " + baseName + ".api, line " + str(line) + "\n"
      except CORBA.SystemException:
        _, e, _ = sys.exc_info()
        print (e)
        result = result + "CORBA.SystemException: in " + baseName + ".api, line " + str(line) + "\n"
      except Exception:
        _, e, _ = sys.exc_info()
        print ("Python exception")
        print (e.args)
        result = result + "Python exception in " + baseName + ".api, line " + str(line) + "\n"
      except:
        result = result + "Unknown exception in " + baseName + ".api, line " + str(line) + "\n"

  ok = util.WriteFile (baseName+".tmp", result)
  if ok:
    resFile = resfile.FindResFile (name)
    ok = (resFile != None)

  if ok:
    ok = resfile.CompareResult (name, baseName+".tmp", resFile, None)

  if util.CleanFile (ok):
    util.DeleteFiles ([baseName+".tmp"])
      
  return ok


