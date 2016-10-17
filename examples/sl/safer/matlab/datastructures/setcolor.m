function drawing = setColor(drawing,color);

switch lower(drawing.type)
case 'composition'
   for i=1:length(drawing.drawinglist)
      drawing.drawinglist{i} = setColor(drawing.drawinglist{i},color);
   end;
case 'patch'
   drawing.color = zeros(rows(drawing.faces),3);
   for i=1:rows(drawing.faces)
      drawing.color(i,1:3) = color;
   end;
case 'line'
   drawing.color = color;
otherwise
end;
