{ mk_TEST`ProofObligationPP(
     "(forall x : G &
(forall xx_1 in set elems (x) &
is_(xx_1,C) and
 Test`inv_C(xx_1) or
 is_(xx_1,rat)))",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Test" ], 41943041 ),
        mk_AS`Name( [ "GF" ], 41943320 ) ) ) }

