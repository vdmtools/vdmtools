{ mk_TEST`ProofObligationPP(
     "8 = 0 or
 8 = 1 or
 rng (((if 6 = 9 then
7
else(if true = false then
{3 |-> 3}
else
f)))) subset dom (((if 6 = 9 then
7
else(if true = false then
{3 |-> 3}
else
f))))",
     <MapIteration>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943067 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_3 : nat &
pre_(((if 6 = 9 then
7
else(if true = false then
{3 |-> 3}
else
f))),xx_3) =>
 pre_(((if 6 = 9 then
7
else(if true = false then
{3 |-> 3}
else
f))),((if 6 = 9 then
7
else(if true = false then
{3 |-> 3}
else
f)))(xx_3)))",
     <FuncIteration>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943067 ) ) ) }

