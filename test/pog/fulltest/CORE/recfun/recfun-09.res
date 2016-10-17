{ mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 not (n = 1) =>
 n - 2 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "log" ], 41943089 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 not (n = 1) =>
 n - 2 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "half" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 not (n = 1) =>
 idhf(n) > idhf(n - 2))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "half" ], 41943068 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "half" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 not (n = 1) =>
 idlog(n) > idlog(1 + half(n - 2)))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "log" ], 41943105 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "log" ], 41943089 ) ) ) }
