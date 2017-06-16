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
     "(forall xx_2 : nat &
pre_(((if true then
1.7
else
lambda x : nat & x + 9)),xx_2) =>
 pre_(((if true then
1.7
else
lambda x : nat & x + 9)),((if true then
1.7
else
lambda x : nat & x + 9))(xx_2)))",
     <FuncIteration>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943069 ) ) ) }

