{ mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op14" ], 41943353 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op17" ], 41943452 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_nat(RESULT)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op14" ], 41943353 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_real(RESULT)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op14" ], 41943353 ) ) ),
  mk_TEST`ProofObligationPP(
     "OpDefTest`pre_myf(a)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "post_myf" ], 41943389 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op14" ], 41943353 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall t : nat &
OpDefTest`post_Op12(t))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op12" ], 41943294 ) ) ),
  mk_TEST`ProofObligationPP(
     "OpDefTest`pre_Op4() =>
 (exists r : nat &
OpDefTest`post_Op4(r))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op4" ], 41943131 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall aa : nat1 &
(exists r : char &
OpDefTest`post_Op18(aa, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op18" ], 41943468 ) ) ),
  mk_TEST`ProofObligationPP(
     "aa = mk_A((1),(true)) or
 (exists mk_A(a,b) : A &
aa = mk_A(a,b))",
     <CasesExpr>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op15" ], 41943395 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall n : nat &
OpDefTest`pre_myf(n) =>
 OpDefTest`post_myf(n, n + 9))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "myf" ], 41943333 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall g : bool &
OpDefTest`pre_Op7(g) =>
 (exists r : rat &
OpDefTest`post_Op7(g, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op7" ], 41943218 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall c,d : nat &
OpDefTest`pre_Op1(c, d) =>
 (exists r : nat &
OpDefTest`post_Op1(c, d, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op1" ], 41943058 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall c,d : nat &
OpDefTest`pre_Op2(c, d) =>
 (exists r : nat &
OpDefTest`post_Op2(c, d, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op2" ], 41943083 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall c,d : nat &
OpDefTest`pre_Op3(c, d) =>
 (exists r : nat &
OpDefTest`post_Op3(c, d, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op3" ], 41943103 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall c,d : nat &
OpDefTest`pre_Op5(c, d) =>
 (exists r : nat &
OpDefTest`post_Op5(c, d, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op5" ], 41943146 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall e,f : int &
OpDefTest`pre_Op6(e, f) =>
 (exists r : nat &
OpDefTest`post_Op6(e, f, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op6" ], 41943167 ) ) ) }

