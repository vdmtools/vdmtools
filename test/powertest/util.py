import cmdline, setup, report, util
#import popen2
import os, re, string, shutil, tempfile, types
true,false = 1,0

templates = {}

#-----------------------------------------------------------------------------
# Returns th extracted name of 'Name': Example /abc/def.hij => def
#-----------------------------------------------------------------------------
def ExtractName(name):
  name = os.path.splitext(os.path.basename(name))[0]
  return name

#-----------------------------------------------------------------------------
# Removes the extension from file
# file - the file name to strip the extension from.
#-----------------------------------------------------------------------------
def StripExt(name):
  return os.path.splitext(name)[0]

def IsWindowsOS():
  return os.environ.get('OS','').lower().startswith('windows')

def UsesVisualStudio():
  return IsWindowsOS() and os.environ.get('OSTYPE') == 'win32'

def getWindowsPath(path):
  if IsWindowsOS():
    cmd = "(readlink -f " + path + " | cygpath -w -m -f -)"
#    (exitCode, stdout, stderr) = RunCommand(cmd, 0, "Path conversion failed.")
#    if exitCode == 0:
#      return stdout.strip()
    output = os.popen(cmd)
    stdout = output.read()
    exitCode = output.close()
    if exitCode == None:
      exitCode = 0
    if exitCode == 0:
      return stdout.strip()
  return path

#-----------------------------------------------------------------------------
# Executes a shell command, and takes care of reporting errors.
# cmd           - the command to execute. This command is executed in an sh shell
#                (this is also true for windows.)
# okExit       - if given, this is the exit code which is seen as an indication
#                of no errors.
# diagnosis    - A diagnosis text, which is written to the error file along
#                with the error message.
# giveMeOutput - If true, both standard outpout and standard error is
#                returned along with the exit code.
# returns      - a 3-tuple, where the first element is the return code of
#                the command, the second and third code is stdout and
#                stderr, if 'giveMeOutput' is true.
#-----------------------------------------------------------------------------
def RunCommand(cmd, okExit=None, diagnosis=None, giveMeOutput = false, expectErr = false):
  report.Progress(4,"Running command: " + cmd)
  stdout = ""
  stderr = None

  if IsWindowsOS():
    cmd = "(" + cmd + ")"
  else:
    cmd = cmd #+ " 2>&1"

  output = os.popen(cmd)
  stdout = output.read()
  exitCode = output.close()
  if exitCode == None:
    exitCode = 0

  if cmdline.LookUp('show-errors-in-progress'):
    if stdout != None and stdout != "":
      report.Progress(1, "*** Standard output\n" + stdout + "\n***")
    if stderr != None and stderr != "":
      report.Progress(1, "*** Standard error\n" + stderr + "\n***")

  if (exitCode != okExit and okExit != None):
    report.Error("Command '" + cmd + "' returned with exit code "+ str(exitCode) + " different from " + str(okExit), diagnosis, stdout, stderr)
  else:
    if stderr != None and stderr != "" and not expectErr:
      report.Error("Command '" + cmd + "' generated the following on standard error", None, None, stderr)

  return (exitCode, stdout, stderr)

#-----------------------------------------------------------------------------
# Deletes a number of files.
# fileList - list of files to delete.
# returns  - nothing
#-----------------------------------------------------------------------------
def DeleteFiles(fileList):
  for filenm in fileList:
    if os.path.exists(filenm) or os.path.islink(filenm):
      try:
        os.unlink(filenm)
      except os.error as err:
        (no, msg) = err
        report.Error("Error when deleting file '" + filenm +"': " + msg)
        
#-----------------------------------------------------------------------------
# Reads the content of a file, and takes care of any error.
# file - the filename fof the file to read.
# returns - the content of the file or None in case of any errors.
#-----------------------------------------------------------------------------
def ReadFile(filenm):
  try:
    FID = open(filenm,"r")
  except IOError as err:
    (no, msg) = err
    report.Error("Couldn't open file '" + filenm + "' for reading: " + msg)
    return None

  try:
    data = FID.read();
  except IOError as err:
    (no, msg) = err
    report.Error("Error when reading data to file '" + filenm +"': " + msg)
    return None

  try:
    FID.close()
  except IOError as err:
    (no, msg) = err
    report.Error("Error when closing file '" + filenm +"': " + msg)
    return None

  return data

#-----------------------------------------------------------------------------
# writes some date to a file, and takes care of any error.
# file - the name of the file to write to.
# data - the data to write.
# mode - the write mode
# returns - true if the data was written to the file without any errors.
#-----------------------------------------------------------------------------
def WriteFile(filenm, data, mode="w"):
  try:
    if mode == "w" and os.path.exists(filenm) or os.path.islink(filenm):
      os.remove(filenm)
  except os.error as err:
    no, msg = err
    report.Error("Error removing file '" + filenm + "': " + msg)
    return false
  
  try:
    FID = open(filenm,mode)
  except IOError as err:
    no, msg = err
    report.Error("Couldn't open file '" + filenm + "' for writting: " + msg)
    return false

  try:
    FID.write(data)
  except IOError as err:
    no, msg = err
    report.Error("Error when writting data to file '" + filenm + "': " + msg)
    return false

  try:
    FID.close()
  except IOError as err:
    no, msg = err
    report.Error("Error when closing file '" + filenm + "': " + msg)
    return false

  return true

def CopyFileIfExists(src, dst):
  if os.path.exists(src):
    CopyFile(src, dst)

def CopyFile(src,dst):
  report.Progress(4,"Copying file '" + src + "' to '" + dst + "'")
  # remove the destination file.
  if os.path.exists(dst) or os.path.islink(dst):
    try:
      os.remove(dst)
    except os.error as err:
      no, msg = err
      report.Error("Error when removing file '" + dst + "': " + msg)
      return false

  try:
    shutil.copy(src,dst)
  except os.error as err:
    no, msg = err
    report.Error("Error when making a symlink from " +  src + " to " + dst + ": " + msg)
    return false

  return true
  
#-----------------------------------------------------------------------------
# Create a symbolic link from 'src' to 'dst'. If 'dst' already exists, then
# it is deleted first. On windows the file is instead copied.
# returns - true if no errors occoured.
#-----------------------------------------------------------------------------
def SymLink(src, dst, force = false):
  report.Progress(4,"Creating symlink from '" + src + "' to '" + dst + "'")
  # remove the destination file.
  if os.path.exists(dst) or os.path.islink(dst):
    try:
      os.remove(dst)
    except os.error as err:
      no, msg = err
      report.Error("Error when removing file '" + dst + "': " + msg)
      return false

  # Normalize path
  # This is not allowed under windows.
  dst = os.path.expanduser(string.strip(dst))
  src = os.path.expanduser(string.strip(src))
  

  # Check if os contain the function symlink (it doesn't under windows!)
  if (os.__dict__.has_key('symlink')) and (not IsWindowsOS() or force):
    try:
      os.symlink(src,dst)
    except os.error as err:
      no, msg = err
      report.Error("Error when making a symlink from '" +  src + "' to '" + dst + "': " + msg)
      return false
  else:
    try:
      shutil.copy(src,dst)
    except os.error as err:
      no, msg = err
      report.Error("Error when making a symlink from '" +  src + "' to '" + dst + "': " + msg)
      return false

  return true

#-----------------------------------------------------------------------------
# Delete files with common basename but different extension.
# fullName  - the basename of the test file (the path is removed from the
#             file!)
# extension - a List of extensions to remove.
# returns   - nothing
#-----------------------------------------------------------------------------
def RemoveTestFiles(fullName, extensions):
  bn = util.ExtractName(fullName)
  files = []
  for ext in extensions:
    files.append(bn+ext)
  util.DeleteFiles(files)

#-----------------------------------------------------------------------------
# Reads a python list from a file
# fileName - the name of the file to read from.
# returns  - None in case of errors, otherwise a python list
#-----------------------------------------------------------------------------
def ReadListFromFile(fileName):
  data = ReadFile(fileName)
  if data == None:
    return None

  try:
    seq = eval(data)
  except:
    report.Error("Data read from file '" + fileName + "' is not Python Sequence (it isn' even a valid Python expression)")
    return None

  if type(seq) != types.ListType:
    report.Error("Data read from file '" + fileName + "' is not a Python sequence")
    return None

  return seq
    
#-----------------------------------------------------------------------------
# Expands %1, %2, ... in 'txt' with arguments from the list 'arguments'
#-----------------------------------------------------------------------------
def ExpandArgs(txt, arguments):
  i = 0
  for arg in arguments:
    i = i + 1
    txt = string.replace(txt,"%" + str(i),arg)
  return txt

#-----------------------------------------------------------------------------
# returns true if a directory containing the test files and generated
# files should be created 
#-----------------------------------------------------------------------------
def GenTestDir():
  return cmdline.LookUp('java-gentestdir')

#-----------------------------------------------------------------------------
# returns true if files should be kept given the return code 'ok'
#-----------------------------------------------------------------------------
def KeepFile(ok):
  return cmdline.LookUp('keep') or not ok and cmdline.LookUp('keep-when-error')

#-----------------------------------------------------------------------------
# returns true if files should be removed given the return code 'ok'
#-----------------------------------------------------------------------------
def CleanFile(ok):
  return not KeepFile(ok)

#-----------------------------------------------------------------------------
# Returns template named 'name'
#-----------------------------------------------------------------------------
def GetTemplate(name):
  global templates
  if not templates.has_key(name):
    templates[name] = util.ReadFile(setup.BaseDir+"/templates/"+name)
  return templates[name]


#-----------------------------------------------------------------------------
# Sets the basename for profiling and creats a directory with that name
#-----------------------------------------------------------------------------
def SetProfileBaseName(nm):
  if cmdline.LookUp('Enable-Profiling'):
    global profileDir, profileCounter
    profileDir = nm
    profileCounter = 0
    try:
      os.mkdir(nm)
    except os.error as err:
      no, msg = err
      report.Error("Error when making directory " + nm + ": " + msg + "\nProfiling disabled")
      profileDir = None
      return false
    util.DeleteFiles(["gmon.out"])
    return true
  else:
    profileDir = None
    return true


#-----------------------------------------------------------------------------
# Move profiling file to profiling directory
#-----------------------------------------------------------------------------
def MoveProfile():
  global profileDir, profileCounter

  if profileDir:
    if os.path.exists("gmon.out"):
      profileCounter = profileCounter +1
      try:
        os.rename("gmon.out",profileDir+"/"+ str(profileCounter))
      except os.error as err:
        no, msg = err
        report.Error("Error when moving gmon.out to " + profileDir+"/"+str(profileCounter) + ": " + msg)
        return false
    else:
      report.Error("gmon.out was not produced, though option 'Enable-Profiling' was enabled")
      return false
  return true
      
#-----------------------------------------------------------------------------
# This is almost like re.sub, but re.sub expands backslashes in 'repl',
# which is a problem, when substituting say an AST into a template.
#-----------------------------------------------------------------------------
def SubString(pattern, repl, template):
  txt = template.split(pattern)
  return string.join(txt,repl)

#------------------------------------------------------------------------------
# This is used to build Make files on the fly in Windows. The VPATH
# directive is used to point to the correct directory.
#------------------------------------------------------------------------------
def BuildMakefile(dirnm):
  cygnusDir = ConvertToCygnus(dirnm)
  vpath = "VPATH="+cygnusDir
  include = "include " + cygnusDir + "/Makefile"
  filename = "GNUmakefile"
  os.system("echo " + vpath + ">  " + filename)
  os.system("echo " + include + ">> " + filename)
  
#------------------------------------------------------------------------------
# Converts Unix paths to cygwin ones. 
# 
#------------------------------------------------------------------------------
def ConvertToCygnus(dirnm):
  pathComponents = dirnm.split("\\")
  drive = pathComponents[0]
  pathComponents = pathComponents[1:]
  colonPos = string.find(drive, ":");
  if colonPos != -1:
    drive = "//" + drive[0:colonPos]
  pathComponents.insert(0, drive)
  return string.join(pathComponents,"/")
  

def MakeDir(dirnm):
  if not os.path.exists(dirnm):
    try:
      os.mkdir(dirnm)
    except os.error as err:
      no, msg = err
      report.Error("Error making directory " + dirnm + ": " + msg)

