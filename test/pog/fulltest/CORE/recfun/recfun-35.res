{ mk_TEST`ProofObligationPP(
     "(forall mk_List(h,t) : List &
not (t = nil) =>
 is_A`List(A`t))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "id" ], 41943053 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_List(h,t) : List &
not (t = nil) =>
 is_A`List(A`t))",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943077 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall mk_List(h,t) : List &
not (t = nil) =>
 id(mk_List(h,t)) > id(t))",
     <RecursiveFunction>,
     [ mk_AS`Name( [ "foo" ], 41943096 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "A" ], 41943041 ),
        mk_AS`Name( [ "foo" ], 41943077 ) ) ) }

