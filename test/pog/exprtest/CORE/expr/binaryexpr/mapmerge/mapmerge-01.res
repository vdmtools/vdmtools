{ mk_TEST`ProofObligationPP(
     "(forall id_1 in set dom ({1 |-> 3}), id_2 in set dom ({true |-> 5}) &
id_1 = id_2 =>
 {1 |-> 3}(id_1) = {true |-> 5}(id_2))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "BinaryTest" ], 41943041 ),
        mk_AS`Name( [ "TestOP" ], 41943043 ) ) ) }

