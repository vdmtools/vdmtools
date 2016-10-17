{ mk_TEST`ProofObligationPP(
     "(forall xx_5 : nat &
pre_(((if 8 <> 5 then
f
else
i)),xx_5) =>
 pre_(((if 5 = 9 then
f
else
g)),((if 8 <> 5 then
f
else
i))(xx_5)))",
     <FuncComp>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943096 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_6 : (char | int) &
pre_(((if 8 <> 5 then
f
else
i)),xx_6) =>
 pre_(((if 5 = 9 then
f
else
g)),((if 8 <> 5 then
f
else
i))(xx_6)))",
     <FuncComp>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943096 ) ) ) }

