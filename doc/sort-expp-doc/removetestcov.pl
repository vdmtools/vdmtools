#! /usr/bin/perl
#
# Remove test coverage table
#

undef $/; $* = 1; 
$file = <STDIN>;
$file =~ s/^\\begin\{rtinfo\}\r\n\[InsertSorted\]\{vdm\.tc\}\[DoSort\]\r\nDoSorting\r\nInsertSorted\r\nSort\r\n\\end\{rtinfo\}//g; 

print $file;      
