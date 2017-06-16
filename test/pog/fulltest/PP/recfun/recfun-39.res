{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 x - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AAA" ], 41943058 ),
        mk_AS`Name( [ "foo" ], 41943060 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 AA`id(x) > AA`id(x - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "AAA",
            "foo" ], 41943072 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AAA" ], 41943058 ),
        mk_AS`Name( [ "foo" ], 41943060 ) ) ) }

