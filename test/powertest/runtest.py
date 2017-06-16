import cmdline, util, setup, report
# Under WinNT some modules imported by apirun are missing! M.Lebedev
import apirun
import tcrun,iprun,cpprun,javarun,        debugrun,parserrun,roserun,metaivrun,rtfrun,java2vdmrun,j2vtfrun,pogrun

def execute():
  for env in setup.availableTestenv:
    if cmdline.RunTestEnv(env):

      report.SetTestEnvName(env)
      for lang in setup.availableLanguages:

        if cmdline.LookUp('Language')[lang]:
          
          report.SetLanguage(lang)
          for type in ['spec', 'impl']:

            if cmdline.LookUp('Run-Type')[type]: 

              report.SetRunType(type)
              report.Progress(1, "Running " +env + " - "+ type + ". test case for " + lang)

              if (env == 'tc'):
                tcrun.execute(lang, type)
              elif (env == 'ip'):
                iprun.execute(lang, type)
              elif (env == 'cpp'):
                cpprun.execute(lang, type)
              elif (env == 'java'):
                javarun.execute(lang, type)
              elif (env == 'api'):
                apirun.execute(lang, type)
              elif (env == 'debug'):
                debugrun.execute(lang, type)
              elif (env == 'parser'):
                parserrun.execute(lang, type)
              elif (env == 'rose'):
                roserun.execute(lang, type)
              elif (env == 'metaiv'):
                metaivrun.execute(lang, type)
              elif (env == 'rtf'):
                rtfrun.execute(lang, type)
              elif (env == 'java2vdm'):
                java2vdmrun.execute(lang, type)
              elif (env == 'j2vtf'):
                j2vtfrun.execute(lang, type)
              elif (env == 'pog'):
                pogrun.execute(lang, type)

