#! /bin/bash


CRES=`cat cres`
IRES=`cat ires`

cat <<EOF>$1
values
  cres = $CRES;
  ires = $IRES;
  epsilon = 10**-9

types
  cmptp = real | set of real | seq of real

functions

  Comp: () -> <OK>
  Comp() ==
    if ires = cres then
      <OK>
    elseif cmp (ires,cres) then
      <OK>
    else
      undefined;

  cmp: cmptp * cmptp -> bool
  cmp (i,c) ==
  cases mk_(i,c):
    mk_( [i1]^ir,[c1]^cr ),
    mk_( {i1} union ir,{c1} union cr ) -> cmp (i1,c1) and cmp (ir,cr),
    mk_( i,c ) -> i=c or (is_real (i) and abs (i-c) < epsilon),
    others -> undefined
  end
EOF
