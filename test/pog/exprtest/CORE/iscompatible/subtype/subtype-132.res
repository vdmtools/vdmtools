{ mk_TEST`ProofObligationPP(
     "(forall x : K &
is_(x.#1,C) and
 Test`inv_C(x.#1) or
 is_(x.#1,rat))",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Test" ], 41943041 ),
        mk_AS`Name( [ "KL" ], 41943320 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : K &
(forall xx_2 in set elems (x.#6) &
is_(xx_2,C) and
 Test`inv_C(xx_2) or
 is_(xx_2,rat)))",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Test" ], 41943041 ),
        mk_AS`Name( [ "KL" ], 41943320 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : K &
(forall yy_4 in set rng (x.#7) &
(forall xx_5 in set elems (yy_4) &
is_(xx_5,C) and
 Test`inv_C(xx_5) or
 is_(xx_5,rat))))",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Test" ], 41943041 ),
        mk_AS`Name( [ "KL" ], 41943320 ) ) ) }

