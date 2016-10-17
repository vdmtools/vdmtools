{ mk_TEST`ProofObligationPP(
     "(forall mk_Tree(x) : Tree &
is_nat(cases x:
mk_(x,y) -> foo(x) + foo(y),
others -> x
end))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943084 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Tree(x) : Tree &
(exists mk_(x,y) : (nat | (Tree * Tree)) &
x = mk_(x,y)) =>
 (let mk_(x,y) = x
in
    id(mk_Tree(x)) > id(x)))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "foo" ], 41943100 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943084 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Tree(x) : Tree &
(exists mk_(x,y) : (nat | (Tree * Tree)) &
x = mk_(x,y)) =>
 (let mk_(x,y) = x
in
    id(mk_Tree(x)) > id(y)))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "foo" ], 41943103 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943084 ) ) ) }

