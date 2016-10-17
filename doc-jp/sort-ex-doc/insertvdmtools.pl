#! /usr/bin/perl
#
# Remove test coverage table
#

undef $/; $MULTILINE_MATCHING = 1; 
$file = <STDIN>;
$file =~ s/\\usepackage\{makeidx}/\\usepackage\{toolbox}\n\\usepackage\{makeidx}\n\\AtBeginDvi\{\\special\{pdf:tounicode 90ms-RKSJ-UCS2\}\}\n\\usepackage[dvipdfm,bookmarks=true,bookmarksnumbered=true,colorlinks,plainpages=true]{hyperref}/g;
# $file =~ s/\\title\{VDM\-SL Sorting Algorithms\}\r\n\\author\{CSK\}\r\n\\date\{February 17\, 1994\}\r\n\r\n\\begin\{document\}\r\n\\maketitle/\\begin\{document\}\n\\vdmtoolsmanual\{VDM-SL Sorting Algorithms\}\{V4\.0\}\{2006\}\{VDM-SL}/g;

print $file;
