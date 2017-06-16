{ mk_TEST`ProofObligationPP(
     "pre_(x,9)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "x" ], 167772314 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyExprTest" ], 167772161 ),
        mk_AS`Name( [ "TestOP" ], 167772279 ) ) ),
  mk_TEST`ProofObligationPP(
     "9 in set dom (x)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyExprTest" ], 167772161 ),
        mk_AS`Name( [ "TestOP" ], 167772279 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_nat(TestOP())",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyExprTest" ], 167772161 ),
        mk_AS`Name( [ "TestOP" ], 167772279 ) ) ),
  mk_TEST`ProofObligationPP(
     "9 in set inds (x)",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyExprTest" ], 167772161 ),
        mk_AS`Name( [ "TestOP" ], 167772279 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_([7,9],T2) and ApplyExprTest`inv_T2([7,9])",
     <Invariants>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "ApplyExprTest" ], 167772161 ),
        mk_AS`Name( [ "g" ], 167772245 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : nat & ApplyExprTest`pre_f(a) => (exists r : nat & ApplyExprTest`post_f(a, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "ApplyExprTest" ], 167772161 ),
        mk_AS`Name( [ "f" ], 167772230 ) ) ) }
