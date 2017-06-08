#
# Makefile to initialise and control the Venus demonstration.
# Copyright (C) 1995 - IFAD.
#
# $Id: Makefile,v 1.9 1997/09/05 11:17:57 jeppe Exp $


WISH=wish

.SILENT:

.PRECIOUS:	msaw-init msaw-data Documentation Makefile README msaw-lib

#
# Prepare (configure) the deomnstration after checkout.
#
configure: msaw-code/MSAW 
	-rm msaw-code/m4tag_rep
	-ln -s msaw-spec/CGinfo msaw-code/m4tag_rep
	echo "Creating class directory MSAW_Prototype....."
	make -f msaw-spec/Makefile
	echo "done."
	echo "### REMEMBER TO CONFIGURE A PROJECT IN MSAW_Prototype ####"

#
# Prepare overall initial settings for the demonstration.
#
init: 
	echo "Creating template file omt_vdm_templates.cd....."
	rm -rf omt_vdm_templates.cd
	cp -r -p msaw-init/omt_vdm_templates.cd .
	echo "done."
	echo "Creating class diagram MSAW_Prototype.cd....."
	rm -rf MSAW_Prototype.cd
	cp -r -p msaw-init/MSAW_Prototype.cd .
	echo "done."
	echo "Updating class library MSAW_Prototype....."
	cp msaw-spec/pistesradar.vpp.err MSAW_Prototype/pistesradar.vpp
	echo "done."
	echo "Creating system file MSAW_Prototype.sys....."
	rm -rf MSAW_Prototype.sys
	cp -r -p msaw-init/MSAW_Prototype.sys .
	echo "done."

#
# Prepare the initial setting for block 1 of the demonstration.
#
block1:
	make oedit

#
# Prepare the initial setting for block 2 of the demonstration.
#
block2:
	make vppde

#
# Prepare the initial setting for block 3 of the demonstration.
#
block3:
	make vppde
	make prototype

#
# Prepare the initial setting for block 4 of the demonstration.
#
block4:
	make init

reverse:
	if [ ! -d MSAW_Prototype.cd_lib ];\
	   then make init;\
	fi;\
	echo "Generating OMT_MSAW_Prototype.cd....."
	vdmrev MSAW_Prototype.cd
	echo "done."

vppde:
	if [ ! -x `which vppde` ];\
	   then echo "Check the installation: vppde cannot be executed.";\
	else\
	   if [ ! -d MSAW_Prototype.cd_lib ];\
              then make init;\
           fi;\
	   echo "Starting the VDM++ Toolbox.....";\
	   vppde MSAW_Prototype.cd_lib&\
	   echo "done.";\
	fi

oedit:
	if [ ! -x `which oedit` ];\
	   then echo "Check the installation: oedit cannot be executed.";\
	else\
	   if [ ! -w MSAW_Prototype.cd ];\
              then make init;\
           fi;\
	   echo "Starting the LOV/OMT Object Editor.....";\
	   oedit  MSAW_Prototype.cd&\
	   echo "done.";\
	fi

prototype: msaw-code/MSAW 
	echo "Creating input data....."
	rm -f *.fl *.ob *.za
	cp msaw-data/*.fl .
	cp msaw-data/*.ob .
	cp msaw-data/*.za .
	echo "done."
	echo "Starting MSAW prototype....."
	msaw-code/MSAW -o eiffeltower kl355 orly -f descending \
                      -f diagonal_up kl400 | $(WISH) msaw-lib/front&
	echo "done."

msaw-code/MSAW:
	echo "Compiling msaw-code/MSAW..."
	(cd msaw-code; make )
	echo "done"

view:
	echo "Starting xv....."
	xv screen_dumps/*.gif &
	echo "done."


clean:
	echo "Cleaning up the remains of the demonstration....."
	rm -r -f  MSAW_Prototype omt_vdm_templates.cd Lov OMT* *.fl *.ob *.za *.h *.cc *.bak core
	echo "done."

cleanall: clean
	(cd msaw-code; make clean)
	\rm -rf msaw-copy-lib
