{ mk_TEST`ProofObligationPP(
     "(forall l : (bool | seq of nat), s : set of ((real | bool)) &
is_bool(l))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunDefSl4" ], 41943059 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a1,a2 : @A, l1 ^ l2 : seq of nat &
is_bool((if l1 <> [] then
f(8)
else
false)))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunDefSl7" ], 41943136 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a1,a2 : @A, l1 ^ l2 : seq of nat &
A`pre_FunDefSl7(a1, a2, l1 ^ l2) =>
 A`post_FunDefSl7(a1, a2, l1 ^ l2, mk_(a1,a1 = a2)))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunDefSl7" ], 41943136 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall xx_2 : @A, xx_3 : @A, xx_4 : seq of nat &
A`pre_FunDefSl7(xx_2, xx_3, xx_4) =>
 (exists l1 ^ l2 : seq of nat &
xx_4 = l1 ^ l2))",
     <FunctionPatterns>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunDefSl7" ], 41943136 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a1,a2 : @A, l1 ^ l2 : seq of nat &
(if l1 <> [] then
f(8)
else
false) =>
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
        mk_AS`Name( [ "FunDefSl7" ], 41943136 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall a1,a2 : @A, l1 ^ l2 : seq of nat &
(if l1 <> [] then
f(8)
else
false) =>
 (forall r : @A, s : bool &
is_bool((if r = mk_((if 4 > 8 then
a1
else
a2),len (l2) > 7) then
f(88)
else
true))))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunDefSl7" ], 41943136 ) ) ) }

