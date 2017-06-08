function drawing = transformDrawing(drawing, T)

switch lower(drawing.type)
case 'composition'
   for i=1:length(drawing.drawinglist)
      drawing.drawinglist{i} = transformDrawing(drawing.drawinglist{i},T);
   end;
otherwise
   drawing.T = T*drawing.T;
end;
