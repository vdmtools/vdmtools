{ mk_TEST`ProofObligationPP(
     "(exists dummy : nat &
A`post_MetA1(dummy))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "A" ], 41943080 ),
        mk_AS`Name( [ "MetA1" ], 41943081 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists dummy : bool &
Pars1`post_MetPars3(dummy))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Pars1" ], 41943041 ),
        mk_AS`Name( [ "MetPars3" ], 41943072 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists dummy : B &
InvokeStmtTest`post_InvokeStmt1(dummy))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt1" ], 41943164 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists dummy : nat &
InvokeStmtTest`post_InvokeStmt7(dummy))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt7" ], 41943221 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists dummy : bool &
InvokeStmtTest`post_InvokeStmt9(dummy))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt9" ], 41943247 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists dummy : nat &
InvokeStmtTest`post_InvokeStmt11(dummy))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt11" ], 41943258 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists dummy : real &
InvokeStmtTest`post_InvokeStmt8(dummy))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt8" ], 41943235 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall arg : nat &
(exists dummy : A &
B`post_MetB1(arg, dummy)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "B" ], 41943091 ),
        mk_AS`Name( [ "MetB1" ], 41943097 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a,b,c : nat &
(exists dummy : nat &
Pars1`post_MetPars1(a, b, c, dummy)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Pars1" ], 41943041 ),
        mk_AS`Name( [ "MetPars1" ], 41943042 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall b2 : B &
(exists dummy : B &
InvokeStmtTest`post_InvokeStmt12(b2, dummy)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt12" ], 41943270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall d : nat &
(exists dummy : nat &
InvokeStmtTest`post_InvokeStmt2(d, dummy)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt2" ], 41943181 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m : map bool to nat &
(exists dummy : real &
Pars1`post_MetPars2(m, dummy)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Pars1" ], 41943041 ),
        mk_AS`Name( [ "MetPars2" ], 41943060 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
(exists dummy : nat &
InvokeStmtTest`post_InvokeStmt13(n, dummy)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt13" ], 41943286 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall arg : bool &
(exists dummy : (A | B) &
InvokeStmtTest`post_InvokeStmt3(arg, dummy)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InvokeStmtTest" ], 41943142 ),
        mk_AS`Name( [ "InvokeStmt3" ], 41943201 ) ) ) }

