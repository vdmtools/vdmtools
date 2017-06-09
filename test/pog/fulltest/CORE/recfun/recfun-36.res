{ mk_TEST`ProofObligationPP(
     "(forall mk_Bla(b) : Bla &
not (b = (0)) =>
 b - 1 >= 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943061 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_Bla(b) : Bla &
not (b = (0)) =>
 idbla(mk_Bla(b)) > idbla(mk_Bla(b - 1)))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "foo" ], 41943079 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943061 ) ) ) }

