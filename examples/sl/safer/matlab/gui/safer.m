function safer(com)

startToolbox = 1;

if(nargin == 0)
   com = 'sl';
end;

if(length(com) < 4 & length(com) > 1)
   
   type = com(1:2);
   
   if(length(com) ==3 & com(3) == '-')
      startToolbox = 0;
   end;
   
   saferSetup = getSaferSetup;
   if(strcmp(upper(saferSetup.OS),'NT4'))
      comSwitch = 'min';
   else
      comSwitch = 'm';
   end;
   
   if(strcmp(lower(type),'pp'))
      cd(saferSetup.saferpp);
      if(startToolbox)
         eval(['!start /',comSwitch,' vppde']);
      end;
   elseif(strcmp(lower(type),'sl'))
      cd(saferSetup.safersl);
      if(startToolbox)
         eval(['!start /',comSwitch,' vdmde']);
      end;
   else
      fprintf('\nBad safer command\n');
      return;
   end;
   
   if(startToolbox)
      if(strcmp(lower(type),'pp'))
         pause(5);
      else
         pause(5);
      end;
   end;
   safedemo(type);
else
   fprintf('\nBad safer command\n');
end;
