{ mk_TEST`ProofObligationPP(
     "is_bool(y)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_real(y)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists y : (bool | real) &
y > 7)",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943052 ) ) ) }

