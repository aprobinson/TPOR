
function [d_seeds, d_needle] = Seed_Repo(k, T)

m= size(T,1);

d_seeds= zeros(m,3);
d_needle= zeros(1,2);

%--- Move needles in x direction (in voxels)
dx= k*sum(T(:,1))/m*10; 
d_seeds(:,1)= round(dx);
d_needle(1,1)= round(dx);

%--- Move needles in y direction (in voxels)
dy= k*sum(T(:,2))/m*10;
d_seeds(:,2)= round(dy);
d_needle(1,2)= round(dy);    
   
%--- Move seeds in z direction
dz= k*T(:,3)*2;
d_seeds(:,3)= round(dz);

