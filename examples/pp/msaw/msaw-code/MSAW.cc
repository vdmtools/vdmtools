/***
*  * WHAT
*  *    Test of VDM++ code generator
*  * PROJECT
*  *    Afrodite - ESPRIT programme, project no. 6500.
*  * AUTHOR
*  *    Peter Demmer   CAP Volmac Inn.
*  * COPYRIGHT
*  *    (C) 1994 CAP Volmac Inn., Netherlands
***/



#include <fstream>
#include <string>
#include <time.h>

#include <metaiv.h>


#include "cg.h"
#include "MSAW.h"
#include "VisualWorkSpace.h"
#include "VisualObstacleArtificiel.h"
#include "VisualZoneAeroport.h"
#include "TclTkView.h"
#include "Clock.h"
#include "HWPistesRadar.h"
#include "VisualPistesRadar.h"
#include "MSAWTypes.h"

static int maxtime = 1; // is updated by ReadPistesRadar
static int now = 1;

Map ReadPistesRadar(Set fl);
Set ReadObstacles(Set obstacles, ObjectRef view);

class TypeConst : public vdm_MSAWTypes {
public:
  static Record mk_ModeA(const string id) {
    Record modea(vdm_ModeA,length_ModeA);
    modea.SetField(pos_ModeA_id,Sequence(id));
    return modea;
  }
};

int main(int argc,char ** argv)   //   MANUAL
{
  vdm_VisualWorkSpace   workspace;
  
  Set fl;
  Set ob;

  int flag = -1;
  for (int i=1;i<argc;i++) {
    if (*argv[i] == '-') { // argument 
      if (strcmp(argv[i],"-f") == 0) flag = 0;
      else if (strcmp(argv[i],"-o") == 0) flag = -1;
      // else ignore
    }
    else {
      if (flag)  ob.Insert(Token(argv[i]));
      else fl.Insert(Token(argv[i]));
    }
  }

  ObjectRef view(new vdm_TclTkView());
  ObjectRef clock(new vdm_Clock());
  ObjectRef hwradar(new vdm_HWPistesRadar());
  ObjectRef radar(new vdm_VisualPistesRadar());
  
  Map data = ReadPistesRadar(fl);
  Set ob_s = ReadObstacles(ob,view);
  workspace.vdm_Init(ob_s,view,radar);
  ObjGet_vdm_HWPistesRadar(hwradar)->vdm_Init(clock,radar,view,data);
  
  time_t waittime, later = time(NULL);
  while (now <= maxtime) {
    later += (time_t) 2;
    ObjGet_vdm_Clock(clock)->vdm_Tick(); // preiodic 1
    ObjGet_vdm_HWPistesRadar(hwradar)->vdm_RefreshPistes();
    workspace.vdm_TraiterPistesRadar();      // periodic 10
    waittime = later - time(NULL);
    now++;
  }
  return 0;
}

// data = map Time to map ModeA to (Coordonnees * Coordonnee)
Map ReadPistesRadar(Set fl)
{
  Map data;
  Generic g;
  for (int b = fl.First(g);b;b = fl.Next(g)) {
    string file = Token(g).GetValue() + ".fl";
    Record modeA = TypeConst::mk_ModeA(Token(g).GetValue());
    modeA.SetField(1,g);
    ifstream in(file.c_str());
    if (in) {
      int x,y,z,i = 1;
      Tuple tp1(2);
      Tuple tp2(2);
      while (in >> x >> y >> z) {
	Map mp;
	tp1.SetField(1,Int(x));
	tp1.SetField(2,Int(y));
	tp2.SetField(1,tp1);
	tp2.SetField(2,Int(z)); // mk_(mk_(x,y),z)
	if (data.DomExists(Int(i))) mp = data[Int(i)];
	mp.Insert(modeA,tp2);
	data.ImpModify(Int(i),mp);
	i++;
      }
      if (i > maxtime) maxtime = i;
    }
  }
  return data;
}

Set ReadObstacles(Set obstacles, ObjectRef view)
{
  Set obst_s;
  Generic g;
  int b;
  for (b = obstacles.First(g);b;b = obstacles.Next(g)) {
    string file = ((Token)g).GetValue() + ".ob";
    ifstream in(file.c_str());
    if (in) {
      int x, y, z;
      float r;
      while (in >> x >> y >> r >> z) {
	ObjectRef obstacle(new vdm_VisualObstacleArtificiel());
	Tuple centre(2);
	centre.SetField (1, Int(x));
	centre.SetField (2, Int(y));
	ObjGet_vdm_VisualObstacleArtificiel(obstacle)->vdm_Set (centre, Real(r), Int(z),view);
	obst_s.Insert (obstacle);
      }
    }
    
  }
  for (b = obstacles.First(g);b;b = obstacles.Next(g)) {
    string file = ((Token)g).GetValue() + ".za";
    ifstream in(file.c_str());
    if (in) {
      int x, y, z;
      float r,rs;
      while (in >> x >> y >> r >> z >> rs) {
	ObjectRef obstacle(new vdm_VisualZoneAeroport());
	Tuple centre(2);
	centre.SetField (1, Int(x));
	centre.SetField (2, Int(y));
	ObjGet_vdm_VisualZoneAeroport(obstacle)->vdm_Set (centre, Real(r), Int(z),Real(rs),view);
	obst_s.Insert (obstacle);
      }
    }
  }
  return obst_s;
}
