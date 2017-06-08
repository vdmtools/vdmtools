ifndef OSTYPE 
include Warning:\ variable\ OSTYPE\ is\ not\ set\ or\ exported
endif

CXX=g++
CC=g++ -static
ifeq ($(OSTYPE),SunOS4)
TBDIR=/usr/local/package/vdm++_toolbox
else
TBDIR=/opt/vdm++_toolbox
endif

CFLAGS=-O -MMD -g -Wall -I$(TBDIR)/cg/include
LOADLIBES=-L$(TBDIR)/cg/lib

CXXFLAGS=$(CFLAGS)
LDLIBS=-lCG -lvdm -lm -lg++

OBJS=WorkSpace.o \
	BulletinMeteo.o \
	Clock.o \
	Cone.o \
	Cylindre.o \
	Detecteur.o \
	MSAWTypes.o \
	Obstacle.o \
	ObstacleArtificiel.o \
	Parametres.o \
	PistesRadar.o \
	Polygone.o \
	ReliefParticulier.o \
	SegmentDePrediction.o \
	Terrain.o \
	Vecteur3D.o \
	ZoneAeroport.o \
	ZoneMilitaire.o \
        HWPistesRadar.o \
        TclTkView.o \
        SpecView.o \
        CGBase.o

VOBJS=	View.o \
	VisualDetecteur.o \
	VisualObstacleArtificiel.o \
	VisualPistesRadar.o \
	VisualSegmentDePrediction.o \
	VisualZoneAeroport.o \
	VisualWorkSpace.o

MSAW: MSAW.o VisualWorkSpace.o $(VOBJS) $(OBJS)

clean:
	-rm *.o MSAW *.d

# Replace the following lines on a regular basis
BulletinMeteo.o : BulletinMeteo.cc BulletinMeteo.h  MSAWTypes.h CGBase.h
Clock.o : Clock.cc Clock.h MSAWTypes.h CGBase.h
Cone.o : Cone.cc Cone.h MSAWTypes.h  CGBase.h
Cylindre.o : Cylindre.cc Cylindre.h MSAWTypes.h  CGBase.h
Detecteur.o : Detecteur.cc Detecteur.h MSAWTypes.h Obstacle.h PistesRadar.h ObstacleArtificiel.h Cylindre.h ReliefParticulier.h \
  Polygone.h Terrain.h ZoneAeroport.h Cone.h ZoneMilitaire.h SegmentDePrediction.h \
  Parametres.h Vecteur3D.h CGBase.h
MSAW.o : MSAW.cc MSAW.h  VisualWorkSpace.h CGBase.h \
 WorkSpace.h MSAWTypes.h VisualObstacleArtificiel.h VisualPistesRadar.h\
 ObstacleArtificiel.h Cylindre.h Obstacle.h VisualZoneAeroport.h \
 ZoneAeroport.h Cone.h View.h
MSAWTypes.o : MSAWTypes.cc MSAWTypes.h  CGBase.h
Obstacle.o : Obstacle.cc Obstacle.h MSAWTypes.h  CGBase.h
ObstacleArtificiel.o : ObstacleArtificiel.cc ObstacleArtificiel.h \
  Cylindre.h MSAWTypes.h Obstacle.h SegmentDePrediction.h Parametres.h Vecteur3D.h  CGBase.h
Parametres.o : Parametres.cc Parametres.h MSAWTypes.h  CGBase.h
PistesRadar.o : PistesRadar.cc PistesRadar.h MSAWTypes.h ObstacleArtificiel.h Cylindre.h Obstacle.h ReliefParticulier.h Polygone.h \
  Terrain.h ZoneAeroport.h Cone.h ZoneMilitaire.h 
Polygone.o : Polygone.cc Polygone.h MSAWTypes.h  CGBase.h
ReliefParticulier.o : ReliefParticulier.cc ReliefParticulier.h \
  Obstacle.h MSAWTypes.h Polygone.h SegmentDePrediction.h Parametres.h Vecteur3D.h  CGBase.h
SegmentDePrediction.o : SegmentDePrediction.cc SegmentDePrediction.h \
  Parametres.h MSAWTypes.h Vecteur3D.h BulletinMeteo.h  CGBase.h
Terrain.o : Terrain.cc Terrain.h \
  MSAWTypes.h SegmentDePrediction.h Parametres.h Vecteur3D.h  CGBase.h
Vecteur3D.o : Vecteur3D.cc Vecteur3D.h MSAWTypes.h  CGBase.h
View.o : View.cc View.h MSAWTypes.h CGBase.h
VisualDetecteur.o : VisualDetecteur.cc VisualDetecteur.h \
  Detecteur.h MSAWTypes.h Obstacle.h PistesRadar.h ObstacleArtificiel.h Cylindre.h \
  ReliefParticulier.h Polygone.h Terrain.h ZoneAeroport.h Cone.h ZoneMilitaire.h \
  SegmentDePrediction.h Parametres.h Vecteur3D.h View.h VisualPistesRadar.h Clock.h \
  VisualObstacleArtificiel.h VisualZoneAeroport.h VisualSegmentDePrediction.h \
  BulletinMeteo.h CGBase.h
VisualObstacleArtificiel.o : VisualObstacleArtificiel.cc VisualObstacleArtificiel.h \
  ObstacleArtificiel.h Cylindre.h \
  MSAWTypes.h Obstacle.h View.h  CGBase.h
VisualPistesRadar.o : VisualPistesRadar.cc VisualPistesRadar.h \
  Clock.h MSAWTypes.h PistesRadar.h ObstacleArtificiel.h Cylindre.h Obstacle.h \
  ReliefParticulier.h Polygone.h Terrain.h ZoneAeroport.h Cone.h ZoneMilitaire.h \
  View.h CGBase.h
VisualSegmentDePrediction.o : VisualSegmentDePrediction.cc VisualSegmentDePrediction.h \
  BulletinMeteo.h MSAWTypes.h SegmentDePrediction.h \
  Parametres.h Vecteur3D.h View.h  CGBase.h
VisualWorkSpace.o : VisualWorkSpace.cc VisualWorkSpace.h \
  BulletinMeteo.h MSAWTypes.h PistesRadar.h ObstacleArtificiel.h Cylindre.h Obstacle.h \
  ReliefParticulier.h Polygone.h Terrain.h ZoneAeroport.h Cone.h ZoneMilitaire.h \
  View.h VisualDetecteur.h Detecteur.h SegmentDePrediction.h Parametres.h Vecteur3D.h \
  VisualPistesRadar.h Clock.h VisualSegmentDePrediction.h WorkSpace.h \
  VisualObstacleArtificiel.h  CGBase.h
VisualZoneAeroport.o : VisualZoneAeroport.cc VisualZoneAeroport.h \
  View.h MSAWTypes.h ZoneAeroport.h Cone.h Cylindre.h Obstacle.h  CGBase.h
WorkSpace.o : WorkSpace.cc WorkSpace.h \
  BulletinMeteo.h MSAWTypes.h Detecteur.h Obstacle.h PistesRadar.h ObstacleArtificiel.h \
  Cylindre.h ReliefParticulier.h Polygone.h Terrain.h ZoneAeroport.h Cone.h ZoneMilitaire.h  CGBase.h\
  SegmentDePrediction.h Parametres.h Vecteur3D.h 
ZoneAeroport.o : ZoneAeroport.cc ZoneAeroport.h \
  Cone.h MSAWTypes.h Cylindre.h Obstacle.h SegmentDePrediction.h Parametres.h \
  Vecteur3D.h  CGBase.h
ZoneMilitaire.o : ZoneMilitaire.cc ZoneMilitaire.h \
  Obstacle.h MSAWTypes.h Polygone.h SegmentDePrediction.h Parametres.h Vecteur3D.h  CGBase.h
HWPistesRadar.o: HWPistesRadar.cc HWPistesRadar.h  CGBase.h MSAWTypes.h \
                 Clock.h PistesRadar.h
TclTkView.o: TclTkView.cc TclTkView.h CGBase.h View.h MSAWTypes.h \
  vdm_TclTkView_prelim.cc
SpecView.o: SpecView.cc SpecView.h CGBase.h View.h MSAWTypes.h Obstacle.h
CGBase.o: CGBase.cc CGBase.h