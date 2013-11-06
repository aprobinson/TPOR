function [T] = Displacement_Vec(pts,seeds,f,num_ndl)

showx= zeros(pts(1),pts(2),pts(3)); 
showy= showx;
showz= showy;
T= zeros(length(seeds),3,length(num_ndl));   %Displacement Vector
unit= T;

dx= 0.1;
dy= 0.1;
dz= 0.5;

mm= 0;
for n= 1:length(num_ndl)
mn= mm+num_ndl(n);
unit= zeros(mn,3);

for k= 1:pts(3)     % z voxels
for j= 1:pts(2)     % y voxels
for i= 1:pts(1) 	% x voxels

   % Compute distance between voxel(i,j,k) and the seeds
   r= [(seeds(mm+1:mn,1)-i)*dx, (seeds(mm+1:mn,2)-j)*dy, (seeds(mm+1:mn,3)-k)*dz];
   % Voxel located at seeds' position
   ind= r==0;
   r(ind)=0.1;
   % Unit vectors between seeds and voxel(i,j,k)
   factor= sqrt(sum(r.^2,2));
   unit(:,1)= r(:,1)./(factor.^3);
   unit(:,2)= r(:,2)./(factor.^3);
   unit(:,3)= r(:,3)./(factor.^3);
   
   %%%%%% TEST %%%%%%
   showx(i,j,k)= f(i,j,k)*unit(1,1);
   showy(i,j,k)= f(i,j,k)*unit(1,2);
   showz(i,j,k)= f(i,j,k)*unit(1,3);
   sum(sum(showx));
   %%%%%%%%%%%%%%%%%%

   
   T(mm+1:mn,:,n)= T + f(i,j,k)*unit;
end
end
end

mm=mn;
end