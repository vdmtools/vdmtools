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

#ifndef _View_h
#define _View_h

#include <math.h>
#include "metaiv.h"
#include "cg.h"
#include "cg_aux.h"
#include "CGBase.h"
#include "MSAWTypes.h"



class vdm_View : public virtual vdm_MSAWTypes {
public:

  virtual vdm_View * Get_vdm_View() { return this; }

  ObjectRef Self() { return ObjectRef(Get_vdm_View()); }

  int vdm_GetId() { return VDM_View; }
  virtual void vdm_ShowAlarm(const Record &, const ObjectRef &) =0;
  virtual void vdm_ShowObstacleArtificiel(const Tuple &, const Real &, const Int &) =0;
  virtual void vdm_ShowZoneAeroport(const Tuple &, const Real &, const Int &, const Real &) =0;
  virtual void vdm_ShowRadarData(const Record &, const Tuple &, const Int &) =0;
  virtual void vdm_ShowSegmentDePrediction(const Tuple &, const Tuple &) =0;
  virtual void vdm_ShowTime(const Int &) =0;
   vdm_View();

  virtual ~vdm_View() {}
};

#endif

