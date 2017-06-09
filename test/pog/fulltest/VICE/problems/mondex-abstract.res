{ mk_TEST`ProofObligationPP(
     "(forall p : AbPurse, val : nat &
p.balance >= val =>
 p.balance - val > 0)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "ReduceBalance" ], 41943152 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall auth : map PurseId to AbPurse &
AbPurseFunctional`inv_AbWorld(mk_AbWorld({},auth)))",
     <Invariants>,
     [ mk_AS`Name( [ "AbWorld" ], 41943204 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "newAbWorld" ], 41943198 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 AbPurseFunctional`pre_ReduceBalance(wrld.abPurses(frm), val))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "ReduceBalance" ], 41943456 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 AbPurseFunctional`pre_ReduceBalance(wrld.abPurses(frm), val))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "ReduceBalance" ], 41943251 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    too in set dom (wrld.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 AbPurseFunctional`inv_AbWorld(((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))))",
     <Invariants>,
     [ mk_AS`Name( [ "AbWorld" ], 41943443 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, xx_7 : PurseId, val : nat &
AbPurseFunctional`pre_TransferLost(wrld, frm, xx_7, val) =>
 AbPurseFunctional`post_TransferLost(wrld, frm, xx_7, val, ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))
in
    frm in set dom (wrld.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))
in
    frm in set dom (RESULT.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
AbPurseFunctional`pre_TransferOk(wrld, frm, too, val) =>
 AbPurseFunctional`post_TransferOk(wrld, frm, too, val, ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))
in
    GetTotal(RESULT.abPurses(frm)) = GetTotal(wrld.abPurses(frm)) =>
 frm in set dom (wrld.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))
in
    GetTotal(RESULT.abPurses(frm)) = GetTotal(wrld.abPurses(frm)) =>
 frm in set dom (RESULT.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 AbPurseFunctional`inv_AbWorld(((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))))",
     <Invariants>,
     [ mk_AS`Name( [ "AbWorld" ], 41943237 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    frm in set dom (wrld.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    too in set dom (wrld.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    frm in set dom (RESULT.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    too in set dom (RESULT.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    (forall m_2,m_3 in set {{frm |-> newFrm},{too |-> newTo}} &
(forall id_4 in set dom (m_2), id_5 in set dom (m_3) &
id_4 = id_5 =>
 m_2(id_4) = m_3(id_5)))))",
     <MapSeqOfCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))
in
    GetTotal(RESULT.abPurses(frm)) = GetTotal(wrld.abPurses(frm)) and
 GetBalance(wrld.abPurses(frm)) >= GetBalance(RESULT.abPurses(frm)) =>
 (forall name in set (dom (RESULT.abPurses)) \\ {frm} &
name in set dom (wrld.abPurses))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))
in
    GetTotal(RESULT.abPurses(frm)) = GetTotal(wrld.abPurses(frm)) and
 GetBalance(wrld.abPurses(frm)) >= GetBalance(RESULT.abPurses(frm)) =>
 (forall name in set (dom (RESULT.abPurses)) \\ {frm} &
name in set dom (RESULT.abPurses))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    (GetTotal(RESULT.abPurses(frm)) + GetTotal(RESULT.abPurses(too))) = (GetTotal(wrld.abPurses(frm)) + GetTotal(wrld.abPurses(too))) =>
 frm in set dom (wrld.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    (GetTotal(RESULT.abPurses(frm)) + GetTotal(RESULT.abPurses(too))) = (GetTotal(wrld.abPurses(frm)) + GetTotal(wrld.abPurses(too))) =>
 too in set dom (wrld.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    (GetTotal(RESULT.abPurses(frm)) + GetTotal(RESULT.abPurses(too))) = (GetTotal(wrld.abPurses(frm)) + GetTotal(wrld.abPurses(too))) =>
 frm in set dom (RESULT.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    (GetTotal(RESULT.abPurses(frm)) + GetTotal(RESULT.abPurses(too))) = (GetTotal(wrld.abPurses(frm)) + GetTotal(wrld.abPurses(too))) =>
 too in set dom (RESULT.abPurses)))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))
in
    GetTotal(RESULT.abPurses(frm)) = GetTotal(wrld.abPurses(frm)) and
 GetBalance(wrld.abPurses(frm)) >= GetBalance(RESULT.abPurses(frm)) =>
 (forall name in set (dom (RESULT.abPurses)) \\ {frm} &
GetBalance(wrld.abPurses(name)) = GetBalance(RESULT.abPurses(name)) =>
 name in set dom (wrld.abPurses))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, - : PurseId, val : nat &
frm in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> IncreaseLost(newFrm, val)})))
in
    GetTotal(RESULT.abPurses(frm)) = GetTotal(wrld.abPurses(frm)) and
 GetBalance(wrld.abPurses(frm)) >= GetBalance(RESULT.abPurses(frm)) =>
 (forall name in set (dom (RESULT.abPurses)) \\ {frm} &
GetBalance(wrld.abPurses(name)) = GetBalance(RESULT.abPurses(name)) =>
 name in set dom (RESULT.abPurses))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferLost" ], 41943432 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    (GetTotal(RESULT.abPurses(frm)) + GetTotal(RESULT.abPurses(too))) = (GetTotal(wrld.abPurses(frm)) + GetTotal(wrld.abPurses(too))) and
 (GetBalance(RESULT.abPurses(frm)) + GetBalance(RESULT.abPurses(too))) = (GetBalance(wrld.abPurses(frm)) + GetBalance(wrld.abPurses(too))) =>
 (forall name in set (dom (RESULT.abPurses)) \\ {frm,too} &
name in set dom (wrld.abPurses))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    (GetTotal(RESULT.abPurses(frm)) + GetTotal(RESULT.abPurses(too))) = (GetTotal(wrld.abPurses(frm)) + GetTotal(wrld.abPurses(too))) and
 (GetBalance(RESULT.abPurses(frm)) + GetBalance(RESULT.abPurses(too))) = (GetBalance(wrld.abPurses(frm)) + GetBalance(wrld.abPurses(too))) =>
 (forall name in set (dom (RESULT.abPurses)) \\ {frm,too} &
name in set dom (RESULT.abPurses))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    (GetTotal(RESULT.abPurses(frm)) + GetTotal(RESULT.abPurses(too))) = (GetTotal(wrld.abPurses(frm)) + GetTotal(wrld.abPurses(too))) and
 (GetBalance(RESULT.abPurses(frm)) + GetBalance(RESULT.abPurses(too))) = (GetBalance(wrld.abPurses(frm)) + GetBalance(wrld.abPurses(too))) =>
 (forall name in set (dom (RESULT.abPurses)) \\ {frm,too} &
(GetBalance(wrld.abPurses(name)) = GetBalance(RESULT.abPurses(name))) =>
 name in set dom (wrld.abPurses))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall wrld : AbWorld, frm : PurseId, too : PurseId, val : nat &
frm <> too and
 frm in set dom (wrld.abPurses) and
 too in set dom (wrld.abPurses) and
 GetBalance(wrld.abPurses(frm)) >= val =>
 (let RESULT:AbWorld = ((let newFrm = ReduceBalance(wrld.abPurses(frm), val), newTo = IncreaseBalance(wrld.abPurses(too), val)
in
    mk_AbWorld(wrld.authentic,wrld.abPurses ++ {frm |-> newFrm,too |-> newTo})))
in
    (GetTotal(RESULT.abPurses(frm)) + GetTotal(RESULT.abPurses(too))) = (GetTotal(wrld.abPurses(frm)) + GetTotal(wrld.abPurses(too))) and
 (GetBalance(RESULT.abPurses(frm)) + GetBalance(RESULT.abPurses(too))) = (GetBalance(wrld.abPurses(frm)) + GetBalance(wrld.abPurses(too))) =>
 (forall name in set (dom (RESULT.abPurses)) \\ {frm,too} &
(GetBalance(wrld.abPurses(name)) = GetBalance(RESULT.abPurses(name))) =>
 name in set dom (RESULT.abPurses))))",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "AbPurseFunctional" ], 41943041 ),
        mk_AS`Name( [ "TransferOk" ], 41943226 ) ) ) }

