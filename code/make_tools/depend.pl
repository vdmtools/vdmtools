#! /usr/bin/perl 

# Next two lines make this script work on strange OS. 
eval 'exec perl -S $0 ${1+"$@"}'
  if $running_under_some_shell;

$lib = shift;

while (<>) {
    s/^(.*\.o):/$lib\(\1\):/g;
    print;
}

