{ mk_TEST`ProofObligationPP(
     "(exists f_1 : map nat to bool &
(forall b : bool &
(exists i_2 in set dom (f_1) &
f_1(i_2) = b)))",
     <FiniteSet>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "SetComprehensionExpr" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ) }

