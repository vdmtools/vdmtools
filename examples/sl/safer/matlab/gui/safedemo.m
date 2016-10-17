function safedemo(toolboxtype)

global saferglobals;

if(isempty(saferglobals))
   initGlobals;
   if(nargin == 1)
      saferglobals.toolboxtype = toolboxtype;
   end;
   saferGui;
   initGui;
   if(strcmp(saferglobals.plottype,'dark'))
      saferPlotFigure;
   else
      brightSaferPlotFig;
   end;
   initPlot;
end;

function initGlobals
global saferglobals;
saferglobals = struct('gui',[],'plot',[],'safer',[],'debug',0,'plottype','dark','toolboxtype','sl','startup',1);
handGrib = ...
   struct( ...
     'context',1, ...
     'aah',0, ...
     'forwardbackward',2, ...
     'pitchupdown',2, ...
     'leftright',2, ...
     'updown',2, ...
     'yawleftright',2, ...
     'rollleftright',2 ...
  );
aahControl = ...
   struct( ...
     'pitchupdown',2, ...
     'yawleftright',2, ...
     'rollleftright',2 ...
  );
saferglobals.gui  = struct('fig',[]);
saferglobals.plot = struct('fig',[],'axis',[],'drawing',[]);
saferglobals.safer = struct('handgrib',handGrib,'aah',aahControl,'switchboard',[]);

function initGui
global saferglobals;
saferglobals.gui.fig = gcf;
pos = get(gcf,'Position');
set(gcf,'Position',[3.75 26.25 pos(3) pos(4)]);
set(gcf,'Name','Safer handgrip');
set(gcf,'NumberTitle','off');

function initPlot
global saferglobals;
saferglobals.safer.switchboard = updateSaferModel(saferglobals.safer);
saferglobals.plot = struct('fig',gcf,'axis',gca,'drawing',plotsafer);
set(gcf,'Position',[385.5 27 210 159.75]);
set(gcf,'Name','Safer backpack');
set(gcf,'NumberTitle','off');
updateSaferView(saferglobals.plot.drawing,saferglobals.safer.switchboard);
