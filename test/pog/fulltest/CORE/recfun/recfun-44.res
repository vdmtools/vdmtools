{ mk_TEST`ProofObligationPP(
     "(forall x : seq of @A &
not (x = []) =>
 x <> [])",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943057 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : seq of @A &
not (x = []) =>
 id(x) > id(tl (x)))",
     <RecursiveFunction>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943057 ) ) ) }

