" Vim syntax file
" Language:     VDM
" Maintainer:   Rui Carlos A. Goncalves <rcgoncalves.pt@gmail.com>
" Last Change:  July 20, 2007
"
" Version:      2.6
" Url:          http://rcgoncalves.net/files/vdm.zip

if version < 600
  syn clear
elseif exists("b:current_syntax")
  finish
endif

" This language is case sensitive
syn case match

" Keywords
syn keyword     vdmPart         exports imports definitions types functions operations values from renamed uselib struct traces
syn keyword     vdmType         bool int nat nat1 real rat char token
syn keyword     vdmBool         true false
syn keyword     vdmConstant     nil
syn keyword     vdmConditional  if then else elseif
syn keyword     vdmStatement    let def be st
syn keyword     vdmStatement    cases others 
syn keyword     vdmStatement    pre post inv ord eq
syn keyword     vdmStatement    compose
syn keyword     vdmStatement    state init
syn keyword     vdmStatement    forall exists exists1 iota
syn keyword     vdmStatement    lambda
syn keyword     vdmStatement    undefined skip
syn keyword     vdmStatement    dcl
syn keyword     vdmStatement    ext rd wr errs
syn keyword     vdmStatement    while do by reverse
syn keyword     vdmStatement    return
syn keyword     vdmStatement    always trap with tixe exit
syn keyword     vdmStatement    error
syn keyword     vdmStatement    static public private protected new
syn keyword     vdmStatement    sync per thread periodis threadid sporadic
syn keyword     vdmStatement    self
syn keyword     vdmStatement    atomic
syn keyword     vdmFunction     mu
syn keyword     vdmFunction     not and or
syn keyword     vdmFunction     abs floor div mod rem
syn keyword     vdmFunction     union inter subset psubset card dunion dinter power
syn keyword     vdmFunction     hd tl len elems inds conc
syn keyword     vdmFunction     dom rng munion merge comp inverse
syn keyword     vdmFunction     isofbaseclass isofclass samebaseclass sameclass
syn keyword     vdmFunction     start startlist stop stoplist

" Delimiters
syn match       vdmDelimiter    "(\|)\|\[\|\]\|{\|}\|,"

" Operators
syn match       vdmOperator     "\.\|:\|::\|->\|+>\|==\|=>\|<=>\|=\|<>\|+\|-\|*\|/\|*\*\|<\|>\|<=\|>=\|&\||\|\^\|\\\|++\||->\|<:\|<-:\|:>\|:->\|.#\|:-\|==>\|:=\|||"

" Strings and constants
syn match       vdmNumber       "\<[0-9]\+\>"
syn match       vdmFloat        "\<[0-9]\+\.[0-9]\+\([eE][-+]\=[0-9]\+\)\=\>"
syn match       vdmConstant     "<[a-zA-Z][a-zA-Z0-9_']*>"
syn match       vdmSpecialChar  "\\\([rntfea\"\\']\|x[0-9a-fA-F][0-9a-fA-F]\|c.\|[0-7][0-7][0-7]\)" contained
syn match       vdmCharacter    "[^a-zA-Z0-9_']'\([^\\]\|\\[^']\+\|\\'\)'"lc=1 contains=vdmSpecialChar
syn match       vdmCharacter    "^'\([^\\]\|\\[^']\+\|\\'\)'" contains=vdmSpecialChar
syn region      vdmString       start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=vdmSpecialChar

" Other types
syn match       vdmType         "\<\(set\|set1\|seq\|seq1\) *of\>"
syn region      vdmNormalMap1   matchgroup=vdmType start="\<\(map\|inmap\)\>" end="\<to\>" contains=vdmType,vdmNormalMap2,vdmStatement,vdmOperator,vdmConstant
syn region      vdmNormalMap2   matchgroup=vdmType start="\<\(map\|inmap\)\>" end="\<to\>" contains=vdmType,vdmNormalMap1,vdmStatement,vdmOperator,vdmConstant contained

" Other statements
syn match       vdmStatement    "\<end\>"
syn match       vdmStatement    "\<of\>"
syn match       vdmStatement    "\<in\>"
syn match       vdmStatement    "\<is *not *yet *specified\>"
syn match       vdmStatement    "\<to\>"
syn match       vdmStatement    "\<\(for *all\|for\)\>"

" Other functions
syn match       vdmFunction     "\<in *set\>"
syn match       vdmFunction     "\<in *seq\>"
syn match       vdmFunction     "\<\(mk_[a-zA-Z][a-zA-Z0-9_'`]*\|mk_\)\>"
syn match       vdmFunction     "\<\(is_[a-zA-Z][a-zA-Z0-9_'`]*\|is_\)\>"
syn match       vdmFunction     "\<init_[a-zA-Z][a-zA-Z0-9_']*\>"
syn match       vdmFunction     "\<inv_[a-zA-Z][a-zA-Z0-9_']*\>"
syn match       vdmFunction     "\<\(pre_[a-zA-Z][a-zA-Z0-9_']*\|pre_\)\>"
syn match       vdmFunction     "\<post_[a-zA-Z][a-zA-Z0-9_']*\>"
syn match       vdmFunction     "\(#act\|#fin\|#active\|#req\|#waiting\)\>"
syn match       vdmFunction     "\<obj_[a-zA-Z][a-zA-Z0-9_']*\>"

" Others
syn match       vdmPart         "\<all\>"
syn match       vdmPart         "\<class\>"
syn match       vdmPart         "\<\(module\|dlmodule\)\>"
syn match       vdmPart         "\<instance *variables"
syn match       vdmPart         "\<is *subclass *of\>"
syn match       vdmPart         "\<end[\ \r\n]*[a-zA-Z][a-zA-Z0-9_']*[\ \r\n]*module\>"he=s+3,me=e-6
syn match       vdmPart         "\<end[\ \r\n]*[a-zA-Z][a-zA-Z0-9_']*[\ \r\n]*dlmodule\>"he=s+3,me=e-8
syn match       vdmPart         "\<end[\ \r\n]*[a-zA-Z][a-zA-Z0-9_']*[\ \r\n]*\%$"he=s+3

" Comments

syn region      vdmComment start="/\*" end="\*/" contains=vppString
syn region      vdmComment start="\(/\%^\|\\end{vdm_al}\)" end="\(\\begin{vdm_al}\|/\%$\)" contains=vppString

syn match       vdmLineComment  "--.*"

hi def  link    vdmOperator     Operator
hi def  link    vdmPart         PreProc
hi def  link    vdmType         Type
hi def  link    vdmStatement    Statement
hi def  link    vdmConditional  Conditional
hi def  link    vdmLineComment  Comment
hi def  link    vdmComment      Comment
hi def  link    vdmSpecialChar  SpecialChar
hi def  link    vdmString       String
hi def  link    vdmCharacter    Character
hi def  link    vdmFloat        Float
hi def  link    vdmNumber       Number
hi def  link    vdmBool         Boolean
hi def  link    vdmConstant     Constant
hi def  link    vdmFunction     Function
