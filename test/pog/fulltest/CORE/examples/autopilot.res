{ mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states(mu(sta,fpa_disp|-><current>)))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943575 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_fpa_reached" ], 41943572 ) ) ),
  mk_TEST`ProofObligationPP(
     "Autopilot`inv_states(mk_states(<engaged>,<off>,<off>,<off_mode>,<current>,<current>,<current>,<away>))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943668 ) ],
     mk_POGTP`LocContext(
        <values>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "st0" ], 41943666 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.cas_eng = <off> then
mu(sta,cas_disp|-><pre_selected>)
else
sta)))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943474 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_input_cas" ], 41943471 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.fpa_sel = <off> then
mu(sta,fpa_disp|-><pre_selected>)
else
sta)))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943451 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_input_fpa" ], 41943448 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.cas_eng = <off> then
mu(sta,cas_eng|-><engaged>)
else
mu(sta,cas_disp|-><current>,cas_eng|-><off>))))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943261 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_cas_eng" ], 41943258 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.att_cws = <off> then
mu(sta,att_cws|-><engaged>,fpa_sel|-><off>,alt_eng|-><off_mode>,fpa_disp|-><current>,alt_disp|-><current>)
else
sta)))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943226 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_att_cws" ], 41943223 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.alt_eng = <armed_mode> then
mu(sta,altitude|-><near_pre_selected>,alt_eng|-><engaged_mode>,fpa_sel|-><off>,fpa_disp|-><current>)
else
mu(sta,altitude|-><near_pre_selected>))))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943497 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_alt_gets_near" ], 41943494 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.alt_eng = <armed_mode> then
mu(sta,altitude|-><at_pre_selected>,alt_disp|-><current>,alt_eng|-><engaged_mode>,fpa_sel|-><off>,fpa_disp|-><current>)
else
mu(sta,altitude|-><at_pre_selected>,alt_disp|-><current>))))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943533 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_alt_reached" ], 41943530 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.fpa_sel = <off> then
mu(sta,fpa_sel|-><engaged>,att_cws|-><off>,alt_eng|-><off_mode>,alt_disp|-><current>)
else
mu(sta,fpa_sel|-><off>,fpa_disp|-><current>,att_cws|-><engaged>,alt_eng|-><off_mode>,alt_disp|-><current>))))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943291 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_fpa_sel" ], 41943288 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states, evts : events &
evts = (<press_att_cws>) or
 evts = (<press_alt_eng>) or
 evts = (<press_fpa_sel>) or
 evts = (<press_cas_eng>) or
 evts = (<input_alt>) or
 evts = (<input_fpa>) or
 evts = (<input_cas>) or
 evts = (<alt_reached>) or
 evts = (<fpa_reached>) or
 evts = (<alt_gets_near>))",
     <CasesExpr>,
     [  ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "nextstate" ], 41943588 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.alt_eng = <off_mode> then
mu(sta,alt_disp|-><pre_selected>)
else
(if (sta.alt_eng = <armed_mode>) or
 (sta.alt_eng = <engaged_mode>) then
mu(sta,alt_eng|-><off_mode>,alt_disp|-><pre_selected>,att_cws|-><engaged>,fpa_sel|-><off>,fpa_disp|-><current>)
else
sta))))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943397 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_input_alt" ], 41943394 ) ) ),
  mk_TEST`ProofObligationPP(
     "(forall sta : states &
Autopilot`inv_states((if sta.alt_eng = <off_mode> and
 sta.alt_disp = <pre_selected> then
((if not ((sta.altitude = <away>)) then
mu(sta,att_cws|-><off>,fpa_sel|-><off>,alt_eng|-><engaged_mode>,fpa_disp|-><current>)
else
mu(sta,att_cws|-><off>,fpa_sel|-><engaged>,alt_eng|-><armed_mode>)))
else
sta)))",
     <Invariants>,
     [ mk_AS`Name( [ "states" ], 41943339 ) ],
     mk_POGTP`LocContext(
        <functions>,
        mk_AS`Name( [ "Autopilot" ], 41943041 ),
        mk_AS`Name( [ "tran_alt_eng" ], 41943336 ) ) ) }

