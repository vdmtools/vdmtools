{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
is_nat((let foo = (if x = 0 then
0
else
foo(x - 1))
in
    foo)))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943053 ) ) ) }

