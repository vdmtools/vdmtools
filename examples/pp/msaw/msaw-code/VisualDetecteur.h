//
// THIS FILE IS AUTOMATICALLY GENERATED!!
//
// Generated at Wed 05-Nov-97 by the VDM++toC++ Code Generator
// (v6.0e - Tue 04-Nov-97)
//
// Supported compilers:
// 	g++ version 2.7.2 on SunOS4, SunOS5, Linux, HP-UX10
// 	Dec cxx version 5.5 on Alpha OSF1
// 	VC++ version 5.0 on Windows95, Windows NT
//

#ifndef _VisualDetecteur_h
#define _VisualDetecteur_h

#include <math.h>
#include "metaiv.h"
#include "cg.h"
#include "cg_aux.h"
#include "CGBase.h"
#include "Detecteur.h"



class vdm_VisualDetecteur : public virtual vdm_Detecteur {
protected:
  ObjectRef vdm_view;
public:

  virtual vdm_VisualDetecteur * Get_vdm_VisualDetecteur() { return this; }

  ObjectRef Self() { return ObjectRef(Get_vdm_VisualDetecteur()); }

  int vdm_GetId() { return VDM_VisualDetecteur; }
  virtual void vdm_Init(const ObjectRef &, const ObjectRef &);
  virtual void vdm_CalculeRisque(const ObjectRef &, const ObjectRef &);
   vdm_VisualDetecteur();

  virtual ~vdm_VisualDetecteur() {}
};

#endif

