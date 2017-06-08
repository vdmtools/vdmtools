SPECS = bulletinmeteo.vpp clock.vpp terrain.vpp cone.vpp vecteur3d.vpp \
        cylindre.vpp view.vpp detecteur.vpp visualdetecteur.vpp \
        msawtypes.vpp visualobstacleartificiel.vpp obstacle.vpp \
        visualpistesradar.vpp obstacleartificiel.vpp \
        visualsegmentdeprediction.vpp parametres.vpp visualworkspace.vpp \
        pistesradar.vpp visualzoneaeroport.vpp polygone.vpp workspace.vpp \
        reliefparticulier.vpp zoneaeroport.vpp segmentdeprediction.vpp \
        zonemilitaire.vpp tcltkview.vpp specview.vpp

SPECDIR = msaw-spec
LIBDIR  = MSAW_Prototype

VPPFILES = $(patsubst %, ../$(SPECDIR)/%, $(SPECS))

copy:
	rm -rf $(LIBDIR)
	-mkdir $(LIBDIR)
	(cd $(LIBDIR); for f in $(VPPFILES) ; do cp $$f . ; done)
	(cd $(LIBDIR); ln -s ../$(SPECDIR)/CGinfo m4tag_rep)
	rm -f ./.tmparg
