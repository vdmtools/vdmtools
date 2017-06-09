{ mk_TEST`ProofObligationPP(
     "(exists a in set {1,true} &
true)",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "IfStmtTest" ], 41943041 ),
        mk_AS`Name( [ "If2" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_bool(let a in set {1,true} in  a)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "IfStmtTest" ], 41943041 ),
        mk_AS`Name( [ "If2" ], 41943043 ) ) ) }

