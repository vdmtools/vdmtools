{ mk_TEST`ProofObligationPP(
     "(forall b : seq of @B &
b = ([]) or
 (exists [xx_3] ^ t : seq of @B &
b = [xx_3] ^ t))",
     <CasesExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943057 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall b : seq of @B &
not (b = ([])) =>
 (exists [xx_1] ^ t : seq of @B &
b = [xx_1] ^ t) =>
 (let [-] ^ t = b
in
    meas(b) > meas(t)))",
     <RecursiveFunction>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943057 ) ) ) }