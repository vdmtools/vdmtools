{ mk_TEST`ProofObligationPP(
     "(forall ps : Path_map &
(forall pid1,pid2 in set dom (ps) &
pid1 <> pid2 =>
 pid2 in set dom (ps)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_map" ], 41943993 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, p : Path &
Path_within_TrackC(trackcs, p) =>
 p.tc in set dom (trackcs))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Direction_Correct" ], 41944292 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, p : Path &
Path_within_TrackC(trackcs, p) =>
 p.startr in set dom (trackcs(p.tc).joints))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Direction_Correct" ], 41944292 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall path : Path, dr : Direction &
Interlock`post_EndJoint(path, dr, (if dr = <ADIR> then
path.endp
else
path.startr)))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "EndJoint" ], 41949489 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, rid : Route_id &
rid in set dom (ar.routes) =>
 Interlock`inv_Area(mu(ar,routes|->{rid} <-: ar.routes)))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41945672 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Route" ], 41945666 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall path : Path, dr : Direction &
Interlock`post_StartJoint(path, dr, (if dr = <ADIR> then
path.startr
else
path.endp)))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "StartJoint" ], 41949442 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area &
(forall pid in set dom (ar.paths) &
(forall dr in set dom (ar.paths(pid).used) &
pid in set dom (ar.paths))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_for_Path" ], 41949080 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, kind : Area_Kind, max : MaxSpeed &
aid not in set dom (ln.areas) =>
 Interlock`inv_Path_map({|->}))",
     <Invariants>,
     [ mk_AS`Name( [ "Path_map" ], 41944153 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Area" ], 41946784 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall routes : Route_map, rid : Route_id &
rid in set dom (routes) =>
 (forall rid1 in set dom (routes) &
rid in set dom (routes)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Route_Exists" ], 41949667 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, kind : Area_Kind, max : MaxSpeed &
aid not in set dom (ln.areas) =>
 Interlock`inv_TrackC_map({|->}))",
     <Invariants>,
     [ mk_AS`Name( [ "TrackC_map" ], 41944149 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Area" ], 41946784 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ps : Path_map &
(forall pid1,pid2 in set dom (ps) &
pid1 <> pid2 =>
 Not_Same_Path(ps(pid1), ps(pid2)) =>
 pid2 in set dom (ps)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_map" ], 41943993 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id &
Interlock`pre_Del_Path(ar, pid) =>
 Interlock`post_Del_Path(ar, pid, mu(ar,paths|->{pid} <-: ar.paths)))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Path" ], 41945463 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id &
Interlock`pre_Del_Area(ln, aid) =>
 Interlock`post_Del_Area(ln, aid, mu(ln,areas|->{aid} <-: ln.areas)))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Area" ], 41946918 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) =>
 (let p = ar.paths(pid)
in
    p.tc in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, pid : Path_id, dr : Direction &
pid in set dom (paths) =>
 (forall pid1 in set dom (paths) &
pid in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Unique" ], 41949742 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, rid : Route_id &
Interlock`pre_Del_Route(ar, rid) =>
 Interlock`post_Del_Route(ar, rid, mu(ar,routes|->{rid} <-: ar.routes)))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Route" ], 41945666 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, r : Route &
Path_Exists(paths, r.paths, r.dr) =>
 (forall i,j in set inds (r.paths) &
i <> j =>
 j in set inds (r.paths)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_not_Circular" ], 41944474 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id &
Interlock`pre_Del_TrackC(ar, tcid) =>
 Interlock`post_Del_TrackC(ar, tcid, mu(ar,trackcs|->{tcid} <-: ar.trackcs)))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_TrackC" ], 41944733 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, r : Route &
Path_Exists(paths, r.paths, r.dr) =>
 (forall i,j in set inds (r.paths) &
i <> j =>
 r.paths(i) in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_not_Circular" ], 41944474 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, r : Route &
Path_Exists(paths, r.paths, r.dr) =>
 (forall i,j in set inds (r.paths) &
i <> j =>
 r.paths(j) in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_not_Circular" ], 41944474 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, paths : Path_map &
(forall tid in set dom (trackcs) &
(forall dr in set dom (trackcs(tid).atc) &
tid in set dom (trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_for_TrackC" ], 41949022 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area &
(forall pid in set dom (ar.paths) &
(forall dr in set dom (ar.paths(pid).used) &
ar.paths(pid).used(dr) =>
 pid in set dom (ar.paths))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_for_Path" ], 41949080 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) =>
 (let p = ar.paths(pid)
in
    p.startr in set dom (ar.trackcs(p.tc).joints)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, paths : Path_map, r : Route &
Path_Exists(paths, r.paths, r.dr) =>
 (forall pid in set elems (r.paths) &
pid in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Exists_ATC_for_Route" ], 41944413 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id &
aid in set dom (ln.areas) and
 pre_Del_Path(ln.areas(aid), pid) =>
 Interlock`pre_Del_Path(ln.areas(aid), pid))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Del_Path" ], 41948273 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Path" ], 41948248 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Area(trackcs,paths,routes,-,-) : Area &
(forall p in set rng (paths) &
Path_within_TrackC(trackcs, p) =>
 Interlock`pre_Direction_Correct(trackcs, p)))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Direction_Correct" ], 41944187 ) ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Area" ], 41944147 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, route : seq of Path_id, dr : Direction &
(forall pid in set elems (route) &
pid in set dom (paths) =>
 dr in set dom (paths(pid).used)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists" ], 41944374 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall routes : Route_map, rid : Route_id &
rid in set dom (routes) =>
 (forall rid1 in set dom (routes) &
(let r = routes(rid)
in
    rid1 in set dom (routes))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Route_Exists" ], 41949667 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, rid : Route_id &
aid in set dom (ln.areas) and
 pre_Del_Route(ln.areas(aid), rid) =>
 Interlock`pre_Del_Route(ln.areas(aid), rid))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Del_Route" ], 41948540 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Route" ], 41948515 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, p : Path &
Path_within_TrackC(trackcs, p) =>
 (let pstart = trackcs(p.tc).joints(p.startr).position
in
    p.tc in set dom (trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Direction_Correct" ], 41944292 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall rm1 : Remark, rm2 : Remark, dir_chng : bool &
(not (dir_chng) =>
 rm1.atc_terminal = rm2.atc_terminal) =>
 dir_chng =>
 <ADIR> in set dom (rm1.atc_terminal))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Remark_Compatible" ], 41943550 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall rm1 : Remark, rm2 : Remark, dir_chng : bool &
(not (dir_chng) =>
 rm1.atc_terminal = rm2.atc_terminal) =>
 dir_chng =>
 <BDIR> in set dom (rm2.atc_terminal))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Remark_Compatible" ], 41943550 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid)
in
    (len (r.paths)) in set inds (r.paths))))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, rid : Route_id, r : Route &
Interlock`pre_Add_Route(ar, rid, r) =>
 Interlock`post_Add_Route(ar, rid, r, mu(ar,routes|->ar.routes ++ {rid |-> r})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Route" ], 41945543 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
n1.aid in set dom (ln.areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
n1.aid in set dom (ln.areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
n1.aid in set dom (ln.areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area &
(forall pid in set dom (ar.paths) &
(forall dr in set dom (ar.paths(pid).used) &
ar.paths(pid).used(dr) =>
 ar.paths(pid).tc in set dom (ar.trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_for_Path" ], 41949080 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, path : Path &
Interlock`pre_Add_Path(ar, pid, path) =>
 Interlock`post_Add_Path(ar, pid, path, mu(ar,paths|->ar.paths ++ {pid |-> path})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Path" ], 41945349 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id &
tcid in set dom (ar.trackcs) and
 (forall p in set rng (ar.paths) &
p.tc <> tcid) =>
 Interlock`inv_TrackC_map({tcid} <-: ar.trackcs))",
     <Invariants>,
     [ mk_AS`Name( [ "TrackC_map" ], 41944149 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_TrackC" ], 41944733 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, path : Path &
pid not in set dom (ar.paths) and
 Path_within_TrackC(ar.trackcs, path) =>
 Interlock`pre_Direction_Correct(ar.trackcs, path))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Direction_Correct" ], 41945394 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Path" ], 41945349 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
tcid in set dom (ar.trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, area : Area &
Interlock`pre_Change_Area(ln, aid, area) =>
 Interlock`post_Change_Area(ln, aid, area, mu(ln,areas|->ln.areas ++ {aid |-> area})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Change_Area" ], 41946848 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area &
(forall pid in set dom (ar.paths) &
(forall dr in set dom (ar.paths(pid).used) &
ar.paths(pid).used(dr) =>
 dr in set dom (ar.trackcs(ar.paths(pid).tc).atc))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_for_Path" ], 41949080 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, p : Path &
Path_within_TrackC(trackcs, p) =>
 (let pstart = trackcs(p.tc).joints(p.startr).position
in
    p.endp in set dom (trackcs(p.tc).joints)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Direction_Correct" ], 41944292 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id &
pid in set dom (ar.paths) and
 (forall r in set rng (ar.routes) &
pid not in set elems (r.paths)) =>
 Interlock`inv_Path_map({pid} <-: ar.paths))",
     <Invariants>,
     [ mk_AS`Name( [ "Path_map" ], 41944153 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Path" ], 41945463 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, tc : TrackC &
Interlock`pre_Add_TrackC(ar, tcid, tc) =>
 Interlock`post_Add_TrackC(ar, tcid, tc, mu(ar,trackcs|->ar.trackcs ++ {tcid |-> tc})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_TrackC" ], 41944617 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
Interlock`pre_Add_Connect(ln, con, r) =>
 Interlock`post_Add_Connect(ln, con, r, mu(ln,connect|->ln.connect ++ {con |-> r})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, rid : Route_id, r : Route &
aid in set dom (ln.areas) and
 pre_Add_Route(ln.areas(aid), rid, r) =>
 Interlock`pre_Add_Route(ln.areas(aid), rid, r))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Add_Route" ], 41948401 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Route" ], 41948371 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, n : Area_Joint &
(exists c in set dom (ln.connect) &
n in set c) =>
 Interlock`inv_Connect_map({c |-> ln.connect(c) | c in set dom (ln.connect) & n not in set c}))",
     <Invariants>,
     [ mk_AS`Name( [ "Connect_map" ], 41946290 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Connect" ], 41947186 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id &
tcid in set dom (ar.trackcs) and
 (forall p in set rng (ar.paths) &
p.tc <> tcid) =>
 Interlock`inv_Area(mu(ar,trackcs|->{tcid} <-: ar.trackcs)))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41944739 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_TrackC" ], 41944733 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths))
in
    pid in set dom (ar.paths))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, rid : Route_id, r : Route &
rid not in set dom (ar.routes) and
 Path_Exists(ar.paths, r.paths, r.dr) =>
 Interlock`pre_Exists_ATC_for_Route(ar.trackcs, ar.paths, r))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Exists_ATC_for_Route" ], 41945593 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Route" ], 41945543 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tc : TrackC &
card (dom (tc.joints)) > 1 and
 dom (tc.atc) = {<ADIR>,<BDIR>} and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints)) =>
 <ADIR> in set dom (tc.atc))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "TrackC" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, pid : Path_id, dr : Direction &
pid in set dom (paths) =>
 (forall pid1 in set dom (paths) &
paths(pid1).tc <> paths(pid).tc =>
 dr in set dom (paths(pid1).used)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Unique" ], 41949742 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id &
pid in set dom (ar.paths) and
 (forall r in set rng (ar.routes) &
pid not in set elems (r.paths)) =>
 Interlock`inv_Area(mu(ar,paths|->{pid} <-: ar.paths)))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41945469 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Path" ], 41945463 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, n : Area_Joint &
(exists c in set dom (ln.connect) &
n in set c) =>
 Interlock`inv_Line(mu(ln,connect|->{c |-> ln.connect(c) | c in set dom (ln.connect) & n not in set c})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41947192 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Connect" ], 41947186 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, n : Area_Joint &
Interlock`pre_Del_Connect(ln, n) =>
 Interlock`post_Del_Connect(ln, n, mu(ln,connect|->{c |-> ln.connect(c) | c in set dom (ln.connect) & n not in set c})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Connect" ], 41947186 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 c in set dom (connect))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 n1.aid in set dom (areas))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 n2.aid in set dom (areas))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, kind : Area_Kind, max : MaxSpeed &
aid not in set dom (ln.areas) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> mk_Area({|->},{|->},{|->},kind,max)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41946796 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Area" ], 41946784 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, con : Condition &
aid in set dom (ln.areas) and
 pre_Add_Condition(ln.areas(aid), pid, con) =>
 Interlock`pre_Add_Condition(ln.areas(aid), pid, con))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Add_Condition" ], 41948668 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Condition" ], 41948638 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, paths : Path_map, r : Route &
Path_Exists(paths, r.paths, r.dr) =>
 (forall pid in set elems (r.paths) &
paths(pid).tc in set dom (trackcs) =>
 pid in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Exists_ATC_for_Route" ], 41944413 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id &
aid in set dom (ln.areas) and
 ((forall c in set dom (ln.connect) &
(forall aj in set c &
aj.aid <> aid))) =>
 Interlock`inv_Line(mu(ln,areas|->{aid} <-: ln.areas)))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41946924 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Area" ], 41946918 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, area : Area &
aid in set dom (ln.areas) and
 inv_Line(mk_Line(ln.areas ++ {aid |-> area},ln.connect)) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> area})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41946857 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Change_Area" ], 41946848 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) =>
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr =>
 p.tc in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall atcA1 : ATC, atcB1 : ATC, rm1 : Remark, atcA2 : ATC, atcB2 : ATC, rm2 : Remark, dir_chng : bool &
Remark_Compatible(rm1, rm2, dir_chng) =>
 not (dir_chng) =>
 <ADIR> in set dom (rm1.atc_terminal))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "ATC_Terminal_and_ATC_Used" ], 41943608 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id &
aid in set dom (ln.areas) and
 pre_Del_Path(ln.areas(aid), pid) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Del_Path(ln.areas(aid), pid)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41948257 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Path" ], 41948248 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, pid : Path_id, dr : Direction &
pid in set dom (paths) =>
 (forall pid1 in set dom (paths) &
paths(pid1).tc <> paths(pid).tc and
 paths(pid1).used(dr) =>
 pid in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Unique" ], 41949742 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id &
Interlock`pre_Line_Del_Path(ln, aid, pid) =>
 Interlock`post_Line_Del_Path(ln, aid, pid, mu(ln,areas|->ln.areas ++ {aid |-> Del_Path(ln.areas(aid), pid)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Path" ], 41948248 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, rid : Route_id &
aid in set dom (ln.areas) and
 pre_Del_Route(ln.areas(aid), rid) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Del_Route(ln.areas(aid), rid)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41948524 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Route" ], 41948515 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
(forall p in set rng (ln.areas(n1.aid).paths) &
dr in set dom (p.used))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
(forall p in set rng (ln.areas(n1.aid).paths) &
dr in set dom (p.used))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
(forall p in set rng (ln.areas(n1.aid).paths) &
dr in set dom (p.used))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, rid : Route_id &
Interlock`pre_Line_Del_Route(ln, aid, rid) =>
 Interlock`post_Line_Del_Route(ln, aid, rid, mu(ln,areas|->ln.areas ++ {aid |-> Del_Route(ln.areas(aid), rid)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Route" ], 41948515 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, route : seq of Path_id, dr : Direction &
Path_Exists(paths, route, dr) =>
 (forall i in set inds (route) &
(i + 1) in set inds (route) =>
 dr = <ADIR> =>
 route(i) in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Connected" ], 41944531 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tc : TrackC &
card (dom (tc.joints)) > 1 and
 dom (tc.atc) = {<ADIR>,<BDIR>} and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints)) =>
 tc.atc(<ADIR>).used =>
 <BDIR> in set dom (tc.atc))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "TrackC" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tcs : TrackC_map &
(forall tid in set dom (tcs) &
(forall jid in set dom (tcs(tid).joints) &
Only_One_Next_TrackC(tcs, tid, jid) =>
 (forall tid2 in set dom (tcs) &
tid <> tid2 =>
 tid in set dom (tcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "TrackC_map" ], 41943260 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, route : seq of Path_id, dr : Direction &
Path_Exists(paths, route, dr) =>
 (forall i in set inds (route) &
(i + 1) in set inds (route) =>
 dr = <ADIR> =>
 route(i + 1) in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Connected" ], 41944531 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, kind : Area_Kind, max : MaxSpeed &
aid not in set dom (ln.areas) =>
 (forall yy_4 in set rng (ln.areas ++ {aid |-> mk_Area({|->},{|->},{|->},kind,max)}) &
Interlock`inv_Area(yy_4)))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41946426 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Area" ], 41946784 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) =>
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr =>
 p.endp in set dom (ar.trackcs(p.tc).joints)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id &
Interlock`pre_Line_Del_TrackC(ln, aid, tcid) =>
 Interlock`post_Line_Del_TrackC(ln, aid, tcid, mu(ln,areas|->ln.areas ++ {aid |-> Del_TrackC(ln.areas(aid), tcid)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_TrackC" ], 41947432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, paths : Path_map, r : Route &
Path_Exists(paths, r.paths, r.dr) =>
 (forall pid in set elems (r.paths) &
paths(pid).tc in set dom (trackcs) =>
 r.dr in set dom (trackcs(paths(pid).tc).atc)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Exists_ATC_for_Route" ], 41944413 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths)), jid = EndJoint(ar.paths(pid), r.dr)
in
    pid in set dom (ar.paths))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, rid : Route_id, r : Route &
rid not in set dom (ar.routes) and
 Path_Exists(ar.paths, r.paths, r.dr) and
 Exists_ATC_for_Route(ar.trackcs, ar.paths, r) =>
 Interlock`pre_Route_not_Circular(ar.paths, r))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Route_not_Circular" ], 41945603 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Route" ], 41945543 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall rm1 : Remark, rm2 : Remark, dir_chng : bool &
(not (dir_chng) =>
 rm1.atc_terminal = rm2.atc_terminal) =>
 dir_chng =>
 rm1.atc_terminal(<ADIR>) = rm2.atc_terminal(<BDIR>) =>
 <ADIR> in set dom (rm2.atc_terminal))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Remark_Compatible" ], 41943550 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall rm1 : Remark, rm2 : Remark, dir_chng : bool &
(not (dir_chng) =>
 rm1.atc_terminal = rm2.atc_terminal) =>
 dir_chng =>
 rm1.atc_terminal(<ADIR>) = rm2.atc_terminal(<BDIR>) =>
 <BDIR> in set dom (rm1.atc_terminal))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Remark_Compatible" ], 41943550 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, rid : Route_id, r : Route &
aid in set dom (ln.areas) and
 pre_Add_Route(ln.areas(aid), rid, r) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Add_Route(ln.areas(aid), rid, r)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41948383 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Route" ], 41948371 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, p : Path &
Interlock`pre_Line_Add_Path(ln, aid, pid, p) =>
 Interlock`post_Line_Add_Path(ln, aid, pid, p, mu(ln,areas|->ln.areas ++ {aid |-> Add_Path(ln.areas(aid), pid, p)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Path" ], 41948049 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall trackcs : TrackC_map, paths : Path_map &
(forall tid in set dom (trackcs) &
(forall dr in set dom (trackcs(tid).atc) &
trackcs(tid).atc(dr).used =>
 (forall p in set rng (paths) &
p.tc = tid =>
 dr in set dom (p.used)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_for_TrackC" ], 41949022 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, rid : Route_id, r : Route &
Interlock`pre_Line_Add_Route(ln, aid, rid, r) =>
 Interlock`post_Line_Add_Route(ln, aid, rid, r, mu(ln,areas|->ln.areas ++ {aid |-> Add_Route(ln.areas(aid), rid, r)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Route" ], 41948371 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, kind : Area_Kind, max : MaxSpeed &
Interlock`pre_Add_Area(ln, aid, kind, max) =>
 Interlock`post_Add_Area(ln, aid, kind, max, mu(ln,areas|->ln.areas ++ {aid |-> mk_Area({|->},{|->},{|->},kind,max)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Area" ], 41946784 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tc : TrackC &
card (dom (tc.joints)) > 1 and
 dom (tc.atc) = {<ADIR>,<BDIR>} and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints)) =>
 tc.atc(<ADIR>).used and
 tc.atc(<BDIR>).used =>
 <ADIR> in set dom (tc.atc))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "TrackC" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tc : TrackC &
card (dom (tc.joints)) > 1 and
 dom (tc.atc) = {<ADIR>,<BDIR>} and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints)) =>
 tc.atc(<ADIR>).used and
 tc.atc(<BDIR>).used =>
 <BDIR> in set dom (tc.atc))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "TrackC" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, area : Area &
aid in set dom (ln.areas) and
 inv_Line(mk_Line(ln.areas ++ {aid |-> area},ln.connect)) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> area})
in
    aid in set dom (RESULT.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Change_Area" ], 41946848 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, con : Condition &
aid in set dom (ln.areas) and
 pre_Add_Condition(ln.areas(aid), pid, con) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Add_Condition(ln.areas(aid), pid, con)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41948650 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Condition" ], 41948638 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, tc : TrackC &
Interlock`pre_Line_Add_TrackC(ln, aid, tcid, tc) =>
 Interlock`post_Line_Add_TrackC(ln, aid, tcid, tc, mu(ln,areas|->ln.areas ++ {aid |-> Add_TrackC(ln.areas(aid), tcid, tc)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_TrackC" ], 41947254 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id &
Interlock`pre_Line_Del_Joint(ln, aid, tcid, jid) =>
 Interlock`post_Line_Del_Joint(ln, aid, tcid, jid, mu(ln,areas|->ln.areas ++ {aid |-> Del_Joint(ln.areas(aid), tcid, jid)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Joint" ], 41947824 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) =>
 (forall n1,n2 in set con &
n1 <> n2 =>
 n1.aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) =>
 (forall n1,n2 in set con &
n1 <> n2 =>
 n2.aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall routes : Route_map, rid : Route_id &
rid in set dom (routes) =>
 (forall rid1 in set dom (routes) &
(let r = routes(rid), r1 = routes(rid1)
in
    r1.dr = r.dr =>
 (forall i in set inds (r1.paths) &
(len (r.paths)) in set inds (r.paths)))))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Route_Exists" ], 41949667 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, jid : Joint_id &
Interlock`pre_Del_Joint(ar, tcid, jid) =>
 Interlock`post_Del_Joint(ar, tcid, jid, (let tc = ar.trackcs(tcid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tcid |-> mu(tc,joints|->{jid} <-: tc.joints)}))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Joint" ], 41945093 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, p : Path &
aid in set dom (ln.areas) and
 pre_Add_Path(ln.areas(aid), pid, p) =>
 (forall c in set dom (ln.connect) &
(forall n1,n2 in set c &
(n1 <> n2 and
 n1.aid = aid) =>
 c in set dom (ln.connect))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Path" ], 41948049 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
aid in set dom (ln.areas) and
 pre_Del_Condition(ln.areas(aid), pid, kind, startr, endp) =>
 Interlock`pre_Del_Condition(ln.areas(aid), pid, kind, startr, endp))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Del_Condition" ], 41948738 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Condition" ], 41948700 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, p : Path &
aid in set dom (ln.areas) and
 pre_Add_Path(ln.areas(aid), pid, p) =>
 (forall c in set dom (ln.connect) &
(forall n1,n2 in set c &
(n1 <> n2 and
 n1.aid = aid) =>
 n2.aid in set dom (ln.areas))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Path" ], 41948049 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 EndJoint(p, dr) = n1.no)) =>
 n1.aid in set dom (ln.areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 StartJoint(p, dr) = n1.no)) =>
 n1.aid in set dom (ln.areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, p : Path &
aid in set dom (ln.areas) and
 pre_Add_Path(ln.areas(aid), pid, p) =>
 (forall c in set dom (ln.connect) &
(forall n1,n2 in set c &
(n1 <> n2 and
 n1.aid = aid) =>
 Interlock`inv_Path_map({pid |-> p}))))",
     <Invariants>,
     [ mk_AS`Name( [ "Path_map" ], 41946627 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Path" ], 41948049 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
Interlock`pre_Add_Condition(ar, pid, con) =>
 Interlock`post_Add_Condition(ar, pid, con, (let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall atcA1 : ATC, atcB1 : ATC, rm1 : Remark, atcA2 : ATC, atcB2 : ATC, rm2 : Remark, dir_chng : bool &
Remark_Compatible(rm1, rm2, dir_chng) =>
 not (dir_chng) =>
 ((atcA1.used <> atcA2.used) = rm1.atc_terminal(<ADIR>)) =>
 <BDIR> in set dom (rm1.atc_terminal))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "ATC_Terminal_and_ATC_Used" ], 41943608 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, rid : Route_id, r : Route &
rid not in set dom (ar.routes) and
 Path_Exists(ar.paths, r.paths, r.dr) and
 Exists_ATC_for_Route(ar.trackcs, ar.paths, r) and
 Route_not_Circular(ar.paths, r) =>
 Interlock`pre_Path_Connected(ar.paths, r.paths, r.dr))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Path_Connected" ], 41945610 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Route" ], 41945543 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id, j : Joint &
Interlock`pre_Line_Add_Joint(ln, aid, tcid, jid, j) =>
 Interlock`post_Line_Add_Joint(ln, aid, tcid, jid, j, mu(ln,areas|->ln.areas ++ {aid |-> Add_Joint(ln.areas(aid), tcid, jid, j)})))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Joint" ], 41947580 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tc1 : TrackC, jid1 : Joint_id, tc2 : TrackC, jid2 : Joint_id, dir_chng : bool &
jid1 in set dom (tc1.joints) and
 jid2 in set dom (tc2.joints) and
 Remark_Compatible(tc1.joints(jid1).remark, tc2.joints(jid2).remark, dir_chng) =>
 <ADIR> in set dom (tc1.atc))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_wf_adjacent_signal" ], 41943422 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tc1 : TrackC, jid1 : Joint_id, tc2 : TrackC, jid2 : Joint_id, dir_chng : bool &
jid1 in set dom (tc1.joints) and
 jid2 in set dom (tc2.joints) and
 Remark_Compatible(tc1.joints(jid1).remark, tc2.joints(jid2).remark, dir_chng) =>
 <ADIR> in set dom (tc2.atc))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_wf_adjacent_signal" ], 41943422 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tc1 : TrackC, jid1 : Joint_id, tc2 : TrackC, jid2 : Joint_id, dir_chng : bool &
jid1 in set dom (tc1.joints) and
 jid2 in set dom (tc2.joints) and
 Remark_Compatible(tc1.joints(jid1).remark, tc2.joints(jid2).remark, dir_chng) =>
 <BDIR> in set dom (tc2.atc))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_wf_adjacent_signal" ], 41943422 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
card ({p | p in set rng (ln.areas(n1.aid).paths) & p.used(dr) and
 EndJoint(p, dr) = n1.no}) > 1 =>
 n1.aid in set dom (ln.areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id &
aid in set dom (ln.areas) and
 pre_Del_TrackC(ln.areas(aid), tcid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
aj.aid = aid =>
 aj.tcid <> tcid)) =>
 Interlock`pre_Del_TrackC(ln.areas(aid), tcid))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Del_TrackC" ], 41947457 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_TrackC" ], 41947432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.endp) in set dunion (dom (connect))) =>
 p.tc in set dom (ar.trackcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_after_End" ], 41949300 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.startr) in set dunion (dom (connect))) =>
 p.tc in set dom (ar.trackcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_before_Start" ], 41949158 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 EndJoint(p, dr) = n1.no)) =>
 n1.tcid in set dom (ln.areas(n1.aid).trackcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 StartJoint(p, dr) = n1.no)) =>
 n1.tcid in set dom (ln.areas(n1.aid).trackcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Area(trackcs,paths,routes,-,-) : Area &
((forall p in set rng (paths) &
Path_within_TrackC(trackcs, p) and
 Direction_Correct(trackcs, p))) =>
 (forall r in set rng (routes) &
Path_Exists(paths, r.paths, r.dr) =>
 Interlock`pre_Exists_ATC_for_Route(trackcs, paths, r)))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Exists_ATC_for_Route" ], 41944208 ) ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Area" ], 41944147 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, route : seq of Path_id, dr : Direction &
Path_Exists(paths, route, dr) =>
 (forall i in set inds (route) &
(i + 1) in set inds (route) =>
 (dr = <ADIR> =>
 paths(route(i)).endp = paths(route(i + 1)).startr) =>
 dr = <BDIR> =>
 route(i) in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Connected" ], 41944531 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tid : TrackC_id, jid : Joint_id, joint : Joint &
Interlock`pre_Add_Joint(ar, tid, jid, joint) =>
 Interlock`post_Add_Joint(ar, tid, jid, joint, (let tc = ar.trackcs(tid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tid |-> mu(tc,joints|->tc.joints ++ {jid |-> joint})}))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Joint" ], 41944812 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
card ({p | p in set rng (ln.areas(n1.aid).paths) & p.used(dr) and
 EndJoint(p, dr) = n1.no}) > 1 =>
 n1.tcid in set dom (ln.areas(n1.aid).trackcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall paths : Path_map, route : seq of Path_id, dr : Direction &
Path_Exists(paths, route, dr) =>
 (forall i in set inds (route) &
(i + 1) in set inds (route) =>
 (dr = <ADIR> =>
 paths(route(i)).endp = paths(route(i + 1)).startr) =>
 dr = <BDIR> =>
 route(i + 1) in set dom (paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Connected" ], 41944531 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.endp) in set dunion (dom (connect))) =>
 p.endp in set dom (ar.trackcs(p.tc).joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_after_End" ], 41949300 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 EndJoint(p, dr) = n1.no)) =>
 n1.no in set dom (ln.areas(n1.aid).trackcs(n1.tcid).joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.startr) in set dunion (dom (connect))) =>
 p.startr in set dom (ar.trackcs(p.tc).joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_before_Start" ], 41949158 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 StartJoint(p, dr) = n1.no)) =>
 n1.no in set dom (ln.areas(n1.aid).trackcs(n1.tcid).joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
card ({p | p in set rng (ln.areas(n1.aid).paths) & p.used(dr) and
 EndJoint(p, dr) = n1.no}) > 1 =>
 n1.no in set dom (ln.areas(n1.aid).trackcs(n1.tcid).joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
aid in set dom (ln.areas) and
 pre_Del_Condition(ln.areas(aid), pid, kind, startr, endp) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Del_Condition(ln.areas(aid), pid, kind, startr, endp)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41948716 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Condition" ], 41948700 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id, j : Joint &
aid in set dom (ln.areas) and
 pre_Add_Joint(ln.areas(aid), tcid, jid, j) and
 ((forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid))) =>
 Interlock`pre_Add_Joint(ln.areas(aid), tcid, jid, j))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Add_Joint" ], 41947615 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Joint" ], 41947580 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 Direction_for_Area_Joint(areas(n1.aid).paths, n1.no, areas(n2.aid).paths, n2.no, connect(c).chng_direction) =>
 n1.aid in set dom (areas))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, path : Path &
pid not in set dom (ar.paths) and
 Path_within_TrackC(ar.trackcs, path) and
 Direction_Correct(ar.trackcs, path) and
 (forall p in set rng (ar.paths) &
Not_Same_Path(p, path) and
 Not_Start_and_End(p, path)) =>
 Interlock`inv_Path_map(ar.paths ++ {pid |-> path}))",
     <Invariants>,
     [ mk_AS`Name( [ "Path_map" ], 41944153 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Path" ], 41945349 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id &
aid in set dom (ln.areas) and
 pre_Del_TrackC(ln.areas(aid), tcid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
aj.aid = aid =>
 aj.tcid <> tcid)) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Del_TrackC(ln.areas(aid), tcid)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41947441 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_TrackC" ], 41947432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id &
aid in set dom (ln.areas) and
 pre_Del_Joint(ln.areas(aid), tcid, jid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
(aj.aid = aid and
 aj.tcid = tcid) =>
 aj.no <> jid)) =>
 Interlock`pre_Del_Joint(ln.areas(aid), tcid, jid))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Del_Joint" ], 41947854 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Joint" ], 41947824 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Area(trackcs,paths,routes,-,-) : Area &
((forall p in set rng (paths) &
Path_within_TrackC(trackcs, p) and
 Direction_Correct(trackcs, p))) =>
 (forall r in set rng (routes) &
Path_Exists(paths, r.paths, r.dr) and
 Exists_ATC_for_Route(trackcs, paths, r) =>
 Interlock`pre_Route_not_Circular(paths, r)))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Route_not_Circular" ], 41944214 ) ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Area" ], 41944147 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 EndJoint(p, dr) = n1.no)) =>
 dr in set dom (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths)), jid = EndJoint(ar.paths(pid), r.dr), tcid = ar.paths(pid).tc
in
    not (mk_Area_Joint(aid,tcid,jid) in set dunion (dom (connect))) =>
 tcid in set dom (ar.trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, rid : Route_id, r : Route &
rid not in set dom (ar.routes) and
 Path_Exists(ar.paths, r.paths, r.dr) and
 Exists_ATC_for_Route(ar.trackcs, ar.paths, r) and
 Route_not_Circular(ar.paths, r) and
 Path_Connected(ar.paths, r.paths, r.dr) =>
 Interlock`inv_Area(mu(ar,routes|->ar.routes ++ {rid |-> r})))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41945552 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Route" ], 41945543 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 StartJoint(p, dr) = n1.no)) =>
 dr in set dom (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
ar.trackcs(tcid).atc(dr).used =>
 (forall rid in set dom (ar.routes) &
ar.routes(rid).dr = dr =>
 (forall pid in set elems (ar.routes(rid).paths) &
pid in set dom (ar.paths))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, path : Path &
pid not in set dom (ar.paths) and
 Path_within_TrackC(ar.trackcs, path) and
 Direction_Correct(ar.trackcs, path) and
 (forall p in set rng (ar.paths) &
Not_Same_Path(p, path) and
 Not_Start_and_End(p, path)) =>
 Interlock`inv_Area(mu(ar,paths|->ar.paths ++ {pid |-> path})))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41945358 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Path" ], 41945349 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
card ({p | p in set rng (ln.areas(n1.aid).paths) & p.used(dr) and
 EndJoint(p, dr) = n1.no}) > 1 =>
 dr in set dom (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, tc : TrackC &
aid in set dom (ln.areas) and
 pre_Add_TrackC(ln.areas(aid), tcid, tc) and
 ((forall jid in set dom (tc.joints) &
(forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid)))) =>
 Interlock`pre_Add_TrackC(ln.areas(aid), tcid, tc))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Add_TrackC" ], 41947284 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_TrackC" ], 41947254 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 Direction_for_Area_Joint(areas(n1.aid).paths, n1.no, areas(n2.aid).paths, n2.no, connect(c).chng_direction) =>
 n1.tcid in set dom (areas(n1.aid).trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, tc : TrackC &
tcid not in set dom (ar.trackcs) and
 (forall jid in set dom (tc.joints) &
Only_One_Next_TrackC(ar.trackcs, tcid, jid) and
 (forall tcid1 in set dom (ar.trackcs) &
Joint_and_Next_TrackC(tc, ar.trackcs(tcid1), jid))) =>
 Interlock`inv_TrackC_map(ar.trackcs ++ {tcid |-> tc}))",
     <Invariants>,
     [ mk_AS`Name( [ "TrackC_map" ], 41944149 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_TrackC" ], 41944617 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths)), jid = EndJoint(ar.paths(pid), r.dr), tcid = ar.paths(pid).tc
in
    not (mk_Area_Joint(aid,tcid,jid) in set dunion (dom (connect))) =>
 jid in set dom (ar.trackcs(tcid).joints))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.endp) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(p.endp).remark.line_terminal) =>
 p.tc in set dom (ar.trackcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_after_End" ], 41949300 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.startr) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(p.startr).remark.line_terminal) =>
 p.tc in set dom (ar.trackcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_before_Start" ], 41949158 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, tc : TrackC &
tcid not in set dom (ar.trackcs) and
 (forall jid in set dom (tc.joints) &
Only_One_Next_TrackC(ar.trackcs, tcid, jid) and
 (forall tcid1 in set dom (ar.trackcs) &
Joint_and_Next_TrackC(tc, ar.trackcs(tcid1), jid))) =>
 Interlock`inv_Area(mu(ar,trackcs|->ar.trackcs ++ {tcid |-> tc})))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41944626 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_TrackC" ], 41944617 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall atcA1 : ATC, atcB1 : ATC, rm1 : Remark, atcA2 : ATC, atcB2 : ATC, rm2 : Remark, dir_chng : bool &
Remark_Compatible(rm1, rm2, dir_chng) =>
 (not (dir_chng) =>
 (((atcA1.used <> atcA2.used) = rm1.atc_terminal(<ADIR>)) and
 ((atcB1.used <> atcB2.used) = rm1.atc_terminal(<BDIR>)))) =>
 dir_chng =>
 <ADIR> in set dom (rm1.atc_terminal))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "ATC_Terminal_and_ATC_Used" ], 41943608 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 EndJoint(p, dr) = n1.no)) =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 n2.aid in set dom (ln.areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall areas : Area_map, n : Area_Joint &
n.aid in set dom (areas) and
 n.tcid in set dom (areas(n.aid).trackcs) and
 n.no in set dom (areas(n.aid).trackcs(n.tcid).joints) and
 not (areas(n.aid).trackcs(n.tcid).joints(n.no).remark.line_terminal) =>
 (forall tcid in set dom (areas(n.aid).trackcs) &
n.tcid <> tcid =>
 n.aid in set dom (areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Area_Joint_Exists" ], 41946515 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 StartJoint(p, dr) = n1.no)) =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 n2.aid in set dom (ln.areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.endp) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(p.endp).remark.line_terminal) =>
 p.endp in set dom (ar.trackcs(p.tc).joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_after_End" ], 41949300 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
card ({p | p in set rng (ln.areas(n1.aid).paths) & p.used(dr) and
 EndJoint(p, dr) = n1.no}) > 1 =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 con in set dom (ln.connect)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id, j : Joint &
aid in set dom (ln.areas) and
 pre_Add_Joint(ln.areas(aid), tcid, jid, j) and
 ((forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid))) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Add_Joint(ln.areas(aid), tcid, jid, j)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41947595 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Joint" ], 41947580 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.startr) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(p.startr).remark.line_terminal) =>
 p.startr in set dom (ar.trackcs(p.tc).joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_before_Start" ], 41949158 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id &
aid in set dom (ln.areas) and
 pre_Del_Joint(ln.areas(aid), tcid, jid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
(aj.aid = aid and
 aj.tcid = tcid) =>
 aj.no <> jid)) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Del_Joint(ln.areas(aid), tcid, jid)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41947836 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Joint" ], 41947824 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) =>
 (forall n1,n2 in set con &
n1 <> n2 =>
 Direction_for_Area_Joint(ln.areas(n1.aid).paths, n1.no, ln.areas(n2.aid).paths, n2.no, r.chng_direction) =>
 n1.aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Area(trackcs,paths,routes,-,-) : Area &
((forall p in set rng (paths) &
Path_within_TrackC(trackcs, p) and
 Direction_Correct(trackcs, p))) =>
 (forall r in set rng (routes) &
Path_Exists(paths, r.paths, r.dr) and
 Exists_ATC_for_Route(trackcs, paths, r) and
 Route_not_Circular(paths, r) =>
 Interlock`pre_Path_Connected(paths, r.paths, r.dr)))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Path_Connected" ], 41944219 ) ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Area" ], 41944147 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 Direction_for_Area_Joint(areas(n1.aid).paths, n1.no, areas(n2.aid).paths, n2.no, connect(c).chng_direction) =>
 (let tc1 = areas(n1.aid).trackcs(n1.tcid)
in
    n2.aid in set dom (areas)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, tc : TrackC &
aid in set dom (ln.areas) and
 pre_Add_TrackC(ln.areas(aid), tcid, tc) and
 ((forall jid in set dom (tc.joints) &
(forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid)))) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Add_TrackC(ln.areas(aid), tcid, tc)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41947266 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_TrackC" ], 41947254 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.endp) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(p.endp).remark.line_terminal) =>
 dr in set dom (ar.trackcs(p.tc).joints(p.endp).remark.atc_terminal)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_after_End" ], 41949300 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths)), jid = EndJoint(ar.paths(pid), r.dr), tcid = ar.paths(pid).tc
in
    not (mk_Area_Joint(aid,tcid,jid) in set dunion (dom (connect)) or
 ar.trackcs(tcid).joints(jid).remark.line_terminal) =>
 tcid in set dom (ar.trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) =>
 (forall n1,n2 in set con &
n1 <> n2 =>
 Direction_for_Area_Joint(ln.areas(n1.aid).paths, n1.no, ln.areas(n2.aid).paths, n2.no, r.chng_direction) =>
 n1.tcid in set dom (ln.areas(n1.aid).trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 Direction_for_Area_Joint(areas(n1.aid).paths, n1.no, areas(n2.aid).paths, n2.no, connect(c).chng_direction) =>
 (let tc1 = areas(n1.aid).trackcs(n1.tcid)
in
    n2.tcid in set dom (areas(n2.aid).trackcs)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.startr) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(p.startr).remark.line_terminal) =>
 dr in set dom (ar.trackcs(p.tc).joints(p.startr).remark.atc_terminal)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_before_Start" ], 41949158 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths)), jid = EndJoint(ar.paths(pid), r.dr), tcid = ar.paths(pid).tc
in
    not (mk_Area_Joint(aid,tcid,jid) in set dunion (dom (connect)) or
 ar.trackcs(tcid).joints(jid).remark.line_terminal) =>
 jid in set dom (ar.trackcs(tcid).joints))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, p : Path &
aid in set dom (ln.areas) and
 pre_Add_Path(ln.areas(aid), pid, p) and
 (forall c in set dom (ln.connect) &
(forall n1,n2 in set c &
(n1 <> n2 and
 n1.aid = aid) =>
 Direction_for_Area_Joint({pid |-> p}, n1.no, ln.areas(n2.aid).paths, n2.no, ln.connect(c).chng_direction))) =>
 Interlock`pre_Add_Path(ln.areas(aid), pid, p))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Add_Path" ], 41948079 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Path" ], 41948049 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
Interlock`pre_Del_Condition(ar, pid, kind, startr, endp) =>
 Interlock`post_Del_Condition(ar, pid, kind, startr, endp, (let p = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Condition" ], 41946008 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 EndJoint(p, dr) = n1.no)) =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 (forall p2 in set rng (ln.areas(n2.aid).paths) &
con in set dom (ln.connect))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 Direction_for_Area_Joint(areas(n1.aid).paths, n1.no, areas(n2.aid).paths, n2.no, connect(c).chng_direction) =>
 (let tc1 = areas(n1.aid).trackcs(n1.tcid), tc2 = areas(n2.aid).trackcs(n2.tcid)
in
    c in set dom (connect)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 StartJoint(p, dr) = n1.no)) =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 (forall p2 in set rng (ln.areas(n2.aid).paths) &
con in set dom (ln.connect))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 Direction_for_Area_Joint(areas(n1.aid).paths, n1.no, areas(n2.aid).paths, n2.no, connect(c).chng_direction) =>
 (let tc1 = areas(n1.aid).trackcs(n1.tcid), tc2 = areas(n2.aid).trackcs(n2.tcid)
in
    n1.no in set dom (tc1.joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 Direction_for_Area_Joint(areas(n1.aid).paths, n1.no, areas(n2.aid).paths, n2.no, connect(c).chng_direction) =>
 (let tc1 = areas(n1.aid).trackcs(n1.tcid), tc2 = areas(n2.aid).trackcs(n2.tcid)
in
    n2.no in set dom (tc2.joints)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
((forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
ar.trackcs(tcid).atc(dr).used =>
 (exists1 rid in set dom (ar.routes) &
ar.routes(rid).dr = dr and
 (exists pid in set elems (ar.routes(rid).paths) &
ar.paths(pid).tc = tcid))))) =>
 (forall r in set rng (ar.routes) &
(len (r.paths)) in set inds (r.paths)))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall atcA1 : ATC, atcB1 : ATC, rm1 : Remark, atcA2 : ATC, atcB2 : ATC, rm2 : Remark, dir_chng : bool &
Remark_Compatible(rm1, rm2, dir_chng) =>
 (not (dir_chng) =>
 (((atcA1.used <> atcA2.used) = rm1.atc_terminal(<ADIR>)) and
 ((atcB1.used <> atcB2.used) = rm1.atc_terminal(<BDIR>)))) =>
 dir_chng =>
 ((atcA1.used <> atcB2.used) = rm1.atc_terminal(<ADIR>)) =>
 <BDIR> in set dom (rm1.atc_terminal))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "ATC_Terminal_and_ATC_Used" ], 41943608 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tc1 : TrackC, jid1 : Joint_id, tc2 : TrackC, jid2 : Joint_id, dir_chng : bool &
jid1 in set dom (tc1.joints) and
 jid2 in set dom (tc2.joints) and
 Remark_Compatible(tc1.joints(jid1).remark, tc2.joints(jid2).remark, dir_chng) =>
 Interlock`pre_ATC_Terminal_and_ATC_Used(tc1.atc(<ADIR>), tc2.atc(<BDIR>), tc1.joints(jid1).remark, tc2.atc(<ADIR>), tc2.atc(<BDIR>), tc2.joints(jid2).remark, dir_chng))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "ATC_Terminal_and_ATC_Used" ], 41943480 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_wf_adjacent_signal" ], 41943422 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, jid : Joint_id &
tcid in set dom (ar.trackcs) and
 jid in set dom (ar.trackcs(tcid).joints) and
 card (dom (ar.trackcs(tcid).joints)) > 2 and
 ((forall path in set rng (ar.paths) &
path.tc <> tcid or
 jid <> path.startr and
 jid <> path.endp)) =>
 (let tc = ar.trackcs(tcid)
in
    Interlock`inv_TrackC_map(ar.trackcs ++ {tcid |-> mu(tc,joints|->{jid} <-: tc.joints)})))",
     <Invariants>,
     [ mk_AS`Name( [ "TrackC_map" ], 41944149 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Joint" ], 41945093 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
((forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
ar.trackcs(tcid).atc(dr).used =>
 (exists1 rid in set dom (ar.routes) &
ar.routes(rid).dr = dr and
 (exists pid in set elems (ar.routes(rid).paths) &
ar.paths(pid).tc = tcid))))) =>
 (forall r in set rng (ar.routes) &
r.paths(len (r.paths)) in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths)), jid = EndJoint(ar.paths(pid), r.dr), tcid = ar.paths(pid).tc
in
    not (mk_Area_Joint(aid,tcid,jid) in set dunion (dom (connect)) or
 ar.trackcs(tcid).joints(jid).remark.line_terminal) =>
 r.dr in set dom (ar.trackcs(tcid).joints(jid).remark.atc_terminal))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) =>
 (forall n1,n2 in set con &
n1 <> n2 =>
 Direction_for_Area_Joint(ln.areas(n1.aid).paths, n1.no, ln.areas(n2.aid).paths, n2.no, r.chng_direction) =>
 (let tc1 = ln.areas(n1.aid).trackcs(n1.tcid)
in
    n2.aid in set dom (ln.areas))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id &
aid in set dom (ln.areas) and
 pre_Del_Path(ln.areas(aid), pid) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Del_Path(ln.areas(aid), pid)})
in
    aid in set dom (RESULT.areas) and
 pid not in set dom (RESULT.areas(aid).paths) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Path" ], 41948248 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, rid : Route_id &
aid in set dom (ln.areas) and
 pre_Del_Route(ln.areas(aid), rid) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Del_Route(ln.areas(aid), rid)})
in
    aid in set dom (RESULT.areas) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas and
 rid not in set dom (RESULT.areas(aid).routes) =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Route" ], 41948515 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
pid in set dom (ar.paths) and
 ((exists c in set ar.paths(pid).condition &
c.kind = kind and
 c.startr = startr and
 c.endp = endp)) =>
 (let p = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))})
in
    Interlock`inv_Path_map(ar.paths ++ {pid |-> p})))",
     <Invariants>,
     [ mk_AS`Name( [ "Path_map" ], 41944153 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Condition" ], 41946008 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, jid : Joint_id &
tcid in set dom (ar.trackcs) and
 jid in set dom (ar.trackcs(tcid).joints) and
 card (dom (ar.trackcs(tcid).joints)) > 2 and
 ((forall path in set rng (ar.paths) &
path.tc <> tcid or
 jid <> path.startr and
 jid <> path.endp)) =>
 Interlock`inv_Area((let tc = ar.trackcs(tcid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tcid |-> mu(tc,joints|->{jid} <-: tc.joints)}))))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41945102 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Joint" ], 41945093 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tid : TrackC_id, jid : Joint_id, joint : Joint &
tid in set dom (ar.trackcs) =>
 (let tc = ar.trackcs(tid)
in
    jid not in set dom (tc.joints) and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints) union {joint}) and
 Only_One_Next_TrackC(ar.trackcs, tid, jid) =>
 (forall tid1 in set dom (ar.trackcs) &
tid1 <> tid =>
 Interlock`inv_TrackC(mu(tc,joints|->tc.joints ++ {jid |-> joint})))))",
     <Invariants>,
     [ mk_AS`Name( [ "TrackC" ], 41943359 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Joint" ], 41944812 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) =>
 (forall n1,n2 in set con &
n1 <> n2 =>
 Direction_for_Area_Joint(ln.areas(n1.aid).paths, n1.no, ln.areas(n2.aid).paths, n2.no, r.chng_direction) =>
 (let tc1 = ln.areas(n1.aid).trackcs(n1.tcid)
in
    n2.tcid in set dom (ln.areas(n2.aid).trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
pid in set dom (ar.paths) and
 ((exists c in set ar.paths(pid).condition &
c.kind = kind and
 c.startr = startr and
 c.endp = endp)) =>
 Interlock`inv_Area((let p = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41946021 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Condition" ], 41946008 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, p : Path &
aid in set dom (ln.areas) and
 pre_Add_Path(ln.areas(aid), pid, p) and
 (forall c in set dom (ln.connect) &
(forall n1,n2 in set c &
(n1 <> n2 and
 n1.aid = aid) =>
 Direction_for_Area_Joint({pid |-> p}, n1.no, ln.areas(n2.aid).paths, n2.no, ln.connect(c).chng_direction))) =>
 Interlock`inv_Line(mu(ln,areas|->ln.areas ++ {aid |-> Add_Path(ln.areas(aid), pid, p)})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41948061 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Path" ], 41948049 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, rid : Route_id, r : Route &
aid in set dom (ln.areas) and
 pre_Add_Route(ln.areas(aid), rid, r) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Add_Route(ln.areas(aid), rid, r)})
in
    aid in set dom (RESULT.areas) and
 rid in set dom (RESULT.areas(aid).routes) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Route" ], 41948371 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) and
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr and
 con.endp <= ar.trackcs(p.tc).joints(p.endp).position and
 ((forall c in set p.condition &
Condition_not_Conflict(c, con)))) =>
 (let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    Interlock`inv_Path_map(ar.paths ++ {pid |-> p})))",
     <Invariants>,
     [ mk_AS`Name( [ "Path_map" ], 41944153 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) and
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr and
 con.endp <= ar.trackcs(p.tc).joints(p.endp).position and
 ((forall c in set p.condition &
Condition_not_Conflict(c, con)))) =>
 Interlock`inv_Area((let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41945740 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) =>
 (forall n1,n2 in set con &
n1 <> n2 =>
 Direction_for_Area_Joint(ln.areas(n1.aid).paths, n1.no, ln.areas(n2.aid).paths, n2.no, r.chng_direction) =>
 (let tc1 = ln.areas(n1.aid).trackcs(n1.tcid), tc2 = ln.areas(n2.aid).trackcs(n2.tcid)
in
    n1.no in set dom (tc1.joints))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) =>
 (forall n1,n2 in set con &
n1 <> n2 =>
 Direction_for_Area_Joint(ln.areas(n1.aid).paths, n1.no, ln.areas(n2.aid).paths, n2.no, r.chng_direction) =>
 (let tc1 = ln.areas(n1.aid).trackcs(n1.tcid), tc2 = ln.areas(n2.aid).trackcs(n2.tcid)
in
    n2.no in set dom (tc2.joints))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths)), jid = EndJoint(ar.paths(pid), r.dr), tcid = ar.paths(pid).tc
in
    not (mk_Area_Joint(aid,tcid,jid) in set dunion (dom (connect)) or
 ar.trackcs(tcid).joints(jid).remark.line_terminal or
 ar.trackcs(tcid).joints(jid).remark.atc_terminal(r.dr)) =>
 Interlock`pre_Following_Route_Exists(ar.routes, rid))))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Following_Route_Exists" ], 41949644 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Line(areas,connect) : Line &
(forall c in set dom (connect) &
((forall n in set c &
Area_Joint_Exists(areas, n))) =>
 (forall n1,n2 in set c &
n1 <> n2 =>
 Direction_for_Area_Joint(areas(n1.aid).paths, n1.no, areas(n2.aid).paths, n2.no, connect(c).chng_direction) =>
 (let tc1 = areas(n1.aid).trackcs(n1.tcid), tc2 = areas(n2.aid).trackcs(n2.tcid)
in
    Joint_Compatible(tc1.joints(n1.no), tc2.joints(n2.no), connect(c)) =>
 c in set dom (connect)))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <types>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line" ], 41946284 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
card ({p | p in set rng (ln.areas(n1.aid).paths) & p.used(dr) and
 EndJoint(p, dr) = n1.no}) > 1 =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 (let dr2 = (if not (ln.connect(con).chng_direction) then
dr
else
(if dr = <ADIR> then
<BDIR>
else
<ADIR>))
in
    n2.aid in set dom (ln.areas))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id &
aid in set dom (ln.areas) and
 pre_Del_TrackC(ln.areas(aid), tcid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
aj.aid = aid =>
 aj.tcid <> tcid)) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Del_TrackC(ln.areas(aid), tcid)})
in
    aid in set dom (RESULT.areas) and
 dom (ln.areas) = dom (RESULT.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_TrackC" ], 41947432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.endp) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(p.endp).remark.line_terminal or
 ar.trackcs(p.tc).joints(p.endp).remark.atc_terminal(dr)) =>
 (forall pid1 in set dom (ar.paths) &
(let p1 = ar.paths(pid1)
in
    p1.tc <> p.tc =>
 dr in set dom (p1.used)))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_after_End" ], 41949300 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall pid in set dom (ar.paths) &
(let p = ar.paths(pid)
in
    (forall dr in set dom (p.used) &
p.used(dr) =>
 not (mk_Area_Joint(aid,p.tc,p.startr) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(p.startr).remark.line_terminal or
 ar.trackcs(p.tc).joints(p.startr).remark.atc_terminal(dr)) =>
 (forall pid1 in set dom (ar.paths) &
(let p1 = ar.paths(pid1)
in
    p1.tc <> p.tc =>
 dr in set dom (p1.used)))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Path_Exists_before_Start" ], 41949158 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
(forall rid in set dom (ar.routes) &
(let r = ar.routes(rid), pid = r.paths(len (r.paths)), jid = EndJoint(ar.paths(pid), r.dr), tcid = ar.paths(pid).tc
in
    not (mk_Area_Joint(aid,tcid,jid) in set dunion (dom (connect)) or
 ar.trackcs(tcid).joints(jid).remark.line_terminal or
 ar.trackcs(tcid).joints(jid).remark.atc_terminal(r.dr) or
 Following_Route_Exists(ar.routes, rid)) =>
 Interlock`pre_Following_Path_Unique(ar.paths, pid, r.dr))))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Following_Path_Unique" ], 41949651 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Route_Exists_to_Terminal" ], 41949536 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 EndJoint(p, dr) = n1.no)) =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 (forall p2 in set rng (ln.areas(n2.aid).paths) &
(let dr2 = (if not (ln.connect(con).chng_direction) then
dr
else
(if dr = <ADIR> then
<BDIR>
else
<ADIR>))
in
    dr2 in set dom (p2.used)))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Following_Path_Exists_at_Connect" ], 41950128 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
((exists p in set rng (ln.areas(n1.aid).paths) &
p.used(dr) and
 StartJoint(p, dr) = n1.no)) =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 (forall p2 in set rng (ln.areas(n2.aid).paths) &
(let dr2 = (if not (ln.connect(con).chng_direction) then
dr
else
(if dr = <ADIR> then
<BDIR>
else
<ADIR>))
in
    dr2 in set dom (p2.used)))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Preceding_Path_Exists_at_Connect" ], 41950270 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line &
(forall con in set dom (ln.connect) &
(forall n1,n2 in set con &
n1 <> n2 =>
 (forall dr in set {<ADIR>,<BDIR>} &
card ({p | p in set rng (ln.areas(n1.aid).paths) & p.used(dr) and
 EndJoint(p, dr) = n1.no}) > 1 =>
 not (ln.areas(n1.aid).trackcs(n1.tcid).joints(n1.no).remark.atc_terminal(dr)) =>
 (let dr2 = (if not (ln.connect(con).chng_direction) then
dr
else
(if dr = <ADIR> then
<BDIR>
else
<ADIR>))
in
    (forall p in set rng (ln.areas(n2.aid).paths) &
dr2 in set dom (p.used)))))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "One_Side_Unique_Path_at_Connection" ], 41949980 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
((forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
ar.trackcs(tcid).atc(dr).used =>
 (exists1 rid in set dom (ar.routes) &
ar.routes(rid).dr = dr and
 (exists pid in set elems (ar.routes(rid).paths) &
ar.paths(pid).tc = tcid))))) =>
 (forall r in set rng (ar.routes) &
(let p = ar.paths(r.paths(len (r.paths))), jid = EndJoint(p, r.dr)
in
    not (mk_Area_Joint(aid,p.tc,jid) in set dunion (dom (connect))) =>
 p.tc in set dom (ar.trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
((forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
ar.trackcs(tcid).atc(dr).used =>
 (exists1 rid in set dom (ar.routes) &
ar.routes(rid).dr = dr and
 (exists pid in set elems (ar.routes(rid).paths) &
ar.paths(pid).tc = tcid))))) =>
 (forall r in set rng (ar.routes) &
(let p = ar.paths(r.paths(len (r.paths))), jid = EndJoint(p, r.dr)
in
    not (mk_Area_Joint(aid,p.tc,jid) in set dunion (dom (connect))) =>
 jid in set dom (ar.trackcs(p.tc).joints))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, tc : TrackC &
aid in set dom (ln.areas) and
 pre_Add_TrackC(ln.areas(aid), tcid, tc) and
 ((forall jid in set dom (tc.joints) &
(forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid)))) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Add_TrackC(ln.areas(aid), tcid, tc)})
in
    aid in set dom (RESULT.areas) and
 dom (ln.areas) = dom (RESULT.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas and
 tcid in set dom (RESULT.areas(aid).trackcs) =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_TrackC" ], 41947254 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tid : TrackC_id, jid : Joint_id, joint : Joint &
tid in set dom (ar.trackcs) and
 (let tc = ar.trackcs(tid)
in
    jid not in set dom (tc.joints) and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints) union {joint}) and
 Only_One_Next_TrackC(ar.trackcs, tid, jid) and
 (forall tid1 in set dom (ar.trackcs) &
tid1 <> tid =>
 Joint_and_Next_TrackC(ar.trackcs(tid1), mu(tc,joints|->tc.joints ++ {jid |-> joint}), jid))) =>
 (let tc = ar.trackcs(tid)
in
    Interlock`inv_TrackC_map(ar.trackcs ++ {tid |-> mu(tc,joints|->tc.joints ++ {jid |-> joint})})))",
     <Invariants>,
     [ mk_AS`Name( [ "TrackC_map" ], 41944149 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Joint" ], 41944812 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
pid in set dom (ar.paths) and
 ((exists c in set ar.paths(pid).condition &
c.kind = kind and
 c.startr = startr and
 c.endp = endp)) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Condition" ], 41946008 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
((forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
ar.trackcs(tcid).atc(dr).used =>
 (exists1 rid in set dom (ar.routes) &
ar.routes(rid).dr = dr and
 (exists pid in set elems (ar.routes(rid).paths) &
ar.paths(pid).tc = tcid))))) =>
 (forall r in set rng (ar.routes) &
(let p = ar.paths(r.paths(len (r.paths))), jid = EndJoint(p, r.dr)
in
    not (mk_Area_Joint(aid,p.tc,jid) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(jid).remark.line_terminal) =>
 p.tc in set dom (ar.trackcs))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tid : TrackC_id, jid : Joint_id, joint : Joint &
tid in set dom (ar.trackcs) and
 (let tc = ar.trackcs(tid)
in
    jid not in set dom (tc.joints) and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints) union {joint}) and
 Only_One_Next_TrackC(ar.trackcs, tid, jid) and
 (forall tid1 in set dom (ar.trackcs) &
tid1 <> tid =>
 Joint_and_Next_TrackC(ar.trackcs(tid1), mu(tc,joints|->tc.joints ++ {jid |-> joint}), jid))) =>
 Interlock`inv_Area((let tc = ar.trackcs(tid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tid |-> mu(tc,joints|->tc.joints ++ {jid |-> joint})}))))",
     <Invariants>,
     [ mk_AS`Name( [ "Area" ], 41944824 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Joint" ], 41944812 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
((forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
ar.trackcs(tcid).atc(dr).used =>
 (exists1 rid in set dom (ar.routes) &
ar.routes(rid).dr = dr and
 (exists pid in set elems (ar.routes(rid).paths) &
ar.paths(pid).tc = tcid))))) =>
 (forall r in set rng (ar.routes) &
(let p = ar.paths(r.paths(len (r.paths))), jid = EndJoint(p, r.dr)
in
    not (mk_Area_Joint(aid,p.tc,jid) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(jid).remark.line_terminal) =>
 jid in set dom (ar.trackcs(p.tc).joints))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) and
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr and
 con.endp <= ar.trackcs(p.tc).joints(p.endp).position and
 ((forall c in set p.condition &
Condition_not_Conflict(c, con)))) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) and
 ((forall n1,n2 in set con &
n1 <> n2 =>
 Direction_for_Area_Joint(ln.areas(n1.aid).paths, n1.no, ln.areas(n2.aid).paths, n2.no, r.chng_direction) and
 (let tc1 = ln.areas(n1.aid).trackcs(n1.tcid), tc2 = ln.areas(n2.aid).trackcs(n2.tcid)
in
    Joint_Compatible(tc1.joints(n1.no), tc2.joints(n2.no), r) and
 Is_wf_adjacent_signal(tc1, n1.no, tc2, n2.no, r.chng_direction)))) =>
 Interlock`inv_Connect_map(ln.connect ++ {con |-> r}))",
     <Invariants>,
     [ mk_AS`Name( [ "Connect_map" ], 41946290 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id, j : Joint &
aid in set dom (ln.areas) and
 pre_Add_Joint(ln.areas(aid), tcid, jid, j) and
 ((forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid))) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Add_Joint(ln.areas(aid), tcid, jid, j)})
in
    aid in set dom (RESULT.areas) and
 tcid in set dom (RESULT.areas(aid).trackcs) and
 jid in set dom (RESULT.areas(aid).trackcs(tcid).joints) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Joint" ], 41947580 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, con : Connect, r : Remark_Connect &
((forall c in set dom (ln.connect) &
c inter con = {})) and
 ((forall n in set con &
Area_Joint_Exists(ln.areas, n))) and
 ((forall n1,n2 in set con &
n1 <> n2 =>
 Direction_for_Area_Joint(ln.areas(n1.aid).paths, n1.no, ln.areas(n2.aid).paths, n2.no, r.chng_direction) and
 (let tc1 = ln.areas(n1.aid).trackcs(n1.tcid), tc2 = ln.areas(n2.aid).trackcs(n2.tcid)
in
    Joint_Compatible(tc1.joints(n1.no), tc2.joints(n2.no), r) and
 Is_wf_adjacent_signal(tc1, n1.no, tc2, n2.no, r.chng_direction)))) =>
 Interlock`inv_Line(mu(ln,connect|->ln.connect ++ {con |-> r})))",
     <Invariants>,
     [ mk_AS`Name( [ "Line" ], 41946993 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Connect" ], 41946984 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id &
aid in set dom (ln.areas) and
 pre_Del_Joint(ln.areas(aid), tcid, jid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
(aj.aid = aid and
 aj.tcid = tcid) =>
 aj.no <> jid)) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Del_Joint(ln.areas(aid), tcid, jid)})
in
    aid in set dom (RESULT.areas) and
 tcid in set dom (RESULT.areas(aid).trackcs) and
 jid not in set dom (RESULT.areas(aid).trackcs(tcid).joints) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Joint" ], 41947824 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, aid : Area_id, connect : Connect_map &
((forall tcid in set dom (ar.trackcs) &
(forall dr in set dom (ar.trackcs(tcid).atc) &
ar.trackcs(tcid).atc(dr).used =>
 (exists1 rid in set dom (ar.routes) &
ar.routes(rid).dr = dr and
 (exists pid in set elems (ar.routes(rid).paths) &
ar.paths(pid).tc = tcid))))) =>
 (forall r in set rng (ar.routes) &
(let p = ar.paths(r.paths(len (r.paths))), jid = EndJoint(p, r.dr)
in
    not (mk_Area_Joint(aid,p.tc,jid) in set dunion (dom (connect)) or
 ar.trackcs(p.tc).joints(jid).remark.line_terminal) =>
 r.dr in set dom (ar.trackcs(p.tc).joints(jid).remark.atc_terminal))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Is_Plain_Area" ], 41949805 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, pid : Path_id, p : Path &
aid in set dom (ln.areas) and
 pre_Add_Path(ln.areas(aid), pid, p) and
 (forall c in set dom (ln.connect) &
(forall n1,n2 in set c &
(n1 <> n2 and
 n1.aid = aid) =>
 Direction_for_Area_Joint({pid |-> p}, n1.no, ln.areas(n2.aid).paths, n2.no, ln.connect(c).chng_direction))) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Add_Path(ln.areas(aid), pid, p)})
in
    aid in set dom (RESULT.areas) and
 pid in set dom (RESULT.areas(aid).paths) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Path" ], 41948049 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, jid : Joint_id &
tcid in set dom (ar.trackcs) and
 jid in set dom (ar.trackcs(tcid).joints) and
 card (dom (ar.trackcs(tcid).joints)) > 2 and
 ((forall path in set rng (ar.paths) &
path.tc <> tcid or
 jid <> path.startr and
 jid <> path.endp)) =>
 (let RESULT:Area = (let tc = ar.trackcs(tcid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tcid |-> mu(tc,joints|->{jid} <-: tc.joints)}))
in
    tcid in set dom (RESULT.trackcs) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tcid} <-: RESULT.trackcs = {tcid} <-: ar.trackcs and
 jid not in set dom (RESULT.trackcs(tcid).joints) =>
 tcid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Joint" ], 41945093 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id, j : Joint &
aid in set dom (ln.areas) and
 pre_Add_Joint(ln.areas(aid), tcid, jid, j) and
 ((forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid))) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Add_Joint(ln.areas(aid), tcid, jid, j)})
in
    aid in set dom (RESULT.areas) and
 tcid in set dom (RESULT.areas(aid).trackcs) and
 jid in set dom (RESULT.areas(aid).trackcs(tcid).joints) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas and
 dom (RESULT.areas(aid).trackcs) = dom (ln.areas(aid).trackcs) =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Joint" ], 41947580 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id &
aid in set dom (ln.areas) and
 pre_Del_Joint(ln.areas(aid), tcid, jid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
(aj.aid = aid and
 aj.tcid = tcid) =>
 aj.no <> jid)) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Del_Joint(ln.areas(aid), tcid, jid)})
in
    aid in set dom (RESULT.areas) and
 tcid in set dom (RESULT.areas(aid).trackcs) and
 jid not in set dom (RESULT.areas(aid).trackcs(tcid).joints) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas and
 dom (RESULT.areas(aid).trackcs) = dom (ln.areas(aid).trackcs) =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Joint" ], 41947824 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) and
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr and
 con.endp <= ar.trackcs(p.tc).joints(p.endp).position and
 ((forall c in set p.condition &
Condition_not_Conflict(c, con)))) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con}) =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, jid : Joint_id &
tcid in set dom (ar.trackcs) and
 jid in set dom (ar.trackcs(tcid).joints) and
 card (dom (ar.trackcs(tcid).joints)) > 2 and
 ((forall path in set rng (ar.paths) &
path.tc <> tcid or
 jid <> path.startr and
 jid <> path.endp)) =>
 (let RESULT:Area = (let tc = ar.trackcs(tcid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tcid |-> mu(tc,joints|->{jid} <-: tc.joints)}))
in
    tcid in set dom (RESULT.trackcs) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tcid} <-: RESULT.trackcs = {tcid} <-: ar.trackcs and
 jid not in set dom (RESULT.trackcs(tcid).joints) and
 RESULT.trackcs(tcid) = mu(ar.trackcs(tcid),joints|->{jid} <-: ar.trackcs(tcid).joints) =>
 tcid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Joint" ], 41945093 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
pid in set dom (ar.paths) and
 ((exists c in set ar.paths(pid).condition &
c.kind = kind and
 c.startr = startr and
 c.endp = endp)) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))}) =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Condition" ], 41946008 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) and
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr and
 con.endp <= ar.trackcs(p.tc).joints(p.endp).position and
 ((forall c in set p.condition &
Condition_not_Conflict(c, con)))) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con}) and
 RESULT.paths(pid).startr = ar.paths(pid).startr =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id, j : Joint &
aid in set dom (ln.areas) and
 pre_Add_Joint(ln.areas(aid), tcid, jid, j) and
 ((forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid))) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Add_Joint(ln.areas(aid), tcid, jid, j)})
in
    aid in set dom (RESULT.areas) and
 tcid in set dom (RESULT.areas(aid).trackcs) and
 jid in set dom (RESULT.areas(aid).trackcs(tcid).joints) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas and
 dom (RESULT.areas(aid).trackcs) = dom (ln.areas(aid).trackcs) and
 {tcid} <-: RESULT.areas(aid).trackcs = {tcid} <-: ln.areas(aid).trackcs =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Joint" ], 41947580 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id &
aid in set dom (ln.areas) and
 pre_Del_Joint(ln.areas(aid), tcid, jid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
(aj.aid = aid and
 aj.tcid = tcid) =>
 aj.no <> jid)) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Del_Joint(ln.areas(aid), tcid, jid)})
in
    aid in set dom (RESULT.areas) and
 tcid in set dom (RESULT.areas(aid).trackcs) and
 jid not in set dom (RESULT.areas(aid).trackcs(tcid).joints) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas and
 dom (RESULT.areas(aid).trackcs) = dom (ln.areas(aid).trackcs) and
 {tcid} <-: RESULT.areas(aid).trackcs = {tcid} <-: ln.areas(aid).trackcs =>
 aid in set dom (ln.areas)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Joint" ], 41947824 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id, j : Joint &
aid in set dom (ln.areas) and
 pre_Add_Joint(ln.areas(aid), tcid, jid, j) and
 ((forall c in set dom (ln.connect) &
(forall n in set c &
n.aid = aid =>
 n.no <> jid))) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Add_Joint(ln.areas(aid), tcid, jid, j)})
in
    aid in set dom (RESULT.areas) and
 tcid in set dom (RESULT.areas(aid).trackcs) and
 jid in set dom (RESULT.areas(aid).trackcs(tcid).joints) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas and
 dom (RESULT.areas(aid).trackcs) = dom (ln.areas(aid).trackcs) and
 {tcid} <-: RESULT.areas(aid).trackcs = {tcid} <-: ln.areas(aid).trackcs =>
 tcid in set dom (ln.areas(aid).trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Add_Joint" ], 41947580 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ln : Line, aid : Area_id, tcid : TrackC_id, jid : Joint_id &
aid in set dom (ln.areas) and
 pre_Del_Joint(ln.areas(aid), tcid, jid) and
 (forall c in set dom (ln.connect) &
(forall aj in set c &
(aj.aid = aid and
 aj.tcid = tcid) =>
 aj.no <> jid)) =>
 (let RESULT:Line = mu(ln,areas|->ln.areas ++ {aid |-> Del_Joint(ln.areas(aid), tcid, jid)})
in
    aid in set dom (RESULT.areas) and
 tcid in set dom (RESULT.areas(aid).trackcs) and
 jid not in set dom (RESULT.areas(aid).trackcs(tcid).joints) and
 dom (RESULT.areas) = dom (ln.areas) and
 {aid} <-: RESULT.areas = {aid} <-: ln.areas and
 dom (RESULT.areas(aid).trackcs) = dom (ln.areas(aid).trackcs) and
 {tcid} <-: RESULT.areas(aid).trackcs = {tcid} <-: ln.areas(aid).trackcs =>
 tcid in set dom (ln.areas(aid).trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Line_Del_Joint" ], 41947824 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, jid : Joint_id &
tcid in set dom (ar.trackcs) and
 jid in set dom (ar.trackcs(tcid).joints) and
 card (dom (ar.trackcs(tcid).joints)) > 2 and
 ((forall path in set rng (ar.paths) &
path.tc <> tcid or
 jid <> path.startr and
 jid <> path.endp)) =>
 (let RESULT:Area = (let tc = ar.trackcs(tcid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tcid |-> mu(tc,joints|->{jid} <-: tc.joints)}))
in
    tcid in set dom (RESULT.trackcs) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tcid} <-: RESULT.trackcs = {tcid} <-: ar.trackcs and
 jid not in set dom (RESULT.trackcs(tcid).joints) and
 RESULT.trackcs(tcid) = mu(ar.trackcs(tcid),joints|->{jid} <-: ar.trackcs(tcid).joints) and
 RESULT.trackcs(tcid).atc = ar.trackcs(tcid).atc =>
 tcid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Joint" ], 41945093 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) and
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr and
 con.endp <= ar.trackcs(p.tc).joints(p.endp).position and
 ((forall c in set p.condition &
Condition_not_Conflict(c, con)))) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con}) and
 RESULT.paths(pid).startr = ar.paths(pid).startr and
 RESULT.paths(pid).endp = ar.paths(pid).endp =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
pid in set dom (ar.paths) and
 ((exists c in set ar.paths(pid).condition &
c.kind = kind and
 c.startr = startr and
 c.endp = endp)) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))}) and
 RESULT.paths(pid).startr = ar.paths(pid).startr =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Condition" ], 41946008 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tid : TrackC_id, jid : Joint_id, joint : Joint &
tid in set dom (ar.trackcs) and
 (let tc = ar.trackcs(tid)
in
    jid not in set dom (tc.joints) and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints) union {joint}) and
 Only_One_Next_TrackC(ar.trackcs, tid, jid) and
 (forall tid1 in set dom (ar.trackcs) &
tid1 <> tid =>
 Joint_and_Next_TrackC(ar.trackcs(tid1), mu(tc,joints|->tc.joints ++ {jid |-> joint}), jid))) =>
 (let RESULT:Area = (let tc = ar.trackcs(tid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tid |-> mu(tc,joints|->tc.joints ++ {jid |-> joint})}))
in
    tid in set dom (RESULT.trackcs) and
 jid in set dom (RESULT.trackcs(tid).joints) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tid} <-: RESULT.trackcs = {tid} <-: ar.trackcs =>
 tid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Joint" ], 41944812 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) and
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr and
 con.endp <= ar.trackcs(p.tc).joints(p.endp).position and
 ((forall c in set p.condition &
Condition_not_Conflict(c, con)))) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con}) and
 RESULT.paths(pid).startr = ar.paths(pid).startr and
 RESULT.paths(pid).endp = ar.paths(pid).endp and
 RESULT.paths(pid).tc = ar.paths(pid).tc =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, jid : Joint_id &
tcid in set dom (ar.trackcs) and
 jid in set dom (ar.trackcs(tcid).joints) and
 card (dom (ar.trackcs(tcid).joints)) > 2 and
 ((forall path in set rng (ar.paths) &
path.tc <> tcid or
 jid <> path.startr and
 jid <> path.endp)) =>
 (let RESULT:Area = (let tc = ar.trackcs(tcid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tcid |-> mu(tc,joints|->{jid} <-: tc.joints)}))
in
    tcid in set dom (RESULT.trackcs) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tcid} <-: RESULT.trackcs = {tcid} <-: ar.trackcs and
 jid not in set dom (RESULT.trackcs(tcid).joints) and
 RESULT.trackcs(tcid) = mu(ar.trackcs(tcid),joints|->{jid} <-: ar.trackcs(tcid).joints) and
 RESULT.trackcs(tcid).atc = ar.trackcs(tcid).atc and
 RESULT.trackcs(tcid).td = ar.trackcs(tcid).td =>
 tcid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Joint" ], 41945093 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
pid in set dom (ar.paths) and
 ((exists c in set ar.paths(pid).condition &
c.kind = kind and
 c.startr = startr and
 c.endp = endp)) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))}) and
 RESULT.paths(pid).startr = ar.paths(pid).startr and
 RESULT.paths(pid).endp = ar.paths(pid).endp =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Condition" ], 41946008 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, kind : Cond_Kind, startr : nat, endp : nat &
pid in set dom (ar.paths) and
 ((exists c in set ar.paths(pid).condition &
c.kind = kind and
 c.startr = startr and
 c.endp = endp)) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->{l | l in set ar.paths(pid).condition & not ((l.kind = kind and
 l.startr = startr and
 l.endp = endp))}) and
 RESULT.paths(pid).startr = ar.paths(pid).startr and
 RESULT.paths(pid).endp = ar.paths(pid).endp and
 RESULT.paths(pid).tc = ar.paths(pid).tc =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Condition" ], 41946008 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, pid : Path_id, con : Condition &
pid in set dom (ar.paths) and
 (let p = ar.paths(pid)
in
    ar.trackcs(p.tc).joints(p.startr).position <= con.startr and
 con.endp <= ar.trackcs(p.tc).joints(p.endp).position and
 ((forall c in set p.condition &
Condition_not_Conflict(c, con)))) =>
 (let RESULT:Area = (let p = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con})
in
    mu(ar,paths|->ar.paths ++ {pid |-> p}))
in
    pid in set dom (RESULT.paths) and
 dom (RESULT.paths) = dom (ar.paths) and
 {pid} <-: RESULT.paths = {pid} <-: ar.paths and
 RESULT.paths(pid) = mu(ar.paths(pid),condition|->ar.paths(pid).condition union {con}) and
 RESULT.paths(pid).startr = ar.paths(pid).startr and
 RESULT.paths(pid).endp = ar.paths(pid).endp and
 RESULT.paths(pid).tc = ar.paths(pid).tc and
 RESULT.paths(pid).length = ar.paths(pid).length =>
 pid in set dom (ar.paths)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Condition" ], 41945731 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tcid : TrackC_id, jid : Joint_id &
tcid in set dom (ar.trackcs) and
 jid in set dom (ar.trackcs(tcid).joints) and
 card (dom (ar.trackcs(tcid).joints)) > 2 and
 ((forall path in set rng (ar.paths) &
path.tc <> tcid or
 jid <> path.startr and
 jid <> path.endp)) =>
 (let RESULT:Area = (let tc = ar.trackcs(tcid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tcid |-> mu(tc,joints|->{jid} <-: tc.joints)}))
in
    tcid in set dom (RESULT.trackcs) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tcid} <-: RESULT.trackcs = {tcid} <-: ar.trackcs and
 jid not in set dom (RESULT.trackcs(tcid).joints) and
 RESULT.trackcs(tcid) = mu(ar.trackcs(tcid),joints|->{jid} <-: ar.trackcs(tcid).joints) and
 RESULT.trackcs(tcid).atc = ar.trackcs(tcid).atc and
 RESULT.trackcs(tcid).td = ar.trackcs(tcid).td and
 RESULT.trackcs(tcid).atbt = ar.trackcs(tcid).atbt =>
 tcid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Del_Joint" ], 41945093 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tid : TrackC_id, jid : Joint_id, joint : Joint &
tid in set dom (ar.trackcs) and
 (let tc = ar.trackcs(tid)
in
    jid not in set dom (tc.joints) and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints) union {joint}) and
 Only_One_Next_TrackC(ar.trackcs, tid, jid) and
 (forall tid1 in set dom (ar.trackcs) &
tid1 <> tid =>
 Joint_and_Next_TrackC(ar.trackcs(tid1), mu(tc,joints|->tc.joints ++ {jid |-> joint}), jid))) =>
 (let RESULT:Area = (let tc = ar.trackcs(tid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tid |-> mu(tc,joints|->tc.joints ++ {jid |-> joint})}))
in
    tid in set dom (RESULT.trackcs) and
 jid in set dom (RESULT.trackcs(tid).joints) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tid} <-: RESULT.trackcs = {tid} <-: ar.trackcs and
 RESULT.trackcs(tid) = mu(ar.trackcs(tid),joints|->ar.trackcs(tid).joints ++ {jid |-> joint}) and
 RESULT.trackcs(tid).joints(jid) = joint =>
 tid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Joint" ], 41944812 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tid : TrackC_id, jid : Joint_id, joint : Joint &
tid in set dom (ar.trackcs) and
 (let tc = ar.trackcs(tid)
in
    jid not in set dom (tc.joints) and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints) union {joint}) and
 Only_One_Next_TrackC(ar.trackcs, tid, jid) and
 (forall tid1 in set dom (ar.trackcs) &
tid1 <> tid =>
 Joint_and_Next_TrackC(ar.trackcs(tid1), mu(tc,joints|->tc.joints ++ {jid |-> joint}), jid))) =>
 (let RESULT:Area = (let tc = ar.trackcs(tid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tid |-> mu(tc,joints|->tc.joints ++ {jid |-> joint})}))
in
    tid in set dom (RESULT.trackcs) and
 jid in set dom (RESULT.trackcs(tid).joints) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tid} <-: RESULT.trackcs = {tid} <-: ar.trackcs and
 RESULT.trackcs(tid) = mu(ar.trackcs(tid),joints|->ar.trackcs(tid).joints ++ {jid |-> joint}) and
 RESULT.trackcs(tid).joints(jid) = joint and
 RESULT.trackcs(tid).atc = ar.trackcs(tid).atc =>
 tid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Joint" ], 41944812 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall ar : Area, tid : TrackC_id, jid : Joint_id, joint : Joint &
tid in set dom (ar.trackcs) and
 (let tc = ar.trackcs(tid)
in
    jid not in set dom (tc.joints) and
 TD_Used_for_NonInsulated_TrackC(tc.td, tc.atbt, rng (tc.joints) union {joint}) and
 Only_One_Next_TrackC(ar.trackcs, tid, jid) and
 (forall tid1 in set dom (ar.trackcs) &
tid1 <> tid =>
 Joint_and_Next_TrackC(ar.trackcs(tid1), mu(tc,joints|->tc.joints ++ {jid |-> joint}), jid))) =>
 (let RESULT:Area = (let tc = ar.trackcs(tid)
in
    mu(ar,trackcs|->ar.trackcs ++ {tid |-> mu(tc,joints|->tc.joints ++ {jid |-> joint})}))
in
    tid in set dom (RESULT.trackcs) and
 jid in set dom (RESULT.trackcs(tid).joints) and
 dom (RESULT.trackcs) = dom (ar.trackcs) and
 {tid} <-: RESULT.trackcs = {tid} <-: ar.trackcs and
 RESULT.trackcs(tid) = mu(ar.trackcs(tid),joints|->ar.trackcs(tid).joints ++ {jid |-> joint}) and
 RESULT.trackcs(tid).joints(jid) = joint and
 RESULT.trackcs(tid).atc = ar.trackcs(tid).atc and
 RESULT.trackcs(tid).td = ar.trackcs(tid).td =>
 tid in set dom (ar.trackcs)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Interlock" ], 41943041 ),
        mk_AS`Name( [ "Add_Joint" ], 41944812 ) ) ) }

