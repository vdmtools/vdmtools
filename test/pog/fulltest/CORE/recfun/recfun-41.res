{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 (let seqe = [1,2,3,4,5], l = seqe(2,...,4)
in
    x - 1 >= 0))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 (let seqe = [1,2,3,4,5], l = seqe(2,...,4)
in
    id(x) > id(x - 1)))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "foo" ], 41943079 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ) }

