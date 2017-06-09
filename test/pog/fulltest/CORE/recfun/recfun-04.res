{ mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "g" ], 41943092 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "h" ], 41943072 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 id(n) > id(n - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "f" ], 41943103 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "g" ], 41943092 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 id(n) > id(n - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "g" ], 41943083 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "h" ], 41943072 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 id(n) > id(n - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "h" ], 41943063 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943052 ) ) ) }

