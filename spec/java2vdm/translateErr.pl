#!/usr/bin/perl

######################################################################
#### Constant definitions
######################################################################
$ErrFile = $ARGV[1] || "j2verrmsg.txt";
$VdmFile = "j2verrmsg.vdm";
$CCFile  = "j2verrmsg.cc";
$ArrayLength = 1000;
$method = $ARGV[0];

#----------------------------------------------------------------------
$VdmHead = <<EOT;
module J2VERRMSG

exports all

definitions
values

ErrMsg : map nat to seq of char = {
EOT
#----------------------------------------------------------------------


#----------------------------------------------------------------------
$VdmFoot = <<EOT;
}
end J2VERRMSG
EOT
#----------------------------------------------------------------------


#----------------------------------------------------------------------
$CCHead = <<EOT;
#include <stdio.h>
#include <statsem.h>
char *VDMPPStatSem::getErrMsg(int num) {
    switch (num) {
EOT
#----------------------------------------------------------------------


#----------------------------------------------------------------------
$CCFoot = <<EOT;
    default: InternalError("the number for the error message didn't exists");
             exit(-1);
    }
    return "Default error message. Should never be shown."; 
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
            print "$line\n";
            die "A number was expected in line $lineNo";
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
        print CC "    case $testno: return \"$mes\";\n";
    }
}

if ($method eq "vdm") {
    print VDM $VdmFoot;
} else {
    print CC $CCFoot;
}
