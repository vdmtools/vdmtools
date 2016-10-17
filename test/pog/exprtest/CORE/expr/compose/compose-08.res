{ mk_TEST`ProofObligationPP(
     "rng (((if 8 <> 5 then
f
else
{9 |-> 4}))) subset dom (((if 5 = 9 then
{8 |-> 9}
else
g)))",
     <MapComposition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943090 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_3 : nat &
pre_(((if 8 <> 5 then
f
else
{9 |-> 4})),xx_3) =>
 pre_(((if 5 = 9 then
{8 |-> 9}
else
g)),((if 8 <> 5 then
f
else
{9 |-> 4}))(xx_3)))",
     <FuncComp>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943090 ) ) ) }

