#! /usr/bin/perl
# Next two lines make this script work on strange OS. 
eval 'exec perl -S $0 ${1+"$@"}'
  if $running_under_some_shell;

# Convert TCL code to embedded C-code. 
# Usage: tcl2code2.pl [ --keep s1 --k s2 ...] files.tcl .../lib/*.tcl > code.c
# 

require "newgetopt.pl"; 
# $newgetopt::debug = 1;
# $newgetopt::ignorecase = 1;
@opt_keep = (); 
&NGetOpt("keep:s\@");

# print STDERR "keep: @opt_keep\n"; 

$arrayname = "TCL_CODE"; 
@sources = @ARGV; 

$fd = "fd00"; 
$Lines = 0; 
$Bytes = 0; 
print "char *${arrayname}_STRINGS[] = {\n"; 
%base_sources = map {m|([^/]*$)|;($1,$_)} @sources;
foreach $f (@sources) {
  &expand_main($f); 
}
print "(char*)0 }; \n"; 

print "int ${arrayname}_LINES = $Lines;\n"; 
print "int ${arrayname}_BYTES = $Bytes;\n"; 

sub expand_main {
  local($m) = @_; 
  local($fileid, $sourcename); 
  return if defined $seen{$m}; 
  $seen{$m} = 1; 
  $fileid = $fd++; 
  print STDERR "Opening: $m\n"; 
  open($fileid, "<$m") || die "open on $m: $!"; 
  while (defined($_ = <$fileid>)) {    
    if (m/^\s*\bsource\b\s+(\S+)/) {
      $sourcename = $1; 
      if (exists $base_sources{$sourcename}) {
        print STDERR "source $sourcename\n"; 
        &expand_main($base_sources{$sourcename}); 
        print STDERR "back in $m\n"; 
        next; 
      }
      
      if (! grep($sourcename eq $_, @opt_keep)) {
        print STDERR "IGNORING $_"; 
        next; 
      } else {
        print STDERR "Keeping: $_"; 
      }
    }

    $Bytes += length($_); 
    chop; 
    s/\\/\\\\/g; 
    s/"/\\"/g; 
    $_ .= "\\n"; 
    print "\"$_\",\n"; 
  }
  close($fileid); 
}
