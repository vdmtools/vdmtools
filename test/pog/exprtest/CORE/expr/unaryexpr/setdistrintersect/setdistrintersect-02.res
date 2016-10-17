{ mk_TEST`ProofObligationPP(
     "x <> {}",
     <NonEmptySet>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "UnaryTest" ], 167772161 ),
        mk_AS`Name( [ "TestOP" ], 167772168 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(TestOP(),set of ((nat | bool | [A])))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "UnaryTest" ], 167772161 ),
        mk_AS`Name( [ "TestOP" ], 167772168 ) ) ) }
