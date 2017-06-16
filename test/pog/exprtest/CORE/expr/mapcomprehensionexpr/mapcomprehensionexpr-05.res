{ mk_TEST`ProofObligationPP(
     "(exists f_5 : map nat to map set of bool to bool &
(forall a : bool, b : nat &
a =>
 (exists i_6 in set dom (f_5) &
f_5(i_6) = {{a} |-> a})))",
     <FiniteMap>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MapComprehensionExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m_1,m_2 in set {{{a} |-> a} | a : bool, b : nat & a} &
(forall id_3 in set dom (m_1), id_4 in set dom (m_2) &
id_3 = id_4 =>
 m_1(id_3) = m_2(id_4)))",
     <MapSetOfCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MapComprehensionExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ) }

