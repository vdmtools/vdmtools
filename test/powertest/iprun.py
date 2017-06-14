import gentestcases, cmdline, util, setup, report, convert, resfile
import os, re, string,sys
true, false = 1,0

#--------------------------------------------------------------------------------
# Execute interpreter test environment.
# lang - language to use (SL, PP, RT)
# type - type of test (either spec or impl)
# return - Nothing.
#--------------------------------------------------------------------------------
def execute(lang, type):
  for dtc in ['dtcon','dtcoff']:
    if cmdline.LookUp('IP-Type')[dtc]:
      report.SetIPType(dtc)
      report.Progress(1, "Running interpreter with " + dtc)

      util.SetProfileBaseName("gmon-ip-"+lang+"-"+type+"-"+dtc+"-"+cmdline.StartDate())
      RegisterExpansionSet(lang, dtc, type)
      if (type == 'spec'):
        executeSpec(lang, dtc)
      else:
        executeImpl(lang, dtc)


#--------------------------------------------------------------------------------
# Execute the interpreter test environment for specifications
# lang - language to use (SL, PP)
# posdef - variant to run (pos, def)
# return - Nothing
#--------------------------------------------------------------------------------
def executeSpec(lang,dtc):
  ok = convert.SetupSpecification(lang, 'ip')
  if not ok:
    report.Error("ABORTING specification test for '" + lang + "'")
    return 

  # counter to indicate progress
  total = 1

  # number of spec. files to run in one vdmde execution
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('ip', lang, 'spec')

  # indicates if any spec test case produced an error.
  anyError = false

  # Initialize coverage file
  coverageFile = "coverage-ip-"+lang+"-"+dtc+"-"+cmdline.StartDate()

  while ( true ):
    testCases = gentestcases.NextTestCases(jobSize)
    if testCases == []:
      break

    startIndex = total
    endIndex = total+len(testCases) -1
    report.Progress(2, "Handling test cases " + str(startIndex) + "..." + str(endIndex))

    # Prepare the next test run - the parameter 'spec-job-size' tells how
    # many testcases should be executed in each run.
    names = []
    util.DeleteFiles([".vdmtest"])

    for name in testCases:

      # prepare test cases.
      ok = PrepareSpecCase(name, lang, dtc)
      anyError = not ok or anyError
      if ok:
        names.append(name)

      # Get ready for next iteration.
      total = total +1

    # Run the test cases
    if names != []:
      report.Progress(3, "Running test cases " + str(startIndex) + "..." + str(endIndex))
      report.setTestCaseName("testcase " + str(startIndex) + "..." + str(endIndex))
      okNames = RunSpecTestCases(names, lang, dtc, coverageFile)
      util.MoveProfile()

    # Clean Up.
    for testName in names:
      ok = (okNames.count(testName) > 0)
      if util.CleanFile(ok):
        baseName = util.ExtractName(testName)
        util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg", baseName+".arg.res",
                          baseName+".debug", baseName+".arg.pt","debug.arg",baseName+".res",
                          baseName+".arg.err", baseName+".arg.msg","rterr.msg", "rterr2.msg"])
      anyError = anyError and ok

  # Clean up .vdmde and .vdmtest
  if util.CleanFile(not anyError):
    util.DeleteFiles([".vdmde",".vdmtest"])
  

#--------------------------------------------------------------------------------
# Execute the interpreter test environment for specifications
# lang - language to use (SL, PP)
# posdef - variant to run (pos, def)
# return - Nothing
#--------------------------------------------------------------------------------
def executeImpl(lang, dtc):
  
  # counter to indicate progress
  total = 1

  # jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('ip', lang, 'impl')
  name = gentestcases.NextTestCase()
  
  while (name != None):
    report.setTestCaseName(name)
    if (total % jobSize) == 1:
      report.Progress(2, "Handling test cases " + str(total) + "..." + str(total + jobSize-1))
    report.Progress(3, "Running " + name)

    ok = PrepareImplCase(name, lang)

    if ok:
      ok = RunImplTestCase(name, lang,dtc)

    if util.CleanFile(ok):
      util.RemoveTestFiles(name,[".vdm",".res",".arg"])
    name = gentestcases.NextTestCase()
    total = total +1
    util.MoveProfile()


#--------------------------------------------------------------------------------
# Prepare a single test case for specification test run.
# name - the full name of the .vdm file to test
# lang - the language to use (SL, PP)
# dtc - variant to run (dtcon, dtcoff)
# return - a boolean which indicates whether the preparation went ok.
#--------------------------------------------------------------------------------
def PrepareSpecCase(fullName, lang, dtc):
  report.Progress(3, "preparing " + fullName)
  report.setTestCaseName(fullName)

  ok = convert.ConvertLanguage(lang, fullName)

  if ok:
    parser = cmdline.LookUpWildCard('ip', lang, 'spec', 'parser', dtc)
    ok = convert.VDM2AST(fullName, parser, false)

  if ok:
    ok = convert.CreateOptionsFile(fullName)


  argStr = CreateArgumentAst(fullName, lang)
  if argStr == None:
    ok = false
    
  if dtc == 'dtcon':
    dtcbool = 'true'
  else:
    dtcbool = 'false'

  if ok:
    convert.CreateDebugFile(fullName, "debug-file-ip", {'<<ARG>>' : argStr, '<<DTC>>' : dtcbool} )

  if ok:
    ok = convert.CreateArgFile(fullName, "arg-file-ip",  {'<<ARG>>' : argStr, '<<DTC>>' : dtcbool} )

  if ok:
    ok = convert.AppendToDotVdmtest(fullName)

  # Clean up if test case failed
  if not ok and util.CleanFile(ok):
    baseName = util.ExtractName(fullName)
    util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg", baseName+".debug"])
        
  return ok


#--------------------------------------------------------------------------------
# Prepare files for running implementation test case
# fullName - name of the test case
# lang     - language to execute
#--------------------------------------------------------------------------------
def PrepareImplCase(fullName, lang):
  ok = convert.ConvertLanguage(lang, fullName)
  return ok


#--------------------------------------------------------------------------------
# Run intepreter for specification test cases.
# fullNames - a list of names of test cases to execute
# lang      - the langauge to execute
# dtc       - 'dtcon' or 'dtcoff'
# coverageFile - name of test coverage file to write to.
#--------------------------------------------------------------------------------
def RunSpecTestCases(fullNames, lang, dtc, coverageFile):
  # remove files we expect output in to.
  for fullName in fullNames:
    util.RemoveTestFiles(fullName, [".arg.pt", ".arg.res", ".arg.err", ".arg.msg"])

  # run the test cases
  interpreter = cmdline.LookUpWildCard('ip', lang, 'spec', 'interpreter', dtc)
  cmd = interpreter + " -a -b -R " + coverageFile
  exitCode = util.RunCommand(cmd, 0, "Possible core dump while interpreting specification.")

  okNames = []
  # Now validate the results
  for fullName in fullNames:
    bn = util.ExtractName(fullName)
    semResName = bn + ".arg.res"
    resName = bn + ".res"
    errName = bn + ".arg.msg"
    report.setTestCaseName(fullName)

    # See if a result file was created
    if not os.path.exists(semResName) and not os.path.exists(errName):
      report.Error("No result or error file generated for test case " + fullName, 
                "Maybe the interpreting toolbox failed for one of the previous test cases")
      continue

    # Find expected result file
    ok = true
    expResultFile = resfile.FindResFile(fullName)
    if expResultFile == None:
      ok = false
  
    if os.path.exists(errName):
      # See if an error file is generated.
      data = util.ReadFile(errName)
      if data == None:
        continue
      # Note the order of the next binary expression ensures that
      # CompareRunTimeError is executed even though no expected result was
      # found! This is necesary as this function generates the result one
      # can copy in place
      ok = CompareRunTimeError(fullName, expResultFile, data) and ok
    else:
      # Strip sem values
      ok = ok and StripSemValue(fullName, lang, dtc)
      
      # validate expected result then
      if ok:
        interpreter = cmdline.LookUpWildCard('ip', lang, 'spec', 'spec-compare', dtc)
        ok = resfile.CompareResult(fullName, resName, expResultFile, interpreter)

    if ok:
      okNames.append(fullName)

  return okNames

#--------------------------------------------------------------------------------
# Run test case for implementation.
# fullName - Name of the original vdm file
# lang     - The language to run
# dtc      - dtcon or dtcoff
# returns true if no errors occur.
#--------------------------------------------------------------------------------
def RunImplTestCase(fullName, lang, dtc):

  # Remove the files we expect output in to
  util.RemoveTestFiles(fullName, [".arg.res"])

  # Run the test
  interpreter = cmdline.LookUpWildCard('ip', lang, 'impl', 'interpreter', dtc)

  localName = util.ExtractName(fullName) + ".vdm"
  outputFile = util.ExtractName(fullName) + ".res"
  argFile = util.ExtractName(fullName) + ".arg"
  logFile = argFile + ".logfile"

  ok = SetupArgFile(fullName,lang)
  if not ok:
    return false

  if dtc == 'dtcon':
    dtccmd = "-DPIQ"
  else:
    dtccmd = ""
  
  cmd = interpreter + " -i " + dtccmd + " -O " + outputFile + " " + argFile + " " + localName
  util.DeleteFiles([outputFile])

  if lang == "rt":
    if(os.path.exists("logfile")):
      os.unlink("logfile")
    
  # Now run the interpreter
  (exitCode, stdout, stderr) = util.RunCommand(cmd, None, None, true)

  if lang == "rt":
    if(os.path.exists("logfile")):
      os.rename('logfile', logFile);
 
  if not os.path.exists(outputFile):
    util.WriteFile(outputFile,stdout);
    exitCode = 1

  expResultFile = resfile.FindResFile(fullName)
  if expResultFile == None:
    ok = false

  if ok:
  
    if exitCode != 0:
      ## Maybe a runtime error occured.

      # Note the order of the next binary expression ensures that
      # CompareRunTimeError is executed even though no expected result was
      # found! This is necesary as this function generates the result one
      # can copy in place
      ok = CompareRunTimeError(fullName, expResultFile, stdout) and ok
      return ok

    # Was output produced?
    if not os.path.exists(outputFile):
      report.Error("'" + cmd + "' didn't produce the expected result file: '" + outputFile + "'",
                   "Either command was malformed, or the interpreter crashed.")
      return false

    interpreter = cmdline.LookUpWildCard('ip', lang, 'impl', 'spec-compare', dtc)
    ok = resfile.CompareResult(fullName, outputFile, expResultFile, interpreter)
    return ok

#--------------------------------------------------------------------------------
# See if 'output' contains a runtime error, and if that is the case, then
# compare that to the expected result.
# fullName      - name of the testcase
# expResultFile - name of the expected result file
# output        - output hopefully containing the expected result.
#--------------------------------------------------------------------------------
def CompareRunTimeError(fullName, expResultFile,  output):
  report.Progress(4, "Searching for run time error.")
  match = re.search("(Run-Time Error *[0-9]+:.*)\"",output, re.M)
  if not match:  
    match = re.search("(Run-Time Error *[0-9]+:.*)",output, re.M)
  if not match:
    report.Error("While searching for runtime error, no runtime error information found for '" + fullName + "'",
                 "Error running interpreter, maybe syntax error")
    return false

  resultFile = util.ExtractName(fullName)+".res"
  
  ok = util.WriteFile(resultFile,match.group(1))
  if not ok:
    return false

  if expResultFile:
    return resfile.CompareResult(fullName, resultFile, expResultFile, None)
  else:
    return false


#--------------------------------------------------------------------------------
# Register expansion set for search for ignore and res files.
#--------------------------------------------------------------------------------
def RegisterExpansionSet(lang, dtc, type):
  expSet = resfile.MakeStdExpansionSet('ip', lang, type)
  if dtc == 'dtcon':
    expSet = resfile.AppendToExpandsionSet(expSet, 'dtcon',['dtcoff'])
  else:
    expSet = resfile.AppendToExpandsionSet(expSet, 'dtcoff',['dtcon'])
  resfile.RegisterExpansionSet(expSet)

#--------------------------------------------------------------------------------
# creates the XXX.arg file. This involves translating it to
# new A().Test() from A`Test()
#--------------------------------------------------------------------------------
def SetupArgFile(fullName,lang):
  argNm = util.StripExt(fullName)+".arg"
  localNm = util.ExtractName(fullName)+".arg"
  if os.path.exists(argNm):
    data = util.ReadFile(argNm)
    if data == None:
      return false
    if lang != 'sl':
      data = re.sub("^(.*)`(.*)\s*$", "new \\1().\\2", data, re.M)
    util.DeleteFiles([localNm])
    return util.WriteFile(localNm, data)
  else:
    argFile = sys.path[0]+"/ip-files/iprun-" + lang + ".arg"
    return util.CopyFile(argFile,localNm)

  
#--------------------------------------------------------------------------------
# The specification requires an AST for the function to be
# interpreted. This function creates this AST from the arg file. There is
# however the constraint that the argument must match the pattern MOD`OP.
#--------------------------------------------------------------------------------
def CreateArgumentAst(fullName, lang):
  info = convert.ExtractArgFileInfo(fullName)
  if info == None:
    return None
  else:
    (modcls, op) = info

  isStatic = false
  if modcls == None:
    isStatic = true
    (modcls, op) = op
  
  if lang == 'sl' or isStatic:
    template = util.GetTemplate("arg-file-ip-arguments-sl")
  elif lang == 'pp' or isStatic:
    template = util.GetTemplate("arg-file-ip-arguments-pp")
  else:
    template = util.GetTemplate("arg-file-ip-arguments-rt")

  data = util.SubString('<<MODORCLASS>>', modcls, template)
  data = util.SubString('<<OPERATION>>', op, data)
  return data

#--------------------------------------------------------------------------------
# Strips sematic values.
# The result of a specification test run is a value packed in a SEM
# value. This function reads the result of a test run and removes the SEM
# record tags, and write it to a .res file.
#--------------------------------------------------------------------------------
def StripSemValue(fullName, lang, dtc):
  interpreter = cmdline.LookUpWildCard('ip', lang, 'spec', 'sem-backend', dtc)

  inNm = util.ExtractName(fullName) + ".arg.res"
  outNm = util.ExtractName(fullName) + ".res"
  cmd = interpreter + " < " + inNm + " > " + outNm
  return util.RunCommand(cmd, 0, "Error while stripping semantic values")
  
