#!/usr/bin/perl -w

######################################################################
### This program replaces the error message text comments in the
### specification and implementations (of the statsem.)
######################################################################

### Configurations
$ErrFile = "errmsg.txt";

### MAIN ###
$message = readErrMsg();

foreach $file (@ARGV) {
    translateFiles($file);
}

sub translateFiles {
    local($file) = @_;

    if (! -e $file) {
        warn "file does not exists: \"$file\"";
        return;
    }
    $file = readlink($file) if -l $file;

    die "file \"$file\" does not have the extension .cc or .vdm" 
        unless $file =~ /\.vdm$|\.cc$/;

    if ($file =~ /\.vdm$/) {
        $VDM = 1;
    } else {
        $VDM = 0;
    }

    convertFile($file,$VDM);
    $res = system('cmp','-s',$file,"$file.new");
    if ($res != 0) {
        print "File $file has changed.\n";
        rename $file,"$file.old";
        rename "$file.new",$file;
    } else {
        system('rm',"$file.new");
    }
}

sub convertFile {
    local($file,$VDM) = @_;

    if ($VDM) {
        $StartToken = "^( *)-- Error message \#([0-9]+)\$";
        $EndToken = "^ *------";
        $COM = "--";
    } else {
        $StartToken = "^( *)// Error message \#([0-9]+)\$";
        $EndToken = "^ *//-------.*";
        $COM = "//";
    }

    open(IN,"$file") or die "couldn't open file";
    open(OUT,">$file.new") or die "couldn't open file";
    
    $last = "";
    $matchingMessage = 0;
    $nextMustBeGenErr = 0;
    $lineno = 0;

    while (defined($line = <IN>)) {
        $lineno++;
        if ($matchingMessage) {
            if ($line =~/$EndToken/) {
                $matchingMessage = 0;
                $nextMustBeGenErr = 1;
                $col = length($spaces);
                $txt = $message{$number};
                $len = max(22,min(80-$col,3+length($txt)))-2;
                
                $prefix = " " x $col . $COM;
                print OUT $prefix . "-" x $len . "\n";
                print OUT $prefix . " Error message \#$number\n";
                foreach $l (splitLine($txt,77-$col)) {
                    print OUT $prefix . $l ."\n";
                }
                print OUT $prefix . "-" x $len . "\n";
            }
            ## insert replacement
            $last = "";
        } 
        elsif ($line =~ /$StartToken/) {
            $spaces = $1;
            $number = $2;
            $matchingMessage = 1;
        } else {
            if ($nextMustBeGenErr) {
                $nextMustBeGenErr = 0;
                if ($line !~ /GenErr/) {
                    print "Warning: A GenErr line was expected at line $lineno\n";
                }
            }
            print OUT $last;
            $last = $line;
        }
    }
    print OUT $last;
    close(OUT);
    close(IN);
}


sub readErrMsg {
    open(IN ,$ErrFile);
    $next = 0;
    $lineNo = 0;
    while(<IN>) {
        $line=$_;
        chop($line);
        $lineNo++;
        
        if (/^[ \t]*#/ || /^[ \t]*$/) {
            next;
        }

        if ($next == 0) {
            ### read the test number
            $testNo = $line;
            if ($line !~ /^[0-9]+$/) {
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
    close(IN);
    return $message;
}

sub min {
    local($a,$b) = @_;
    if ($a<$b) {
        return $a;
    } else {
        return $b;
    }
}
sub max {
    local($a,$b) = @_;
    if ($a>$b) {
        return $a;
    } else {
        return $b;
    }
}

sub splitLine {
    local($line,$size) = @_;
    $res = "";
    @lines = ();
    @words = split ' ',$line;
    foreach $word (@words) {
        if (length($res)+length($word) <= $size) {
            $res = $res . " " . $word;
        } else {
            push(@lines,$res);
            $res = " ".$word;
        }
    }
    push(@lines,$res);
    return @lines;
}
