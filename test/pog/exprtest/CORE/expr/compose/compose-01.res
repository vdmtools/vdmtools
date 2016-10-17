{ mk_TEST`ProofObligationPP(
     "is_nat1(((if 5 <> 7 then
f
else
{8 |-> 9,7 |-> \"string\"}))(xx_1))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943090 ) ) ),
  mk_TEST`ProofObligationPP(
     "rng (((if 5 <> 7 then
f
else
{8 |-> 9,7 |-> \"string\"}))) subset dom (((if 7 < 6 then
g
else
{9 |-> true,8 |-> 8})))",
     <MapComposition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943090 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_3 : nat &
pre_(((if 5 <> 7 then
f
else
{8 |-> 9,7 |-> \"string\"})),xx_3) =>
 pre_(((if 7 < 6 then
g
else
{9 |-> true,8 |-> 8})),((if 5 <> 7 then
f
else
{8 |-> 9,7 |-> \"string\"}))(xx_3)))",
     <FuncComp>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943090 ) ) ) }

