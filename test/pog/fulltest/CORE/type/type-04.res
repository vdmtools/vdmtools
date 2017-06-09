{ mk_TEST`ProofObligationPP(
     "(forall a : (A | B) &
is_(a(99),(bool | B)))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TYPE" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943063 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A | B) &
is_(a,nat -> bool) =>
 pre_(a,99))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "a" ], 41943077 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TYPE" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943063 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A | B) &
is_(a,real -> (A | bool)) =>
 pre_(a,99))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "a" ], 41943077 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TYPE" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943063 ) ) ) }

