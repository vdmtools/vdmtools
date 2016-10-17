{ mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, sta : State &
Safe(sta) =>
 Example3`pre_Alloc(addr, sta))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Alloc" ], 41943426 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "AllocSafe" ], 41943409 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, sta : State &
Example3`post_AllocSafe(addr, sta, Safe(sta) =>
 Safe(Alloc(addr, sta))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "AllocSafe" ], 41943409 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, sta : State &
addr not in set sta.used =>
 Safe(sta) =>
 Example3`pre_Alloc(addr, sta))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Alloc" ], 41943454 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "AllocSafe0" ], 41943437 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, sta : State &
addr not in set sta.used and
 addr in set sta.access =>
 Safe(sta) =>
 Example3`pre_Alloc(addr, sta))",
     <FunctionApplication>,
     [ mk_AS`Name( [ "Alloc" ], 41943487 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "AllocSafe2" ], 41943470 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State &
addr not in set used =>
 (let used' = used union {addr}
in
    (exists c : CON &
true)))",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc" ], 41943114 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, sta : State &
Example3`pre_AllocSafe0(addr, sta) =>
 Example3`post_AllocSafe0(addr, sta, Safe(sta) =>
 Safe(Alloc(addr, sta))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "AllocSafe0" ], 41943437 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, sta : State &
Example3`pre_AllocSafe2(addr, sta) =>
 Example3`post_AllocSafe2(addr, sta, Safe(sta) =>
 Safe(Alloc(addr, sta))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "AllocSafe2" ], 41943470 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State &
addr not in set used and
 addr in set access =>
 (let used' = used union {addr}
in
    (exists c : CON &
true)))",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc2" ], 41943167 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State &
addr not in set used and
 addr in set access =>
 (let used' = used union {addr}
in
    (exists c : CON &
true)))",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc21" ], 41943275 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State, mem2 : map ADDR to CON &
addr not in set used and
 addr in set access =>
 (let used' = used union {addr}
in
    (exists c : CON &
true)))",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc3" ], 41943330 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State &
addr not in set used =>
 Example3`inv_State((let used' = used union {addr}, mem' = mem ++ {addr |-> let c : CON in  c}
in
    mk_State(mem',access,used'))))",
     <Invariants>,
     [ mk_AS`Name( [ "State" ], 41943120 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc" ], 41943114 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State &
addr not in set used and
 addr in set access =>
 Example3`inv_State((let used' = used union {addr}, mem' = mem ++ {addr |-> <c0>}
in
    mk_State(mem',access,used'))))",
     <Invariants>,
     [ mk_AS`Name( [ "State" ], 41943230 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc20" ], 41943224 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State &
addr not in set used and
 addr in set access =>
 Example3`inv_State((let used' = used union {addr}, mem' = {addr |-> let c : CON in  c}
in
    mk_State(mem',access,used'))))",
     <Invariants>,
     [ mk_AS`Name( [ "State" ], 41943281 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc21" ], 41943275 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State &
addr not in set used and
 addr in set access =>
 Example3`inv_State((let used' = used union {addr}, mem' = mem munion {addr |-> let c : CON in  c}
in
    mk_State(mem',access,used'))))",
     <Invariants>,
     [ mk_AS`Name( [ "State" ], 41943173 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc2" ], 41943167 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State, mem2 : map ADDR to CON &
addr not in set used and
 addr in set access =>
 Example3`inv_State((let used' = used union {addr}, mem' = mem munion {addr |-> let c : CON in  c}
in
    mk_State(mem',access,used'))))",
     <Invariants>,
     [ mk_AS`Name( [ "State" ], 41943342 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc3" ], 41943330 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State &
addr not in set used and
 addr in set access =>
 (let used' = used union {addr}
in
    (forall id_2 in set dom (mem), id_3 in set dom ({addr |-> let c : CON in  c}) &
id_2 = id_3 =>
 mem(id_2) = {addr |-> let c : CON in  c}(id_3))))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc2" ], 41943167 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State, mem2 : map ADDR to CON &
addr not in set used and
 addr in set access =>
 (let RESULT:State = (let used' = used union {addr}, mem' = mem munion {addr |-> let c : CON in  c}
in
    mk_State(mem',access,used'))
in
    (exists c : CON &
true)))",
     <LetBeStExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc3" ], 41943330 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State, mem2 : map ADDR to CON &
addr not in set used and
 addr in set access =>
 (let used' = used union {addr}
in
    (forall id_4 in set dom (mem), id_5 in set dom ({addr |-> let c : CON in  c}) &
id_4 = id_5 =>
 mem(id_4) = {addr |-> let c : CON in  c}(id_5))))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc3" ], 41943330 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State, mem2 : map ADDR to CON &
Example3`pre_Alloc3(addr, mk_State(mem,access,used), mem2) =>
 Example3`post_Alloc3(addr, mk_State(mem,access,used), mem2, (let used' = used union {addr}, mem' = mem munion {addr |-> let c : CON in  c}
in
    mk_State(mem',access,used'))))",
     <PostCondition>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc3" ], 41943330 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall addr : ADDR, mk_State(mem,access,used) : State, mem2 : map ADDR to CON &
addr not in set used and
 addr in set access =>
 (let RESULT:State = (let used' = used union {addr}, mem' = mem munion {addr |-> let c : CON in  c}
in
    mk_State(mem',access,used'))
in
    (forall id_6 in set dom (mem), id_7 in set dom ({addr |-> let c : CON in  c}) &
id_6 = id_7 =>
 mem(id_6) = {addr |-> let c : CON in  c}(id_7))))",
     <MapCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Example3" ], 41943041 ),
        mk_AS`Name( [ "Alloc3" ], 41943330 ) ) ) }

