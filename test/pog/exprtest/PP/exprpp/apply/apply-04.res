{ mk_TEST`ProofObligationPP(
     "n - 1 >= 0",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyTest" ], 41943041 ),
        mk_AS`Name( [ "Test1" ], 41943042 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists x : X &
true)",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943072 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(ApplyTest,x.CreateApplyTest2())",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ApplyTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943072 ) ) ) }

