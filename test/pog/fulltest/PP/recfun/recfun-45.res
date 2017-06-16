{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
x >= 1 =>
 not (x = 1) =>
 x - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
x >= 1 =>
 not (x = 1) =>
 A`pre_foo(x - 1))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "foo" ], 41943063 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
x >= 1 =>
 not (x = 1) =>
 id(x) > id(x - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "foo" ], 41943063 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ) }
