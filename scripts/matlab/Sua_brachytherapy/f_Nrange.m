%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% f_Nrange.m - patient %%%%%
%%%%% (c)suayoo - Mar 2003  %%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% determine range for the number of needles
% function [minN,maxN] = f_Nrange(numTa)

%function [minN,maxN] = f_Nrange(numTa)

function [minN]= f_Nrange(numTa);

vol= numTa*0.1*0.1*0.5;	%[cc]=[ml]=[cm^3]
numN= 0.24*vol + 11.33;
%minN= floor(numN)-1;
%maxN= ceil(numN)+1;
minN= floor(numN);

return
