% transform3D
%
% Transforms any 3D point according to a transformation matrix.
%
% INPUT
%   T: 4 x 4 space transformation matrix
%   p: Point in space as 1 X 3 vector
%
% OUTPUT
%   pt: Transformed point in space as 1 X 3 vector.
%
% Technical documentation on this subject:
%
% Mario Juric, Mapping weld groove profiles to welding lines in space,
% OSS, March 1998. INFORMA Doc.id.: INFORMA-OSS-16-V1.0.
%
% copyright INFORMA-OSS 1998

function pt = transform3D(T,p)

% Add extra vector element for the transformation
p(4) = 1;
% Transform by multiplying transformation matrix with the 1 X 4 point vector
px = T*p';
pt = px(1:3)';
