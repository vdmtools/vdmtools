{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
(def bla = [1,2,3]
in
    id(x) > id(bla(2))))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "foo" ], 41943065 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
(def bla = [1,2,3]
in
    2 in set inds (bla)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ) }

