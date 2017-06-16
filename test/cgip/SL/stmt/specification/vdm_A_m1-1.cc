
if (floor(vdm_A_rl) == vdm_A_rl) {
  vdm_A_i = vdm_A_j + vdm_A_rl;
  vdm_A_res =  vdm_A_j;
}
else
  RunTime("Precondition in method m1 does not hold.");
