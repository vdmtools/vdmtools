{ mk_TEST`ProofObligationPP(
     "(forall tran : TranCommand &
<X> in set dom (tran))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "PrioritizedTranCmd" ], 41943953 ) ) ),
  mk_TEST`ProofObligationPP(
     "SAFERCL`inv_RotCommand({a |-> <Zero> | a in set rot_axis_set})",
     <Invariants>,
     [ mk_AS`Name( [ "RotCommand" ], 41943315 ) ],
     mk_POGTP`LocContext(
        <values>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "null_rot_command" ], 41943313 ) ) ),
  mk_TEST`ProofObligationPP(
     "SAFERCL`inv_TranCommand({a |-> <Zero> | a in set tran_axis_set})",
     <Invariants>,
     [ mk_AS`Name( [ "TranCommand" ], 41943302 ) ],
     mk_POGTP`LocContext(
        <values>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "null_tran_command" ], 41943300 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tran : TranCommand &
tran(<X>) <> <Zero> =>
 <X> in set dom (tran))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "PrioritizedTranCmd" ], 41943953 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tran : TranCommand &
not (tran(<X>) <> <Zero>) =>
 <Y> in set dom (tran))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "PrioritizedTranCmd" ], 41943953 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tran : TranCommand &
not (tran(<X>) <> <Zero>) =>
 tran(<Y>) <> <Zero> =>
 <Y> in set dom (tran))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "PrioritizedTranCmd" ], 41943953 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tran : TranCommand &
not (tran(<X>) <> <Zero>) =>
 not (tran(<Y>) <> <Zero>) =>
 <Z> in set dom (tran))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "PrioritizedTranCmd" ], 41943953 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm_rot : RotCommand, aah : RotCommand, ignore_hcm : set of RotAxis &
(forall a in set rot_axis_set &
a in set dom (hcm_rot)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "CombinedRotCmds" ], 41944007 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tran : TranCommand &
not (tran(<X>) <> <Zero>) =>
 not (tran(<Y>) <> <Zero>) =>
 tran(<Z>) <> <Zero> =>
 <Z> in set dom (tran))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "PrioritizedTranCmd" ], 41943953 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm_rot : RotCommand, aah : RotCommand, ignore_hcm : set of RotAxis &
(let aah_axes = ignore_hcm union {a | a in set rot_axis_set & hcm_rot(a) = <Zero>}
in
    (forall a in set aah_axes &
a in set dom (aah))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "CombinedRotCmds" ], 41944007 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm)
in
    <X> in set dom (tran)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm)
in
    <Yaw> in set dom (rot)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm)
in
    <Pitch> in set dom (rot)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm_rot : RotCommand, aah : RotCommand, ignore_hcm : set of RotAxis &
(let aah_axes = ignore_hcm union {a | a in set rot_axis_set & hcm_rot(a) = <Zero>}
in
    (forall a in set rot_axis_set \\ aah_axes &
a in set dom (hcm_rot))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "CombinedRotCmds" ], 41944007 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm_rot : RotCommand, aah : RotCommand, ignore_hcm : set of RotAxis &
SAFERCL`inv_RotCommand((let aah_axes = ignore_hcm union {a | a in set rot_axis_set & hcm_rot(a) = <Zero>}
in
    {a |-> aah(a) | a in set aah_axes} munion {a |-> hcm_rot(a) | a in set rot_axis_set \\ aah_axes})))",
     <Invariants>,
     [ mk_AS`Name( [ "RotCommand" ], 41944017 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "CombinedRotCmds" ], 41944007 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall tran : TranCommand &
SAFERCL`inv_TranCommand((if tran(<X>) <> <Zero> then
null_tran_command ++ {<X> |-> tran(<X>)}
else(if tran(<Y>) <> <Zero> then
null_tran_command ++ {<Y> |-> tran(<Y>)}
else(if tran(<Z>) <> <Zero> then
null_tran_command ++ {<Z> |-> tran(<Z>)}
else
null_tran_command)))))",
     <Invariants>,
     [ mk_AS`Name( [ "TranCommand" ], 41943956 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "PrioritizedTranCmd" ], 41943953 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm), mk_(bf_mandatory,bf_optional) = BFThrusters(tran(<X>), rot(<Pitch>), rot(<Yaw>))
in
    <Y> in set dom (tran)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm), mk_(bf_mandatory,bf_optional) = BFThrusters(tran(<X>), rot(<Pitch>), rot(<Yaw>))
in
    <Z> in set dom (tran)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm), mk_(bf_mandatory,bf_optional) = BFThrusters(tran(<X>), rot(<Pitch>), rot(<Yaw>))
in
    <Roll> in set dom (rot)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall button_pos : ControlButton, hcm_cmd : SixDofCommand, clock : nat, aahstate : AAH &
(let mk_AAH(active_axes,ignore_hcm,toggle,timeout) = aahstate, engage = ButtonTransition(toggle, button_pos, active_axes, clock, timeout), starting = (toggle = <AAH_off>) and
 (engage = <AAH_started>)
in
    (forall a in set rot_axis_set &
starting =>
 a in set dom (hcm_cmd.rot))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "Transition" ], 41943448 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_HandGripPosition(vert,horiz,trans,twist) : HandGripPosition, mode : ControlModeSwitch &
(let tran = {<X> |-> horiz,<Y> |-> (if mode = <Tran> then
trans
else
<Zero>),<Z> |-> (if mode = <Tran> then
vert
else
<Zero>)}, rot = {<Roll> |-> (if mode = <Rot> then
vert
else
<Zero>),<Pitch> |-> twist,<Yaw> |-> (if mode = <Rot> then
twist
else
<Zero>)}
in
    SAFERCL`inv_RotCommand(rot)))",
     <Invariants>,
     [ mk_AS`Name( [ "RotCommand" ], 41943399 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "GripCommand" ], 41943779 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_HandGripPosition(vert,horiz,trans,twist) : HandGripPosition, mode : ControlModeSwitch &
(let tran = {<X> |-> horiz,<Y> |-> (if mode = <Tran> then
trans
else
<Zero>),<Z> |-> (if mode = <Tran> then
vert
else
<Zero>)}, rot = {<Roll> |-> (if mode = <Rot> then
vert
else
<Zero>),<Pitch> |-> twist,<Yaw> |-> (if mode = <Rot> then
twist
else
<Zero>)}
in
    SAFERCL`inv_TranCommand(tran)))",
     <Invariants>,
     [ mk_AS`Name( [ "TranCommand" ], 41943395 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "GripCommand" ], 41943779 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm), mk_(bf_mandatory,bf_optional) = BFThrusters(tran(<X>), rot(<Pitch>), rot(<Yaw>)), mk_(lrud_mandatory,lrud_optional) = LRUDThrusters(tran(<Y>), tran(<Z>), rot(<Roll>))
in
    <Roll> in set dom (rot)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall button_pos : ControlButton, hcm_cmd : SixDofCommand, clock : nat, aahstate : AAH &
(let mk_AAH(active_axes,ignore_hcm,toggle,timeout) = aahstate, engage = ButtonTransition(toggle, button_pos, active_axes, clock, timeout), starting = (toggle = <AAH_off>) and
 (engage = <AAH_started>)
in
    (forall a in set rot_axis_set &
not (starting) =>
 engage <> <AAH_off> and
 a in set active_axes =>
 a in set dom (hcm_cmd.rot))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "Transition" ], 41943448 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm_rot : RotCommand, aah : RotCommand, ignore_hcm : set of RotAxis &
(let aah_axes = ignore_hcm union {a | a in set rot_axis_set & hcm_rot(a) = <Zero>}
in
    (forall id_85 in set dom ({a |-> aah(a) | a in set aah_axes}), id_86 in set dom ({a |-> hcm_rot(a) | a in set rot_axis_set \\ aah_axes}) &
id_85 = id_86 =>
 {a |-> aah(a) | a in set aah_axes}(id_85) = {a |-> hcm_rot(a) | a in set rot_axis_set \\ aah_axes}(id_86))))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "CombinedRotCmds" ], 41944007 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm), mk_(bf_mandatory,bf_optional) = BFThrusters(tran(<X>), rot(<Pitch>), rot(<Yaw>)), mk_(lrud_mandatory,lrud_optional) = LRUDThrusters(tran(<Y>), tran(<Z>), rot(<Roll>)), bf_thr = (if rot(<Roll>) = <Zero> then
bf_optional union bf_mandatory
else
bf_mandatory)
in
    <Pitch> in set dom (rot)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall hcm : SixDofCommand, aah : RotCommand, active_axes : set of RotAxis, ignore_hcm : set of RotAxis &
(let mk_SixDofCommand(tran,rot) = IntegratedCommands(hcm, aah, active_axes, ignore_hcm), mk_(bf_mandatory,bf_optional) = BFThrusters(tran(<X>), rot(<Pitch>), rot(<Yaw>)), mk_(lrud_mandatory,lrud_optional) = LRUDThrusters(tran(<Y>), tran(<Z>), rot(<Roll>)), bf_thr = (if rot(<Roll>) = <Zero> then
bf_optional union bf_mandatory
else
bf_mandatory)
in
    rot(<Pitch>) = <Zero> =>
 <Yaw> in set dom (rot)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "SelectedThrusters" ], 41944897 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_SwitchPositions(mode,aah) : SwitchPositions, raw_grip : HandGripPosition, aah_cmd : RotCommand, saferstate : SAFER, aahstate : AAH &
SAFERCL`post_ControlCycle(mk_SwitchPositions(mode,aah), raw_grip, aah_cmd, saferstate, aahstate, (let mk_SAFER(clock) = saferstate, grip_cmd = GripCommand(raw_grip, mode), thrusters = SelectedThrusters(grip_cmd, aah_cmd, aahstate.active_axes, aahstate.ignore_hcm), aahstate' = Transition(aah, grip_cmd, clock, aahstate), saferstate' = mu(saferstate,clock|->clock + 1)
in
    mk_(thrusters,saferstate',aahstate'))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "ControlCycle" ], 41943049 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall estate : EngageState, button : ControlButton, active : set of RotAxis, clock : nat, timeout : nat &
mk_(estate,button) = mk_((<AAH_off>),(<Up>)) or
 mk_(estate,button) = mk_((<AAH_off>),(<Down>)) or
 mk_(estate,button) = mk_((<AAH_started>),(<Up>)) or
 mk_(estate,button) = mk_((<AAH_started>),(<Down>)) or
 mk_(estate,button) = mk_((<AAH_on>),(<Up>)) or
 mk_(estate,button) = mk_((<AAH_on>),(<Down>)) or
 mk_(estate,button) = mk_((<pressed_once>),(<Up>)) or
 mk_(estate,button) = mk_((<pressed_once>),(<Down>)) or
 mk_(estate,button) = mk_((<AAH_closing>),(<Up>)) or
 mk_(estate,button) = mk_((<AAH_closing>),(<Down>)) or
 mk_(estate,button) = mk_((<pressed_twice>),(<Up>)) or
 mk_(estate,button) = mk_((<pressed_twice>),(<Down>)))",
     <CasesExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "ButtonTransition" ], 41943602 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall A : AxisCommand, B : AxisCommand, C : AxisCommand &
mk_(A,B,C) = mk_((<Neg>),(<Neg>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Neg>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Neg>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Zero>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Zero>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Zero>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Pos>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Pos>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Pos>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Neg>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Neg>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Neg>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Zero>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Zero>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Zero>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Pos>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Pos>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Pos>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Neg>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Neg>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Neg>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Zero>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Zero>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Zero>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Pos>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Pos>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Pos>),(<Pos>)))",
     <CasesExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "BFThrusters" ], 41944137 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall A : AxisCommand, B : AxisCommand, C : AxisCommand &
mk_(A,B,C) = mk_((<Neg>),(<Neg>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Neg>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Neg>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Zero>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Zero>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Zero>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Pos>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Pos>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Neg>),(<Pos>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Neg>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Neg>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Neg>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Zero>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Zero>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Zero>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Pos>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Pos>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Zero>),(<Pos>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Neg>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Neg>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Neg>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Zero>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Zero>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Zero>),(<Pos>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Pos>),(<Neg>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Pos>),(<Zero>)) or
 mk_(A,B,C) = mk_((<Pos>),(<Pos>),(<Pos>)))",
     <CasesExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "SAFERCL" ], 41943041 ),
        mk_AS`Name( [ "LRUDThrusters" ], 41944527 ) ) ) }

