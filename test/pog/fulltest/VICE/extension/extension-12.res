{ mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "VolumeKnob" ], 41943659 ),
        mk_AS`Name( [ "HandleEvent" ], 41943661 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "TransmitTMC" ], 41943773 ),
        mk_AS`Name( [ "HandleEvent" ], 41943775 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <OpPostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InsertAddress" ], 41943716 ),
        mk_AS`Name( [ "HandleEvent" ], 41943718 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_real(max)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943562 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_real(min)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943562 ) ) ),
  mk_TEST`ProofObligationPP(
     "diff + dt > 0",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943562 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(dt,[nat])",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943562 ) ) ),
  mk_TEST`ProofObligationPP(
     "cnt in set dom (e2s)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943562 ) ) ),
  mk_TEST`ProofObligationPP(
     "card (dom (s2e)) <> 0",
     <NonZero>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "getMinMaxAverage" ], 41943562 ) ) ),
  mk_TEST`ProofObligationPP(
     "\"VolumeKnob\" in set dom (World`envTasks)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MMI" ], 41943236 ),
        mk_AS`Name( [ "UpdateScreen" ], 41943276 ) ) ),
  mk_TEST`ProofObligationPP(
     "\"TransmitTMC\" in set dom (World`envTasks)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MMI" ], 41943236 ),
        mk_AS`Name( [ "UpdateScreen" ], 41943276 ) ) ),
  mk_TEST`ProofObligationPP(
     "\"InsertAddress\" in set dom (World`envTasks)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MMI" ], 41943236 ),
        mk_AS`Name( [ "UpdateScreen" ], 41943276 ) ) ),
  mk_TEST`ProofObligationPP(
     "VolumeKnob`pre_checkResponseTimes(e2s, s2e, 1000)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "checkResponseTimes" ], 41943672 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "VolumeKnob" ], 41943659 ),
        mk_AS`Name( [ "HandleEvent" ], 41943661 ) ) ),
  mk_TEST`ProofObligationPP(
     "TransmitTMC`pre_checkResponseTimes(e2s, s2e, 10000)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "checkResponseTimes" ], 41943786 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "TransmitTMC" ], 41943773 ),
        mk_AS`Name( [ "HandleEvent" ], 41943775 ) ) ),
  mk_TEST`ProofObligationPP(
     "InsertAddress`pre_checkResponseTimes(e2s, s2e, 2000)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "checkResponseTimes" ], 41943729 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "InsertAddress" ], 41943716 ),
        mk_AS`Name( [ "HandleEvent" ], 41943718 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_11 in set dom (e2s), id_12 in set dom ({pev |-> time}) &
id_11 = id_12 =>
 e2s(id_11) = {pev |-> time}(id_12))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "logEnvToSys" ], 41943530 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_13 in set dom (s2e), id_14 in set dom ({pev |-> time}) &
id_13 = id_14 =>
 s2e(id_13) = {pev |-> time}(id_14))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "logSysToEnv" ], 41943546 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_8 in set dom (envTasks), id_9 in set dom ({pnm |-> penv}) &
id_8 = id_9 =>
 envTasks(id_8) = {pnm |-> penv}(id_9))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "World" ], 41943829 ),
        mk_AS`Name( [ "addEnvironmentTask" ], 41943846 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall pe2s : map nat to nat, ps2e : map nat to nat, plim : nat &
dom (ps2e) inter dom (pe2s) = dom (ps2e) =>
 (forall idx in set dom (ps2e) &
idx in set dom (pe2s)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "EnvironmentTask" ], 41943418 ),
        mk_AS`Name( [ "checkResponseTimes" ], 41943443 ) ) ) }

