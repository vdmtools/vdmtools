{ mk_TEST`ProofObligationPP(
     "is_bool(y)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTStmtTest" ], 41943041 ),
        mk_AS`Name( [ "LetBeST4" ], 41943112 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_real(y)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTStmtTest" ], 41943041 ),
        mk_AS`Name( [ "LetBeST4" ], 41943112 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists x : nat &
true)",
     <LetBeStStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTStmtTest" ], 41943041 ),
        mk_AS`Name( [ "LetBeST2" ], 41943070 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists b : bool &
b =>
 false)",
     <LetBeStStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTStmtTest" ], 41943041 ),
        mk_AS`Name( [ "LetBeST5" ], 41943132 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists y : (bool | real) &
y > 7)",
     <LetBeStStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTStmtTest" ], 41943041 ),
        mk_AS`Name( [ "LetBeST4" ], 41943112 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists a in set {3,true,nil} &
true)",
     <LetBeStStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTStmtTest" ], 41943041 ),
        mk_AS`Name( [ "LetBeST1" ], 41943051 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists x in set (if 7 = 6 then
{x | x in set {1,2} & false}
else
{8,9,7}) &
true)",
     <LetBeStStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTStmtTest" ], 41943041 ),
        mk_AS`Name( [ "LetBeST3" ], 41943083 ) ) ) }

