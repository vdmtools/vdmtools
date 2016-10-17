{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 x - 1 > 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 x - 1 > 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "B" ], 41943075 ),
        mk_AS`Name( [ "goo" ], 41943076 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 A`id(x) > id(x - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "A",
            "foo" ], 41943088 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "B" ], 41943075 ),
        mk_AS`Name( [ "goo" ], 41943076 ) ) ) }

