import gentestcases, cmdline, util, setup, report, convert, resfile
import os, string

true, false = 1,0

def execute(lang, type):
  global ext, binext

#  ok = convert.SetupSpecification(lang, 'j2vtf')
#  if not ok:
#    report.Error("ABORTING specification test for " + `lang`)
#    return 

  #counter to indicate progress
  total = 1
  
  #setting the coverage file
  coverageFile = "rtinfo.ast"
  
  #Set expected results  
  expSet = resfile.MakeStdExpansionSet('j2vtf', lang, type)
  resfile.RegisterExpansionSet(expSet)
  
  #jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')
  
  #initialize the extraction of test cases
  gentestcases.StartSearch('j2vtf', lang, type)
  name = gentestcases.NextTestCase()

  while (name != None):

    #setting report
    report.setTestCaseName(name)
    
    if (total % jobSize) == 1:
      report.Progress(2, "Handling test cases " + `total` + "..." +`total + jobSize - 1`)
    report.Progress(3, "Running " + name)
    
    ok = convert.ConvertLanguage(lang, name)

    if ok:
      ok = PrepareSpecCase(name, lang)
    
    if ok:
      ok = RunTestCase(name, lang, type, coverageFile)

    #cleaning up
    if ok:  
      if util.CleanFile(ok):
        baseName = util.ExtractName(name)  
        util.DeleteFiles([baseName + ".vdm", baseName + ".ast", baseName + ".arg", baseName + ".debug", baseName + ".res",  baseName + ".arg.res",  baseName + ".arg.pt", "debug.arg" ]) 
    #else:
    #  break

    #generates next test case
    name = gentestcases.NextTestCase()

    total = total + 1

  #util.DeleteFiles([".vdmde", ".vdmtest"])
#---------------------------------------------------------------------------
# Preparing the test case
#---------------------------------------------------------------------------

def PrepareSpecCase(name, lang):
  
  report.Progress(3, "Preparing " + name)
  report.setTestCaseName(name)

  parser = cmdline.LookUpWildCard('j2vtf', lang, 'spec', 'parser')
  ok = convert.VDM2AST(name, parser, false)

  if ok:
    ok = convert.CreateOptionsFile(name)

  if ok:
    extra = GetIdSet()
    ok = convert.CreateDebugFile(name, "debug-file-j2vtf", extra)
  
  if ok:
    ok = convert.CreateArgFile(name, "arg-file-j2vtf", extra)
    
  
  util.DeleteFiles([".vdmtest"])
  
  if ok:
    ok = convert.AppendToDotVdmtest(name)


  # Clean up if preparation failed
  if not ok and util.CleanFile(ok):
    baseName = util.ExtractName(name)
    util.DeleteFiles([baseName + ".arg", baseName + ".ast", baseName + ".debug", "debug.arg"])
 
  return ok

#-------------------------------------------------------------------------------
# Compiles each test case with the driver program, starts it and checks the 
# result with the expected result file
# Return true if everything works fine
#------------------------------------------------------------------------------- 
def RunTestCase(name, lang, type, coverageFile):

  stdout = None
  stderr = None
  
  baseName = util.ExtractName(name)
  report.setTestCaseName(name)
  ok = true

  #run the test cases
  cmd = cmdline.LookUpWildCard('cpp', lang, 'spec', 'interpreter')
  cmd = cmd + " -D -P -a -b -R " + coverageFile
  exitCode = util.RunCommand(cmd, 0, "Possible core dump while interpreting specification.")

  # See if a result file was created
  if ok:
    if not os.path.exists(baseName + ".arg.res"):
      report.Error("No result file generated for test case " + name, "Maybe it failed before")
      ok = false
  
  if ok:
    resFile = resfile.FindResFile(name)
    ok = (resFile != None)
  if ok:
    ok = resfile.CompareResult(name, baseName + ".arg.res", resFile, None)

  return ok

def GetIdSet():

  stmtids = cmdline.LookUp('stmt-transform-ids')
  exprids = cmdline.LookUp('expr-transform-ids')
  stmtidset = '{mk_XF`SXFId(a)|a in set ' + stmtids + ' }'
  expridset = '{mk_XF`EXFId(a)|a in set ' + exprids + ' }'
  idunion = stmtidset + ' union ' + expridset
  return { '<<SID>>' : idunion }
