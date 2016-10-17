function drawing = plotDrawing(drawing)

switch lower(drawing.type)
case 'composition'
   for i=1:length(drawing.drawinglist)
      drawing.drawinglist{i} = plotDrawing(drawing.drawinglist{i});
   end;
case 'line'
   if(isempty(drawing.handle) | strcmp(get(drawing.handle,'Visible'),'on'))
      linedata = transformVertices(drawing.line,drawing.T);
      drawing.handle = line(linedata(:,1),linedata(:,2),linedata(:,3),'Color',drawing.color);
      set(drawing.handle,'LineWidth',drawing.width);
   end;
case 'patch'
   if(isempty(drawing.handle) | strcmp(get(drawing.handle,'Visible'),'on'))
      verticeData = transformVertices(drawing.vertices,drawing.T);
      drawing.handle = patch( ...
        'Vertices', verticeData, ...
        'Faces',drawing.faces, ...
        'FaceVertexCData',drawing.color, ...
        'FaceColor','flat' ...
      );
   end
otherwise
end;
