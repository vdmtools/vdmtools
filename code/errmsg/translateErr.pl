#!/usr/bin/perl

######################################################################
#### Constant definitions
######################################################################
$ErrFile = $ARGV[1] || "errmsg.txt";
$VdmFile = "errmsg.vdm";
$CCFile  = "errmsg.cc";
$ArrayLength = 1000;
$method = $ARGV[0];

#----------------------------------------------------------------------
$VdmHead = <<EOT;
module ERRMSG

exports all

definitions
values

ErrMsg : map nat to seq of char = {
EOT
#----------------------------------------------------------------------


#----------------------------------------------------------------------
$VdmFoot = <<EOT;
}
end ERRMSG
EOT
#----------------------------------------------------------------------


#----------------------------------------------------------------------
$CCHead = <<EOT;
#include <stdio.h>
#include <statsem.h>
#ifdef __linux__
#include <stdlib.h>
#endif // __linux__
#ifdef __Cygwin__
#include <stdlib.h>
#endif // __Cygwin__
const wchar_t *StatSem::getErrMsg(int num) {
    switch (num) {
EOT
#----------------------------------------------------------------------


#----------------------------------------------------------------------
$CCFoot = <<EOT;
    default: InternalError(L"the number for the error message didn't exists");
             exit(-1);
    }
    return L"Default error message. Should never be shown."; 
}
EOT
#----------------------------------------------------------------------


### open the input and output files
open(IN,$ErrFile) || die "can't open file :  $ErrFile";

if ($method eq "vdm") { 
    open(VDM,">$VdmFile");
} else {
    open(CC,">$CCFile");
}

### run through the lines of the input file.
$next = 0;
$lineNo = 0;
while(<IN>) {
    $line=$_;
    chop($line);
    $lineNo++;

    if (/^[\s]*#/ || /^[\s]*$/) {
        next;
    }

    if ($next == 0) {
        ### read the test number
        $testNo = $line;
        if ($line !~ /^[0-9]+[\s]*$/) {
            die "A number was expected in line $lineNo ";
        }
        $next=1;
    } elsif ($next == 1) {
        ### read the error message
        $msg = $line;
        if ($line =~ /^[0-9]+$/) {
            die "An error message was expected in line $lineNo ";
        }
        $next=0;
        $message{$testNo} = $msg;
    }
}


### Write the output files
sub bynum {$a <=> $b}
$first=1;
print VDM $VdmHead;
print CC $CCHead;

foreach $testno (sort bynum keys(%message)) {
    if (!$first) {
        if ($method eq "vdm") {
            print VDM ",\n";
        }
    } else {
        $first = 0;
    };
    $mes = $message{$testno};
    $mes !~ s/\"/\\\"/g;
    if ($method eq "vdm") {
        print VDM "\t$testno |-> \"$mes\"";
    } else {
        print CC "    case $testno: return L\"$mes\";\n";
    }
}

if ($method eq "vdm") {
    print VDM $VdmFoot;
} else {
    print CC $CCFoot;
}
