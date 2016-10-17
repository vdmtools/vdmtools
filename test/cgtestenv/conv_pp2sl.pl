#! /usr/bin/perl
# Next two lines make this script work on strange OS. 
eval 'exec perl -S $0 ${1+"$@"}'
  if $running_under_some_shell;

undef $/; $* = 1; 
$file = <STDIN>;
$file =~ s/^class(\s+[A-Za-z_\47]+\s*)/module $1 exports all definitions /g; 
$file =~s/^instance variables\s*(\w+\s*:\s*\w+\s*;\s*)+//g;
$file =~s/public//g;
print $file;      

