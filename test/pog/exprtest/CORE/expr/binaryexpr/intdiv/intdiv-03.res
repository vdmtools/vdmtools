{ mk_TEST`ProofObligationPP(
     "n <> 0",
     <NonZero>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "BinaryTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_int(n)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "BinaryTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ) }

