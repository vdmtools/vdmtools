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



#include "VisualDetecteur.h"
#include "Obstacle.h"
#include "PistesRadar.h"
#include "SegmentDePrediction.h"
#include "Terrain.h"
#include "View.h"

vdm_VisualDetecteur::vdm_VisualDetecteur() { RegisterAsDerived(vdm_GetId()); }

void vdm_VisualDetecteur::vdm_Init(const ObjectRef &vdm_radar_uinformation, const ObjectRef &vdm_view_q) {
  vdm_pistes_uradar = vdm_radar_uinformation;
  vdm_view = vdm_view_q;
}

void vdm_VisualDetecteur::vdm_CalculeRisque(const ObjectRef &vdm_segment_ude_uprediction, const ObjectRef &vdm_obstacle) {
  Record tmpVal_2 = ObjGet_vdm_SegmentDePrediction(vdm_segment_ude_uprediction)->vdm_GetmodeA();
  Record vdm_modeA(vdm_MSAWTypes::vdm_ModeA, vdm_MSAWTypes::length_ModeA);
  vdm_modeA = tmpVal_2;
  Bool tmpVal_4;
  if (ObjGet_vdm_Terrain(vdm_obstacle)) 
    tmpVal_4 = ObjGet_vdm_Terrain(vdm_obstacle)->vdm_Intersection(vdm_segment_ude_uprediction);
  else 
    if (ObjGet_vdm_Obstacle(vdm_obstacle)) 
      tmpVal_4 = ObjGet_vdm_Obstacle(vdm_obstacle)->vdm_Intersection(vdm_segment_ude_uprediction);
  Bool vdm_risque_udetecte;
  vdm_risque_udetecte = tmpVal_4;
  if (vdm_risque_udetecte.GetValue()) {
    ObjGet_vdm_PistesRadar(vdm_pistes_uradar)->vdm_SetAlarm(vdm_modeA, vdm_obstacle);
    ObjGet_vdm_View(vdm_view)->vdm_ShowAlarm(vdm_modeA, vdm_obstacle);
  }
}
