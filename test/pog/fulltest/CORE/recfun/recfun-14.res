{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
(let foo = [1,2,3]
in
    2 in set inds (foo)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "goo" ], 41943065 ) ) ) }