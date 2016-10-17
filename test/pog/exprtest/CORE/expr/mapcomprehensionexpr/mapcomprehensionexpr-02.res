{ mk_TEST`ProofObligationPP(
     "is_bool(a)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MapComprehensionExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_real(a)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MapComprehensionExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists f_1 : map nat to map (real | bool) to (real | bool) &
(forall mk_(a,b) : ((((bool | real)) * nat)) &
a =>
 (exists i_2 in set dom (f_1) &
f_1(i_2) = {a |-> (if a then
a * 4
else
a and
 true)})))",
     <FiniteMap>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MapComprehensionExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ) }

