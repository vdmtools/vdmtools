#include <iostream>

void vdm_TclTkView::vdm_ShowAlarm(const Record& vdm_ModeA,
				  const ObjectRef& vdm_obstacle)
{
	//AD: ...
	cout << "Alarm: " << vdm_ModeA.GetField(1) << '\n';
	// << " " << vdm_obstacle.ascii() << '\n';
	cout.flush();
}

void vdm_TclTkView::vdm_ShowObstacleArtificiel(const Tuple& vdm_centre,
					       const Real& vdm_rayon,
					       const Int& vdm_altitude__maximale)
{
	//AD: ...
	cout << "OA: "
		<< vdm_centre.GetField(1) << " "
		<< vdm_centre.GetField(2) << " "
		<< vdm_rayon << " "
		<< vdm_altitude__maximale << '\n';
	cout.flush();
}

void vdm_TclTkView::vdm_ShowRadarData(const Record& vdm_ModeA,
				      const Tuple& vdm_position,
				      const Int& vdm_modeC)
{
	//AD: ...
	cout << "Radar: " << vdm_ModeA.GetField(1) << " "
		<< vdm_position.GetField(1) << " "
		<< vdm_position.GetField(2) << " "
		<< vdm_modeC << "\n";
	cout.flush();
}

void vdm_TclTkView::vdm_ShowZoneAeroport(const Tuple& vdm_centre,
					 const Real& vdm_rayon,
	const Int& vdm_altitude__maximale, const Real& vdm_rayon_au_sol)
{
	//AD: ...
	cout << "ZA: "
		<< vdm_centre.GetField(1) << " "
		<< vdm_centre.GetField(2) << " "
		<< vdm_rayon << " "
		<< vdm_altitude__maximale << " "
		<< vdm_rayon_au_sol << '\n';
	cout.flush();
}

void vdm_TclTkView::vdm_ShowSegmentDePrediction(const Tuple& vdm_xy_queue,
					        const Tuple& vdm_xy_tete)
{
  Int vdm_x_queue = vdm_xy_queue.GetField(1);
  Int vdm_y_queue = vdm_xy_queue.GetField(2);
  Int vdm_x_tete = vdm_xy_tete.GetField(1);
  Int vdm_y_tete = vdm_xy_tete.GetField(2);
  
	//AD: ...
	cout << "SP: "
		<< vdm_x_queue << " "
		<< vdm_y_queue << " "
		<< vdm_x_tete << " "
		<< vdm_y_tete << "\n";
	cout.flush();
}

void vdm_TclTkView::vdm_ShowTime(const Int& vdm_t)
{
	//AD: ...
	cout << "T: "
		<< vdm_t << '\n';
}

