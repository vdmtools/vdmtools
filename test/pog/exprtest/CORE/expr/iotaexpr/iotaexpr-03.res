{ mk_TEST`ProofObligationPP(
     "is_real(a)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "IotaExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists1 [a,b] in set {[1,2],[3,true]} &
a < 3)",
     <ExistsUnique>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "IotaExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ) }

