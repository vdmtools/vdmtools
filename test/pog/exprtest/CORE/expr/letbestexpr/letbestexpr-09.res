{ mk_TEST`ProofObligationPP(
     "is_(mk_(a,b),(((nat * nat)) | ((((bool * bool)) | ((token * token))))))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists mk_(a,b) : (((nat * nat)) | ((((bool * bool)) | ((token * token))))) &
true)",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "LetBeSTExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943052 ) ) ) }

