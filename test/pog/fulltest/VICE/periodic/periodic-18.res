{ mk_TEST`ProofObligationPP(
     "latest in set dom (missilePriority)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943415 ),
        mk_AS`Name( [ "HigherPriority" ], 41943571 ) ) ),
  mk_TEST`ProofObligationPP(
     "current in set dom (missilePriority)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943415 ),
        mk_AS`Name( [ "HigherPriority" ], 41943571 ) ) ),
  mk_TEST`ProofObligationPP(
     "currentMissileType in set dom (responseDB)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943415 ),
        mk_AS`Name( [ "StepPlan" ], 41943727 ) ) ),
  mk_TEST`ProofObligationPP(
     "currentStep in set inds (responseDB(currentMissileType))",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943415 ),
        mk_AS`Name( [ "StepPlan" ], 41943727 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(MissileDetector,FlareController`missileDetectorRef)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943415 ),
        mk_AS`Name( [ "Init" ], 41943596 ) ) ) }

