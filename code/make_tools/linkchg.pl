#! /usr/bin/perl
# Next two lines make this script work on strange OS. 
eval 'exec perl -S $0 ${1+"$@"}'
  if $running_under_some_shell;

$conv = shift; 

foreach $a (@ARGV) {
    if ($conv) {
        $a =~ s/^-L.*//;        # remove -L... option
        $a =~ s/^--?verbose//;  # remove --verbose
        $a =~ s/^-l([\w-]+)/$1.lib/; 
        $a =~ s/([\w-]+)\.o$/$1.obj/; 
        $a =~ s/lib([\w-]+)\.a/$1.lib/; 
        $a =~ s/^-v$/-verbose/; 
    }
    print $a, " "; 
}
  
