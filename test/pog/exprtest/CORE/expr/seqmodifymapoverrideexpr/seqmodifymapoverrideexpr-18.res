{ mk_TEST`ProofObligationPP(
     "dom ({abc |-> t,6 |-> 88.7000000000000028421709430404007434844970703125}) subset inds (m)",
     <SeqModification>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "SeqModifyMapOverrideExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943079 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(m ++ {abc |-> t,6 |-> 88.7000000000000028421709430404007434844970703125},(seq of nat | map nat to nat))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "SeqModifyMapOverrideExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943079 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(m ++ {abc |-> t,6 |-> 88.7000000000000028421709430404007434844970703125},((map nat to nat) | seq of nat))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "SeqModifyMapOverrideExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943079 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m_1,m_2 in set {{abc |-> t},{6 |-> 88.7000000000000028421709430404007434844970703125}} &
(forall id_3 in set dom (m_1), id_4 in set dom (m_2) &
id_3 = id_4 =>
 m_1(id_3) = m_2(id_4)))",
     <MapSeqOfCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "SeqModifyMapOverrideExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943079 ) ) ) }

