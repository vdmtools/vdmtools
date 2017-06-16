import os, re, string

def Id2Name(id):
  if re.search("StmtTransformId\( 1 \)", id):
    return " whileIfTestTrue "
  elif re.search("StmtTransformId\( 2 \)", id):
    return " ifToCases "
  elif re.search("StmtTransformId\( 3 \)", id):
    return " ifToEquiv "
  elif re.search("StmtTransformId\( 4 \)", id):
    return " nestedifsNoElses "
  elif re.search("StmtTransformId\( 5 \)", id):
    return " isMapCompLoop "
  elif re.search("StmtTransformId\( 6 \)", id):
    return " ifTestTrue "
  elif re.search("StmtTransformId\( 7 \)", id):
    return " ifTestFalse "
  elif re.search("StmtTransformId\( 8 \)", id):
    return " isRedundantIfBlock "
  elif re.search("StmtTransformId\( 9 \)", id):
    return " ifToAnd"
  elif re.search("OpTransformId\( 1 \)", id):
    return " isRedundantDcl "
  elif re.search("ExprTransformId\( 1 \)", id):
    return " isRedundantDef "
  elif re.search("ExprTransformId\( 2 \)", id):
    return " isRenamingDef "
  elif re.search("ExprTransformId\( 3 \)", id):
    return " orToNotEquiv "
  else:
    return "ERROR\n"

resFile = open("result", "w")
for f in os.listdir(os.getcwd()):
  if f[len(f) - 7:] == "arg.res":
    tmpFile = open(f, 'r')
    resFile.write(f[:len(f) - 8] + " : \n  " )
    lineList = tmpFile.readlines()
    transformIdLine = ""
    i = 0
    while i < len(lineList):
      line = lineList[i]
      if re.search("mk_J2VX", line):
        transformIdLine = line
      if re.match('  (\{| ) [0-9]+ |->', line):
        resFile.write(Id2Name(transformIdLine))
      i = i + 1
    resFile.write('\n\n')
    tmpFile.close()  
resFile.close()

