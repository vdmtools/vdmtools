{ mk_TEST`ProofObligationPP(
     "(forall fun : (nat -> nat), n : nat &
(forall x : nat &
pre_(fun,x) =>
 pre_(fun,fun(x))) =>
 n > 1 =>
 (forall xx_2 : nat &
pre_(fun,xx_2) =>
 pre_(fun,fun(xx_2))))",
     <FuncIteration>,
     [ mk_AS`Name( [ "fun" ], 41943059 ),
       mk_AS`Name( [ "fun" ], 41943059 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "FunIter" ], 41943042 ) ) ) }

