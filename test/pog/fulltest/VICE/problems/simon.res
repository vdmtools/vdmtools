{ mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Press" ], 41943984 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Press" ], 41943984 ),
        mk_AS`Name( [ "getElement" ], 41944004 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Press" ], 41943984 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Press" ], 41943984 ),
        mk_AS`Name( [ "putElement" ], 41944029 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Conveyer" ], 41943699 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ "adv" ], 41943863 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Conveyer" ], 41943699 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ "Conveyer" ], 41943740 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Conveyer" ], 41943699 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ "getElement" ], 41943795 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "Conveyer" ], 41943699 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ "putElement" ], 41943767 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "PassiveLink" ], 41944113 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "PassiveLink" ], 41944113 ),
        mk_AS`Name( [ "getElement" ], 41944159 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "PassiveLink" ], 41944113 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "PassiveLink" ], 41944113 ),
        mk_AS`Name( [ "putElement" ], 41944133 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "PassiveModule" ], 41943564 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "PassiveModule" ], 41943564 ),
        mk_AS`Name( [ "attach" ], 41943594 ) ) ),
  mk_TEST`ProofObligationPP(
     [  ],
     <StateInvariants>,
     [ mk_AS`Name( [ "PassiveConsumer" ], 41944361 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "PassiveConsumer" ], 41944361 ),
        mk_AS`Name( [ "putElement" ], 41944371 ) ) ),
  mk_TEST`ProofObligationPP(
     "(slots) <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ "sensor1" ], 41943896 ) ) ),
  mk_TEST`ProofObligationPP(
     "(slots) <> []",
     <NonEmptySeq>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ "putElement" ], 41943767 ) ) ),
  mk_TEST`ProofObligationPP(
     "length - 1 > 0",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EventQueue" ], 41943278 ),
        mk_AS`Name( [ "get" ], 41943297 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_Robot`Global(e)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Robot" ], 41944409 ),
        mk_AS`Name( [ "releaseA" ], 41944791 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_Robot`Global(e)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Robot" ], 41944409 ),
        mk_AS`Name( [ "releaseB" ], 41944824 ) ) ),
  mk_TEST`ProofObligationPP(
     "Robot`pre_releaseB()",
     <FunctionApplication>,
     [ mk_AS`Name( [ "releaseB" ], 41944781 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Robot" ], 41944409 ),
        mk_AS`Name( [ "performSequence" ], 41944696 ) ) ),
  mk_TEST`ProofObligationPP(
     "p in set dom (output)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ActiveModule" ], 41943454 ),
        mk_AS`Name( [ "generateOutput" ], 41943530 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(ActiveModule,dm)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "PassiveModule" ], 41943564 ),
        mk_AS`Name( [ "attach" ], 41943594 ) ) ),
  mk_TEST`ProofObligationPP(
     "length in set inds (queue)",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "EventQueue" ], 41943278 ),
        mk_AS`Name( [ "get" ], 41943297 ) ) ),
  mk_TEST`ProofObligationPP(
     "length in set inds (slots)",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ "sensor2" ], 41943909 ) ) ),
  mk_TEST`ProofObligationPP(
     "length in set inds (slots)",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ "getElement" ], 41943795 ) ) ),
  mk_TEST`ProofObligationPP(
     "IO`pre_fecho(\"\", text, nil)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "fecho" ], 41943194 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "IO" ], 41943114 ),
        mk_AS`Name( [ "echo" ], 41943186 ) ) ),
  mk_TEST`ProofObligationPP(
     "isofclass(PassiveModule,dm)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ActiveModule" ], 41943454 ),
        mk_AS`Name( [ "attach" ], 41943478 ) ) ),
  mk_TEST`ProofObligationPP(
     "(exists p in set dom (output) &
true)",
     <LetBeStStmt>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "PassiveModule" ], 41943564 ),
        mk_AS`Name( [ "generateOutput" ], 41943651 ) ) ),
  mk_TEST`ProofObligationPP(
     "Press`pre_generateOutput(<ElementAvailable>)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "generateOutput" ], 41944069 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Press" ], 41943984 ),
        mk_AS`Name( [ "process" ], 41944053 ) ) ),
  mk_TEST`ProofObligationPP(
     "PassiveLink`pre_generateOutput(<ElementAvailable>)",
     <FunctionApplication>,
     [ mk_AS`Name( [ "generateOutput" ], 41944149 ) ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "PassiveLink" ], 41944113 ),
        mk_AS`Name( [ "putElement" ], 41944133 ) ) ),
  mk_TEST`ProofObligationPP(
     "mk_(generalizeState(current_state),e) in set dom (transition_table)",
     <MapApplication>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "Robot" ], 41944409 ),
        mk_AS`Name( [ "handleEvent" ], 41944636 ) ) ),
  mk_TEST`ProofObligationPP(
     "((exists e in set elems (slots(1,...,length - 1)) &
not (e = <Empty>))) =>
 length in set inds (slots)",
     <SeqApplication>,
     [  ],
     mk_POGTP`LocContext(
        <synchronisation>,
        mk_AS`Name( [ "Conveyer" ], 41943699 ),
        mk_AS`Name( [ [  ] ], -1 ) ) ),
  mk_TEST`ProofObligationPP(
     "is_(e.Sender.getElement(),compose Global`ElementData of Color : nat
InputTime : nat
OutputTime : nat
end)",
     <Subtype>,
     [  ],
     mk_POGTP`LocContext(
        <operations>,
        mk_AS`Name( [ "ActiveConsumer" ], 41944192 ),
        mk_AS`Name( [ "putEvent" ], 41944233 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall m_13,m_14 in set {{mk_(mk_State(<p4>,<None>,<None>),<ElementAvailable_P4>) |-> [<PICKUP_A>,<MOVETO_2>,<RELEASE_A>]},{mk_(mk_State(<p2>,<None>,<None>),<ElementAvailable_P2>) |-> [<MOVETO_3>,<PICKUP_B>,<MOVETO_2>,<RELEASE_B>]},{mk_(mk_State(<p2>,<None>,<None>),<ElementAvailable_P4>) |-> [<MOVETO_4>,<PICKUP_A>,<MOVETO_2>,<RELEASE_A>]}} &
(forall id_15 in set dom (m_13), id_16 in set dom (m_14) &
id_15 = id_16 =>
 m_13(id_15) = m_14(id_16)))",
     <MapSeqOfCompatible>,
     [  ],
     mk_POGTP`LocContext(
        <instanceVariables>,
        mk_AS`Name( [ "Robot" ], 41944409 ),
        mk_AS`Name( [ "Robot",
             "transition_table" ], -1 ) ) ) }

