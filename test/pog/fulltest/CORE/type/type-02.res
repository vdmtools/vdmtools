{ mk_TEST`ProofObligationPP(
     "(forall mk_D(a,b,c) : D &
is_(b,bool -> bool) =>
 pre_(b,true))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "b" ], 41943067 ) ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "D" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_D(a,b,c) : D &
is_(b,seq of bool) =>
 is_nat1(true))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "D" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall arg : (D | E) &
is_((arg.f),seq of bool) =>
 is_nat1(true))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "test" ], 41943097 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_D(a,b,c) : D &
is_(b,seq of bool) =>
 true in set inds (b))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "D" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall arg : (D | E) &
is_((arg.f),map real to bool) =>
 is_real(true))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "test" ], 41943097 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall arg : (D | E) &
is_(arg.f,seq of bool) =>
 true in set inds (arg.f))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "test" ], 41943097 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall arg : (D | E) &
is_(arg.f,map real to bool) =>
 true in set dom (arg.f))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "test" ], 41943097 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall arg : (D | E) &
is_(arg.f,bool -> bool) =>
 is_(arg.f,compose E of r : real
f : (bool -> bool | map real to bool)
end) =>
 pre_(arg.f,true))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "f" ], 41943109 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "test" ], 41943097 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall arg : (D | E) &
is_(arg.f,bool -> bool) =>
 is_(arg.f,compose D of n : nat
f : (seq of bool | bool -> bool)
t : token
end) =>
 pre_(arg.f,true))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "f" ], 41943109 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "test" ], 41943097 ) ) ) }

