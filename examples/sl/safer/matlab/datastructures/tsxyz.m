% function T=Tsxyz(s)
% 
% INPUT
%
% OUTPUT
%
% Technical documentation:
%
% Carsten Bro Sørensen, Conversion rules between Weld Joint Geometry
% and optical sensor data (RST points). Technical Report, OSS, January 1998.
%
% Espen Dalsgaard, Weld Groove Geometry Modelling.
% Technical Report, OSS, January 1998.
%
% Carsten Bro and Sten Agerholm, Architectural Description of the OSS Demonstration,
% OSS, December 1997. INFORMA Doc.id.: INFORMA-OSS-9-V1.0.
%
% Mario Juric, Calculating weld groove geometry from RST point profiles,
% OSS, January 1998. INFORMA Doc.id.: INFORMA-OSS-12-V1.0.
%
% Mario Juric, Calculating RST profiles from weld groove geometry attributes,
% OSS, February 1998. INFORMA Doc.id.: INFORMA-OSS-13-V1.0.
%
% Mario Juric, Mapping weld groove profiles to welding lines in space,
% OSS, March 1998. INFORMA Doc.id.: INFORMA-OSS-16-V1.0.
%
% copyright INFORMA-OSS 1998

function T=Tsxyz(s)

T = [s(1) 0 0 0; 0 s(2) 0 0; 0 0 s(3) 0; 0 0 0 1];
