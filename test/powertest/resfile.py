import report, setup, util
import re, os, dircache, string
true, false = 1,0

#--------------------------------------------------------------------------------
#
#                   Description of the Expansion Set
#                   --------------------------------
#
# Several expected result files may exist for a single test case, which one
# to use is determinded by the type of the test case. That is, is it an SL,
# PP or RT language test, is it on specification level or on
# implementation level, is it pos or def mode (specific for the type
# checker), and so on.
# The function  `FindResFile' takes an argument, which is an expansion set,
# to help finding the correct expected result file.. An expandsion set is 
# build using the function `MakeStdExpansionSet' and `AppendToExpandsionSet'.
#
# Technically an expansion set is a seqeunce of tuples, where the first
# element in each tuple is an text allowed in the name of the test case,
# while the second is a list of string not to be found in the correct
# expected result file. (The first element of the tuple is only used as
# part of an error message, in case several expected result files exists.
#
#--------------------------------------------------------------------------------

#--------------------------------------------------------------------------------
# returns a standard expansion set based on testenv type, language, and run type.
# For a description of an expansion see the top of this file.
# env      - the test environment (tc, ip, cpp, ...)
# lang     - the language (SL, PP, RT)
# specimpl - run type (spec, impl)
# returns  - an expansion set
#--------------------------------------------------------------------------------
def MakeStdExpansionSet(env, lang, specimpl):
  res = []

  envs = setup.availableTestenv[:]
  envs.remove(env)
  res.append((env,envs))

  langs = setup.availableLanguages[:]
  langs.remove(lang)
  res.append((lang, langs))

  if specimpl == 'spec':
    res.append(('spec', ['impl']))
  else:
    res.append(('impl', ['spec']))

  return res

#--------------------------------------------------------------------------------
# Appends (pos,neg) to expandsion set `expansionSet'. pos most me a string,
# while neg must be a sequence of string.
# For a description of an expansion see the top of this file.
#--------------------------------------------------------------------------------
def AppendToExpandsionSet(expansionSet, pos, neg):
  expansionSet.append((pos,neg))
  return expansionSet

def RegisterExpansionSet(expansionSet):
  global currentExpansionSet
  currentExpansionSet = expansionSet


#--------------------------------------------------------------------------------
# Finds an appropriate expected result file
# fullname     - the name of the input test case, for which we are searching
#                for an expected result file.
# expansionSet - an expansion set used to guide us to the correct result
#                file. For a description of an expansion see the top of
#                this file.
#--------------------------------------------------------------------------------
def FindResFile(fullName):
  (files,posList) = FindFile(fullName, "res")
  path = os.path.dirname(fullName)
  
  if files == []:
    report.Error("No expected result file found for " + string.join(posList,", "))
    return None
  elif len(files) > 1:
    report.Error("Several possible expected result files found: " + string.join(files, ", "))
    return None
  else:
    report.Progress(4,"Expected result file is: " + path + "/" + files[0])

  return path + "/" + files[0]

#--------------------------------------------------------------------------------
# Returns true if there exists an ignore file for fullName
#--------------------------------------------------------------------------------
def HasIgnoreFile(fullName):
  (files,posList) = FindFile(fullName, "ignore")
  path = os.path.dirname(fullName)
  if files == []:
    return (false, false)
  else:
    report.Progress(4,"Ignore files found: "  + path + "/" + files[0])
    silence = (string.find(files[0],"-silently") != -1)
    return (true, silence)

#--------------------------------------------------------------------------------
# Returns file matches expansionSet registered with RegisterExpansionSet.
#--------------------------------------------------------------------------------
def FindFile(fullName, ext):
  path = os.path.dirname(fullName)
  bn = util.ExtractName(fullName)

  files = dircache.listdir(path)
  files = files[:] # The result of dircach must not be edited!

  # remove filenames ending with ~
  files = RemoveNonMatchingFiles(files, "~$", true)

  # remove every file, which do not match the base name
  files = RemoveNonMatchingFiles(files, "^"+bn+"."+ext, false)

  posList = []
  for (pos,negList) in currentExpansionSet:
    posList.append(pos)
    for neg in negList:
      files = RemoveNonMatchingFiles(files,"-" + neg , true)
  return (files, posList)
  
#--------------------------------------------------------------------------------
# Removes files from a list of files based on a regular expression.
# files - a list of files.
# regexp - the regular expression which every file is matched with
# removeIfMatch - true if the files should be removed if they match.
#--------------------------------------------------------------------------------
def RemoveNonMatchingFiles(files, regexp, removeIfMatch):
  reg = re.compile(regexp)
  res = []
  for file in files:
    if reg.search(file) != None:
      keep = not removeIfMatch
    else:
      keep = removeIfMatch

    if keep:
      res.append(file)

  return res

#--------------------------------------------------------------------------------
# Compares result for a test case with expected result. First the result
# and expected result are compare letter for letter. If this fails, then
# the a vdm value is build, which compares the actual and expected result.
#
# fullName - the name of the test case
# outputFile - the file containing the actual output
# resFile - the file containing the expected result
# interpreter - interpreter to use to compare vdm values.
#--------------------------------------------------------------------------------
def CompareResult(fullName, outputFile, resFile, interpreter, structTest=true):

  # compare the results using normal "diff"
  actualResult = util.ReadFile(outputFile)
  expectedResult = util.ReadFile(resFile)
  
  # Remove duplicate white spaces and line breaks, spaces around commas and parenthesis.
  actualResult = string.strip(re.sub("\s+", " ", actualResult))
  expectedResult = string.strip(re.sub("\s+", " ", expectedResult))
  actualResult = string.strip(re.sub("\s*,\s*", ",", actualResult))
  expectedResult = string.strip(re.sub("\s*,\s*", ",", expectedResult))
  actualResult = string.strip(re.sub("\s*\(\s*", "(", actualResult))
  expectedResult = string.strip(re.sub("\s*\(\s*", "(", expectedResult))
  actualResult = string.strip(re.sub("\s*\)\s*", ")", actualResult))
  expectedResult = string.strip(re.sub("\s*\)\s*", ")", expectedResult))

  if actualResult == expectedResult:
    return true

  # Expected Result is Run Time Error 
  if expectedResult.startswith("Run-Time Error"):
    report.Error("Actual result is different from expected result for " + `fullName`,
                 "expected result : " + expectedResult + "\n" +
                 "actual result   : " + actualResult)
    return false

  if expectedResult.startswith("<UTIL.NumberToInt>:") or actualResult.startswith("<UTIL.NumberToInt>:"):
    report.Error("Actual result is different from expected result for " + `fullName`,
                 "expected result : " + expectedResult + "\n" +
                 "actual result   : " + actualResult)
    return false

  if expectedResult.startswith("The construct is not supported:") or actualResult.startswith("The construct is not supported:"):
    report.Error("Actual result is different from expected result for " + `fullName`,
                 "expected result : " + expectedResult + "\n" +
                 "actual result   : " + actualResult)
    return false

  report.Progress(4, "Comparing result for " + `fullName` + " using diff method")
  # Hmmm we need to try to compare using VDMDE then.
  if structTest and interpreter != None:
    report.Progress(4, "Comparing result for " + `fullName` + " by build VDM value")
    template = util.GetTemplate("compare-result")
    if template == None:
      return false

    data = util.SubString('<<EXPECTED_RESULT>>', expectedResult, template)
    data = util.SubString('<<ACTUAL_RESULT>>', actualResult, data)
    ok = util.WriteFile("compare.vdm", data)
    if not ok:
      return false

    util.CopyFile(setup.BaseDir+"/general-files/compare.arg","compare.arg")
    cmd = interpreter + " -i compare.arg compare.vdm"
    (exitcode, dummy1, dummy2) = util.RunCommand(cmd)
    ok = (exitcode == 0)
  else:
    ok = false

  if not ok:
    report.Error("Actual result is different from expected result for " + `fullName`,
                 "expected result : " + expectedResult + "\n" +
                 "actual result   : " + actualResult)
    
  if util.CleanFile(ok):
    util.DeleteFiles(["comapre.arg"])
  return ok

  
