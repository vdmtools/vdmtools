function updateSaferView(safer,thrusterSwitchBoard)

offThrusters = {};
onThrusters = {};

for i=1:rows(thrusterSwitchBoard)
   for j=1:cols(thrusterSwitchBoard)
      if(thrusterSwitchBoard(i,j))
         onThrusters = union(onThrusters,{['thruster',num2str(i),num2str(j)]});
      else
         offThrusters = union(offThrusters,{['thruster',num2str(i),num2str(j)]});
      end;
   end;
end;

setVisible(safer,onThrusters,'on');
setVisible(safer,offThrusters,'off');
