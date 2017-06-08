#! /usr/bin/perl
#
# Denne konverter fra IFAD VDM-SL tex til Html-kode er lavet af
# Jesper Kjær Pedersen (URL : http://www.imada.ou.dk/~blackie/)
#
# Der må ikke være nogle linier i input filen, der indeholder tex
# kommentare. dvs %
#
# Følgende er en list over hvad dette program forventer af tex dokumentet:
#
# o regler starter med \Rule, og må ikke afsluttes på samme linie
#   som de startes.
# o Der må ikke være blanke linier i en regel (Disse bruges til at checke
#   at jeg ikke har overset noget)
# o Der må ikke være %-tegn på samme linie som \Rule
# o Hvis konstruksioner som fx. {\bf ...} bruges skal dette med i funktionen
#   clip.

$infile = shift; 
$outfile = shift; 
open (TEX, "<$infile") || die "Couldn't open $infile: $!";
open (OUTPUT, ">$outfile") || die "Couldn't open $outfile: $!";

&printInit();
&readUntilRules();
while (&readRule()) {
    &printRule();
}
&printExtra();
&printEnd();
close TEX;
close OUTPUT;
&testReferences();

############################################################
# This function read from the file, until the start 
# of the bnf diagram.
############################################################
sub readUntilRules {
    $found = 0;
    while (<TEX>) {
        if (/^% Start Of Rules/) {
            $found = 1;
            last;
        }
    }
    if (!$found) {
        die "Couldn't find start of bnf diagram, might be wrong pattern in readUntilRules function";
    }
}

                

############################################################
# This function read a single rule from the file
############################################################
sub readRule {
    $ruleBody = "";

    while (($line = <TEX>) !~ /^\s*\\Rule/) { # reads the next rule.
	if ($line eq "") {
	    return 0;
	}
	chop $line;
	if ($line =~ /\\Rule/ && $line !~ /^%\\Rule/) {
	    warn "There were a \\Rule in the midle of the line: \"$line\", please correct this.";
	}
	if ($line =~ /section\{([^\}]*)\}/) {
	    $pre = "<hr><h1>$1</h1>\n";
	}
    }
    $found = ($line =~ /^\s*\\Rule\{\s*(.*)\}\{/);
    if (!$found) {
	die "Couldn`t parse \"$line\"";
    }
    if ($line =~ /%/) {
        die "can't handle comment on the same line as the \\Rule, line = \"$line\"";
    }
    $ruleName = $1;

    # læser den del af reglen som står på samme linie som reglen selv.
    if ($line =~ /^\s*\\Rule\{.*\}\{\s*([^\s].*)$/) {
	$ruleBody = $1;
    }

    $brace = 1;
    $char == "";
    
    ### læser sålænge at vi ikke har mødt slut tuborgen til kroppen af 
    ### programmet.
    while ($brace != 0) {
	$ruleBody = $ruleBody." ";
	$line = <TEX>;
	if ($line eq "") {
	    die "no end of body for rule $ruleName";
	}
	chop $line;
	$length = length($line);
        $slash = 0;
	for ($i = 0;$i< $length; $i++) {
	    $char = substr($line,$i,1);
            if ($char eq "\\") {
                $slash = ($slash+1)%2;
            } else {
                $slash = 0;
            }
            if ($char eq "\{" && !$slash) {
		$brace++;
	    }
	    if ($char eq "\}" && !$slash) {
		$brace--;
	    }
            if ($char eq "%" && !$slash) {
                last;
            }
	    if ($brace) {
		$ruleBody = $ruleBody.$char;
	    }
	    if ($brace == 0) {
		$rest = substr($line, $i+1);
		if ($rest !~ /^\s*$/ && $rest !~ /^\s*%?\\marginpar/) {
		    warn "in rule \"$ruleName\", there were something after the end brace on the same line:\n\"$rest\"\nThis text has been ignored.";
		}
		last;
	    }
	}
    }
    return 1;
}

############################################################
# writing the header
############################################################
sub printInit {
    print OUTPUT "<html> <head>\n";
    print OUTPUT "<title>Concrete Syntax of IFAD VDM-SL</title>\n";
    print OUTPUT "</head>\n";

    print OUTPUT "<body>\n";
    print OUTPUT "<h1>Concrete Syntax of IFAD VDM-SL</h1>\n";
}

############################################################
# writing the footer to the document
############################################################
sub printEnd {
    print OUTPUT "<hr>\n";
    print OUTPUT "<address><a href=\"http://www.ifad.dk/products/toolbox.html\">The IFAD VDM-SL Toolbox Group</a> &lt;<a href=\"mailto:toolbox\@ifad.dk\">toolbox\@ifad.dk</a>&gt;</address>\n";
    print OUTPUT "Last modified:", `/usr/bin/date`;
    print OUTPUT "</body> </html>\n";
}

############################################################
# writing a single rule
############################################################
sub printRule {
    print OUTPUT $pre;
    $pre = "";
    $ruleName =~ s/\s+$//g;
    $ruleName =~ /^\s*(.*)$/;
    $ruleName = $1;
    print OUTPUT "<a name=\"$ruleName\">$ruleName</a> = ";
    $body = $ruleBody;
    $body =~ s/\\dsepl/|/g;
    $body =~ s/\\dsep/|/g;

    &printElm($body);
    print OUTPUT "<p>\n\n"
}

############################################################
# writing part of the rule
############################################################
sub printElm {
    local ($body) = @_;
    while (length($body) > 0) {
	if ($body =~ /^\s*\\Lop\{(.*)$/ || $body =~ /^\s*\\Sig\{(.*)$/) {
	    &findInBrace($1);
	    &clip;
	    print OUTPUT " `$firstPart'";
	    $body = $lastPart;
	}
	elsif ($body =~ /^\s*\\SeqPt\{(.*)$/) {
	    print OUTPUT " {";
	    &findInBrace($1);
	    $body = $lastPart;
	    &printElm($firstPart);
	    print OUTPUT "}";
	}
	elsif ($body =~ /^\s*\\OptPt\{(.*)$/) {
	    print OUTPUT " [";
	    &findInBrace($1);
	    $body = $lastPart;
	    &printElm($firstPart);
	    print OUTPUT "]";
	}
	elsif ($body =~ /^\s*\\Brack\{(.*)$/) {
	    print OUTPUT " (";
	    &findInBrace($1);
	    $body = $lastPart;
	    &printElm($firstPart);
	    print OUTPUT ")";
	}
	elsif ($body =~ /^\s*\\lfeed\s*(.*)$/) {
	    $body = $1;
	}
	elsif ($body =~ /^\s*\\nmk\s*(.*)$/) {
	    $body = $1;
	}
	elsif ($body =~ /^\s*\\Lit\{(.*)$/) {
	    &findInBrace($1);
	    &clip;
	    print OUTPUT " `$firstPart'";
	    $body = $lastPart;
	}
	elsif ($body =~ /^\s*\\Lks[b]*\{(.*)$/) {
	    &findInBrace($1);
	    &clip;
	    print OUTPUT " `$firstPart' | ";
	    $body = $lastPart;
	}
	else {
	    $found = ($body =~ /^\s*([^,|-]*)([|,-])-?(.*)\s*$/);
	    if ($found) {
		if ($1 eq "") {
		    print OUTPUT " $2 ";
		}else {
		    $link = $1;
		    $delim = $2;
		    $link =~ s/\s+$//g;
		    print OUTPUT " <a href=\"#$link\">$link</a> $delim ";
		}
		$body = $3;
	    } else {
		if ($body !~ /^\s*$/) {
		    $body =~ /^\s*(.*)$/;
		    $link = $1;
		    $found = $link =~ s/\s+$//g;
		    print OUTPUT " <a href=\"#$link\">$link</a> ";
		}
		$body = "";
	    }
	}
    }
}

############################################################
# finding the matching end brace
############################################################
sub findInBrace {
    local($string) = @_;

    $brace = 1;
    $slash = 0;
    $length = length($string);
    for ($i = 0;$i< $length; $i++) {
	$char = substr($string,$i,1);
	if ($char eq "\\") {
	    $slash = ($slash+1) %2;
	}
	elsif ($char eq "\{") {
	    if ($slash) {
		$slash = 0;
	    } else {
		$brace++;
	    }
	}
	elsif ($char eq "\}") {
	    if ($slash) {
		$slash = 0;
	    } else {
		$brace--;
	    }
	}
	else {
	    $slash = 0;
	}
	if ($brace == 0) {
	    $firstPart = substr($string,0,$i);
	    $lastPart = substr($string,$i+1);
	    return;
	}
    }
    die "No end slash in string $string, called from $ruleName";
}

############################################################
# converting special characters.
############################################################
sub clip {
    $firstPart =~ s/\\&/&/g;
    $firstPart =~ s/\\\}/\}/g;
    $firstPart =~ s/\\\{/\{/g;
    $firstPart =~ s/\\_/_/g;
    $firstPart =~ s/\\char\'176/~/g;
    $firstPart =~ s/\\char\'134/\\/g;
    $firstPart =~ s/\\char\'136/^/g;
    $firstPart =~ s/\\Range/.../g;
    $firstPart =~ s/\{\\tt(.*)\}/$1/g;
    $firstPart =~ s/\\\mbox\{\\,\}//g;
}

############################################################
# prints the extra element which couldn't be read from the
# manual by this program
############################################################
sub printExtra {
    open (EXTRA,"extra.html") || die "Couldn't read file extra.html";
    while (<EXTRA>) {
        print OUTPUT;
    }
}

############################################################
# This function test the references in the generated html file
############################################################
sub testReferences {
  open(FILE,"<$outfile");
    
  while(<FILE>) {
    while (/name=\"([^"]+)\"/g) {
      $tags{$1} = 1;
    }
    while (/href=\"#([^"]+)\"/g) {
      $references{$1} = 1;
    }
  }
  foreach $tag (keys %references) {
    if (!defined $tags{$tag}) {
      push(@undefs,$tag);
    }
  }
  if (@undefs) {
      print "********************************************************\n";
      print "                         WARNING\n";
      print "           THE FOLLOWING TAGS WERE NOT FOUND\n";
      foreach $tag (@undefs) {
          print "   $tag\n";
      }
      print "********************************************************\n";
  }
}
