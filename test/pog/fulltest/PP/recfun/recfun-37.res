{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 x - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AA" ], 41943054 ),
        mk_AS`Name( [ "foo" ], 41943056 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 AA`id(x) > AA`id(x - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "AA",
            "foo" ], 41943068 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AA" ], 41943054 ),
        mk_AS`Name( [ "foo" ], 41943056 ) ) ) }

