import gentestcases, cmdline, util, setup, report, convert, resfile
import os, re, string
true, false = 1,0

#-----------------------------------------------------------------------------
# Execute Java code generator test environment.
# lang - language to use (SL, PP, RT)
# type - type of test (either spec or impl)
# return - Nothing.
#-----------------------------------------------------------------------------
def execute(lang, type):
  global packageMap
  packageMap = {}
  expSet = resfile.MakeStdExpansionSet('java', lang, 'impl')
  expSet = resfile.AppendToExpandsionSet(expSet,'',['dtcoff','dtcon'])
  resfile.RegisterExpansionSet(expSet)
  util.SetProfileBaseName("gmon-cpp-"+lang+"-"+type+"-"+cmdline.StartDate())
  if (type == 'spec'):
    executeSpec(lang)
  else:
    executeImpl(lang)


#-----------------------------------------------------------------------------
# Execute Java code generator test environment for specifications
# lang - language to use (SL, PP, RT)
# return - Nothing
#-----------------------------------------------------------------------------
def executeSpec(lang):
  modules = None
  ok = convert.SetupSpecification(lang, 'java')

  if not ok:
    report.Error("ABORTING specification test for " + `lang`)
    return 

  # counter to indicate progress
  total = 1

  # number of spec. files to run in one vdmde execution
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('java', lang, 'spec')

  # indicates if any spec test case produced an error.
  anyError = false

  # Initialize coverage file
  coverageFile = "coverage-java-"+lang+"-"+cmdline.StartDate()

  while ( true ):
    testCases = gentestcases.NextTestCases(jobSize)
    if testCases == []:
      break

    startIndex = total
    endIndex = total+len(testCases) -1
    report.Progress(2, "Handling test cases " + `startIndex` + "..." + `endIndex`)

    # Prepare the next test run - the parameter `spec-job-size' tells how
    # many testcases should be executed in each run.
    names = []
    util.DeleteFiles([".vdmtest"])

    for name in testCases:

      # prepare test cases.
      ok = PrepareSpecCase(name, lang)
      anyError = not ok or anyError
      if ok:
        names.append(name)

      # Get ready for next iteration.
      total = total +1

    # Run the test cases
    if names != []:
      report.Progress(3, "Running test cases " + `startIndex` + "..." + `endIndex`)
      report.setTestCaseName("testcase " + `startIndex` + "..." + `endIndex`)
      (okNames, modules) = RunSpecTestCases(names, lang, coverageFile)
      util.MoveProfile()

      baseName = util.ExtractName(name)
      if util.GenTestDir():
        util.MakeDir(baseName)
        files = [baseName+".vdm", baseName+".ast", baseName+".arg", 
                 baseName+".arg.res", baseName+".debug", baseName+".opt",
                 baseName+".arg.pt","debug.arg",baseName+".res"]
      
        for mod in modules:
          files = files + [mod+".java", "external_"+mod+".java", mod + ".java.bak"]
          util.DeleteFiles([mod + ".class", mod + "$" + mod + "Sentinel.class"])
          for file in files:
            util.CopyFileIfExists(file, baseName + "/" + file)

    # Clean Up.
    for testName in names:
      ok = (okNames.count(testName) > 0)
      if util.CleanFile(ok):
        util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg", 
                          baseName+".arg.res", baseName+".debug", baseName+".opt",
                          baseName+".arg.pt","debug.arg",baseName+".res"])
        anyError = anyError or not ok

    if util.CleanFile(not anyError):
      util.DeleteFiles(["TMAIN.java", "TMAIN.class"])
      if modules != None:
        for mod in modules:
          util.DeleteFiles([mod+".java", "external_"+mod+".java",
                            mod + ".class", mod + "$" + mod + "Sentinel.class",
                            mod + ".java.bak"])

  # Clean up .vdmde and .vdmtest
  if util.CleanFile(not anyError):
    util.DeleteFiles([".vdmde",".vdmtest"])

#-----------------------------------------------------------------------------
# Execute Java code generator test environment for specifications
# lang - language to use (SL, PP, RT)
# return - Nothing
#-----------------------------------------------------------------------------
def executeImpl(lang):
  # counter to indicate progress
  total = 1

  # jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('java', lang, 'impl')
  name = gentestcases.NextTestCase()

  while (name != None):
    report.setTestCaseName(name)
    if (total % jobSize) == 1:
      report.Progress(2, "Handling test cases " + `total` + "..." + `total + jobSize-1`)
    report.Progress(3, "Running " + name)

    (ok, modules) = RunImplTestCase(name, lang)

    if util.CleanFile(ok):
      bn = util.ExtractName(name)
      util.DeleteFiles([bn+".vdm", bn+".res", "TMAIN.java", "compare.arg", "compare.vdm"])
      if modules:
        for mod in modules:
          util.DeleteFiles([mod+".java", "external_"+mod+".java", mod + ".class"])
    name = gentestcases.NextTestCase()
    total = total +1

#-----------------------------------------------------------------------------
# Prepare a single test case for specification test run.
# name - the full name of the .vdm file to test
# lang - the language to use (SL, PP)
# return - a boolean which indicates whether the preparation went ok.
#-----------------------------------------------------------------------------
def PrepareSpecCase(fullName, lang):
  report.Progress(3, "preparing " + fullName)
  report.setTestCaseName(fullName)

  ok = convert.ConvertLanguage(lang, fullName)

  if ok:
    parser = cmdline.LookUpWildCard('java', lang, 'spec', 'parser')
    ok = convert.VDM2AST(fullName, parser, true)

  if ok:
    ok = convert.CreateOptionsFile(fullName)

  if ok:
    convert.CreateDebugFile(fullName, "debug-file-java", {'<<CGKIND>>' : '<JAVA>'} )

  if ok:
    ok = convert.CreateArgFile(fullName, "arg-file-java", {'<<CGKIND>>' : '<JAVA>'} )

  if ok:
    ok = convert.AppendToDotVdmtest(fullName)

  # Clean up if test case failed
  if not ok and util.CleanFile(ok):
    baseName = util.ExtractName(fullName)
    util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg",
                      baseName+".debug", "debug.arg"])
        
  return ok


#-----------------------------------------------------------------------------
# Run intepreter for specification test cases.
# fullNames - a list of names of test cases to execute
# lang      - the langauge to execute
# dtc       - 'dtcon' or 'dtcoff'
# coverageFile - name of test coverage file to write to.
#-----------------------------------------------------------------------------
def RunSpecTestCases(fullNames, lang, coverageFile):

  # remove files we expect output in to.
  for fullName in fullNames:
    util.RemoveTestFiles(fullName, [".arg.pt", ".arg.res", ".arg.err", ".arg.msg"])

  exitCode0 = os.system("rm -f quotes/*.java") 

  # run the test cases
  cmd = cmdline.LookUpWildCard('java', lang, 'spec', 'code-generator')
  cmd = cmd + " -D -a -b -R " + coverageFile
  exitCode = util.RunCommand(cmd, 0, "Possible core dump while interpreting specification.")

  okNames = []
  # Now validate the results
  for fullName in fullNames:
    bn = util.ExtractName(fullName)
    semResName = bn + ".arg.res"
    resName = bn + ".res"
    report.setTestCaseName(fullName)
    ok = true

    if ok:
      # Find the module or class names of the input specification.
      modules = convert.ModulesInSpecification(fullName, lang)
      if modules == None:
        ok = false

    # Clean out the files we expect as output from the code generation.
    for mod in modules:
      util.DeleteFiles([mod+".java", "external_"+mod+".java"])

    # See if a result file was created
    if ok:
      if not os.path.exists(semResName):
        report.Error("No result file generated for test case " + fullName, 
                     "Maybe the code generation failed for " +
                     "one of the previous test cases")
        ok = false

    convert.ExtractArgFileInfo(fullName)

    if ok:
      ok = ExtractSourceFiles(fullName, lang, modules)

    if ok:
      ok = CompileRunAndCompare(fullName, lang, 'spec', modules)

    if ok:
      okNames.append(fullName)

  return (okNames, modules)

#-----------------------------------------------------------------------------
# Run test case for implementation.
# fullName - Name of the original vdm file
# lang     - The language to run
# returns true if no errors occur.
#-----------------------------------------------------------------------------
def RunImplTestCase(fullName, lang):
  ok = true
  modules = None
  
  if ok:
    ok = convert.ConvertLanguage(lang, fullName)

  if ok:
    ok = convert.CreateOptionsFile(fullName)

  if ok:
    # Find the module or class names of the input specification.
    modules = convert.ModulesInSpecification(fullName, lang)
    if modules == None:
      ok = false

  convert.ExtractArgFileInfo(fullName)

  if ok:
    ok = CodeGenerateImpl(fullName, lang, modules)


  if ok:
    ok = CompileRunAndCompare(fullName, lang, 'impl', modules)

  return (ok, modules)
  

#-----------------------------------------------------------------------------
# Compiles generated code, exectes the compiled code and finally compares
# it against the expected result for the given test case.
# fullName - the fullName of the testcase
# lang - language executed
# modules - list of modules in the given test case.
#-----------------------------------------------------------------------------
def CompileRunAndCompare(fullName, lang, type, modules):
  global packageMap
  baseName = util.ExtractName(fullName)
  ok = true
  
  if ok:
    ok = CreateArgFile(fullName, lang, modules)

  standardlibs = convert.GetStandardLibs()
  #libdir = cmdline.LookUp('java-stdlib-dir')
  libdir = os.path.expandvars(cmdline.LookUp('java-stdlib-dir'))
  for lib in standardlibs:
    libfile = lib + ".java"
    util.CopyFile(libdir + "/" + libfile, libfile)

  if ok:
    ok = CompileJavaFiles(fullName, lang, type, modules)

  for mod in packageMap.keys():
    packagePaths = string.split(packageMap[mod],'.')
    packageDir = string.join(packagePaths,'/')
    if not os.path.exists(packageDir):
      os.makedirs(packageDir)
    report.Progress(4, "cp " + mod + "*.class " + packageDir + "/.")
    exitCode = os.system("cp " + mod + "*.class " + packageDir + "/.")

  interpreter = os.path.expandvars(cmdline.LookUpWildCard('java', lang, type, 'interpreter'))
    
  if ok:
    # Execute the binary
    #flags = cmdline.LookUpWildCard('java',lang,type,'rtflags')
    flags = os.path.expandvars(cmdline.LookUpWildCard('java',lang,type,'rtflags'))

    #if util.IsWindowsOS():
    #  flags = string.replace(flags, ":", ";")

    (exitCode, stdout, stderr) = util.RunCommand(interpreter + " " + flags + " TMAIN",
                                                 0,
                                                 "Error running Java "+
                                                 "generated  binary",
                                                 true)
    print ("java " + flags + " TMAIN run")
    ok = (exitCode == 0)

  if ok:
    ok = util.WriteFile(baseName+".res", stdout)

  if ok:
    resFile = resfile.FindResFile(fullName)
    ok = (resFile != None)

  if ok:
    ok = CompareRunTimeError(fullName, resFile)

    lib = ""

    if ok == None: # It was not a runtime error
      interpreter = lib + cmdline.LookUpWildCard('java', lang, type, 'spec-compare')

      # Compare the result
      ok = resfile.CompareResult(fullName,baseName+".res", resFile, interpreter)

  return ok
  
#-----------------------------------------------------------------------------
# Checks to see if the output from teh command was a runtime error. Is this
# the case then compare this agains the expected result. The glitch here is
# that the C++ code generator does not include the error number (which is
# the case for the interpreter.
# fullName - fullName for the testcase
# resFile  - name of the file containing the actual result
# returns  - true  - if the output was a runtime error and it was the expected one.  
#            false - if the output was a runtime error but not the expecte one.
#            None  - if the output was not a runtime error.
#-----------------------------------------------------------------------------
def CompareRunTimeError(fullName, resFile):
  report.Progress(4,"Searching for runtime error")
  bn = util.ExtractName(fullName)
  actualResult = util.ReadFile(bn+".res")
  if actualResult == None:
    return None
  if string.find(actualResult, "Run-Time Error") != -1:
    expectedResult = string.rstrip(util.ReadFile(resFile), "\n")
    ok = (string.find(expectedResult, "Run-Time Error") != -1)
    if ok:
      actualResult = string.strip(re.sub("\s+", " ", actualResult))
      expectedResult = re.sub("Run-Time Error[ 0-9]*: ", "Run-Time Error:", expectedResult)
      #expectedResult = string.strip(re.sub("\s+", " ", expectedResult))
      ok = (actualResult == expectedResult)

    if not ok:
      report.Error("Actual result is different from expected result for " + `fullName`,
                   "expected result : " + expectedResult + "\n" +
                   "actual result   : " + actualResult)
    return ok
  else:
    return None

#-----------------------------------------------------------------------------
# Checks any files which are to be merged to see if they specify a
# package. FIXME: Sets this up even if package has keep tag "NO"
# fullName - fullName for the test case
# modules  - list of all the modules in the test case.
#-----------------------------------------------------------------------------
def GetPackage(fullName):
  data = util.ReadFile(fullName)
  if data == None:
    return ""

  regexp = re.compile("^package (.*)\s*;", re.M)
  match = regexp.search(data, 0)
  if match == None:
    return ""
  else:
    report.Progress(4, "Found package " + match.group(1) + " in " + fullName)
    return match.group(1)


#-----------------------------------------------------------------------------
# Copies any java files from the test case's directory which should be
# merged with the generated file
# fullName - fullName for the test case
# modules  - list of all the modules in the test case.
#-----------------------------------------------------------------------------
def CopyFilesForMerging(fullName, modules):
  global packageMap

  # Copy any files needed for merging
  dir = os.path.dirname(fullName)
  for mod in modules:
    javaFile = dir + "/" + mod + ".java"
    if os.path.exists(javaFile):
      util.CopyFile(javaFile,"./" + mod + ".java")
      package = GetPackage("./" + mod + ".java");
      if package != "":
        packageMap[mod] = package
    externalJavaFile = dir + "/external_" + mod + ".java"
    if os.path.exists(externalJavaFile):
      util.CopyFile(externalJavaFile,"./external_" + mod + ".java")
      package = GetPackage("./external_" + mod + ".java");
      if package != "":
        packageMap[mod] = package

#-----------------------------------------------------------------------------
# Generates code for an implementation test.
# fullName - fullName for the test case
# lang     - language
# modules  - list of all the modules in the test case.
#-----------------------------------------------------------------------------
def CodeGenerateImpl(fullName, lang, modules):
  localName = util.ExtractName(fullName)+".vdm"

  # Clean out the files we expect as output from the code generation.
  for mod in modules:
    util.DeleteFiles([mod+".java", "external_"+mod+".java"])

  exitCode0 = os.system("rm -f ./quotes/* ./*.class")

  CopyFilesForMerging(fullName, modules)
  
  # Run the code generation
  interpreter = cmdline.LookUpWildCard('java', lang, 'impl', 'code-generator')

  options = convert.ParseOptionsFile(fullName)

  cmd = interpreter + options + " -j -e -P " + localName
  #cmd = interpreter + options + " -j -e -P -L " + localName
  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0,
                                 "Problem code generating specification, " +
                                 "maybe syntax or type error in test case")
  ok = (exitCode == 0)

  if ok:
    ok = VerifyPresenceOfGeneratedFiles(fullName, modules)

  return ok

#-----------------------------------------------------------------------------
# Creates the TMAIN.java file, which contains the main function for the
# generated code.
# fullName - fullName for the test case.
# lang     - language
# modules  - sequence of all the modules in the test specification
#-----------------------------------------------------------------------------
def CreateArgFile(fullName,lang, modules):
  info = convert.ExtractArgFileInfo(fullName)
  if info == None:
    return None
  else:
    (modcls, op) = info

  if lang == 'sl':
    template = util.GetTemplate('arg-file-java-sl')
    str = ""
    for mod in modules:
      str = str + "init_" + mod + "();"
    data = util.SubString('<<INIT>>', str, template)
    data = util.SubString('<<MOD>>', modcls, data)
    data = util.SubString('<<OP>>', op, data)
  else:
    template = util.GetTemplate('arg-file-java-pp')
    data = util.SubString('<<CLASS>>',modcls, template)
    data = util.SubString('<<OP>>', op, data)

  if data == None:
    return false

  return util.WriteFile("TMAIN.java", data)
#???? Is TMAIN.java ????


#-----------------------------------------------------------------------------
# Compile the generated files
# fullName - fullName for the test case.
# lang     - language
# type     - either SPEC or IMPL
# modules  - sequence of all the modules in the test specification
#-----------------------------------------------------------------------------

def CompileJavaFiles(fullName, lang, type, modules):
  baseName = util.ExtractName(fullName)

  # Find the compiler to use
  compiler = os.path.expandvars(cmdline.LookUpWildCard('java', lang, type, 'compiler'))

  # figure out the names of all the Java files
  javaFiles = "TMAIN.java"

  for mod in modules:
    if os.path.exists(mod + ".java"):
      javaFiles = javaFiles + " " + mod+".java"
    else:
      package = convert.GetModCls()
      packageStrings = string.split(package, '.')
      packageDir = string.join(packageStrings, '/')
      if os.path.exists(packageDir + ".java") and not packageDir in modules:
        print "-------> here"
        javaFiles = javaFiles + " " + packageDir + ".java"
      
    if os.path.exists("external_"+mod+".java"):
      javaFiles = javaFiles + " external_"+mod+".java"


  # Find the flags for the compiler
  flags = os.path.expandvars(os.path.expanduser(cmdline.LookUpWildCard('java', lang, type, 'cflags')))

  # First delete the binary.
  util.DeleteFiles(["TMAIN.class"])
  
  # build the command and execute it.
  cmd = compiler + " " + flags + " " + javaFiles

  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, "Problem when compiling generated code")
  ok = (exitCode == 0)

  if ok:
    if not os.path.exists("TMAIN.class"):
      report.Error("TMAIN.class was not created as a result of compiling the generated Java files")
      return false

  return ok

#-----------------------------------------------------------------------------
# Extract the source files from the vdm value obtained by
# running the specification java code generator
# fullName - fullName for the test case.
# lang     - language
# modules  - sequence of all the modules in the test specification
#-----------------------------------------------------------------------------
def ExtractSourceFiles(fullName, lang, modules):
  resName = util.ExtractName(fullName) + ".arg.res"
  cmd = cmdline.LookUpWildCard('java', lang, 'spec', 'extract-source-backend')
  CopyFilesForMerging(fullName, modules)

  cmd = cmd + " java  " + resName
  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0,
                                 "Problem extracting source files from " +
                                 "output from Java code generator " +
                                 "specification")
  ok = (exitCode == 0)

  if ok:
    ok = VerifyPresenceOfGeneratedFiles(fullName, modules)
  return ok

#-----------------------------------------------------------------------------
# Verifies that the correct files was created when code generated.
# fullName - fullName for the test case.
# modules  - sequence of all the modules in the test specification
#-----------------------------------------------------------------------------
def VerifyPresenceOfGeneratedFiles(fullName, modules):
  ok = true
  package = convert.GetModCls()
  packageStrings = string.split(package, '.')
  packageDir = string.join(packageStrings, '/')
  for mod in modules:
    file1 = mod + ".java"
    file2 = "vdm_" + mod+".java"
    file3 = packageDir + ".java"
    files = file1 + " or " + file2 + " or " + file3
    if not os.path.exists(file1) and not os.path.exists(file2) and not os.path.exists(file3):
      report.Error("file " + files + " was not generated for testcase " + `fullName`)
      ok = false
  return ok

