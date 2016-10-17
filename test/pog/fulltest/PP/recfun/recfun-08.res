{ mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 (let foo = [x | x in set {1,2,3}]
in
    1 in set inds (foo)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943042 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 (let goo = [x | x in set {1,2,3}]
in
    1 in set inds (goo)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "goo" ], 41943077 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 (let goo = [x | x in set {1,2,3}]
in
    (goo(1) - 1) in set inds (goo)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "goo" ], 41943077 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
not (n = 0) =>
 (let foo = [x | x in set {1,2,3}]
in
    (foo(1) - 1) in set inds (A`foo)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943042 ) ) ) }