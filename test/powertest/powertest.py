import sys
import cmdline, runtest, report

cmdline.Init(sys.argv)
report.Init()
runtest.execute()
report.genReport()

