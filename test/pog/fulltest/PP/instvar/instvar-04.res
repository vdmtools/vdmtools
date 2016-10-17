{ mk_TEST`ProofObligationPP(
     "i1 <> 0",
     <NonZero>,
     [  ],
     mk_POGTP`LocContext(
        <instanceVariables>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "A",
             "i3" ], -1 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_nat(1 / i1)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <instanceVariables>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "A",
             "i3" ], -1 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_nat((if true then
1
else
Op(9)))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <instanceVariables>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "A",
             "i4" ], -1 ) ) ) }

