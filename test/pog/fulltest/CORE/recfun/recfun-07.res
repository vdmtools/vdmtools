{ mk_TEST`ProofObligationPP(
     "(forall l : seq of nat &
l = ([]) or
 (exists [h] ^ t : seq of nat &
l = [h] ^ t))",
     <CasesExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "revers" ], 41943064 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall l : seq of nat, g : Graph, vis : seq of nat &
l = ([]) or
 (exists [h] ^ t : seq of nat &
l = [h] ^ t))",
     <CasesExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "depthf" ], 41943094 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall l : seq of nat, g : Graph, vis : seq of nat &
not (l = ([])) =>
 (exists [h] ^ t : seq of nat &
l = [h] ^ t) =>
 (let [h] ^ t = l
in
    not (h in set elems (vis)) =>
 h in set dom (g)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "depthf" ], 41943094 ) ) ) }