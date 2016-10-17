{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 x - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943055 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 x - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "goo" ], 41943089 ) ) ) }
