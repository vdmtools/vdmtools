import gentestcases, cmdline, util, setup, report, convert, resfile
import os, re, string
true, false = 1,0

# srcext and binext are used to represent the extensions used for C++ files 
# and executables respectively. This allows use of the environment on both 
# Windows and Linux
srcext = ""
objext = ""
binext = ""

#--------------------------------------------------------------------------------
# Execute C++ code generator test environment.
# lang - language to use (SL, PP, RT)
# type - type of test (either spec or impl)
# return - Nothing.
#--------------------------------------------------------------------------------
def execute(lang, type):
  global srcext, objext, binext
  expSet = resfile.MakeStdExpansionSet('cpp', lang, 'impl')
  expSet = resfile.AppendToExpandsionSet(expSet,'',['dtcoff','dtcon'])
  resfile.RegisterExpansionSet(expSet)
  util.SetProfileBaseName("gmon-cpp-"+lang+"-"+type+"-"+cmdline.StartDate())
  if util.UsesVisualStudio():
    srcext = "cpp"
    objext = "obj"
    binext = ".exe"
  else:
    srcext = 'cc'
  if (type == 'spec'):
    executeSpec(lang)
  else:
    executeImpl(lang)


#--------------------------------------------------------------------------------
# Execute C++ code generator test environment for specifications
# lang - language to use (SL, PP, RT)
# return - Nothing
#--------------------------------------------------------------------------------
def executeSpec(lang):
  ok = convert.SetupSpecification(lang, 'cpp')

  if not ok:
    report.Error("ABORTING specification test for " + `lang`)
    return 

  # counter to indicate progress
  total = 1

  # number of spec. files to run in one vdmde execution
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('cpp', lang, 'spec')

  # indicates if any spec test case produced an error.
  anyError = false

  # Initialize coverage file
  #coverageFile = "coverage-cpp-"+lang+"-"+cmdline.StartDate()
  coverageFile = "rtinfo.ast"

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
      okNames = RunSpecTestCases(names, lang, coverageFile)
      util.MoveProfile()

    # Clean Up.
    for testName in names:
      ok = (okNames.count(testName) > 0)
      if util.CleanFile(ok):
        baseName = util.ExtractName(testName)
        util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg", baseName+".arg.res", baseName+".debug",
                          baseName+".arg.pt","debug.arg",baseName+".res"])
      anyError = anyError or not ok

  # Clean up .vdmde and .vdmtest
  if util.CleanFile(not anyError):
    util.DeleteFiles([".vdmde",".vdmtest"])

#--------------------------------------------------------------------------------
# Execute C++ code generator test environment for specifications
# lang - language to use (SL, PP, RT)
# return - Nothing
#--------------------------------------------------------------------------------
def executeImpl(lang):
  global srcext, binext
  # counter to indicate progress
  total = 1

  # jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('cpp', lang, 'impl')
  name = gentestcases.NextTestCase()

  while (name != None):
    report.setTestCaseName(name)
    if (total % jobSize) == 1:
      report.Progress(2, "Handling test cases " + `total` + "..." + `total + jobSize-1`)
    report.Progress(3, "Running " + name)

    (ok, modules) = RunImplTestCase(name, lang)

    if util.CleanFile(ok):
      bn = util.ExtractName(name)
      util.DeleteFiles([bn+".vdm",bn+".res", "icode." + srcext, "icode" + binext,"CGBase." + srcext, "CGBase.h", "CGBase.obj", "icode.obj", "compare.arg", "compare.vdm"])
      if modules:
        for mod0 in modules:
          mod = string.replace(mod0, "_", "_u")
          util.DeleteFiles([mod+"." + srcext, mod+".h", mod+"_anonym." + srcext, mod+"_anonym.h",mod+"_userdef.h", mod + ".obj"])
    name = gentestcases.NextTestCase()
    total = total +1

#--------------------------------------------------------------------------------
# Prepare a single test case for specification test run.
# name - the full name of the .vdm file to test
# lang - the language to use (SL, PP)
# return - a boolean which indicates whether the preparation went ok.
#--------------------------------------------------------------------------------
def PrepareSpecCase(fullName, lang):
  report.Progress(3, "preparing " + fullName)
  report.setTestCaseName(fullName)

  ok = convert.ConvertLanguage(lang, fullName)

  if ok:
    parser = cmdline.LookUpWildCard('cpp', lang, 'spec', 'parser')
    ok = convert.VDM2AST(fullName, parser, true)

  if ok:
    ok = convert.CreateOptionsFile(fullName)

  if ok:
    convert.CreateDebugFile(fullName, "debug-file-cpp", {'<<CGKIND>>' : '<CPP>'} )

  if ok:
    ok = convert.CreateArgFile(fullName, "arg-file-cpp",  {'<<CGKIND>>' : '<CPP>'} )

  if ok:
    ok = convert.AppendToDotVdmtest(fullName)

  # Clean up if test case failed
  if not ok and util.CleanFile(ok):
    baseName = util.ExtractName(fullName)
    util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg", baseName+".debug", "debug.arg"])
        
  return ok


#--------------------------------------------------------------------------------
# Run intepreter for specification test cases.
# fullNames - a list of names of test cases to execute
# lang      - the langauge to execute
# dtc       - 'dtcon' or 'dtcoff'
# coverageFile - name of test coverage file to write to.
#--------------------------------------------------------------------------------
def RunSpecTestCases(fullNames, lang, coverageFile):

  # remove files we expect output in to.
  for fullName in fullNames:
    util.RemoveTestFiles(fullName, [".arg.pt", ".arg.res", ".arg.err", ".arg.msg"])

  # run the test cases
  #cmd = "LD_LIBRARY_PATH=/usr/local/omniORB/lib "
  #cmd = cmd + cmdline.LookUpWildCard('cpp', lang, 'spec', 'interpreter')
  cmd = cmdline.LookUpWildCard('cpp', lang, 'spec', 'interpreter')

  cmd = cmd + " -D -P -a -b -R " + coverageFile
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

    # See if a result file was created
    if ok:
      if not os.path.exists(semResName):
        report.Error("No result file generated for test case " + fullName, 
                     "Maybe the code generation failed for one of the previous test cases")
        ok = false

    if ok:
      ok = ExtractSourceFiles(fullName, lang, modules)

    if ok:
      ok = CompileRunAndCompare(fullName, lang, 'spec', modules)

    if ok:
      okNames.append(fullName)

  return okNames

#--------------------------------------------------------------------------------
# Run test case for implementation.
# fullName - Name of the original vdm file
# lang     - The language to run
# returns true if no errors occur.
#--------------------------------------------------------------------------------
def RunImplTestCase(fullName, lang):
  ok = true
  modules = None
  
  if ok:
    ok = convert.ConvertLanguage(lang, fullName)

  if ok:
    # Find the module or class names of the input specification.
    modules = convert.ModulesInSpecification(fullName, lang)
    if modules == None:
      ok = false

  if ok:
    ok = CodeGenerateImpl(fullName, lang, modules)

  if ok:
    ok = CompileRunAndCompare(fullName, lang, 'impl', modules)

  return (ok, modules)
  

#--------------------------------------------------------------------------------
# Compiles generated code, exectes the compiled code and finally compares
# it agains the expected result for the given test case.
# fullName - the fullName of teh testcase
# lang - language executed
# modules - list of modules in the given test case.
#--------------------------------------------------------------------------------
def CompileRunAndCompare(fullName, lang, type, modules):
  baseName = util.ExtractName(fullName)
  ok = true
  stdout = None
  stderr = None
  
  if ok:
    CreateUserdefFile(modules)

  if ok:
    ok = CreateArgFile(fullName, lang, modules)

  if ok:
    ok = CompileCPPFiles(fullName, lang, type, modules)

  if ok:
    # Execute the binary
    (exitCode, stdout, stderr) = util.RunCommand("icode", 0, "Error running c++ generated binary", true)
    ok = (exitCode == 0)
  

  if ok:
    ok = util.WriteFile(baseName+".res", stdout)
  else:
    report.Error("Output", None, stdout, stderr)

  if ok:
    resFile = resfile.FindResFile(fullName)
    ok = (resFile != None)

  if ok:
    ok = CompareRunTimeError(fullName, resFile)

    if ok == None: # It was not a runtime error
      interpreter = cmdline.LookUpWildCard('cpp', lang, type, 'spec-compare')

      # Compare the result
      ok = resfile.CompareResult(fullName,baseName+".res", resFile, interpreter)

  return ok
  
#--------------------------------------------------------------------------------
# Checks to see if the output from teh command was a runtime error. Is this
# the case then compare this agains the expected result. The glitch here is
# that the C++ code generator does not include the error number (which is
# the case for the interpreter.
# fullName - fullName for the testcase
# resFile  - name of the file containing the actual result
# returns  - true  - if the output was a runtime error an it was the expected one.
#            false - if the output was a runtime error but not the expecte one.
#            None  - if the output was not a runtime error.
#--------------------------------------------------------------------------------
def CompareRunTimeError(fullName, resFile):
  report.Progress(4,"Searching for runtime error")
  bn = util.ExtractName(fullName)
  actualResult = util.ReadFile(bn+".res")
  if actualResult == None:
    return None
  if string.find(actualResult, "Run-Time Error") != -1:
    #expectedResult = util.ReadFile(resFile)
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

  
#--------------------------------------------------------------------------------
# Generates code for an implementation test.
# fullName - fullName for the test case
# lang     - language
# modules  - list of all the modules in the test case.
#--------------------------------------------------------------------------------
def CodeGenerateImpl(fullName, lang, modules):
  global srcext
  localName = util.ExtractName(fullName)+".vdm"

  # Clean out the files we expect as output from the code generation.
  for mod0 in modules:
    mod = string.replace(mod0, "_", "_u")
    util.DeleteFiles([mod+"." + srcext, mod+".h", mod+"_anonym." + srcext, mod+"_anonym.h", mod+"_userdef.h"])

  # Run the code generation
  interpreter = cmdline.LookUpWildCard('cpp', lang, 'impl', 'code-generator')

  cmd = interpreter + " -c -P " + localName
  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, "Problem code generating specification, maybe syntax or type error in test case")
  ok = (exitCode == 0)

  if ok:
    ok = VerifyPresenceOfGeneratedFiles(fullName, modules)

  return ok

#--------------------------------------------------------------------------------
# Creates the file MOD_userdef.h for each module in test case.
# modules - list of the modules in the test case.
#--------------------------------------------------------------------------------
def CreateUserdefFile(modules):
  # Create _usedef.h files
  count = 1
  ok = true
  for mod0 in modules:
    mod = string.replace(mod0, "_", "_u")
    data = "#define TAG_" + mod + " " + `count*10000`+"\n"
    count = count +1
    ok = util.WriteFile(mod+"_userdef.h", data) and ok

  return ok

#--------------------------------------------------------------------------------
# Creates the icode.cc file, which contains the main function for the
# generated code.
# fullName - fullName for the test case.
# lang     - language
# modules  - sequence of all the modules in the test specification
#--------------------------------------------------------------------------------
def CreateArgFile(fullName,lang, modules):
  global srcext
  info = convert.ExtractArgFileInfo(fullName)
  if info == None:
    return None
  else:
    (modcls, op) = info

  if lang == 'sl':
    template = util.GetTemplate('arg-file-cpp-sl')
    str = ""
    for mod0 in modules:
      mod = string.replace(mod0, "_", "_u")
      str = str + "init_" + mod + "();"
    data = util.SubString('<<INIT>>', str, template)
    data = util.SubString('<<MOD>>', modcls, data)
    data = util.SubString('<<OP>>', op, data)
  else:
# Remove any package path if it exists (n/a for c++)
    match = re.match("^.*\.([^.]+)$", modcls, re.M)
    if match != None:
      modcls = match.group(1)
    template = util.GetTemplate('arg-file-cpp-pp')
    data = util.SubString('<<CLASS>>',modcls, template)
    data = util.SubString('<<OP>>', op, data)

  if data == None:
    return false
  return util.WriteFile("icode." + srcext, data)

#--------------------------------------------------------------------------------
# Compile the generated files
# fullName - fullName for the test case.
# lang     - language
# type     - either SPEC or IMPL
# modules  - sequence of all the modules in the test specification
#--------------------------------------------------------------------------------

def CompileCPPFiles(fullName, lang, type, modules):
  global srcext, objext, binext
  baseName = util.ExtractName(fullName)
  # Find the compiler to use
  if util.UsesVisualStudio():
    compiler = 'cl.exe /nologo'
  else:
    compiler = cmdline.LookUpWildCard('cpp', lang, type, 'compiler')

  # figure out the names of all the CC files
  ccFiles = "icode." + srcext
  if lang != 'sl':
    ccFiles = ccFiles + " CGBase." + srcext
  for mod0 in modules:
    mod = string.replace(mod0, "_", "_u")
    ccFiles = ccFiles + " " + mod + "." + srcext

  # If there were any dl classes specified, add the corresponding files to 
  # be linked against.
  # FIXME: Only works on Linux now (assumes xxx.so uselib). 
  linkFiles = ""
  uselibs = convert.GetUselibs()
  regexp = re.compile("(.*).so", re.M)
  for ul in uselibs:
    match = regexp.search(ul,1)
    if match == None:
      report.Progress(4, "uselib " + ul + " has unexpected format - skipping")
    else:
      if util.UsesVisualStudio():
        dllib = match.group(1) + ".obj"
      else:
        dllib = match.group(1) + ".o"
      linkFiles = linkFiles + dllib + " "

  # Find the flags for the compiler
  flags = os.path.expandvars(cmdline.LookUpWildCard('cpp', lang, type, 'cflags'))

  # Append language define.
  if lang == 'sl':
    flags = " -DVDMSL " + flags
  else:
    flags = " -DVDMPP " + flags

  flags = flags + " -D__" + string.replace(baseName, "-", "_") + "__"

  # Append link path to the directory containing the source file, as
  # this directory may also contain the extra _userimpl.cc file.
  if util.UsesVisualStudio():
    #flags = flags + " /IC:/cygwin" + os.path.dirname(fullName)
    flags = flags + " /I" + util.getWindowsPath(os.path.dirname(fullName))
  else:
    flags = flags + " -I" + os.path.dirname(fullName)

  # First delete the binary.
  if util.UsesVisualStudio():
    util.DeleteFiles(["icode.exe", "icode.exe.manifest", "math.h"])
  else:
    util.DeleteFiles(["icode"])
  
  # build the command and execute it.
  if util.UsesVisualStudio():
    cmd = compiler + " -c " + ccFiles + " " + flags
  else:
    cmd = compiler + " -o icode " + ccFiles + " " + linkFiles + flags

  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, "Problem when compiling generated code")
  ok = (exitCode == 0)

  if util.UsesVisualStudio():
    if ok:
      objFiles = "icode." + objext
      if lang != 'sl':
        objFiles = objFiles + " CGBase." + objext
      for mod0 in modules:
        mod = string.replace(mod0, "_", "_u")
        objFiles = objFiles + " " + mod + "." + objext

      libdir = os.path.expandvars(os.path.expanduser(cmdline.LookUp('cpp-lib-dir')))
      #cmd = "link.exe /INCREMENTAL:NO /NOLOGO /out:icode.exe " + objFiles + " " + linkFiles + "vdm.lib CG.lib user32.lib"
      cmd = "link.exe /INCREMENTAL:NO /NOLOGO /out:icode.exe " + objFiles + " " + linkFiles + "/LIBPATH:" + util.getWindowsPath(libdir) + " CG.lib vdm.lib user32.lib"
      (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, "Problem when compiling generated code")
      ok = (exitCode == 0)

  if ok:
    if not os.path.exists("icode" + binext):
      report.Error("icode binary was not created as a result of compiling the generated C++ files")
      return false

  return ok

#--------------------------------------------------------------------------------
# Extract the source and header files from the vdm value obtained by
# running the specification c++ code generator
# fullName - fullName for the test case.
# lang     - language
# modules  - sequence of all the modules in the test specification
#--------------------------------------------------------------------------------
def ExtractSourceFiles(fullName, lang, modules):
  resName = util.ExtractName(fullName) + ".arg.res"
  cmd = cmdline.LookUpWildCard('cpp', lang, 'spec', 'extract-source-backend')

  cmd = cmd + " cpp " + resName
  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, "Problem extracting source files from output from C++ code generator specification")
  ok = (exitCode == 0)

  if ok:
    ok = VerifyPresenceOfGeneratedFiles(fullName, modules)
  return ok

#--------------------------------------------------------------------------------
# Verifies that the correct files was created when code generated.
# fullName - fullName for the test case.
# modules  - sequence of all the modules in the test specification
#--------------------------------------------------------------------------------
def VerifyPresenceOfGeneratedFiles(fullName, modules):
  global srcext
  ok = true
  extensions = ["." + srcext, ".h", "_anonym." + srcext, "_anonym.h"]
  for mod0 in modules:
    mod = string.replace(mod0, "_", "_u")
    for thisext in extensions:
      if not util.UsesVisualStudio() or mod != 'MATH' or thisext != '.h':
        if not os.path.exists(mod+thisext):
          report.Error("file " + `mod+thisext` + " was not generated for testcase " + `fullName`)
          report.Progress(4,"file " + `mod+thisext` + " was not generated for testcase " + `fullName`)
          ok = false
  
  return ok

