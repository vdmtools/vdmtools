{ mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op17" ], 41943426 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall t : nat &
OpDefTest`post_Op12(t))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op12" ], 41943295 ) ) ),
  mk_TEST`ProofObligationPP(
     "OpDefTest`pre_Op4() =>
 (exists r : nat &
OpDefTest`post_Op4(r))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op4" ], 41943132 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall aa : nat1 &
(exists r : char &
OpDefTest`post_Op18(aa, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "OpDefTest" ], 41943041 ),
        mk_AS`Name( [ "Op18" ], 41943442 ) ) ),
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
        mk_AS`Name( [ "Op7" ], 41943219 ) ) ),
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
        mk_AS`Name( [ "Op1" ], 41943059 ) ) ),
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
        mk_AS`Name( [ "Op2" ], 41943084 ) ) ),
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
        mk_AS`Name( [ "Op3" ], 41943104 ) ) ),
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
        mk_AS`Name( [ "Op5" ], 41943147 ) ) ),
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
        mk_AS`Name( [ "Op6" ], 41943168 ) ) ) }

