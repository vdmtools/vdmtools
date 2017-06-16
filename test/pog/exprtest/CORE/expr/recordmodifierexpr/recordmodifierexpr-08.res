{ mk_TEST`ProofObligationPP(
     "is_((if 5 < 9 then
mk_A(1,7.5,<RED>)
else
mk_B(7,nil)),compose A of n : nat1
r : real
b : (bool | <RED>)
end)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "RecordModifierExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943073 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(mu((if 5 < 9 then
mk_A(1,7.5,<RED>)
else
mk_B(7,nil)),r|->5.5,b|->true),compose A of n : nat1
r : real
b : (bool | <RED>)
end)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "RecordModifierExprTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943073 ) ) ) }

