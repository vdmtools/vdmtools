% Trxyz
%
% Returns the transformation matrix for any rotation about the x-,y- and z-axis.
%
% INPUT
%   r: Yaw, pitch and roll angles in vector form [yaw, pitch, roll].
%
% OUTPUT
%   T:  Transformation matrix including yaw, pitch and roll.
%
% Technical documentation:
%
% Mario Juric, Mapping weld groove profiles to welding lines in space,
% OSS, March 1998. INFORMA Doc.id.: INFORMA-OSS-16-V1.0.
%
% copyright INFORMA-OSS 1998
function T = Trxyz(r)

cr1 = cos(r(1));
cr2 = cos(r(2));
cr3 = cos(r(3));

sr1 = sin(r(1));
sr2 = sin(r(2));
sr3 = sin(r(3));

T = [ cr3*cr2,cr3*sr2*sr1-sr3*cr1, ...
      cr3*sr2*cr1+sr3*sr1,0;...
      sr3*cr2,sr3*sr2*sr1+cr3*cr1, ...
      sr3*sr2*cr1-cr3*sr1,0;...
      -sr2, cr2*sr1, cr2*cr1, 0; 0, 0, 0, 1];
