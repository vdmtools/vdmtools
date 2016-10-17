#! /usr/bin/perl
#
# Remove test coverage table
#

undef $/; $* = 1; 
$file = <STDIN>;
$file =~ s/\\usepackage\{makeidx}/\\usepackage\{toolbox}\n\\usepackage\{makeidx}\n\\AtBeginDvi\{\\special\{pdf:tounicode 90ms-RKSJ-UCS2\}\}\n\\usepackage[dvipdfm,bookmarks=true,bookmarksnumbered=true,colorlinks,plainpages=true]{hyperref}\n\\usepackage{cite}/g;
# $file =~ s/\\title\{VDM\+\+ Sorting Algorithms\}\r\n\\author\{CSK\}\r\n\\date\{August\, 1997\}\r\n\r\n\\begin\{document\}\r\n\\maketitle/\\begin\{document\}\n\\vdmtoolsmanual\{VDM\+\+ Sorting Algorithms\}\{V7\.0\}\{2006\}\{VDM\+\+\}/g;

print $file;      
