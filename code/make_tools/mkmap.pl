#! /usr/bin/perl

#
# This script searches a code generated header file and produces
# an array of tag strings and the corresponding tag name.
# The array should later be used to construct a name map, see
# for instance umltag.cc
#

# Next two lines make this script work on strange OS. 
eval 'exec perl -S $0 ${1+"$@"}'
  if $running_under_some_shell;

while (<>) {
    if (/\s*vdm_([A-Z]*)_(\w*) = (.*),/) {
        print "  {\"$2\", vdm_$1_$2},\n";
    }
}



