#! /usr/bin/perl

# Next two lines make this script work on strange OS. 
eval 'exec perl -S $0 ${1+"$@"}'
  if $running_under_some_shell;


while (<>) {
    if (/^#define/) {
	s/^#define[ ]+([a-zA-Z0-9_]+).*/  \{\"$1\_AS", $1\},/g;
	print;
    }
}

