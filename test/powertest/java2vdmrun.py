import gentestcases, cmdline, util, setup, report, convert, resfile
#import popen2
import os, re, string
import shutil, tempfile, types

true, false = 1,0

parseRes = "m4cpp"

#-----------------------------------------------------------------------------
# Execute Java2VDM Parser
# lang - language to use (SL, PP, RT) - not used
# type - type of test (either spec or impl) - not used yet
# return - Nothing.
#-----------------------------------------------------------------------------
def execute(lang, type):
  global vdmde, speccompare, backend, javac, java, vppde
  global parserOnly, runJavacBeforeParser, staticSemanticsOnly

  expSet = resfile.MakeStdExpansionSet('java2vdm', lang, 'impl')
  expSet = resfile.AppendToExpandsionSet(expSet,'',['dtcoff','dtcon'])
  resfile.RegisterExpansionSet(expSet)

  javac = cmdline.LookUpWildCard('java', lang, 'spec', 'compiler')
  java = cmdline.LookUpWildCard('java', lang, 'spec', 'interpreter')
  vppde= cmdline.LookUpWildCard('java', 'pp', 'impl', 'code-generator')
  vdmde = cmdline.LookUpWildCard('java', 'sl', 'impl', 'code-generator')

  speccompare = vdmde
  backend = cmdline.LookUp('java2vdm-backend')

  staticSemanticsOnly = cmdline.LookUp('java2vdm-static-semantics-only')
  runJavacBeforeParser = cmdline.LookUp('java2vdm-javac-before-parser')
  parserOnly = cmdline.LookUp('java2vdm-java-parser-only')

  if (type == 'spec'):
    executeSpec(lang)
  else:
    executeImpl(lang)


#----------------------------------------------------------------------------
# Execute Java2VDM translator at spec level
# lang - language to use (SL, PP, RT)
# return - Nothing
#-----------------------------------------------------------------------------
def executeSpec(lang):
  if not util.IsWindowsOS():
    ok = convert.SetupSpecification(lang, "java2vdm")
    if not ok:
      report.Error("ABORTING specification test for " + lang)

  # counter to indicate progress
  total = 1

  # jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases from javacg-code and java2vdm.
  StartCollectionOfTests()

  # indicates if any spec test case produced an error.
  anyError = false

  # Initialize coverage file
  coverageFile = "coverage-java2vdm-"+lang+"-"+cmdline.StartDate()

  oneTestCase = cmdline.LookUp('java2vdm-test-case')

  if oneTestCase != "" :
    testCasesDirectories.append(oneTestCase)
  else:
    AddTestDirectoriesFromHierarchy(cmdline.GetTestCases('java2vdm'), lang)
  
  while ( true ):
    cases = NextTestCasesDir(jobSize)

    if (cases==[]):
      break

    startIndex = total
    
    endIndex = total+len(testCasesDirectories) -1
    report.Progress(2, "Handling test cases " + str(startIndex) + "..." + str(endIndex))

    # Prepare the next test run - the parameter 'spec-job-size' tells how
    # many testcases should be executed in each run.
    names = []
    util.DeleteFiles([".vdmtest"])

    for case in cases:
      # prepare test cases.

      ok = PrepareSpecCaseDir(case, lang)
      anyError = not ok or anyError
      if ok:
        names.append(case)

      # Get ready for next iteration.
      total = total +1

    # Run the test cases
    if names != []:
      report.Progress(3, "Running test cases " + str(startIndex) + "..." + str(endIndex))
      report.setTestCaseName("testcase " + str(startIndex) + "..." + str(endIndex))
      (okNames, modules) = RunSpecTestCases(names, lang, coverageFile)

    # Clean Up.
    for testName in names:
      ok = (okNames.count(testName) > 0)
      baseName = GetTestName(testName)
      if util.CleanFile(ok):
        util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg", 
                          baseName+".arg.res", baseName+".debug",
                          baseName+".opt", baseName+".res.exp",
                          baseName+".arg.pt","debug.arg",baseName+".res"])
        util.DeleteFiles(["vpp.arg"])
        anyError = anyError or not ok

  #clean directories
  filesToDelete = []
  FID=os.popen( "find" + " -type f ")
  line = FID.readline()
  while (line != ''):
    if string.strip(line[len(line)-4:]) == 'vpp' or string.strip(line[len(line)-6:]) == 'class':
      filesToDelete.append(line[:-1])
    line = FID.readline()
  util.DeleteFiles(filesToDelete)

  for f in os.listdir(os.getcwd()):
    if os.path.isdir(f):
      os.system("rmdir " + f)
  
  #Clean up .vdmde and .vdmtest
  if util.CleanFile(not anyError):
    util.DeleteFiles([".vdmde", ".vdmtest", parseRes, "nuloptions"])
 
  if not util.IsWindowsOS():
    util.SymLink(coverageFile, "rtinfo.ast")
  return

#-----------------------------------------------------------------------------
# Execute Java2VDM translator at impl level
# lang - language to use (SL, PP, RT) - not used
# return - Nothing
#-----------------------------------------------------------------------------
def executeImpl(lang):
  # counter to indicate progress
  total = 1

  # jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases from javacg-code and java2vdm.
  StartCollectionOfTests()

  AddTestsFromHierarchy(cmdline.GetTestCases('java2vdm'), lang)

  while (testCases != []):
    test = testCases[0]
    report.setTestCaseName(test)
    if (total % jobSize) == 1:
      report.Progress(2, "Handling test cases " + str(total) + "..." + str(total + jobSize-1))
    ok = RunImplTestCase(test, lang)
    total = total +1

    GetNextTest()

  return

#-----------------------------------------------------------------------------
def StartCollectionOfTests():
  global testCases, testCasesDirectories
  testCases = []
  testCasesDirectories = []
  
#-----------------------------------------------------------------------------
# Adds tests from hierarchy of tests to testCases
# root - path to the root of the hierarchy of tests
# lang - language to use (SL, PP, RT) - not used
# return - Nothing
# Collects Java2VDM++ tests AT THE MOMENT
#-----------------------------------------------------------------------------

def AddTestsFromHierarchy(root, lang):
  global testCases

  testCases = []
  FID=os.popen("find " + root + " -type f -name \*.java")
  line = FID.readline()
  while (line != ''):
    testCases.append(line[:-1])
    line = FID.readline()

#-----------------------------------------------------------------------------
# Returns the next test case 
#----------------------------------------------------------------------------- 
def GetNextTest():
  global testCases
  if (testCases != []):
    nextTest = testCases[0]
    testCases = testCases[1:]  
    return nextTest

#-----------------------------------------------------------------------------
# Adds all hiers directories of root
# which contain testCases to testCasesDirectories
#
# root - path to the root of the hierarchy of tests
# lang - language to use (SL, PP, RT) - not used
# return - Nothing
# Collects Java2VDM++ tests AT THE MOMENT
#-----------------------------------------------------------------------------

def AddTestDirectoriesFromHierarchy(root, lang):
  global testCasesDirectories, parserOnly, staticSemanticsOnly

  if (parserOnly=="yes" or staticSemanticsOnly=="yes"):
    pattern = "arg.res.exp"
  else:
    pattern = "exp"

  testCandidates = []
 
  if not util.IsWindowsOS():
    FID=os.popen("find " + root[0] + " -type d -name \*  2>/dev/null")
  else:
    FID=os.popen("find " + root[0] + " -type d -name \"*\" ")

  line = FID.readline()
  while (line != ''):
    testCandidates.append(line[:-1])
    line = FID.readline()
  
  for candidate in testCandidates:
    if not util.IsWindowsOS():
      FID=os.popen("ls " + candidate + "/*." +pattern + " 2>/dev/null")
    else:
      FID=os.popen("ls " + candidate + "/*." +pattern + " ")

    line = FID.readline()
    if line !='':
      testCasesDirectories.append(candidate)

#-----------------------------------------------------------------------------
# Returns the next test case 
#----------------------------------------------------------------------------- 
def GetNextTestDir():
  global testCasesDirectories
  if (testCasesDirectories != []):
    nextTest = testCasesDirectories[0]
    testCasesDirectories = testCasesDirectories[1:]  
    return nextTest

def NextTestCasesDir(jobSize):
  cases = []
  while (jobSize>0 and testCasesDirectories!=[]):
    case = GetNextTestDir()
    if case == None:
      return cases
    else:
      cases.append(case)
    
    jobSize=jobSize-1
  
  return cases

#-----------------------------------------------------------------------------
# Get a test name from a directory name 
# dirName - the full name of the directory to test
# lang - the language to use (SL, PP)
# return - the terminal directory name 
#-----------------------------------------------------------------------------
def GetTestName(dirName):
  if not util.IsWindowsOS():
    namelist = re.split("/", os.path.abspath(dirName))
  else:
    p = util.SubString('\\', "/", os.path.abspath(dirName))
    namelist = re.split("/", p)
  namelist.reverse() 
  return namelist[0]


#-----------------------------------------------------------------------------
# Delete files with common basename but different extension.
# fullName  - the basename of the test file (the path is removed from the
#             file!)
# extension - a List of extensions to remove.
# returns   - nothing
#-----------------------------------------------------------------------------
def RemoveTestFiles(fullName, extensions):
  bn = os.path.dirname(fullName)+"/"+util.ExtractName(fullName)
  files = []
  for ext in extensions:
    files.append(bn+ext)
  util.DeleteFiles(files)

#-----------------------------------------------------------------------------
# Prepare a directory containing *.java file for specification test run.
# name - the full name of the directory to test
# lang - the language to use (SL, PP)
# return - a boolean which indicates whether the preparation went ok.
#-----------------------------------------------------------------------------
def PrepareSpecCaseDir(dirName, lang):
  global runJavacBeforeParser

  testName = GetTestName(dirName)

  fullName = os.getcwd()
    
#
#  Produce a list of java files in dirName
#  and put it testCases 
#
  AddTestsFromHierarchy(dirName, lang)
  
  util.DeleteFiles([parseRes])
  util.RemoveTestFiles(testName, [".arg.pt", ".arg.res", ".arg.err",  ".res.arg",
                                    ".arg.msg", "vpp.arg", ".res", ".debug", ".ast"])
  
  ok = convert.CreateOptionsFile(fullName)

  if runJavacBeforeParser=="yes":
    ok = CompileJavaFiles(dirName)
    if not ok:
      return false

  
  javafiles = " "
  javafiles = javafiles + setup.BaseDir + "/../j2v-test/Object.java " 
  javafiles = javafiles + setup.BaseDir + "/../j2v-test/Exception.java " 
  javafiles = javafiles + setup.BaseDir + "/../j2v-test/String.java " 
  javafiles = javafiles + setup.BaseDir + "/../j2v-test/Character.java " 

#
# Iterate over the list of java files in the dirName (except MAIN.java)
# and runs javamain
#
  mainJava = dirName + "/" + "MAIN.java"

  while ok and (testCases != []):
    test = GetNextTest()
    if os.path.abspath(test) != os.path.abspath(mainJava):
      javafiles = javafiles + test + " "

#  javafiles = javafiles + setup.BaseDir+"/../j2v-test/Object.java "
#  javafiles = javafiles + setup.BaseDir+"/../j2v-test/Exception.java "
#  javafiles = javafiles + setup.BaseDir+"/../j2v-test/String.java "

#
#   Run java parser to get CPP`Files
#
  if not ParseJavaFile(javafiles, lang):
    ok=false

  if ok:
    ast = util.ReadFile(parseRes)

  if ok:
    ok=util.WriteFile(testName+".ast", ast)

  if ok:
    convert.CreateDebugFile(dirName, "debug-file-java2vdm", {} )

  if ok:
    ok = convert.CreateArgFile(dirName, "arg-file-java2vdm", {} )

  if ok:
    ok = convert.AppendToDotVdmtest(dirName)

  # Clean up if test case failed
  if not ok and util.CleanFile(ok):
    util.DeleteFiles([testName+".vdm", testName+".ast", testName+".arg",
                      testName+".debug", "debug.arg"])
  
  return ok

#-----------------------------------------------------------------------------
# Run backend for the VDM++ AS value to get
# asv - file with AS` value
# cs - directory where to put hierarchy of VDM++ concrete syntax files
# !!! The current directory must NOT contain other .vpp files
# returns true if no errors occur.
#-----------------------------------------------------------------------------
def AS2CS(asv, cs):
  global backend

  cmd = backend + " -d " + cs + " " + asv
  (exitCode, dummy1, dummy2) = util.RunCommand(cmd,0,"") 
  ok = exitCode==0
  if exitCode != 0:
    return false

  return ok

#-----------------------------------------------------------------------------
# Run test cases for specification.
# fullNames - Names of the directories of java files prepared for running translator
# lang      - The language to run
# coverageFile - name of test coverage file to write to.
# returns true if no errors occur.
#-----------------------------------------------------------------------------
def RunSpecTestCases(fullNames, lang, coverageFile):
  global vdmde, speccompare, backend, javac, java, vppde
  global staticSemanticsOnly, parserOnly

  testvdm="test-j2v.vdm"

  if parserOnly=="yes":
    testvdm="test-javamain.vdm"

  if staticSemanticsOnly=="yes":
    testvdm="test-jss.vdm"

  testvdm=setup.BaseDir+"/../j2v-test/"+testvdm

  cmd = vdmde + " -DP -a -b -R " + coverageFile + " " + testvdm
  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, 
                             "Possible core dump while interpreting specification.",true)
  ok = exitCode==0
  if exitCode != 0:
    report.Error("Toolbox returns an error ", "'" + dummy1 + "'")

  if ok and string.find(dummy1, "Run-Time Error") != -1:
    report.Error("Toolbox returns an run-time error. Look for the latest .arg.err file ", "")

  modules = []

  if parserOnly=="no" and ok:
#
#   Run backend for the VDM++ AS value to get
#   VDM++ concrete syntax 
#
    for fullName in fullNames:
      bn = GetTestName(fullName)
      semResName = bn + ".arg.res"

      if not os.path.exists(semResName):
        continue

      if staticSemanticsOnly=="yes":
        util.CopyFile(semResName, fullName+"/"+semResName)

      report.setTestCaseName(fullName)
      
      modules.append(fullName)
#      
#     The second arg should be directory where to put all 
#     generated .vpp
#
      if staticSemanticsOnly=="no":
        if AS2CS(semResName, os.getcwd()):
  #
  #       Run java to produce expected result
  #
          expRes = os.getcwd() + "/" + GetTestName(fullName) + ".res.exp"
  #
  #       If test directory contains its own MAIN.java, use it,
  #       otherwise, use the generic MAIN.java in the current dir
  #
          j2vutilJava = setup.BaseDir+"/../j2v-test/J2VUTIL.java"
          genericMainJava = setup.BaseDir+"/../j2v-test/MAIN.java"

          mainJava = fullName+"/MAIN.java"
          if not os.path.exists(mainJava):
            mainJava = genericMainJava

          javaFiles = fullName + "/*.java " + mainJava + " " + j2vutilJava + " "
  #
  #       Compile MAIN.java and J2VUTIL.java using
  #       classes of test java files prepared earlier
  #
 
          if not util.IsWindowsOS():
            cmd = javac + " -d " + os.getcwd() + " " + javaFiles
          else:
            cmd = javac + " -classpath ./VDM.jar -d ./ " + javaFiles
          (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, 
                                     "Javac returns errors. ")
          if exitCode != 0:
            break

          if not util.IsWindowsOS():
            cmd = java + " -classpath $CLASSPATH:" + os.getcwd() + " MAIN"
          else:
            cmd = java + " MAIN"
          (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, 
                                     "Java returns errors. ", true)
          if exitCode != 0:
            break

          if not util.WriteFile(expRes, dummy1):        
            break
  #
  #       Run vppde to produce the result itself
  #
          prefix = os.getcwd() + "/" + GetTestName(fullName)
          res = bn + ".res"
          vpparg = "vpp.arg"
  #
  #       Create arg file
  #
          util.WriteFile(vpparg, "new A().Test()")

          objectVpp = setup.BaseDir+"/../j2v-test/Object.vpp"
          vppFiles = ""

          if not util.IsWindowsOS():
            FID=os.popen("find " + os.getcwd() + " -type f -name \*.vpp")
          else:
            FID=os.popen("find ./ -type f -name \*.vpp")
          line = FID.readline()
          while (line != ''):
            vppFiles=vppFiles + line[:-1] + " "
            line = FID.readline()
          vppFiles = vppFiles + " " + objectVpp

          cmd = vppde + " -iD -O " + res + " " + vpparg + " " +vppFiles
          (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, 
                                     "Vppde returns errors. ", true)
          if exitCode != 0:
            report.Error("Vppde returns errors " + fullName, "")

      filesToDelete = []
      FID=os.popen("ls *.vpp *.class 2>/dev/null")
      line = FID.readline()
      while (line != ''):
        filesToDelete.append(line[:-1])
        line = FID.readline()
      util.DeleteFiles(filesToDelete)                 

    okNames = []
#
# Now validate the results
#
  for fullName in fullNames:
    argErr = bn + ".arg.err"
    semResName = bn + ".arg.res"
    resName = bn + ".res"

    expSemResName = fullName + "/" + bn + ".arg.res.exp"
    expResName = bn + ".res.exp"
    report.setTestCaseName(fullName)
    ok = true

    if ok and staticSemanticsOnly=="no" and parserOnly=="no":
      if not os.path.exists(resName):
        report.Error("No result file generated for test case " + fullName, 
                     "Maybe the code generation failed for " +
                     "one of the previous test cases")
        ok = false

    if ok:
      if staticSemanticsOnly=="no" and parserOnly=="no":
        if os.path.exists(expResName):
          ok=resfile.CompareResult(
                                bn, resName,
                                expResName, speccompare, false)
      else:

        if staticSemanticsOnly=="yes" and parserOnly=="no":
          if os.path.exists(semResName):
            ok=resfile.CompareResult(
                                  bn, semResName,
                                  expSemResName, speccompare, false)
        else:
          report.Error("No result file generated for test case " + fullName, 
                       "Look for the latest .arg.err file ")
          ok=false

        if parserOnly=="yes":
          if os.path.exists(argErr):
            errorText = util.ReadFile(argErr)
            if errorText!="":
              report.Error("Parser failure " + fullName, 
                           errorText)


    if ok:
      okNames.append(fullName)

  return (okNames, modules)

#-----------------------------------------------------------------------------
# Run test case for implementation.
# fullName - Name of the original Java file
# lang     - The language to run
# returns true if no errors occur.
#-----------------------------------------------------------------------------
def RunImplTestCase(fullName, lang):
  return RunSpecTestCase(fullName, lang, coverageFile)
  
#-----------------------------------------------------------------------------
# Checks to see if the output from the command was a runtime error.
# resFile  - name of the file containing the result
# returns  - true  - if the output was a runtime error 
#            false - if the output was a runtime error.
#            None  - if the output was not a runtime error.
#-----------------------------------------------------------------------------
def CompareRunTimeError(resFile):
  report.Progress(4,"Searching for runtime error")
  actualResult = util.ReadFile(resFile)
  if actualResult == None:
    return None
  if string.find(actualResult, "Run-Time Error") != -1:
    expectedResult = util.ReadFile(resFile)
    actualResult = string.strip(re.sub("\s+", " ", actualResult))
    expectedResult = re.sub("Run-Time Error[ 0-9]*:", "Run-Time Error ", expectedResult)
    expectedResult = string.strip(re.sub("\s+", " ", expectedResult))
    return actualResult == expectedResult
  else:
    return None

#-----------------------------------------------------------------------------
# Parses Java files, and outputs the AstCI value generated
# testName - a test case directory 
# javafiles - string of java files separated by a space for the test case
# lang     - language
# modules  - list of all the modules in the test case.
#-----------------------------------------------------------------------------
def ParseJavaFile(javafiles, lang):
  javamain = cmdline.LookUp('java2vdm-parser')
  options = " "

  resultFile = parseRes

  cmd = javamain + options + javafiles

  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, "Problem parsing Java File", true)
  ok = (exitCode==0)

  if ok:
    ok = VerifyPresenceOfGeneratedFiles(resultFile)
#
# Find errors in parser output
#
  if ok:
    if string.find(dummy1, "Error detected") != -1:
      ok = false



  if ok:
    ok = VerifyPresenceOfGeneratedFiles(resultFile)

  if not ok:
    report.Error("Parser failed to produce '" + resultFile + "' ", "")

  if not ok:
    report.Error("Parser failed '" + testName + "' ", dummy1)

  return ok

#-----------------------------------------------------------------------------
# Verifies that the correct files was created when code generated.
# fullName - fullName for the test case.
#-----------------------------------------------------------------------------
def VerifyPresenceOfGeneratedFiles(fullName):
  ok = true
  if not os.path.exists(fullName):
    ok=false

  return ok

#-----------------------------------------------------------------------------
# Verifies that the required files are exist
# fullName - fullName for the test case.
#-----------------------------------------------------------------------------
def VerifyPresenceOfRequiredFiles(fullName):
  if not VerifyPresenceOfGeneratedFiles(fullName):
    report.Error("Can not find file '" + fullName + "'")
  else:
    return true

#-----------------------------------------------------------------------------
# Compile all java files found in 'dirName'
# directory recursively (except MAIN.java).
# If Makefile exists in 'dirName' directory, it is used for compilation.
# dirName - the full name of the directory to test
# return - a boolean which indicates whether the compilation went ok.
#-----------------------------------------------------------------------------
def CompileJavaFiles(dirName):
  global javac

  mainJava = dirName+"/MAIN.java"
  makeFile = dirName+"/Makefile"
  javaFiles= ""
#
# If Makefile is provided, use it to compile test java files.
# Otherwise, compile all java files found recursively in dirName
#
  if not os.path.exists(makeFile):
    FID=os.popen("ls `find " + dirName + " -type f -name \*.java`")
    line = FID.readline()
    while (line != ''):
      file=line[:-1]
      if os.path.abspath(file)!=os.path.abspath(mainJava):
        javaFiles=file + " " + javaFiles 
      line = FID.readline()

    classpath = " -classpath $CLASSPATH:" + dirName
    
    cmd = javac + classpath + " -d " + os.getcwd() + " " + javaFiles 
  else:
    cmd = "make -C " + dirName

  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0,
                               "Javac returns errors. ")
  return exitCode==0
