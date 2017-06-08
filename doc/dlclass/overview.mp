input boxes
\beginfig(1)

  pickup pencircle scaled 0.75pt;  

  boxit.vpp1(btex\strut VDM++ Specification etex);
  vpp1.e - vpp1.w = (2in,0in);
  vpp1.n - vpp1.s = (0in,2in);
  drawboxed(vpp1);

  boxit.dl1(btex\strut Dynamic Link Class etex);
  dl1.w = vpp1.e + (0.5in,-0.25in);
  dl1.e - dl1.w = (1.5in, 0in);
  dl1.n - dl1.s = (0in,1.5in);

  boxit.dl2("");
  dl2.c = dl1.c + (0.1in,0.1in);
  dl2.e - dl2.w = (1.5in, 0in);
  dl2.n - dl2.s = (0in,1.5in);

  boxit.dl3("");
  dl3.c = dl2.c + (0.1in,0.1in);
  dl3.e - dl3.w = (1.5in, 0in);
  dl3.n - dl3.s = (0in,1.5in);

  fill bpath dl3 withcolor .8white;
  drawboxed(dl3);

  fill bpath dl2 withcolor .8white;
  drawboxed(dl2);

  fill bpath dl1 withcolor .8white;
  drawboxed(dl1);

  boxit.vppbound("");
  vppbound.nw = vpp1.nw + (-0.1in,0.1in);
  vppbound.se = dl1.se + (0.4in,-0.1in);
  drawboxed(vppbound);

  boxit.vpplabel(btex\strut VDM++ Specification Level etex);
  vpplabel.ne = vppbound.ne - (0.05in,0.05in);
  drawunboxed(vpplabel);

 








  boxit.cpp1(btex\strut External C++ code etex);
  cpp1.n = vpp1.s - (0in,0.5in);
  cpp1.e - cpp1.w = (2in,0in);
  cpp1.n - cpp1.s = (0in,2in);
  drawboxed(cpp1);

  boxit.il1(btex\strut Interface Layer etex);
  il1.w = cpp1.e + (0.5in,0.25in);
  il1.e - il1.w = (1.5in, 0in);
  il1.n - il1.s = (0in,1.5in);

  boxit.il2("");
  il2.c = il1.c + (0.1in,-0.1in);
  il2.e - il2.w = (1.5in, 0in);
  il2.n - il2.s = (0in,1.5in);

  boxit.il3("");
  il3.c = il2.c + (0.1in,-0.1in);
  il3.e - il3.w = (1.5in, 0in);
  il3.n - il3.s = (0in,1.5in);

  fill bpath il3 withcolor .8white;
  drawboxed(il3);

  fill bpath il2 withcolor .8white;
  drawboxed(il2);

  fill bpath il1 withcolor .8white;
  drawboxed(il1);
 
  boxit.cppbound("");
  cppbound.nw = cpp1.nw + (-0.1in,0.1in);
  cppbound.se = il3.se + (0.2in, -0.4in);
  drawboxed(cppbound);

  boxit.cpplabel(btex\strut C++ Code Level etex);
  cpplabel.se = cppbound.se + (-0.05in,0.05in);
  drawunboxed(cpplabel);


  pickup pencircle scaled 1pt;

  drawdblarrow vpp1.se+(-5,5){dir 60}..dl1.sw+(5,5);

  drawdblarrow cpp1.ne-(5,5){dir 300}..il1.nw+(5,-5);

  drawdblarrow dl1.s..il1.n;

endfig;
 
end;
