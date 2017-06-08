// SetToSeq[@Elem]
// s : set of @Elem
// -> seq of @Elem
type_typevariableL vdm_CHECK_SetToSeq (const type_typevariableS & s)
{
  Set e_s (s);  
  Sequence e_l (e_s.ToSequence());
  return e_l;
}

