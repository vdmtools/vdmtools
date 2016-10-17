{ mk_TEST`ProofObligationPP(
     "(forall x : H &
not (is_(x,nilType)) =>
 (forall xx_1 in set dom (x) &
xx_1 <> 0))",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Test" ], 41943041 ),
        mk_AS`Name( [ "HI" ], 41943320 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : H &
not (is_(x,nilType)) =>
 (forall yy_2 in set rng (x) &
yy_2 <> []))",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Test" ], 41943041 ),
        mk_AS`Name( [ "HI" ], 41943320 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : H &
not (is_(x,nilType)) =>
 (forall id_4,id_5 in set x &
id_4 <> id_5 =>
 x(id_4) <> x(id_5)))",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Test" ], 41943041 ),
        mk_AS`Name( [ "HI" ], 41943320 ) ) ) }

