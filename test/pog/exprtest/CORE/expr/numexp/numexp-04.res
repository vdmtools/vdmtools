{ mk_TEST`ProofObligationPP(
     "is_nat(((if 5 <> 8 then
f
else
g))(xx_2))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943067 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_3 : nat &
pre_(((if 5 <> 8 then
f
else
g)),xx_3) =>
 pre_(((if 5 <> 8 then
f
else
g)),((if 5 <> 8 then
f
else
g))(xx_3)))",
     <FuncIteration>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943067 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_4 : nat &
pre_(((if 5 <> 8 then
f
else
g)),xx_4) =>
 pre_(((if 5 <> 8 then
f
else
g)),((if 5 <> 8 then
f
else
g))(xx_4)))",
     <FuncIteration>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943067 ) ) ) }

