{ mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "nest" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 id(n) > id(n - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "nest" ], 41943064 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "nest" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 id(n) > id(nest(n - 1)))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "nest" ], 41943063 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "nest" ], 41943052 ) ) ) }
