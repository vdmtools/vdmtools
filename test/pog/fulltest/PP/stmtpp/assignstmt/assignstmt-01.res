{ mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "AssignStmtTest" ], 41943053 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "AssignStmtTest" ], 41943053 ),
        mk_AS`Name( [ "Assign1" ], 41943102 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "AssignStmtTest" ], 41943053 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "AssignStmtTest" ], 41943053 ),
        mk_AS`Name( [ "Assign2" ], 41943108 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "AssignStmtTest" ], 41943053 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "AssignStmtTest" ], 41943053 ),
        mk_AS`Name( [ "Assign4" ], 41943116 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a1,a2 : real &
is_nat(a1))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "AssignStmtTest" ], 41943053 ),
        mk_AS`Name( [ "Assign4" ], 41943116 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a1,a2 : real &
is_nat(a2))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "AssignStmtTest" ], 41943053 ),
        mk_AS`Name( [ "Assign4" ], 41943116 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a1,a2 : real &
AssignStmtTest`post_Assign4(a1, a2))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "AssignStmtTest" ], 41943053 ),
        mk_AS`Name( [ "Assign4" ], 41943116 ) ) ) }

