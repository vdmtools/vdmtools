{ mk_TEST`ProofObligationPP(
     "(forall x : nat &
x <> 1000 =>
 not (x = 0) =>
 pre_foo(x) =>
 x - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
x <> 1000 =>
 not (x = 0) =>
 pre_foo(x) =>
 A`pre_foo(x - 1))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "foo" ], 41943066 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall x : nat &
x <> 1000 =>
 not (x = 0) =>
 pre_foo(x) =>
 id(x) > id(x - 1))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "foo" ], 41943066 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943052 ) ) ) }

