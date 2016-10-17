#include "metaiv.h"

// setCheckMark
// gpoId : [POGTP`LocContext * nat]
// ==> bool
Bool vdm_POG_uMEDIATOR_setCheckMark (const Generic & gpoId)
{
  if(gpoId.IsNil()) return Bool(false);

  Tuple poId (gpoId);
  TYPE_POGTP_LocContext loc (poId.GetField(1));
  Int idx (poId.GetField(2));

//
// vdm_POG_uMEDIATOR_pogs : map [AS`Name] to
//                            map [AS`Name] * POGTP`LocClassification to seq of PO
  Generic gmodPogs; // map [AS`Name] * POGTP`LocClassification to seq of PO
  if (!vdm_POG_uMEDIATOR_pogs.DomExists(loc.get_modName(), gmodPogs)) {
     return Bool(false);
  }  

  Tuple member (mk_(loc.get_name(), loc.get_class()));

  Generic po; // seq of PO
  Map modPogs (gmodPogs); // map [AS`Name] * POGTP`LocClassification to seq of PO
  if (!modPogs.DomExists(member, po)) {
     return Bool(false);
  }  

  SEQ<TYPE_POG_uMEDIATOR_PO> po_l (po);
  TYPE_POG_uMEDIATOR_PO poInfo (po_l[idx.GetValue()]);
  Bool checkMark (!poInfo.GetBool(pos_POG_uMEDIATOR_PO_checked));
  poInfo.SetField(pos_POG_uMEDIATOR_PO_checked, checkMark);
  po_l.ImpModify(idx.GetValue(), poInfo);
  modPogs.ImpModify(member, po_l);
  vdm_POG_uMEDIATOR_pogs.ImpModify(loc.get_modName(), modPogs);
 
  return checkMark;
}
