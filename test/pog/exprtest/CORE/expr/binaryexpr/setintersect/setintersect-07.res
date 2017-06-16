{ mk_TEST`ProofObligationPP(
     "(forall m_1,m_2 in set {{mk_T('a') |-> 2},{mk_T1('r') |-> 5}} &
(forall id_3 in set dom (m_1), id_4 in set dom (m_2) &
id_3 = id_4 =>
 m_1(id_3) = m_2(id_4)))",
     <MapSeqOfCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <values>,
        mk_AS`Name( [ "BinaryTest" ], 41943041 ),
        mk_AS`Name( [ "v1" ], 41943043 ) ) ) }

