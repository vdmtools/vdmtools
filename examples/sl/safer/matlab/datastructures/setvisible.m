function setVisible(drawing,identifiers,appearance)

switch lower(drawing.type)
case 'composition'
   for i=1:length(drawing.drawinglist)
      if(ismember(drawing.drawinglist{i}.label,identifiers))
         setAppearance(drawing.drawinglist{i},appearance);
      else
         setVisible(drawing.drawinglist{i},identifiers,appearance);
      end;
   end;
otherwise
   if(ismember(drawing.label,identifiers))
      setAppearance(drawing,appearance);
   end;
end;

function setAppearance(drawing,appearance)

switch lower(drawing.type)
case 'composition'
   for i=1:length(drawing.drawinglist)
      setAppearance(drawing.drawinglist{i},appearance);
   end;
otherwise
   set(drawing.handle,'Visible',appearance);
end;
