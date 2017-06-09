{ mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "F" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943042 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "G" ], 41943066 ),
        mk_AS`Name( [ "goo" ], 41943067 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "H" ], 41943100 ),
        mk_AS`Name( [ "hoo" ], 41943101 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "I" ], 41943125 ),
        mk_AS`Name( [ "ioo" ], 41943126 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 G`id(n) > G`id(n - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "ioo" ], 41943141 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "I" ], 41943125 ),
        mk_AS`Name( [ "ioo" ], 41943126 ) ) ) }
