# Makefile for KLV system 

#
# Generate distribution
#

RTFFILES = Beacon.rtf CabDisplay.rtf CheckSpeedEvent.rtf EmergencyBreak.rtf Event.rtf \
           FLTV.rtf HeadMeetBeaconEvent.rtf KLV.rtf KLVStateEvent.rtf MaxSpeedEvent.rtf \
           NoBeaconMetEvent.rtf OnBoardComp.rtf TIV_A.rtf TIV_D.rtf TIV_E.rtf \
           TailMeetBeaconEvent.rtf Test.rtf
TESTFILES = test/*.arg test/*.arg.exp test/vdmloop test/vdmtest \
            test/vdmloop.bat test/vdmtest.bat

distribution: klv.jar
	rm -rf klv
	mkdir klv
	mkdir klv/test
	cp -f $(RTFFILES) klv
	cp -f klv_spec.doc klv_req.doc klv.mdl klv.prj klv
	cp -f klv.jar klv
	cp -f $(TESTFILES) klv/test

#
# Generate klv.jar
#

klv.jar:
	cd java; make klvjar



#
# Clean up
#

clean:
	rm -rf klv
	cd java; make clean
