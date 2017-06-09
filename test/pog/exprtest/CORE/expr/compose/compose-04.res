{ mk_TEST`ProofObligationPP(
     "(forall n : (nat | seq of char | bool) &
is_real(n))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "f" ], 41943043 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_nat1(((if false then
{false |-> \"test\"}
else
g))(xx_1))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943098 ) ) ),
  mk_TEST`ProofObligationPP(
     "rng (((if false then
{false |-> \"test\"}
else
g))) subset dom (((if true then
{6 |-> true}
else
f)))",
     <MapComposition>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943098 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_3 : (seq of char | nat) &
pre_(((if false then
{false |-> \"test\"}
else
g)),xx_3) =>
 pre_(((if true then
{6 |-> true}
else
f)),((if false then
{false |-> \"test\"}
else
g))(xx_3)))",
     <FuncComp>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "NumExpTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943098 ) ) ) }

