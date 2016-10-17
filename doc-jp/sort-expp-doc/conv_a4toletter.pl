#! /usr/bin/perl
#
# Substitute "\newcommand{\pformat}{a4paper}" with 
#            "\newcommand{\pformat}{a4letter}"
#

undef $/; $* = 1; 
$file = <STDIN>;
$file =~ s/^\\newcommand{\\pformat}{a4paper}/\\newcommand{\\pformat}{letterpaper}/g; 
print $file;      
