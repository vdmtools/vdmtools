{ mk_TEST`ProofObligationPP(
     "(9 - 77) in set inds ([1,8,9,true])",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943160 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_([7,9],T2) and
 ApplyExprTest`inv_T2([7,9])",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "ApplyExprTest" ], 41943041 ),
        mk_AS`Name( [ "g" ], 41943126 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : nat &
ApplyExprTest`pre_f(a) =>
 (exists r : nat &
ApplyExprTest`post_f(a, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "ApplyExprTest" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943111 ) ) ) }

