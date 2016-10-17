#! /usr/bin/perl

#
# This script searches a code generated header file in order
# to modify tag definitions. Tags are re-numbered according
# to the offsets defined in the file "tag_offset" which defines 
# a mapping between a module name and an offset. This mapping
# should be constructed in order to prevent overlapping tag numbers.
# 
# Special treatment:
# If modules AS or REP are being patched/modified, they are treated
# in a special manner. In this case the tags are not numbered, by
# given the proper name as defined in "tag.h"
# CPP also treated specially.

# Next two lines make this script work on strange OS. 
eval 'exec perl -S $0 ${1+"$@"}'
  if $running_under_some_shell;

$map_file = "tag_offsets";


while($tmp = shift(@ARGV)){
    if($tmp eq "-m"){
        $map_file = shift(@ARGV);
    }
    else{
        print "Error in command line arguments.\n";
        print "Usage:\n";
        print $0; print " -m map-file\n";
        exit 0;
    }
}

# Read offset map from $map_file:
open MAPFILE, "<$map_file" or die "The file $map_file does not exist.";
while(defined($_=<MAPFILE>)){
    ($mod,$num) = split ' ',$_;
    $m{$mod}=$num;
}

while (<>) {
    if (/^#define _CPP_h/) {
        print;
        print "#include \"cgtag.h\"\n";
        next;
    }
    if (/\s*vdm_([A-Z]*)_(\w*)\s*=\s*(\d+).*/) {
        if ($1 eq "AS" || $1 eq "REP") {
            # Code generated AS and REP modules are treated specially
            print "  vdm_$1_$2 = $2,\n";
        }
        else {
           if ($1 eq "CPP") {
               # Code generated CPP module are treated specially
               print "  vdm_$1_$2 = $2_CPP,\n";
           }
           else {
               print "  vdm_$1_$2 = ", $m{$1}++, ",\n";
           }
        }
    }
    else {
        print;
    }
}





