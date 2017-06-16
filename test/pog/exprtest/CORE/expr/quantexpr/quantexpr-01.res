{ mk_TEST`ProofObligationPP(
     "(forall l1 ^ l2 in set {} &
is_(l1,seq of top))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "QuantExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestF" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall l1 ^ l2 in set {} &
is_(l2,seq of top))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "QuantExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestF" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall l1 ^ l2 : seq1 of top &
is_(l1 ^ l2,set of top))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "QuantExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestF" ], 41943043 ) ) ) }

