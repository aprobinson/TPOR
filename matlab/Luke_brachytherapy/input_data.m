clear all
close all
%%%%%   LOAD DATA   %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%
fid= fopen('organ_mesh-SUA');    %Opens Sua's organ data file
dimensions= textscan(fid, '%s %n',6, 'delimiter', ':');    %read dimensions in header
%---Number of mesh points [x y z]
xpts= dimensions{2}(1);  %Number of x mesh points
ypts= dimensions{2}(3);  %Number of y mesh points
zpts= dimensions{2}(5);  %Number of z mesh points
Npts= xpts*ypts*zpts;    %Total number of mesh points
%---Distance between mesh points
dx= dimensions{2}(2);    %distance between x mesh points
dy= dimensions{2}(4);    %distance between y mesh points
dz= dimensions{2}(6);    %distance between z mesh points

pts= [xpts ypts zpts*5];
dr= [dx dy dz];

%---Target Data
Ntarget= textscan(fid, '%*s %n',1);   %mesh points that make up target
target= zeros(xpts,ypts,zpts*5);      %[0 1] matrix of target organ
%target_c= cell(zpts,1);
%---Read target data from organ_mesh-SUA file
for i= 1:zpts
   data= textscan(fid,repmat('%n',1,xpts),ypts,'CollectOutput',1);
   for ii= 1:5
       target(:,:,ii+5*(i-1))= permute(data{1},[2 1 3]);
       %target_c{ii+5*(i-1)}= data{1};
   end
     
end

%---Urethra Data
Nurethra= textscan(fid, '%*s %n',1);
urethra= zeros(xpts,ypts,zpts*5);
%urethra_c= cell(zpts,1);
%---Read urethra data from organ_mesh-SUA file
for i= 1:zpts
   data= textscan(fid,repmat('%n',1,xpts),ypts,'CollectOutput',1);
   for ii=1:5
       urethra(:,:,ii+5*(i-1))= permute(data{1},[2 1 3]);
%        urethra_c{ii+5*(i-1)}= data{1};
   end
end

%---Rectum Data
Nrectum= textscan(fid, '%*s %n',1);
rectum= zeros(xpts,ypts,zpts*5);
%rectum_c= cell(zpts,1);
%---Read rectum data from organ_mesh-SUA file
for i= 1:zpts
   data= textscan(fid,repmat('%n',1,xpts),ypts,'CollectOutput',1);
   for ii=1:5
       rectum(:,:,ii+5*(i-1))= permute(data{1},[2 1 3]);
%       rectum_c{ii+5*(i-1)}= data{1};
   end
end

%---Normal Data
Nnormal= textscan(fid, '%*s %n',1);
normal= zeros(xpts,ypts,zpts*5);
%normal_c= cell(zpts,1);

for i= 1:zpts
   data= textscan(fid,repmat('%n',1,xpts),ypts,'CollectOutput',1);
   for ii=1:5
       normal(:,:,ii+5*(i-1))= permute(data{1},[2 1 3]);
%       normal_c{ii+5*(i-1)}= data{1};
   end
end

%---Margin Data
Nmargin= textscan(fid, '%*s %n',1);
margin= zeros(xpts,ypts,zpts*5);
%margin_c= cell(zpts,1);
%---Read margin data from organ_mesh-SUA file
for i= 1:zpts
   data= textscan(fid,repmat('%n',1,xpts),ypts,'CollectOutput',1);
   for ii=1:5
       margin(:,:,ii+5*(i-1))= permute(data{1},[2 1 3]);
%       margin_c{ii+5*(i-1)}= data{1};
   end
end

% %pretty picture
% target_con= target(:,:,1);
% for i= 2:zpts
%     target_con= target_con+target(:,:,i);
% end
% 
% surf(target_con)

% Seed position in vector matrix (a,b,c)
a= 70; b= 70; c= 70;
unit= zeros(139,139,139,3);

for k= 1:139	% k= 0.1cm
for j= 1:139	% j= 0.1cm
for i= 1:139 	% i= 0.1cm

   % Compute distance between voxel(i,j,k) and the seeds
   r= [(a-i)*dx, (b-j)*dy, (c-k)*dz];
   % Unit vectors between seeds and voxel(i,j,k)
   factor= sqrt(sum(r.^2,2));
   unitx(i,j,k)= r(1)/(factor^3);
   unity(i,j,k)= r(2)/(factor.^3);
   unitz(i,j,k)= r(3)/(factor.^3);
   % Voxel located at seeds' position

end
end
end
if any(isnan(unit))
    unit(isnan(unit))=0;
end

save Input target urethra rectum normal margin pts unit
