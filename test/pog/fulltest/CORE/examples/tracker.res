{ mk_TEST`ProofObligationPP(
     "(forall phases : PhaseInfo &
(forall p1,p2 in set dom (phases) &
p1 <> p2 =>
 p2 in set dom (phases)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "PhasesDistinguished" ], 41943467 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tkr : Tracker, cid : ContainerId, source : PhaseId &
pre_Remove(tkr, cid, source) =>
 TRACKER`pre_Remove(tkr, cid, source))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Remove" ], 41943425 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Delete" ], 41943399 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trk : Tracker, cid : ContainerId, ptoid : PhaseId, pfromid : PhaseId &
Permission(trk, cid, ptoid) and
 pre_Remove(trk, cid, pfromid) =>
 ptoid in set dom (trk.phases))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Move" ], 41943245 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tkr : Tracker, cid : ContainerId, source : PhaseId &
pre_Remove(tkr, cid, source) =>
 TRACKER`inv_Tracker(mk_Tracker({cid} <-: tkr.containers,Remove(tkr, cid, source).phases)))",
     <Invariants>,
     [ mk_AS`Name( [ "Tracker" ], 41943408 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Delete" ], 41943399 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trk : Tracker, cid : ContainerId, quan : real, mat : Material &
cid not in set dom (trk.containers) =>
 TRACKER`inv_Tracker(mk_Tracker(trk.containers munion {cid |-> mk_Container(quan,mat)},trk.phases)))",
     <Invariants>,
     [ mk_AS`Name( [ "Tracker" ], 41943151 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Introduce" ], 41943140 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trk : Tracker, cid : ContainerId, quan : real, mat : Material &
cid not in set dom (trk.containers) =>
 (forall id_5 in set dom (trk.containers), id_6 in set dom ({cid |-> mk_Container(quan,mat)}) &
id_5 = id_6 =>
 trk.containers(id_5) = {cid |-> mk_Container(quan,mat)}(id_6)))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Introduce" ], 41943140 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trk : Tracker, cid : ContainerId, ptoid : PhaseId, pfromid : PhaseId &
Permission(trk, cid, ptoid) and
 pre_Remove(trk, cid, pfromid) =>
 (let pha = mk_Phase(trk.phases(ptoid).contents union {cid},trk.phases(ptoid).expected_materials,trk.phases(ptoid).capacity)
in
    TRACKER`pre_Remove(trk, cid, pfromid)))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Remove" ], 41943302 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Move" ], 41943245 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Tracker(containers,phases) : Tracker, cid : ContainerId, source : PhaseId &
source in set dom (phases) and
 cid in set phases(source).contents =>
 TRACKER`inv_Tracker((let pha = mk_Phase(phases(source).contents \\ {cid},phases(source).expected_materials,phases(source).capacity)
in
    mk_Tracker(containers,phases ++ {source |-> pha}))))",
     <Invariants>,
     [ mk_AS`Name( [ "Tracker" ], 41943338 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Remove" ], 41943329 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Tracker(containers,phases) : Tracker, cid : ContainerId, source : PhaseId &
source in set dom (phases) and
 cid in set phases(source).contents =>
 (let pha = mk_Phase(phases(source).contents \\ {cid},phases(source).expected_materials,phases(source).capacity)
in
    (forall yy_4 in set rng (phases ++ {source |-> pha}) &
TRACKER`inv_Phase(yy_4))))",
     <Invariants>,
     [ mk_AS`Name( [ "Phase" ], 41943084 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Remove" ], 41943329 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trk : Tracker, cid : ContainerId, ptoid : PhaseId, pfromid : PhaseId &
Permission(trk, cid, ptoid) and
 pre_Remove(trk, cid, pfromid) =>
 TRACKER`inv_Tracker((let pha = mk_Phase(trk.phases(ptoid).contents union {cid},trk.phases(ptoid).expected_materials,trk.phases(ptoid).capacity)
in
    mk_Tracker(trk.containers,Remove(trk, cid, pfromid).phases ++ {ptoid |-> pha}))))",
     <Invariants>,
     [ mk_AS`Name( [ "Tracker" ], 41943257 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Move" ], 41943245 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trk : Tracker, cid : ContainerId, ptoid : PhaseId, pfromid : PhaseId &
Permission(trk, cid, ptoid) and
 pre_Remove(trk, cid, pfromid) =>
 (let pha = mk_Phase(trk.phases(ptoid).contents union {cid},trk.phases(ptoid).expected_materials,trk.phases(ptoid).capacity)
in
    (forall yy_2 in set rng (Remove(trk, cid, pfromid).phases ++ {ptoid |-> pha}) &
TRACKER`inv_Phase(yy_2))))",
     <Invariants>,
     [ mk_AS`Name( [ "Phase" ], 41943084 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "TRACKER" ], 41943041 ),
        mk_AS`Name( [ "Move" ], 41943245 ) ) ) }

