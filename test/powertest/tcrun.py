import gentestcases, cmdline, util, setup, report, convert, resfile
import os, re, string
true, false = 1,0

#--------------------------------------------------------------------------------
# Execute type checker test environment.
# lang - language to use (SL, PP, RT)
# type - type of test (either spec or impl)
# return - Nothing.
#--------------------------------------------------------------------------------
def execute(lang, type):
#  if lang == 'rt':
#    report.Progress(3,"Skipping rt for type checker - not applicable")
#    return

  for posdef in ['pos','def']:
    if cmdline.LookUp('TC-Type')[posdef]:
      report.setTypeTestType(posdef)
      report.Progress (1, "Running " + posdef + " test cases")
      util.SetProfileBaseName("gmon-tc-"+lang+"-"+type+"-"+posdef+"-"+cmdline.StartDate())

      RegisterExpansionSet(lang, type, posdef)
      
      if (type == 'spec'):
        executeSpec(lang,posdef)
      else:
        executeImpl(lang,posdef)

#--------------------------------------------------------------------------------
# Execute type checker test environment for specifications
# lang - language to use (SL, PP)
# posdef - variant to run (pos, def)
# return - Nothing
#--------------------------------------------------------------------------------
def executeSpec(lang,posdef):
  ok = convert.SetupSpecification(lang, 'tc')
  if not ok:
    report.Error("ABORTING specification test for '" + langi + "'")
    return 

  # counter to indicate progress
  total = 1

  # number of spec. files to run in one vdmde execution
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('tc', lang, 'spec')

  # indicates if any spec test case produced an error.
  anyError = false

  # Initialize coverage file
  coverageFile = "coverage-tc-"+lang+"-"+posdef+"-"+cmdline.StartDate()

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
      ok = PrepareSpecCase(name, lang, posdef)
      anyError = not ok or anyError
      if ok:
        names.append(name)

      # Get ready for next iteration.
      total = total +1

    # Run the test cases
    if names != []:
      report.Progress(3, "Running test cases " + str(startIndex) + "..." + str(endIndex))
      report.setTestCaseName("testcase " + str(startIndex) + "..." + str(endIndex))
      okNames = RunSpecTestCases(names, lang, posdef, coverageFile)
      util.MoveProfile()

    # Clean Up.
    for testName in names:
      ok = (okNames.count(testName) > 0)
      if util.CleanFile(ok):
        baseName = util.ExtractName(testName)
        util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg", baseName+".arg.res", baseName+".debug", baseName+".arg.pt","debug.arg"])
      anyError = anyError and ok

  # Clean up .vdmde and .vdmtest
  if util.CleanFile(not anyError):
    util.DeleteFiles([".vdmde",".vdmtest"])

#--------------------------------------------------------------------------------
# Execute type checker test environment for specifications
# lang - language to use (SL, PP)
# posdef - variant to run (pos, def)
# return - Nothing
#--------------------------------------------------------------------------------
def executeImpl(lang,posdef):
  
  # counter to indicate progress
  total = 1

  # jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')

  # Initialize the extraction of test cases.
  gentestcases.StartSearch('tc', lang, 'impl')
  name = gentestcases.NextTestCase()

  while (name != None):
    report.setTestCaseName(name)
    if (total % jobSize) == 1:
      report.Progress(2, "Handling test cases " + str(total) + "..." + str(total + jobSize-1))
    report.Progress(3, "Running " + name)

    ok = RunImplTestCase(name, lang, posdef)

    if util.CleanFile(ok):
      bn = util.ExtractName(name)
      util.DeleteFiles([bn+".vdm"])
    name = gentestcases.NextTestCase()
    total = total +1
    util.MoveProfile()


#--------------------------------------------------------------------------------
# Prepare a single test case for specification test run.
# name - the full name of the .vdm file to test
# lang - the language to use (SL, PP)
# posdef - variant to run (pos,def)
# return - a boolean which indicates whether the preparation went ok.
#--------------------------------------------------------------------------------
def PrepareSpecCase(name, lang, posdef):
  report.Progress(3, "preparing " + name)
  report.setTestCaseName(name)

  ok = convert.ConvertLanguage(lang, name)

  if ok:
    parser = cmdline.LookUpWildCard('tc', lang, 'spec', 'parser', posdef)
    ok = convert.VDM2AST(name, parser,false)

  if ok:
    ok = convert.CreateOptionsFile(name)

  if posdef == 'pos':
    posdefStr = '<POS>'
  else:
    posdefStr = '<DEF>'

  if ok:
    convert.CreateDebugFile(name, "debug-file-tc", {'<<POSDEF>>' : posdefStr} )

  if ok:
    ok = convert.CreateArgFile(name, "arg-file-tc",  {'<<POSDEF>>' : posdefStr} )

  if ok:
    ok = convert.AppendToDotVdmtest(name)

  # Clean up if test case failed
  if not ok and util.CleanFile(ok):
    baseName = util.ExtractName(name)
    util.DeleteFiles([baseName+".vdm", baseName+".ast", baseName+".arg", baseName+".debug"])
        
  return ok

#--------------------------------------------------------------------------------
# Execute the vdmde binary with a number of test cases, and verify that they 
# produces an expected result.
# names  - A sequence of full names of the input vdm file.
# lang   - Language to use (SL or PP)
# posdef - Variant to run.
# return - A sequence of full names for all the test cases which failed.
#--------------------------------------------------------------------------------
def RunSpecTestCases(names, lang, posdef, coverageFile):

  # remove files we expect output in to.
  for fullName in names:
    util.RemoveTestFiles(fullName, [".arg.pt", ".arg.res"])

  # run the test cases
  interpreter = cmdline.LookUpWildCard('tc', lang, 'spec', 'interpreter', posdef)
  cmd = interpreter +  " -a -b -I -D -P -R " + coverageFile
  exitCode = util.RunCommand(cmd, 0, "Possible core dump while interpreting specification.", false, true)

  
  okNames = []
  # Now validate the results
  for fullName in names:
    bn = util.ExtractName(fullName)
    resName = bn + ".arg.res"
    report.setTestCaseName(fullName)

    # See if a result file was created
    if not os.path.exists(resName):
      report.Error("No result generated for test case " + fullName, 
                "Maybe the interpreting toolbox failed for one of the previous test cases")
      continue

    # read the result from the result file, and translate it to a list of numbers
    result = TranslateResultSpec(fullName)
    if result == None:
      continue

    # Find the expected result file
    expResName = resfile.FindResFile(fullName)

    if expResName == None :
      if util.KeepFile(false):
        WriteResult(fullName, result)
      continue

    # Validate the result.
    report.Progress(4,"Validating result with result file: '" + expResName + "'")
    ok = ValidateResult(fullName, expResName, result, None, None)
    if ok:
      okNames.append(fullName)
    
    if util.KeepFile(ok):
      WriteResult(fullName, result)

  return okNames

#--------------------------------------------------------------------------------
# Run test case for implementation.
# fullName - Name of the original vdm file
# lang     - The language to run
# posdef   - either pos or def.
# returns true if no errors occur.
#--------------------------------------------------------------------------------
def RunImplTestCase(fullName, lang, posdef):

  ok = convert.ConvertLanguage(lang, fullName)

  # Remove the files we expect output in to
  if ok:
    util.RemoveTestFiles(fullName, [".arg.res"])

  # Run the test
  if ok:
    interpreter = cmdline.LookUpWildCard('tc', lang, 'impl', 'interpreter', posdef)
    if posdef == 'pos':
      defFlg = ""
    else:
      defFlg = " -d "
    localName = util.ExtractName(fullName) + ".vdm"
    resName = util.ExtractName(fullName) + ".arg.res"

    cmd = interpreter + defFlg + " -t -f " + localName + " 2>" + resName

    # Now run the interpreter
    (exitCode, stdout, stderr) = util.RunCommand(cmd, None, None, true, true)

    # Ensure that the interpreter actually did type check anything.
    # That is in contract to if it stoped due to a syntax error or a core dump.
#    if re.search("Type checking [^\n]* done", stdout) == None:
#      report.Error("text 'type check ... done' not found on stdout while running the type checker",
#                   "This might be due to a core dump, a syntax error or the like\n" +
#                   "This does anyway indicate that the type checking was never done",
#                   stdout, stderr)
#      ok = false

  if ok:
    expResName = resfile.FindResFile(fullName)

    # read the result from the result file, and translate it to a list of numbers
#    result = TranslateResultImpl(stdout)
    result = TranslateResultImpl(util.ReadFile(resName))
    if result == None:
      ok = false

  if ok:
    if expResName == None:
      print ("Not validating result (2)")
      if util.KeepFile(false):
        WriteResult(fullName, result)
      ok = false

  if ok:
    # Validate the result.
    report.Progress(4,"Validating result with result file: '" + expResName + "'")
    ok = ValidateResult(fullName, expResName, result, stdout, stderr)

    if util.KeepFile(ok):
      WriteResult(fullName, result)
      
  return ok


#--------------------------------------------------------------------------------
# Reads in file 'fullName' and returns a sequence of error numbers for
# specifications.
# fullName - name of the vdm file, from which the result stems.
#--------------------------------------------------------------------------------
def TranslateResultSpec(fullName):
  bn = util.ExtractName(fullName)
  testName = bn+".arg.res"

  data = util.ReadFile(testName)
  if data == None:
    return None

  return TranslateResultCommon(data)

#--------------------------------------------------------------------------------
# Translate the result from an implementation test case to a sequence of
# numbers of error messages.
# stderr - the output on standard error from an implementation test case.
#--------------------------------------------------------------------------------
def TranslateResultImpl(stderr):
  if stderr == None:
    return None
  else:
    return TranslateResultCommon(stderr)

#--------------------------------------------------------------------------------
# Extract error numers from test runs.
# data   - a text string which represent tha data from a test run.
# result - a sequence of error numbers.
#--------------------------------------------------------------------------------
def TranslateResultCommon(data):
  res = []
  pos = 0
  regexp = re.compile("(Error|Warning)\[([0-9]+)\] :")
  match = regexp.search(data,pos)
  while match:
    number = string.atoi(match.group(2))
    pos = match.end(1)
    res.append(number)
    match = regexp.search(data,pos)
  return res

#--------------------------------------------------------------------------------
# Validates the result of a test run agains the expected result.
# name       - name of the test case, which this result is validated.
#              This information is used when reporting errors.
# expResFile - the name of the expected result file
# result     - result of the test case
# stdout     - stdout for the test run (only used when running on
#              implementation level)
# stderr     - stderr for the test run (only for impl.)
# returns    - true if the result matched.
#--------------------------------------------------------------------------------
def ValidateResult(name, expResFile, result, stdout,stderr):
  expResult = util.ReadListFromFile(expResFile);
  if expResult == None:
    return false

  expResult.sort()
  result.sort()

  if expResult == result:
    return true

  map = {}
  for key in result:
    if map.has_key(key):
      map[key] = map[key] +1
    else:
      map[key] = 1

  for key in expResult:
    if map.has_key(key):
      map[key] = map[key] -1
    else:
      map[key] = -1

  tomuch, tolittle = [],[]
  for key in map.keys():
    if map[key] > 0:
      tomuch.append(str(map[key]) + " * " + str(key))
    elif map[key] < 0:
      tolittle.append(str(abs(map[key])) + " * " + str(key))

  if tomuch != [] or tolittle != []:
    res = ""
    if tomuch != []:
      res = res + "Output not expected: " + string.join(tomuch, ", ") + "\n"
    if tolittle != []:
      res = res + "Output not present: " + string.join(tolittle, ", ") + "\n"
    report.Error("Actual result differs from expected result for " + name, res, stdout, stderr)
    return false

  return true
    
#--------------------------------------------------------------------------------
# Register expansion set for search for ignore and res files.
#--------------------------------------------------------------------------------
def RegisterExpansionSet(lang, type, posdef):
  expansionSet = resfile.MakeStdExpansionSet('tc',lang,type)
  if posdef == 'pos':
    expansionSet = resfile.AppendToExpandsionSet(expansionSet, 'pos',['def'])
  else:
    expansionSet = resfile.AppendToExpandsionSet(expansionSet, 'def',['pos'])
  resfile.RegisterExpansionSet(expansionSet)
  
#--------------------------------------------------------------------------------
# writes result to a file in the working directory.
# fullName - the name of the input test case
# result   - a sequence of numbers to be written to the result file.
#--------------------------------------------------------------------------------
def WriteResult(fullName, result):
  name = util.ExtractName(fullName)+".res"
  util.WriteFile(name, str(result))

