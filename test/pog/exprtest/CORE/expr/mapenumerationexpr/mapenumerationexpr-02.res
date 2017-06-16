{ mk_TEST`ProofObligationPP(
     "(forall m_1,m_2 in set {{2 |-> 6},{2 |-> true}} &
(forall id_3 in set dom (m_1), id_4 in set dom (m_2) &
id_3 = id_4 =>
 m_1(id_3) = m_2(id_4)))",
     <MapSeqOfCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MapEnumerationExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ) }

