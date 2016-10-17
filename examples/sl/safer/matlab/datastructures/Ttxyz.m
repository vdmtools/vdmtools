% Ttxyz
%
% Returns the translation matrix according to a 3D translation vector.
%
% INPUT
%   tv: 1 x 3 space translation vector
%
% OUTPUT
%   Tt: 4x4 space translation matrix.
%
% Technical documentation on this subject:
%
% Mario Juric, Mapping weld groove profiles to welding lines in space,
% OSS, March 1998. INFORMA Doc.id.: INFORMA-OSS-16-V1.0.
%
% copyright INFORMA-OSS 1998

function Tt = Ttxyz(tv)

Tt = [1 0 0 tv(1); 0 1 0 tv(2); 0 0 1 tv(3); 0 0 0 1];
