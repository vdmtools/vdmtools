{ mk_TEST`ProofObligationPP(
     "n - 1 >= 0",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyTest" ], 41943041 ),
        mk_AS`Name( [ "Test1" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists a : ApplyTest &
true)",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943073 ) ) ) }

