{ mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Environment" ], 41944549 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "addSensor" ], 41944635 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Environment" ], 41944549 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "Environment" ], 41944613 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Environment" ], 41944549 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "handleEvent" ], 41944760 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Environment" ], 41944549 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "createSignal" ], 41944675 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "FlareController" ], 41943741 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "addThreat" ], 41943873 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "FlareController" ], 41943741 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "getThreat" ], 41943904 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "FlareController" ], 41943741 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "addDispenser" ], 41943806 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "FlareController" ], 41943741 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "FlareController" ], 41943792 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "MissileDetector" ], 41944819 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944819 ),
        mk_AS`Name( [ "addThreat" ], 41944910 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "MissileDetector" ], 41944819 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944819 ),
        mk_AS`Name( [ "getThreat" ], 41944941 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "MissileDetector" ], 41944819 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944819 ),
        mk_AS`Name( [ "addController" ], 41944865 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <WhileLoopStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "createSignal" ], 41944675 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <WhileLoopStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41944024 ),
        mk_AS`Name( [ "evalQueue" ], 41944221 ) ) ),
  mk_TEST`ProofObligationPP(
     "curplan <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41944024 ),
        mk_AS`Name( [ "evalQueue" ], 41944221 ) ) ),
  mk_TEST`ProofObligationPP(
     "inlines <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "createSignal" ], 41944675 ) ) ),
  mk_TEST`ProofObligationPP(
     "newplan <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41944024 ),
        mk_AS`Name( [ "addThreat" ], 41944143 ) ) ),
  mk_TEST`ProofObligationPP(
     "threats <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "getThreat" ], 41943904 ) ) ),
  mk_TEST`ProofObligationPP(
     "threats <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944819 ),
        mk_AS`Name( [ "getThreat" ], 41944941 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(pa)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944400 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "createSignal" ], 41944675 ) ) ),
  mk_TEST`ProofObligationPP(
     "id in set dom (sensors)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "createSignal" ], 41944675 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(papplhs)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944489 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "createSignal" ], 41944675 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(aperture)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944489 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Sensor" ], 41944346 ),
        mk_AS`Name( [ "trip" ], 41944397 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(pappsize)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944492 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "createSignal" ], 41944675 ) ) ),
  mk_TEST`ProofObligationPP(
     "pmt in set dom (responseDB)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41944024 ),
        mk_AS`Name( [ "addThreat" ], 41944143 ) ) ),
  mk_TEST`ProofObligationPP(
     "Environment`inv_Angle(xx_7.#2)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944554 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "Environment" ], 41944613 ) ) ),
  mk_TEST`ProofObligationPP(
     "Environment`inv_Angle(yy_4.#1)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944589 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "addSensor" ], 41944635 ) ) ),
  mk_TEST`ProofObligationPP(
     "Environment`inv_Angle(yy_4.#2)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944591 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "addSensor" ], 41944635 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(Environment,World`env)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "World" ], 41943527 ),
        mk_AS`Name( [ "Run" ], 41943711 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(Environment,World`env)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "World" ], 41943527 ),
        mk_AS`Name( [ "World" ], 41943535 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(Environment,World`env)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41944024 ),
        mk_AS`Name( [ "releaseFlare" ], 41944299 ) ) ),
  mk_TEST`ProofObligationPP(
     "pmt in set dom (missilePriority)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41944024 ),
        mk_AS`Name( [ "addThreat" ], 41944143 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(SENSOR_APERTURE)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944492 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Sensor" ], 41944346 ),
        mk_AS`Name( [ "trip" ], 41944397 ) ) ),
  mk_TEST`ProofObligationPP(
     "FlareController`inv_Angle(yy_24.#1)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41943750 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "addDispenser" ], 41943806 ) ) ),
  mk_TEST`ProofObligationPP(
     "FlareController`inv_Angle(yy_24.#2)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41943752 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "addDispenser" ], 41943806 ) ) ),
  mk_TEST`ProofObligationPP(
     "MissileDetector`inv_Angle(yy_31.#1)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944823 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944819 ),
        mk_AS`Name( [ "addController" ], 41944865 ) ) ),
  mk_TEST`ProofObligationPP(
     "MissileDetector`inv_Angle(yy_31.#2)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944825 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944819 ),
        mk_AS`Name( [ "addController" ], 41944865 ) ) ),
  mk_TEST`ProofObligationPP(
     "sensors(id).Sensor`pre_trip(pmt, pa)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "trip" ], 41944724 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "createSignal" ], 41944675 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_5 in set dom (sensors), id_6 in set dom ({id |-> psens}) &
id_5 = id_6 =>
 sensors(id_5) = {id |-> psens}(id_6))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "addSensor" ], 41944635 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_25 in set dom (dispensers), id_26 in set dom ({id |-> pfldisp}) &
id_25 = id_26 =>
 dispensers(id_25) = {id |-> pfldisp}(id_26))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "addDispenser" ], 41943806 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_32 in set dom (controllers), id_33 in set dom ({nid |-> pctrl}) &
id_32 = id_33 =>
 controllers(id_32) = {nid |-> pctrl}(id_33))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944819 ),
        mk_AS`Name( [ "addController" ], 41944865 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_1 in set dom (ranges), id_2 in set dom ({id |-> psens.getAperture()}) &
id_1 = id_2 =>
 ranges(id_1) = {id |-> psens.getAperture()}(id_2))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944549 ),
        mk_AS`Name( [ "addSensor" ], 41944635 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_28 in set dom (ranges), id_29 in set dom ({nid |-> pctrl.getAperture()}) &
id_28 = id_29 =>
 ranges(id_28) = {nid |-> pctrl.getAperture()}(id_29))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944819 ),
        mk_AS`Name( [ "addController" ], 41944865 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_21 in set dom (ranges), id_22 in set dom ({id |-> mk_(angle,DISPENSER_APERTURE)}) &
id_21 = id_22 =>
 ranges(id_21) = {id |-> mk_(angle,DISPENSER_APERTURE)}(id_22))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943741 ),
        mk_AS`Name( [ "addDispenser" ], 41943806 ) ) ) }

