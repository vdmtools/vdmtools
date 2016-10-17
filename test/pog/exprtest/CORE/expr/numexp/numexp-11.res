{ mk_TEST`ProofObligationPP(
     "is_nat(((if false then
7
else
g(7))))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943069 ) ) ),
  mk_TEST`ProofObligationPP(
     "((if false then
7
else
g(7))) = 0 or
 ((if false then
7
else
g(7))) = 1 or
 rng (((if true then
1.7
else
{8 |-> 9}))) subset dom (((if true then
1.7
else
{8 |-> 9})))",
     <MapIteration>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943069 ) ) ) }

