{ mk_TEST`ProofObligationPP(
     "(forall a1,a2 : @A, l1 ^ l2 : seq of nat &
A`pre_FunDefSl7(a1, a2, l1 ^ l2) =>
 A`post_FunDefSl7(a1, a2, l1 ^ l2, mk_(a1,a1 = a2)))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunDefSl7" ], 41943087 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_1 : @A, xx_2 : @A, xx_3 : seq of nat &
A`pre_FunDefSl7(xx_1, xx_2, xx_3) =>
 (exists l1 ^ l2 : seq of nat &
xx_3 = l1 ^ l2))",
     <FunctionPatterns>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunDefSl7" ], 41943087 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a1,a2 : @A, l1 ^ l2 : seq of nat &
l1 <> [] =>
 (forall r : @A, s : bool &
is_(r,(@A * bool)) or
 is_(mk_((if 4 > 8 then
a1
else
a2),len (l2) > 7),@A)))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunDefSl7" ], 41943087 ) ) ) }

