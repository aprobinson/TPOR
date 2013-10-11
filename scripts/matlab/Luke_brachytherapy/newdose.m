close all
clear all



%%%%%   LOAD DATA   %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%
fid= fopen('I6711-SUA');    %Opens Sua's organ data file
dimensions= textscan(fid, '%s %n',9, 'delimiter', ':');    %read dimensions in header
%---Number of mesh points [x y z]
xpts= dimensions{2}(1);  %Number of x mesh points
ypts= dimensions{2}(4);  %Number of y mesh points
zpts= dimensions{2}(7);  %Number of z mesh points
Npts= xpts*ypts*zpts;    %Total number of mesh points
%---Distance between mesh points
dx= dimensions{2}(3);    %distance between x mesh points
dy= dimensions{2}(6);    %distance between y mesh points
dz= dimensions{2}(9);    %distance between z mesh points

%---Target Data
Ntarget= textscan(fid, '%*s %n',1);   %mesh points that make up target
SUA_DOSE= zeros(xpts,ypts,zpts);        %[0 1] matrix of target organ
%target_c= cell(zpts,1);
%---Read target data from organ_mesh-SUA file
for i= 1:zpts
   data= textscan(fid,repmat('%n',1,xpts),ypts,'CollectOutput',1);
   SUA_DOSE(:,:,i)= permute(data{1},[2 1 3]);
   SUA_DOSE_c{i}= data{1};
end

save suadose SUA_DOSE