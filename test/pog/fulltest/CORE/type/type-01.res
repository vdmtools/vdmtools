{ mk_TEST`ProofObligationPP(
     "(forall mk_(a,b,c) : D &
pre_(b,true))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "b" ], 41943081 ) ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "D" ], 41943065 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_(a,b,c) : D &
b(true) =>
 a <> 0)",
     <NonZero>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "D" ], 41943065 ) ) ) }

