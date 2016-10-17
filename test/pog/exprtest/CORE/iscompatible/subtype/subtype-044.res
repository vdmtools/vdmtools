{ mk_TEST`ProofObligationPP(
     "(forall x : D &
(forall yy_2 in set rng (x) &
(forall xx_3 in set elems (yy_2) &
is_(xx_3,C) and
 Test`inv_C(xx_3) or
 is_(xx_3,rat))))",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Test" ], 41943041 ),
        mk_AS`Name( [ "DH" ], 41943320 ) ) ) }

