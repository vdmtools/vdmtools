{ mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "VolumeKnob" ], 41943606 ),
        mk_AS`Name( [ "HandleEvent" ], 41943608 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "TransmitTMC" ], 41943720 ),
        mk_AS`Name( [ "HandleEvent" ], 41943722 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InsertAddress" ], 41943663 ),
        mk_AS`Name( [ "HandleEvent" ], 41943665 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_real(max)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943509 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_real(min)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943509 ) ) ),
  mk_TEST`ProofObligationPP(
     "diff + dt > 0",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943509 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(dt,[nat])",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943509 ) ) ),
  mk_TEST`ProofObligationPP(
     "cnt in set dom (e2s)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943509 ) ) ),
  mk_TEST`ProofObligationPP(
     "card (dom (s2e)) <> 0",
     <NonZero>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943509 ) ) ),
  mk_TEST`ProofObligationPP(
     "\"VolumeKnob\" in set dom (World`envTasks)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MMI" ], 41943182 ),
        mk_AS`Name( [ "UpdateScreen" ], 41943222 ) ) ),
  mk_TEST`ProofObligationPP(
     "\"TransmitTMC\" in set dom (World`envTasks)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MMI" ], 41943182 ),
        mk_AS`Name( [ "UpdateScreen" ], 41943222 ) ) ),
  mk_TEST`ProofObligationPP(
     "\"InsertAddress\" in set dom (World`envTasks)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MMI" ], 41943182 ),
        mk_AS`Name( [ "UpdateScreen" ], 41943222 ) ) ),
  mk_TEST`ProofObligationPP(
     "VolumeKnob`pre_checkResponseTimes(e2s, s2e, 12000)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "checkResponseTimes" ], 41943619 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "VolumeKnob" ], 41943606 ),
        mk_AS`Name( [ "HandleEvent" ], 41943608 ) ) ),
  mk_TEST`ProofObligationPP(
     "TransmitTMC`pre_checkResponseTimes(e2s, s2e, 30000)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "checkResponseTimes" ], 41943733 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "TransmitTMC" ], 41943720 ),
        mk_AS`Name( [ "HandleEvent" ], 41943722 ) ) ),
  mk_TEST`ProofObligationPP(
     "InsertAddress`pre_checkResponseTimes(e2s, s2e, 14000)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "checkResponseTimes" ], 41943676 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InsertAddress" ], 41943663 ),
        mk_AS`Name( [ "HandleEvent" ], 41943665 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_7 in set dom (e2s), id_8 in set dom ({pev |-> time}) &
id_7 = id_8 =>
 e2s(id_7) = {pev |-> time}(id_8))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "logEnvToSys" ], 41943477 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_9 in set dom (s2e), id_10 in set dom ({pev |-> time}) &
id_9 = id_10 =>
 s2e(id_9) = {pev |-> time}(id_10))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "logSysToEnv" ], 41943493 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_5 in set dom (envTasks), id_6 in set dom ({pnm |-> penv}) &
id_5 = id_6 =>
 envTasks(id_5) = {pnm |-> penv}(id_6))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "World" ], 41943776 ),
        mk_AS`Name( [ "addEnvironmentTask" ], 41943793 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall pe2s : map nat to nat, ps2e : map nat to nat, plim : nat &
dom (ps2e) inter dom (pe2s) = dom (ps2e) =>
 (forall idx in set dom (ps2e) &
idx in set dom (pe2s)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943364 ),
        mk_AS`Name( [ "checkResponseTimes" ], 41943389 ) ) ) }

