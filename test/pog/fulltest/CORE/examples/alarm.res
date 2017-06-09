{ mk_TEST`ProofObligationPP(
     "(forall a : Alarm, peri : Period, plant : Plant &
alarm`pre_ExpertToPage(a, peri, plant) =>
 (exists r : Expert &
alarm`post_ExpertToPage(a, peri, plant, r)))",
     <Satisfiability>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "alarm" ], 41943041 ),
        mk_AS`Name( [ "ExpertToPage" ], 41943229 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a : Alarm, peri : Period, plant : Plant &
peri in set dom (plant.schedule) and
 a in set plant.alarms =>
 (forall r : Expert &
peri in set dom (plant.schedule)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "alarm" ], 41943041 ),
        mk_AS`Name( [ "ExpertToPage" ], 41943229 ) ) ) }

