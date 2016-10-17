import sys

### Accepted keywords in the configuration file.
###
###            NOTE: all names in this list must be lower case!!!
###
keywordTypeMap = {
  'general' : ['conf-file', 'what-to-run', 'language', 'run-type',
               'debug-level', 'spec-job-size', 'keep', 'keep-when-error',
               'show-errors-in-progress', 'enable-profiling'],
  'tc'  : [ 'tc-type', 'tc-test-case-dir', 'tc-spec-dir'],
  'ip'  : [ 'ip-type', 'ip-test-case-dir', 'ip-spec-dir'],
  'cpp' : [ 'cpp-test-case-dir', 'cpp-spec-dir', 'cpp-lib-dir'],
  'api' : [ 'api-sl-server', 'api-pp-server', 
            'api-run-interactive', 'api-run-start-toolbox',
            'corba-init-args','server-init-args', 
            'api-test-case-dir', 'api-gen-path', 'api-source-path'],
  'java' : [ 'java-test-case-dir', 'java-spec-dir', 'java-stdlib-dir', 'java-gentestdir'],
  'java2vdm' : ['java2vdm-test-case-dir', 'javacg-code-test-case-dir', 
                'java2vdm-parser', 'java2vdm-spec-dir',
                'java2vdm-backend', 'java2vdm-static-semantics-only',
                'java2vdm-javac-before-parser',
                'java2vdm-java-parser-only', 'java2vdm-test-case'],
  'metaiv' : ['metaiv-test-case-dir'],
  'j2vtf' : ['j2vtf-test-case-dir', 'j2vtf-spec-dir', 'stmt-transform-ids','expr-transform-ids'],
  'pog': ['pog-test-case-dir', 'pog-spec-dir']
}

keywords = {}
for type in keywordTypeMap.keys():
  for keyword in keywordTypeMap[type]:
    keywords[keyword] = type

wildCardKeywords = ['parser', 'interpreter', 'spec-compare', 'code-generator',
                    'compiler', 'cflags', 'extract-source-backend', 'sem-backend', 'rtflags']

wildCardExecutable = wildCardKeywords[:]
wildCardExecutable.remove('cflags')
wildCardExecutable.remove('rtflags')

### Existing test environment (including all those who should come with TB45).
availableTestenv = ['tc', 'ip', 'cpp', 'java', 'api', 'debug', 'parser', 
                    'rose', 'metaiv', 'rtf', 'java2vdm', 'javacg-code', 'j2vtf', 'pog']

availableLanguages = ['sl', 'pp', 'rt', 'java2vdm']

directories = ['tc-test-case-dir', 'ip-test-case-dir', 'cpp-test-case-dir',
               'api-test-case-dir', 'java-test-case-dir', 'java2vdm-test-case-dir',
               'javacg-code-test-case-dir','metaiv-test-case-dir','j2vtf-test-case-dir',
               'pog-test-case-dir']

### Base Directory for powertest
BaseDir = sys.path[0]
