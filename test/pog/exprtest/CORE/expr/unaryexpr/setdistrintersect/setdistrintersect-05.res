{ mk_TEST`ProofObligationPP(
     "{(let mk_A(b) = a
in
    b) | a in set {mk_A({1}),mk_A({2})}} <> {}",
     <NonEmptySet>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "UnaryTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943049 ) ) ) }

