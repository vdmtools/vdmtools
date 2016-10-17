{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 x - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "odd" ], 41943082 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 x - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "even" ], 41943062 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 ideven(x) > idodd(x - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "odd" ], 41943073 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "even" ], 41943062 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
not (x = 0) =>
 idodd(x) > ideven(x - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "even" ], 41943093 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "odd" ], 41943082 ) ) ) }

