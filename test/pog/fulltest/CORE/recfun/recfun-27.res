{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
(def foo = [x | x in set {1,2,3}]
in
    2 in set inds (foo)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ) }

