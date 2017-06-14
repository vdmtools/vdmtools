#-----------------------------------------------------------------------------
# Global variables
# files       - seperate test case files.
# directories - test case directories
# FID         - FID for reading files found in the directories 
# ignoreList  - list of patterns for files to ignore.
# leftOver    - left over files from previous run. This is to ensure that
#               no two test cases with same base name is executed in the same run.
#-----------------------------------------------------------------------------

import os, string
import cmdline, util, report, setup, resfile
true, false = 1,0

#-----------------------------------------------------------------------------
# Initiates the search for test cases.
# 'env' indicates which test case directories should be searched. It should
# be one of setup.availableTestenv 
# 'lang' and 'type' is used to ignore some of the test cases in the
# subdirectories.
# 'lang' is one of SL and PP and 'type' is one of SPEC or IMPL
#-----------------------------------------------------------------------------
def StartSearch(env, lang, type):
  global files, directories, FID, ignoreList, leftOver, actualEnv

  actualEnv = env

  testCases = cmdline.GetTestCases(env)

  files = []
  directories = []

  for testCase in testCases:
    print (testCase)
    if os.path.isfile(testCase):
      files.append(testCase)
    else:
      directories.append(testCase)

  ignoreList = []
  if (lang == 'sl'):
    ignoreList.append('/PP/')
    ignoreList.append('/VICE/')
  elif (lang == 'pp'):
    ignoreList.append('/SL/')
    ignoreList.append('/VICE/')
    ignoreList.append('/IMPLMOD/')
  else:
    ignoreList.append('/SL/')
    ignoreList.append('/IMPLMOD/')
    
  if (type == 'spec'):
    ignoreList.append("/IMPL/")
  else:
    ignoreList.append("/SPEC/")

  envs = setup.availableTestenv[:]
  envs.remove(env)
  for env in envs:
    ignoreList.append("/" + string.upper(env) + "/")
  
  ignoreList.append("/CVS/")
  FID = None
  leftOver = None

#-----------------------------------------------------------------------------
# Returns the next set of test cases or an empty sequence if no more test
# cases are available. No more than 'max' cases are returned. On the other
# hand fewer cases might be returned in case "the next test case" would
# have the same base name as an existing one.
#-----------------------------------------------------------------------------
def NextTestCases(max):
  global leftOver
  count = 0
  cases = []
  baseNames = []

  if leftOver != None:
    cases = [leftOver]
    count = count +1
    leftOver = None

  while (count < max):
    case = NextTestCase()
    if case == None:
      return cases

    bn = os.path.basename(case)
    if baseNames.count(bn) > 0:
      leftOver = case
      return cases
    else:
      cases.append(case)
      baseNames.append(bn)
      count = count +1

  return cases
    
#-----------------------------------------------------------------------------
# Returns the next test case. (This function accept the .ignore files)
# returns - None if all test cases has been returned, otherwise the next
# test case.
#-----------------------------------------------------------------------------
def NextTestCase():

  while true:
    case = NextTestCaseIgnoreIgnoreFile()
    if case == None:
      return None

    again = false
    for pat in ignoreList:
      if (string.find(case, pat) != -1):
        again = true
        break
    if again:
      report.Progress(4,"Skipping case '" + case + "', since it is in the ignore list.")
    else:
      (ignore, silence) = resfile.HasIgnoreFile(case)
      if ignore:
        # We should ignore this file.
        if not silence:
          report.IgnoreFile(case)
      else:
        report.IncrTestcaseCounter()
        return case

#-----------------------------------------------------------------------------
# Returns the next test case (Ignores the .ignore file)
#----------------------------------------------------------------------------- 
def NextTestCaseIgnoreIgnoreFile():
  global files, directories, FID

  if files != []:
    file = files[0]
    files = files[1:]
    return file
  elif (FID == None):
    if directories != []:
      dir = directories[0]
      directories = directories[1:]
      if actualEnv == 'api':
        FID=os.popen("find " + dir + " -type f -name \*.api")
      elif actualEnv == 'java2vdm':
        FID=os.popen("find " + dir + " -type f -name \*.java")
      elif actualEnv == 'metaiv':
        FID=os.popen("find " + dir + " -type f -name \*.cc") 
      else:
        FID=os.popen("find " + dir + " -type f -name \*.vdm")

      return NextTestCaseIgnoreIgnoreFile()
    else:
      return None
  else:
    line = FID.readline()
    if (line == ''):
      FID = None
      return NextTestCaseIgnoreIgnoreFile()
    else:
      return line[:-1]
