import gentestcases, cmdline, util, setup, report, convert, resfile
import os, string

true, false = 1,0
ext = ""
binext = ""

def execute(lang, type):
  global ext, binext

  #counter to indicate progress
  total = 1
  
  #os type
  if util.IsWindowsOS() and os.environ.get('OSTYPE') == 'win32':
    ext = "cpp"
    binext = ".exe"
  else:
    ext = "cc"
  
  #main() file
  compiler = cmdline.LookUpWildCard('metaiv', lang, type, 'compiler')
  flags = cmdline.LookUpWildCard('metaiv', lang, type, 'cflags')
  MakeDriverAndObjectFiles("driver." + ext, compiler, flags)
  
  #Set expected results  
  expSet = resfile.MakeStdExpansionSet('metaiv', lang, 'impl')
  resfile.RegisterExpansionSet(expSet)
  
  #jobSize is used to give a low level of outputting
  jobSize = cmdline.LookUp('spec-job-size')
  
  #initialize the extraction of test cases
  gentestcases.StartSearch('metaiv', lang, type)
  name = gentestcases.NextTestCase()

  while (name != None):

    #setting report
    report.setTestCaseName(name)
    
    if (total % jobSize) == 1:
      report.Progress(2, "Handling test cases " + `total` + "..." +`total + jobSize - 1`)
    report.Progress(3, "Running " + name)
    
    ok = RunTestCase(name, lang, type)

    #cleaning up
    if ok:  
      if util.CleanFile(ok):
        baseName = util.ExtractName(name)  
        util.DeleteFiles([baseName + ".res"])#, "driver" + binext]) 
    else:
      break

    #generates next test case
    name = gentestcases.NextTestCase()
    
    total = total + 1
  
  #delete the driver and object files at the end
  #util.DeleteFiles(["driver." + ext])  


#-------------------------------------------------------------------------------
# Compiles each test case with the driver program, starts it and checks the 
# result with the expected result file
# Return true if everything works fine
#------------------------------------------------------------------------------- 
def RunTestCase(name, lang, type):
  baseName = util.ExtractName(name)
  
  #compile and run
  compiler = cmdline.LookUpWildCard('metaiv', lang, type, 'compiler')
  flags = cmdline.LookUpWildCard('metaiv', lang, type, 'cflags')
  cmd = compiler + " -o driver driver.cc " + name + " " + flags
  (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, "Problem whith compiling")

  ok = (exitCode == 0)
  if ok:
    if not os.path.exists("driver"):
      report.Error("Driveri binary is not created")

  (exitCode, stdout, stderr) = util.RunCommand("./driver" + binext, 0, "Error running c++ generated c++ binary", true)
  ok = (exitCode == 0)

  #compare results with expected result files
  if ok:
    ok = util.WriteFile(baseName + ".res", stdout)
  else:
    report.Error("Output", None, stdout, stderr)
  if ok:
    resFile = resfile.FindResFile(name)
    ok = (resFile != None)
  if ok:
    resfile.CompareResult(name, baseName + ".res", resFile, None)

  return ok

#--------------------------------------------------------------
# Makes driver file which is a main() file for all test cases.
# Also compiles necesary library files.
#--------------------------------------------------------------~
def MakeDriverAndObjectFiles(fileName, compiler, flags):
  driverFile = open(fileName,"w")
  driverFile.write('extern void metaivtest();\n')
  driverFile.write('int main()\n')
  driverFile.write('{\n')
  driverFile.write('  metaivtest();\n')
  driverFile.write('}\n')
  driverFile.close()
  
  #filesToCompile = ["driver", "~/toolbox/linux/backslash","~/toolbox/linux/m4lib_errmsg","~/toolbox/linux/metaiv"]
  #filesToCompile = ["driver"]
  
  #for fileToCompile in filesToCompile:
  #  if (not os.path.exists(util.ExtractName(fileToCompile) + ".o")):
  #    cmd = compiler + " " + "-c" + " " +  fileToCompile +  "." + ext + " " + flags
  #    (exitCode, dummy1, dummy2) = util.RunCommand(cmd, 0, "Problem whith compiling")

