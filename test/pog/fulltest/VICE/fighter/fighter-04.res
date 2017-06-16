{ mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Environment" ], 41944090 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "addSensor" ], 41944172 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Environment" ], 41944090 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "Environment" ], 41944154 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Environment" ], 41944090 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "handleEvent" ], 41944293 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Environment" ], 41944090 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "createSignal" ], 41944210 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "FlareController" ], 41943284 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943284 ),
        mk_AS`Name( [ "addThreat" ], 41943416 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "FlareController" ], 41943284 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943284 ),
        mk_AS`Name( [ "getThreat" ], 41943447 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "FlareController" ], 41943284 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943284 ),
        mk_AS`Name( [ "addDispenser" ], 41943349 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "FlareController" ], 41943284 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943284 ),
        mk_AS`Name( [ "FlareController" ], 41943335 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "MissileDetector" ], 41944350 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944350 ),
        mk_AS`Name( [ "addThreat" ], 41944436 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "MissileDetector" ], 41944350 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944350 ),
        mk_AS`Name( [ "getThreat" ], 41944467 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "MissileDetector" ], 41944350 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944350 ),
        mk_AS`Name( [ "addController" ], 41944396 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <WhileLoopStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "createSignal" ], 41944210 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <WhileLoopStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41943567 ),
        mk_AS`Name( [ "evalQueue" ], 41943764 ) ) ),
  mk_TEST`ProofObligationPP(
     "curplan <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41943567 ),
        mk_AS`Name( [ "evalQueue" ], 41943764 ) ) ),
  mk_TEST`ProofObligationPP(
     "inlines <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "createSignal" ], 41944210 ) ) ),
  mk_TEST`ProofObligationPP(
     "newplan <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41943567 ),
        mk_AS`Name( [ "addThreat" ], 41943686 ) ) ),
  mk_TEST`ProofObligationPP(
     "threats <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943284 ),
        mk_AS`Name( [ "getThreat" ], 41943447 ) ) ),
  mk_TEST`ProofObligationPP(
     "threats <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944350 ),
        mk_AS`Name( [ "getThreat" ], 41944467 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(pa)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41943941 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "createSignal" ], 41944210 ) ) ),
  mk_TEST`ProofObligationPP(
     "id in set dom (sensors)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "createSignal" ], 41944210 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(papplhs)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944030 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "createSignal" ], 41944210 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(pappsize)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944033 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "createSignal" ], 41944210 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(aperature)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944030 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Sensor" ], 41943887 ),
        mk_AS`Name( [ "trip" ], 41943938 ) ) ),
  mk_TEST`ProofObligationPP(
     "pmt in set dom (responseDB)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41943567 ),
        mk_AS`Name( [ "addThreat" ], 41943686 ) ) ),
  mk_TEST`ProofObligationPP(
     "Environment`inv_Angle(xx_7.#2)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944095 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "Environment" ], 41944154 ) ) ),
  mk_TEST`ProofObligationPP(
     "Environment`inv_Angle(yy_4.#1)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944130 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "addSensor" ], 41944172 ) ) ),
  mk_TEST`ProofObligationPP(
     "Environment`inv_Angle(yy_4.#2)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944132 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "addSensor" ], 41944172 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(Environment,World`env)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "World" ], 41943203 ),
        mk_AS`Name( [ "Run" ], 41943254 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(Environment,World`env)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "World" ], 41943203 ),
        mk_AS`Name( [ "World" ], 41943211 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(Environment,World`env)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41943567 ),
        mk_AS`Name( [ "releaseFlare" ], 41943840 ) ) ),
  mk_TEST`ProofObligationPP(
     "pmt in set dom (missilePriority)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareDispenser" ], 41943567 ),
        mk_AS`Name( [ "addThreat" ], 41943686 ) ) ),
  mk_TEST`ProofObligationPP(
     "GLOBAL`inv_Angle(SENSOR_APERATURE)",
     <Invariants>,
     [ mk_AS`Name( [ "GLOBAL",
            "Angle" ], 41944033 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Sensor" ], 41943887 ),
        mk_AS`Name( [ "trip" ], 41943938 ) ) ),
  mk_TEST`ProofObligationPP(
     "MissileDetector`inv_Angle(yy_29.#1)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944354 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944350 ),
        mk_AS`Name( [ "addController" ], 41944396 ) ) ),
  mk_TEST`ProofObligationPP(
     "MissileDetector`inv_Angle(yy_29.#2)",
     <Invariants>,
     [ mk_AS`Name( [ "Angle" ], 41944356 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944350 ),
        mk_AS`Name( [ "addController" ], 41944396 ) ) ),
  mk_TEST`ProofObligationPP(
     "sensors(id).Sensor`pre_trip(pmt, pa)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "trip" ], 41944258 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "createSignal" ], 41944210 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(ranges munion {id |-> mk_(angle,DISPENSER_APERATURE)},map nat to (Angle * Angle))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943284 ),
        mk_AS`Name( [ "addDispenser" ], 41943349 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_5 in set dom (sensors), id_6 in set dom ({id |-> psens}) &
id_5 = id_6 =>
 sensors(id_5) = {id |-> psens}(id_6))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "addSensor" ], 41944172 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_23 in set dom (dispensers), id_24 in set dom ({id |-> pfldisp}) &
id_23 = id_24 =>
 dispensers(id_23) = {id |-> pfldisp}(id_24))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943284 ),
        mk_AS`Name( [ "addDispenser" ], 41943349 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_30 in set dom (controllers), id_31 in set dom ({nid |-> pctrl}) &
id_30 = id_31 =>
 controllers(id_30) = {nid |-> pctrl}(id_31))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944350 ),
        mk_AS`Name( [ "addController" ], 41944396 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_1 in set dom (ranges), id_2 in set dom ({id |-> psens.getAperature()}) &
id_1 = id_2 =>
 ranges(id_1) = {id |-> psens.getAperature()}(id_2))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Environment" ], 41944090 ),
        mk_AS`Name( [ "addSensor" ], 41944172 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_26 in set dom (ranges), id_27 in set dom ({nid |-> pctrl.getAperature()}) &
id_26 = id_27 =>
 ranges(id_26) = {nid |-> pctrl.getAperature()}(id_27))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "MissileDetector" ], 41944350 ),
        mk_AS`Name( [ "addController" ], 41944396 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall id_21 in set dom (ranges), id_22 in set dom ({id |-> mk_(angle,DISPENSER_APERATURE)}) &
id_21 = id_22 =>
 ranges(id_21) = {id |-> mk_(angle,DISPENSER_APERATURE)}(id_22))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "FlareController" ], 41943284 ),
        mk_AS`Name( [ "addDispenser" ], 41943349 ) ) ) }

