{ mk_TEST`ProofObligationPP(
     "(forall m : nat, n : nat &
not (m = 0) =>
 n = 0 =>
 m - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "ack" ], 41943061 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m : nat, n : nat &
not (m = 0) =>
 not (n = 0) =>
 m - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "ack" ], 41943061 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m : nat, n : nat &
not (m = 0) =>
 not (n = 0) =>
 n - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "ack" ], 41943061 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m : nat, n : nat &
not (m = 0) =>
 n = 0 =>
 id2(m, n) (LEX2 >) id2(m - 1, 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "ack" ], 41943081 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "ack" ], 41943061 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m : nat, n : nat &
not (m = 0) =>
 not (n = 0) =>
 id2(m, n) (LEX2 >) id2(m, n - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "ack" ], 41943091 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "ack" ], 41943061 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m : nat, n : nat &
not (m = 0) =>
 not (n = 0) =>
 id2(m, n) (LEX2 >) id2(m - 1, ack(m, n - 1)))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "ack" ], 41943087 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "ack" ], 41943061 ) ) ) }

