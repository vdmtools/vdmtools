#! /usr/bin/perl

while (defined ($data = <STDIN>)) {
  $data =~ s/^([ ]*)vdm_AS_([a-zA-Z0-9]+)[^,]+/$1vdm_AS_$2 = $2/g;
  print $data;
}

