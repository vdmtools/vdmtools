{ mk_TEST`ProofObligationPP(
     "(forall mk_(a,b,c) : D &
pre_(b,true))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "b" ], 41943080 ) ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "D" ], 41943064 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A2 | B | C) &
is_real(a({})))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943091 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_(a,b,c) : D &
b(true) =>
 a <> 0)",
     <NonZero>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "D" ], 41943064 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A2 | B | C) &
{} in set dom (a))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943091 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A2 | B | C) &
is_real(a(lambda x : bool & false)))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943091 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A2 | B | C) &
lambda x : bool & false in set dom (a))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943091 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A2 | B | C) &
is_(a,(inmap A2 to nat | inmap bool -> bool to [bool])))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943091 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A2 | B | C) &
is_(a,inmap A2 to nat) =>
 is_A`A2(lambda x : bool & false))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943091 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : (A2 | B | C) &
is_(a,inmap bool -> bool to [bool]) =>
 is_({},bool -> bool))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943091 ) ) ) }

