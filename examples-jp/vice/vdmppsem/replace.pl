#
# $Id: replace.pl,v 1.1 2005/11/21 04:41:20 vdmtools Exp $
#
$fnamein = "<$ARGV[0]";
$fnameout = ">$ARGV[0].tex";
$fnameout =~ s/src/doc/g;
$toggle = 0;
open(IPF,$fnamein);
open(OPF,$fnameout);
while($line = <IPF>) {
  if ($line =~ /vdm_al/) {
    if ($toggle eq 0) {
      $toggle = 1;
    } else {
      $toggle = 0;
    }
    $line =~ s/vdm_al/alltt/;
  }
  elsif ($toggle eq 1) {
    # change carefully, order is important!
    # escape backslash characters
    # $line =~ s/\\/\\\( \\backslash \\\)/g;
    $line =~ s/\\/\\verb+\\+/g;
    # escape curly brackets
    $line =~ s/{/\\{/g;
    $line =~ s/}/\\}/g;
    # escape underscore characters
    $line =~ s/_/\\_/g;
    # expand rogue tab characters
    $line =~ s/\t/        /g;
    # escape percent sign in strings
    $line =~ s/\x25/\\perc/;
    # emphasize in-line comments
    $line =~ s/(--[^\r\n]*)/\\emph{\1}/g;
  }
  printf OPF $line;
}
close(OPF);
close(IPF);
