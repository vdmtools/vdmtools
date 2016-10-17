#! /usr/bin/perl

local $/; 
undef $/; 
$file = <>; 
$search = "yyerrlab:";

$new = "$search
  get_repair().SetFirstPos(yylloc);
  get_repair().RecoverStart(yyss,yyssp, yychar,yystacksize);
  return 1;
"; 
 
$search_re = quotemeta($search); 
if ($file !~ s/^$search_re/$new/msx) {
    die "$0: pattern not found, "; 
} 
print $file; 

