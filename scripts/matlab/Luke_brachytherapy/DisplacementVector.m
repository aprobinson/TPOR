function [T] = DisplacementVector(pts,seeds,f)

T= zeros(size(seeds,1),3);         %Displacement Vector
unit= T;

dx= 0.1;
dy= 0.1;
dz= 0.1;

for k= 1:pts(3)     % z voxels
for j= 1:pts(2)     % y voxels
for i= 1:pts(1) 	% x voxels

   % Compute distance between voxel(i,j,k) and the seeds
   r= [(seeds(:,1)-i)*dx, (seeds(:,2)-j)*dy, (seeds(:,3)-k)*dz];
   % Unit vectors between seeds and voxel(i,j,k)
   factor= sqrt(sum(r.^2,2));
   unit(:,1)= r(:,1)./(factor.^3);
   unit(:,2)= r(:,2)./(factor.^3);
   unit(:,3)= r(:,3)./(factor.^3);
   % Voxel located at seeds' position
   if any(isnan(unit))
       unit(isnan(unit))=0;
   end

   
   T= T + f(i,j,k)*unit;
end
end
end