{ mk_TEST`ProofObligationPP(
     "(exists a in set {1,true} &
true)",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "IfExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943079 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_bool(let a in set {1,true} in  a)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "IfExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943079 ) ) ),
  mk_TEST`ProofObligationPP(
     "true =>
 is_({2 |-> 5},inmap rat to nat)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "IfExprTest" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943043 ) ) ) }

