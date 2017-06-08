function quitsafer

global saferglobals;

saferguifig = findobj('Tag','safergui');
if(~isempty(saferguifig) & ishandle(saferguifig) & saferguifig ~= gcbo)
   close(saferguifig);
end;

saferplotfig = findobj('Tag','saferplotfigure');
if(~isempty(saferplotfig) & ishandle(saferplotfig) & saferplotfig ~= gcbo)
   close(saferplotfig);
end;

saferglobals =[];