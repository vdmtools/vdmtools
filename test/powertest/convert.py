true, false = 1,0
import report, util, cmdline, setup
import re, os

# uselibs is used to accumulate the names of classes which are dlclasses
uselibs = []
modcls = ""

standardlibs = []
#--------------------------------------------------------------------------------
# Reads the file 'fileName', and converts it to language 'lang', and writes
# it to the current directory with the same name.
#--------------------------------------------------------------------------------
def ConvertLanguage(lang, fileName):
  newName = util.ExtractName(fileName) + ".vdm"  
  report.Progress(4, "Converting testcase to language " + lang + " (creating " + newName +")")
  
  data = util.ReadFile(fileName)
  if data == None:
    return false

  if IsLanguageSl(data):
    if (lang == 'pp'):
      report.Error("Input file: '" + fileName + "' is a SL file, but a PP file is required")
      return false
  else:
    if (lang == 'sl'):
      data = PP2SL(data)

  ok = util.WriteFile(newName, data)
  return ok

#--------------------------------------------------------------------------------
# returns true if the vdm file located in the string 'data' is written in SL
#--------------------------------------------------------------------------------
def IsLanguageSl(data):
  moduleMatch = re.search("^ *module", data, re.MULTILINE)
  classMatch  = re.search("^ *class", data, re.MULTILINE)
  
  if not moduleMatch:
    return false
  elif not classMatch:
    return true
  else:
    return moduleMatch.start() < classMatch.start()

#--------------------------------------------------------------------------------
# Translate an PP specification to an SL specification
#--------------------------------------------------------------------------------
def PP2SL(data):
  pattern = re.compile("^(\s*)class(\s*\w*\s)", re.MULTILINE)
  data = re.sub(pattern,"\\1module\\2 exports all definitions ", data)
  return re.sub("\\bpublic\\b","",data)


#--------------------------------------------------------------------------------
# Translate the vdm file 'fileName' to an AST using the vdmde parser 'parser'
#--------------------------------------------------------------------------------
def VDM2AST(fileName,parser, typeCheck):
  base = util.ExtractName(fileName)
  vdmFile = base + ".vdm"
  astFile = base + ".ast" 
  report.Progress(4, "Converting testcase from VDM to AST (creating " + astFile + ")")

  # First delete the m4pp file.
  if (os.path.exists("m4pp")):
    try:
      os.unlink("m4pp")
    except os.error:
      _, (_, msg), _ = sys.exc_info()
      report.Error("Error while removing 'm4pp': " + msg)
      return false

  # run the parser
  if typeCheck:
    cmd = parser + " -ta " + vdmFile
  else:
    cmd = parser + " -pa " + vdmFile
  (exitCode, stdout, stderr) = util.RunCommand(cmd, 0, "Error in input file (" + vdmFile +"), possible syntax error")
  if exitCode != 0:
    return false

  # verify that a m4pp file has been created.
  if not os.path.exists("m4pp"):
    report.Error("command '" + cmd + "' didn't produce a m4pp file, though return code was 0",
          "Command may not be a vdmde command with -pa flag")
    return false

  # Finally move m4pp to 'astFile'
  if (os.path.exists(astFile)):
    try:
      os.unlink(astFile)
    except os.error:
      _, (_, msg), _ = sys.exc_info()
      report.Error("Error while removing " + astFile + ": " + msg)
      return false
  try:
    os.rename('m4pp', astFile)
  except os.error:
    _, (_, msg), _ = sys.exc_info()
    report.Error("Couldn't move file 'm4pp' to " + astFile + ": " + msg)
    return false

  return true

#-----------------------------------------------------------------------------
# Creates a debug file for interactive debugging the given test case.
# fileName - the name of the test case
# returns  - false if something went wrong.
#-----------------------------------------------------------------------------
def CreateOptionsFile(fileName):
  filestem = util.StripExt(fileName)
  optionsFileName = filestem + '.opt'
  if os.path.exists(optionsFileName):
    linkName = os.path.basename(optionsFileName)
  else:
    linkName = "nulloptions"
    optionsFileName = setup.BaseDir+"/general-files/nulloptions"
  report.Progress(3, "Linking options file " + optionsFileName);
  ok = util.SymLink(optionsFileName, linkName)

  return ok

#-----------------------------------------------------------------------------
# Parses the options file for a given test case and generates the
# corresponding command-line flags for the specified options
# fileName - the name of the test case
# returns  - string representing command-line flags
#-----------------------------------------------------------------------------
def ParseOptionsFile(fileName):
  filestem = util.StripExt(fileName)
  optionsFileName = filestem + '.opt'
  if not os.path.exists(optionsFileName):
    return "";

  report.Progress(4, "Parsing " + optionsFileName)
  data = util.ReadFile(optionsFileName)
  startIndex = data.find("SetOptions")
  if startIndex == -1:
    report.Error("Unable to parse options file (1)")
    return ""

  reStart = re.compile("==\s+");
  startMatch = reStart.search(data, startIndex)
  if startMatch == None:
    report.Error("Unable to parse options file (2)")
    return

  startIndex = startMatch.start()
  reEnd = re.compile(";?\s*end\s+OPTIONS", re.M)
  endMatch = reEnd.search(data, startIndex)

  if endMatch == None: #Ill formed options file
    report.Error("Unable to parse options file (3)")
    return ""

  interfacesFlag = "-U"
  interfaceSeparator = ','
  packageFlag = "-z"
  
  endIndex = endMatch.start() - 1
  commands = re.split(';',data[startIndex:endIndex])
  commstring = ""
  for comm in commands:
    report.Progress(4, "**> command is" + comm)
    reComName = re.compile("CGMAIN`(\w+)")
    commMatch = reComName.search(comm,0)
    if commMatch != None:
      commandName = commMatch.group(1)
      if commandName == "set_interfaces":
        reArgs = re.compile("mk_AS`Name\(\[\"(\w+)\"],-1\)")
        args = comm[commMatch.end()+1:]
        arglist = ""
        nextArg = reArgs.search(args,0)
        while nextArg != None:
          if arglist == "":
            arglist = " " + nextArg.group(1)
          else:
            arglist = arglist + interfaceSeparator + nextArg.group(1)
          nextArg = reArgs.search(args, nextArg.end()+1)
        commstring =  commstring + ' ' + interfacesFlag + arglist
      elif commandName == "set_package_option":
        report.Progress(4, "Command is set_package_option")
        reArgs = re.compile("\"([\.\w]*)\"")
        args = comm[commMatch.end()+1:]
        arg = reArgs.search(args, 0)
        if arg != None:
          commstring = commstring + ' ' + packageFlag + " " + arg.group(1)
        else:
          report.Error("args didn't match: " + args)
      else:
        print ("Unknown option " + comm)
    else:
      report.Error("Command didn't match: " + comm) 
  report.Progress(4, "Finished parsing " + optionsFileName)
  return commstring

  
    

#--------------------------------------------------------------------------------
# Creates a debug file for interactive debugging the given test case.
# fullName - the name of the input file.
# templateName - the name of the template to use when writting the debug file.
#                In this template the AST is inserted in place of <<AST>>.
# extra    - A map with extra expansions.
# returns  - false if something went wrong.
#--------------------------------------------------------------------------------
def CreateDebugFile(fileName, templateName, extra):
  base = util.ExtractName(fileName)
  astFile   = base + ".ast"
  debugFile = base + ".debug"  
  report.Progress(4, "Creating debug (" + debugFile +")")

  ast = util.ReadFile(astFile)
  if ast == None:
    return false

  template = util.GetTemplate(templateName)
  if template == None:
    return false
  
  data = util.SubString("<<AST>>", ast, template)
  for key in extra.keys():
    data = util.SubString(key, extra[key], data)

  ok = util.WriteFile(debugFile, data)
  if not ok:
    return false

  # Make a symlink from the debug file til debug.arg
  ok = util.SymLink(debugFile, "debug.arg")
  return ok

#--------------------------------------------------------------------------------
# Creates an .arg file with the ast of the test case.
# fileName - the name of the input file.
# templateName - The name of the template to use when creating the .arg file. 
#                In this template <<AST>> is expanded with the AST.
# extra    - A map with extra expansions.
#--------------------------------------------------------------------------------
def CreateArgFile(fileName, templateName, extra):
  argFile = util.ExtractName(fileName) + ".arg"  
  astFile = util.ExtractName(fileName) + ".ast"
  report.Progress(4, "Creating arg file (" + argFile +")")
  
  template = util.GetTemplate(templateName)
  if template == None:
    return false
  
  ast = util.ReadFile(astFile)
  if ast == None:
    return false

  data = util.SubString('<<AST>>', ast, template)
  for key in extra.keys():
    data = util.SubString(key, extra[key], data)

  ok = util.WriteFile(argFile, data)
  return ok

#--------------------------------------------------------------------------------
# Appends a name to the .vdmtest file
# fileName - name of the input file.
# returns  - false if anything went wrong.
#--------------------------------------------------------------------------------
def AppendToDotVdmtest(fileName):
  newName = util.ExtractName(fileName) + ".arg"  
  report.Progress(4, "Adding " + newName + " to .vdmtest file")

  return util.WriteFile(".vdmtest", newName+"\n","a")

#--------------------------------------------------------------------------------
# Prepares specifications files for a specification test run.
# That is, every vdm file is preprocessed, and a .vdmde file is created.
# lang    - the language (SL,PP,RT)
# env     - the test environment being executed (TC, IP, CPP, ...)
# returns - false if anything went wrong 
#--------------------------------------------------------------------------------
def SetupSpecification(lang, env):
  report.Progress(2, "Creating *.vdm" + lang + " files and .vdmde")


  ### Create the .vdmsl or vdmpp files
  if (env == 'tc'):
    specDir = cmdline.LookUp('tc-spec-dir')
    if lang == 'sl':
      dotVdm = '.vdmde-tc-sl'
    elif lang == 'pp':
      dotVdm = '.vdmde-tc-pp'
    else:
      dotVdm = '.vdmde-tc-rt'
  elif (env == 'ip'):
    specDir = cmdline.LookUp('ip-spec-dir')
    if lang == 'sl':
      dotVdm = '.vdmde-dyn-sl'
    elif lang == 'pp':
      dotVdm = '.vdmde-dyn-pp'
    else:
      dotVdm = '.vdmde-dyn-rt'

  elif (env == 'cpp'):
    specDir = cmdline.LookUp('cpp-spec-dir')
    if lang == 'sl':
      dotVdm = '.vdmde-cg-sl'
    elif lang == 'pp':
      dotVdm = '.vdmde-cg-pp'
    else:
      dotVdm = '.vdmde-cg-rt'
  elif (env == 'java'):
    specDir = cmdline.LookUp('java-spec-dir')
    if lang == 'sl':
      dotVdm = '.vdmde-cg-sl'
    elif lang == 'pp':
      dotVdm = '.vdmde-cg-pp'
    else:
      dotVdm = '.vdmde-cg-rt'
  elif (env == 'java2vdm'):
    specDir = cmdline.LookUp('java2vdm-spec-dir')
    if lang == 'sl':
      dotVdm = '.vdmde-j2v-sl'
    elif lang == 'pp':
      dotVdm = '.vdmde-j2v-pp'
    else:
      dotVdm = '.vdmde-j2v-rt'
  elif (env == 'j2vtf'):
    specDir = cmdline.LookUp('j2vtf-spec-dir')
    if lang == 'sl':
      dotVdm = '.vdmde-j2vtf-sl'
    elif lang == 'pp':
      dotVdm = '.vdmde-j2vtf-pp'
    else:
      dotVdm = '.vdmde-j2vtf-rt'  
  elif (env == 'pog'):
    specDir = cmdline.LookUp('pog-spec-dir')
    if lang == 'sl':
      dotVdm = '.vdmde-pog-sl'
    elif lang == 'pp':
      dotVdm = '.vdmde-pog-pp'
    else:
      dotVdm = '.vdmde-pog-rt'
  else:
    report.Error("Internal error in CreateSpecFiles, unknow env: '" + env + "'")
    return false;

  # Windows needs a different dotVdm file since file paths are different
  if util.IsWindowsOS() and os.environ.get('OSTYPE') == 'win32':
    dotVdm = dotVdm + '-win'
  
  if lang == 'sl':
    action = 'SL'
  elif lang == 'pp':
    action = 'PP'
  else:
    action = 'RT'

  if (env == 'java2vdm'):
    action = 'dot'

  # For Windows, build a Makefile on the fly
  if util.IsWindowsOS() and os.environ.get('OSTYPE') == 'win32':
    util.BuildMakefile(specDir)
    (exitCode, stdout, stderr) = util.RunCommand("make " + action, 
                                                 0, 'Possible syntax error in input specification')
  #else:
  #  (exitCode, stdout, stderr) = util.RunCommand('cd ' + specDir + "; make -f Makefile " + action, 
  #                                               0, 'Possible syntax error in input specification')

    if exitCode != 0:
      return false

  ### Create the .vdmde file
  # For Windows, execute the Makefile built locally
  if util.IsWindowsOS() and os.environ.get('OSTYPE') == 'win32':
    (exitCode, stdout, stderr) = util.RunCommand("make " + dotVdm, 0)
  else:
    (exitCode, stdout, stderr) = util.RunCommand('cd ' + specDir + "; make -f Makefile " + dotVdm, 0)

  if exitCode != 0:
    return false

  ### Copying the file to .vdmde
  return util.SymLink(specDir+'/' + dotVdm, ".vdmde")


#--------------------------------------------------------------------------------
# Returns the list of modules or classes in input file
# fullName - Name of the original test case
# lang     - The lanaguage the converted testcase (ConvertLanguage) is written in
# return   - a sequence of module or class names
#--------------------------------------------------------------------------------
def ModulesInSpecification(fullName, lang):
  global uselibs, standardlibs
  localName = util.ExtractName(fullName)+".vdm"
  
  data = util.ReadFile(localName)
  if data == None:
    return None
  
  if lang == 'sl':
    keyword = 'module '
  else:
    keyword = 'class '

  regexp = re.compile("^\s*" + keyword + "\s*(\w+)", re.M)

  index = 0
  result = []
  finished = 0
  standardlibs = []

  while finished != 1:
    match = regexp.search(data, index)
    if match == None:
      if result == []:
        report.Error("Could find any module or class in specification file '" + fullName + "'")
        return None
      finished = 1
    else:
      report.Progress(4,"Found module " + match.group(1))
      if IsStandardLib(match.group(1)):
        standardlibs.append(match.group(1))
      result.append(match.group(1))
      index = match.end()

  # If this is VDM-SL, return immediately, as we do not yet support
  # dlmodules in the test environment. FIXME
  if lang == 'sl':
    return result

  # Accumulate those modules that are dlclasses in uselibs
  keyword = 'dlclass'
  regexp1 = re.compile("^\s*" + keyword + "\s*(\w+)", re.M)
  regexp2 = re.compile("^\s*" + "uselib" + "\s*(\".+\")", re.M)

  index = 0
  finished = 0
  uselibs = []

  while finished != 1:
    match = regexp1.search(data, index)
    if match == None:
      finished = 1
    else:
      report.Progress(4,"Found dlclass " + match.group(1))
      result.append(match.group(1))
      uselib = regexp2.search(data, match.start())
      if uselib == None:
        report.Progress(4,"No uselib found for dlclass " + match.group(1))
      else:
        uselibs.append(uselib.group(1))
      index = match.end()
  return result

# Accessor function to retrieve uselibs
def GetUselibs():
  return uselibs

def IsStandardLib(mod):
  return mod == "MATH" or mod == "IO" or mod == "VDMUtil"

def GetStandardLibs():
  return standardlibs

#--------------------------------------------------------------------------------
# Extract module or class name and operation name from an .arg file
# fullName - the name of the test case
# returns  - a tuple with module or class name as its first component and
#            operation, function, or method name as its second component
#--------------------------------------------------------------------------------
def ExtractArgFileInfo(fullName):
  global modcls
  argFile = util.StripExt(fullName)+".arg"

  if os.path.exists(argFile):
    str = util.ReadFile(argFile)
    if str == None:
      return None

    match = re.match("^ *(.+)`(.+)\(\) *$", str, re.M)
    if match == None:
      match = re.match("^ *(.+)\.(.+)\(\) *$", str, re.M)
      if match == None:
        report.Error("Could parse argument file '" + argFile + "' with syntax MOD`OP()")
        return None
      else:
        modcls = match.group(1)
        op = match.group(2)
        return (None, (modcls, op))
    else:
      modcls = match.group(1)
      op = match.group(2)
  else:
    modcls = "A"
    op = "Test"

  return (modcls, op)

def GetModCls():
  return modcls
