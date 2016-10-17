#! /usr/bin/perl
# Next two lines make this script work on strange OS. 
eval 'exec perl -S $0 ${1+"$@"}'
  if $running_under_some_shell;

while (<>) {
    if (/^yyparse\(YYPARSE_PARAM_ARG\)/) {
	$parsefunction = $_; 
	# Hack: make start of parse function into prototype, because
	# its return value was on the previous line and we need to 
	# insert stuff before the function. 
	print $_, ";\n"; 
	last; 
    } else {
	print; 
    }
}
# Before yyNparse function. 
# build case function. 
# Look for case N: ... break; 

$function = "int " . $_; 

while (<>) {
    if (/^case ([0-9]+):/) {
	last; 
    } else {
	$function .= $_; 
    }
}
while (1) {
    if (/^case ([0-9]+):/) {
	$function .= $_; 
	$switchnumber = $1; 
	$_ = <>; 
    } else {
	if (/^\#line/ || /^\}$/) {
	    # In the last 'case'
	    $function .= $_; 
	    last; 
	} else {
	    print STDERR "Expected \#line in line: $. got: '$_'"; 
	    exit(1); 
	}
    }
    # print STDERR "Read: $_"; 
    if (/#line/) {
	$function .= $_;  
	$_ = <>; 
    } else {
	print STDERR "Line $. : expected \#line. got $_\n"; 
	exit(0); 
    }
    $body = ""; 
  body: 
    while (1) { 
	# print STDERR "body = $_"; 
	$body .= $_; 
	$_ = <>; 
	if (s/^(\s+break;)\}/$1/) {
	    $fnumber = $switchnumber;  
	    $function .= "  frule$fnumber (yyvsp, yylsp, yyval); \n" . $_; 
	    $rule .= "static void frule$fnumber (register YYSTYPE *yyvsp, YYLTYPE *yylsp, YYSTYPE & yyval)  $body } \n";
            # print STDERR "Found break\n"; 
	    last body; 
	}
    } 
    # print STDERR "Body = '$body'"; 
    # after "break"
    $_ = <>; 
} 

print $rule; 
print $function; 
while (<>) {
    print; 
}

