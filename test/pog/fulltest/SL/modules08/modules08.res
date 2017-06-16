{ mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "C" ], 41943081 ),
        mk_AS`Name( [ "op3" ], 41943146 ) ) ),
  mk_TEST`ProofObligationPP(
     "j in set inds (l)",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "C" ], 41943081 ),
        mk_AS`Name( [ "op3" ], 41943146 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(B`l,seq of nat)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "A" ], 41943346 ),
        mk_AS`Name( [ "Test" ], 41943417 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists e in set s &
(forall e1 in set s \\ {e} &
e1 < e))",
     <LetBeStStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "C" ], 41943081 ),
        mk_AS`Name( [ "op1" ], 41943092 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(res ^ [is_E`C(c),is_E`D(d),d.a,d.b],seq of (set of real | seq of real | real | token | bool))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "A" ], 41943346 ),
        mk_AS`Name( [ "Test" ], 41943417 ) ) ) }

