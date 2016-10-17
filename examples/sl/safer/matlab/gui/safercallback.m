function saferCallback
global saferglobals;

if(isempty(saferglobals))
   errordlg('Safer data was purged. Closing down. Please restart SafeDemo.');
   quitsafer;
   return;
end;

eval([get(gcbo,'Tag')]);

function contextswitch
global saferglobals;

if(get(gcbo,'Value') == 1 & saferglobals.safer.handgrib.context ~= 1)
   set(findobj('Tag','hgryawleft'),'Visible','off');
   set(findobj('Tag','hgrylroff'),'Visible','off');
   set(findobj('Tag','hgryawright'),'Visible','off');
   set(findobj('Tag','hgrrollleft'),'Visible','off');
   set(findobj('Tag','hgrrlroff'),'Visible','off');
   set(findobj('Tag','hgrrollright'),'Visible','off');
   set(findobj('Tag','hgtleft'),'Visible','on');
   set(findobj('Tag','hgtlroff'),'Visible','on');
   set(findobj('Tag','hgtright'),'Visible','on');
   set(findobj('Tag','hgtup'),'Visible','on');
   set(findobj('Tag','hgtudoff'),'Visible','on');
   set(findobj('Tag','hgtdown'),'Visible','on');
   saferglobals.safer.handgrib.context = 1;
elseif(get(gcbo,'Value') == 2 & saferglobals.safer.handgrib.context ~=2)
   set(findobj('Tag','hgtleft'),'Visible','off');
   set(findobj('Tag','hgtlroff'),'Visible','off');
   set(findobj('Tag','hgtright'),'Visible','off');
   set(findobj('Tag','hgtup'),'Visible','off');
   set(findobj('Tag','hgtudoff'),'Visible','off');
   set(findobj('Tag','hgtdown'),'Visible','off');
   set(findobj('Tag','hgryawleft'),'Visible','on');
   set(findobj('Tag','hgrylroff'),'Visible','on');
   set(findobj('Tag','hgryawright'),'Visible','on');
   set(findobj('Tag','hgrrollleft'),'Visible','on');
   set(findobj('Tag','hgrrlroff'),'Visible','on');
   set(findobj('Tag','hgrrollright'),'Visible','on');
   saferglobals.safer.handgrib.context = 2;
end;
if(saferglobals.debug)
   fprintf('\ncontext: %d\n',saferglobals.safer.handgrib.context);
end;


function hgaah
global saferglobals;
saferglobals.safer.handgrib.aah = get(gcbo,'Value');
if(saferglobals.debug)
   fprintf('\nAAH: %d\n',saferglobals.safer.handgrib.aah);
end;

function hgforward
global saferglobals;
saferglobals.safer.handgrib.forwardbackward = 1;
set(findobj('Tag','hgforward'),'Value',1);
set(findobj('Tag','hgbackward'),'Value',0);
set(findobj('Tag','hgfboff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nforward-backward: %d\n',saferglobals.safer.handgrib.forwardbackward);
end;

function hgfboff
global saferglobals;
saferglobals.safer.handgrib.forwardbackward = 2;
set(findobj('Tag','hgforward'),'Value',0);
set(findobj('Tag','hgfboff'),'Value',1);
set(findobj('Tag','hgbackward'),'Value',0);
if(saferglobals.debug)
   fprintf('\nforward-backward: %d\n',saferglobals.safer.handgrib.forwardbackward);
end;

function hgbackward
global saferglobals;
saferglobals.safer.handgrib.forwardbackward = 0;
set(findobj('Tag','hgforward'),'Value',0);
set(findobj('Tag','hgbackward'),'Value',1);
set(findobj('Tag','hgfboff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nforward-backward: %d\n',saferglobals.safer.handgrib.forwardbackward);
end;

function hgpitchup
global saferglobals;
saferglobals.safer.handgrib.pitchupdown = 1;
set(findobj('Tag','hgpitchup'),'Value',1);
set(findobj('Tag','hgpitchdown'),'Value',0);
set(findobj('Tag','hgpudoff'),'Value',0);
if(saferglobals.debug)
   fprintf('\npitch up-down: %d\n',saferglobals.safer.handgrib.pitchupdown);
end;

function hgpudoff
global saferglobals;
saferglobals.safer.handgrib.pitchupdown = 2;
set(findobj('Tag','hgpitchup'),'Value',0);
set(findobj('Tag','hgpitchdown'),'Value',0);
set(findobj('Tag','hgpudoff'),'Value',1);
if(saferglobals.debug)
   fprintf('\npitch up-down: %d\n',saferglobals.safer.handgrib.pitchupdown);
end;

function hgpitchdown
global saferglobals;
saferglobals.safer.handgrib.pitchupdown = 0;
set(findobj('Tag','hgpitchup'),'Value',0);
set(findobj('Tag','hgpitchdown'),'Value',1);
set(findobj('Tag','hgpudoff'),'Value',0);
if(saferglobals.debug)
   fprintf('\npitch up-down: %d\n',saferglobals.safer.handgrib.pitchupdown);
end;

function hgtleft
global saferglobals;
saferglobals.safer.handgrib.leftright = 0;
set(findobj('Tag','hgtleft'),'Value',1);
set(findobj('Tag','hgtright'),'Value',0);
set(findobj('Tag','hgtlroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nleft-right: %d\n',saferglobals.safer.handgrib.leftright);
end;

function hgtlroff
global saferglobals;
saferglobals.safer.handgrib.leftright = 2;
set(findobj('Tag','hgtleft'),'Value',0);
set(findobj('Tag','hgtright'),'Value',0);
set(findobj('Tag','hgtlroff'),'Value',1);
if(saferglobals.debug)
   fprintf('\nleft-right: %d\n',saferglobals.safer.handgrib.leftright);
end;

function hgtright
global saferglobals;
saferglobals.safer.handgrib.leftright = 1;
set(findobj('Tag','hgtleft'),'Value',0);
set(findobj('Tag','hgtright'),'Value',1);
set(findobj('Tag','hgtlroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nleft-right: %d\n',saferglobals.safer.handgrib.leftright);
end;

function hgtup
global saferglobals;
saferglobals.safer.handgrib.updown = 0;
set(findobj('Tag','hgtup'),'Value',1);
set(findobj('Tag','hgtdown'),'Value',0);
set(findobj('Tag','hgtudoff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nup-down: %d\n',saferglobals.safer.handgrib.updown);
end;

function hgtudoff
global saferglobals;
saferglobals.safer.handgrib.updown = 2;
set(findobj('Tag','hgtup'),'Value',0);
set(findobj('Tag','hgtdown'),'Value',0);
set(findobj('Tag','hgtudoff'),'Value',1);
if(saferglobals.debug)
   fprintf('\nup-down: %d\n',saferglobals.safer.handgrib.updown);
end;

function hgtdown
global saferglobals;
saferglobals.safer.handgrib.updown = 1;
set(findobj('Tag','hgtup'),'Value',0);
set(findobj('Tag','hgtdown'),'Value',1);
set(findobj('Tag','hgtudoff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nup-down: %d\n',saferglobals.safer.handgrib.updown);
end;

function hgryawleft
global saferglobals;
saferglobals.safer.handgrib.yawleftright = 0;
set(findobj('Tag','hgryawleft'),'Value',1);
set(findobj('Tag','hgryawright'),'Value',0);
set(findobj('Tag','hgrylroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nyaw left-right: %d\n',saferglobals.safer.handgrib.yawleftright);
end;

function hgrylroff
global saferglobals;
saferglobals.safer.handgrib.yawleftright = 2;
set(findobj('Tag','hgryawleft'),'Value',0);
set(findobj('Tag','hgryawright'),'Value',0);
set(findobj('Tag','hgrylroff'),'Value',1);
if(saferglobals.debug)
   fprintf('\nyaw left-right: %d\n',saferglobals.safer.handgrib.yawleftright);
end;

function hgryawright
global saferglobals;
saferglobals.safer.handgrib.yawleftright = 1;
set(findobj('Tag','hgryawleft'),'Value',0);
set(findobj('Tag','hgryawright'),'Value',1);
set(findobj('Tag','hgrylroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nyaw left-right: %d\n',saferglobals.safer.handgrib.yawleftright);
end;

function hgrrollleft
global saferglobals;
saferglobals.safer.handgrib.rollleftright = 0;
set(findobj('Tag','hgrrollleft'),'Value',1);
set(findobj('Tag','hgrrollright'),'Value',0);
set(findobj('Tag','hgrrlroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nroll left-right: %d\n',saferglobals.safer.handgrib.rollleftright);
end;

function hgrrlroff
global saferglobals;
saferglobals.safer.handgrib.rollleftright = 2;
set(findobj('Tag','hgrrollleft'),'Value',0);
set(findobj('Tag','hgrrollright'),'Value',0);
set(findobj('Tag','hgrrlroff'),'Value',1);
if(saferglobals.debug)
   fprintf('\nroll left-right: %d\n',saferglobals.safer.handgrib.rollleftright);
end;

function hgrrollright
global saferglobals;
saferglobals.safer.handgrib.rollleftright = 1;
set(findobj('Tag','hgrrollleft'),'Value',0);
set(findobj('Tag','hgrrollright'),'Value',1);
set(findobj('Tag','hgrrlroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\nroll left-right: %d\n',saferglobals.safer.handgrib.rollleftright);
end;

function aahyawleft
global saferglobals;
saferglobals.safer.aah.yawleftright = 0;
set(findobj('Tag','aahyawleft'),'Value',1);
set(findobj('Tag','aahyawright'),'Value',0);
set(findobj('Tag','aahylroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\naah yaw left-right: %d\n',saferglobals.safer.aah.yawleftright);
end;

function aahylroff
global saferglobals;
saferglobals.safer.aah.yawleftright = 2;
set(findobj('Tag','aahyawleft'),'Value',0);
set(findobj('Tag','aahyawright'),'Value',0);
set(findobj('Tag','aahylroff'),'Value',1);
if(saferglobals.debug)
   fprintf('\naah yaw left-right: %d\n',saferglobals.safer.aah.yawleftright);
end;

function aahyawright
global saferglobals;
saferglobals.safer.aah.yawleftright = 1;
set(findobj('Tag','aahyawleft'),'Value',0);
set(findobj('Tag','aahyawright'),'Value',1);
set(findobj('Tag','aahylroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\naah yaw left-right: %d\n',saferglobals.safer.aah.yawleftright);
end;

function aahpitchup
global saferglobals;
saferglobals.safer.aah.pitchupdown = 1;
set(findobj('Tag','aahpitchup'),'Value',1);
set(findobj('Tag','aahpitchdown'),'Value',0);
set(findobj('Tag','aahpudoff'),'Value',0);
if(saferglobals.debug)
   fprintf('\naah pitch up-down: %d\n',saferglobals.safer.aah.pitchupdown);
end;

function aahpudoff
global saferglobals;
saferglobals.safer.aah.pitchupdown = 2;
set(findobj('Tag','aahpitchup'),'Value',0);
set(findobj('Tag','aahpitchdown'),'Value',0);
set(findobj('Tag','aahpudoff'),'Value',1);
if(saferglobals.debug)
   fprintf('\naah pitch up-down: %d\n',saferglobals.safer.aah.pitchupdown);
end;

function aahpitchdown
global saferglobals;
saferglobals.safer.aah.pitchupdown = 0;
set(findobj('Tag','aahpitchup'),'Value',0);
set(findobj('Tag','aahpitchdown'),'Value',1);
set(findobj('Tag','aahpudoff'),'Value',0);
if(saferglobals.debug)
   fprintf('\naah pitch up-down: %d\n',saferglobals.safer.aah.pitchupdown);
end;

function aahrollleft
global saferglobals;
saferglobals.safer.aah.rollleftright = 0;
set(findobj('Tag','aahrollleft'),'Value',1);
set(findobj('Tag','aahrollright'),'Value',0);
set(findobj('Tag','aahrlroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\naah roll left-right: %d\n',saferglobals.safer.aah.rollleftright);
end;

function aahrlroff
global saferglobals;
saferglobals.safer.aah.rollleftright = 2;
set(findobj('Tag','aahrollleft'),'Value',0);
set(findobj('Tag','aahrollright'),'Value',0);
set(findobj('Tag','aahrlroff'),'Value',1);
if(saferglobals.debug)
   fprintf('\naah roll left-right: %d\n',saferglobals.safer.aah.rollleftright);
end;

function aahrollright
global saferglobals;
saferglobals.safer.aah.rollleftright = 1;
set(findobj('Tag','aahrollleft'),'Value',0);
set(findobj('Tag','aahrollright'),'Value',1);
set(findobj('Tag','aahrlroff'),'Value',0);
if(saferglobals.debug)
   fprintf('\naah roll left-right: %d\n',saferglobals.safer.aah.rollleftright);
end;

function applythrust
global saferglobals;
saferglobals.safer.switchboard = updateSaferModel(saferglobals.safer);
updateSaferView(saferglobals.plot.drawing,saferglobals.safer.switchboard);

function refreshplot
global saferglobals;

saferglobals.plot.drawing = plotSafer;
updateSaferView(saferglobals.plot.drawing,saferglobals.safer.switchboard);
