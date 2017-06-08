function runThrusterTest

global saferglobals;

allThrustOn;

continue = 1;

for i=1:rows(saferglobals.safer.switchboard)
   for j=1:cols(saferglobals.safer.switchboard)
      continue = userContinue;
      if(~continue)
         break;
      end;
      thrustoff(i,j);
      fprintf('Thruster%d%d is now off\n',i,j);
      continue = userContinue;
      if(~continue)
         break;
      end;
      thruston(i,j);
      fprintf('Thruster%d%d is now on\n',i,j);
   end;
   if(~continue)
      break;
   end;
end;

function c = userContinue

s = input('Press q to quit (followed by enter) or any other key (followed by enter) to continue...','s');
if(strcmp(lower(s),'q'))
   c = 0;
else
   c = 1;
end;
