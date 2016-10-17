function thrusterSwitchBoard = updateSaferModel(safer)

global saferglobals;

if(saferglobals.startup)
   saferglobals.startup = 0;
   thrusterSwitchBoard = zeros(4,6);
   return;
end;

%thrusterSwitchBoard=round(rand(4,6));
if(strcmp(lower(saferglobals.toolboxtype),'pp'))
   thrusterSwitchBoard = saferinterfpp(matlabSafer2vdmSafer(safer));
else
   thrusterSwitchBoard = saferinterf(matlabSafer2vdmSafer(safer));
end;

% Calls the saferinterf DLL which calls a running VDM-SL through its API.
%thrusterSwitchBoard = zeros(4,6);

function vdmSafer = matlabSafer2vdmSafer(safer);
% Converts the matlab safer structure to matlab safer array for VDM input

vdmSafer = zeros(1,9);

vdmSafer(1) = safer.handgrib.context;
vdmSafer(2) = safer.handgrib.aah;
vdmSafer(3) = safer.handgrib.forwardbackward;
vdmSafer(6) = safer.handgrib.pitchupdown;
vdmSafer(7) = safer.aah.rollleftright;
vdmSafer(8) = safer.aah.pitchupdown;
vdmSafer(9) = safer.aah.yawleftright;

if(safer.handgrib.context == 1)
   vdmSafer(4) = safer.handgrib.leftright;
   vdmSafer(5) = safer.handgrib.updown;
else
   vdmSafer(4) = safer.handgrib.yawleftright;
   vdmSafer(5) = safer.handgrib.rollleftright;
end;
