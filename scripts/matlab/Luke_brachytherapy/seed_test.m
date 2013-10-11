close all
clear all

%%%%%   LOAD DATA   %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%
xpts= 55;  %Number of x mesh points
ypts= 55;  %Number of y mesh points
zpts= 55;  %Number of z mesh points
Npts= xpts*ypts*zpts;    %Total number of mesh points
%---Distance between mesh points
dx= 0.1;    %distance between x mesh points
dy= 0.1;    %distance between y mesh points
dz= 0.1;    %distance between z mesh points

pts= [xpts ypts zpts];
dr= [dx dy dz];

target= ones(pts);
urethra= zeros(pts);
rectum= zeros(pts);
normal= zeros(pts);

rng(511)
%%%%%   PLACE SEEDS   %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%
Nseeds= 4;
seeds= zeros(Nseeds, 3);
x= [23 28 28 33];

% y= [17, 11, 23, 17];
% z= [17, 17, 17, 17];

z= [28, 23, 33, 28];
y= [28, 28, 28, 28];


seeds(:,1)= x;
seeds(:,2)= y;
seeds(:,3)= z;
% Nseeds=1;
% seeds= [28 28 6];

D=zeros(xpts,ypts,zpts);    %dose-rate matrix

% Calculate dose

%---Use SeedDose.m to calculate dose by how many voxels away 
%   from the seed in the x y z direction 
load DoseMatrix
load DoseNist99
load DoseNist99_OLD
load suadose
% Seed position in dose rate matrix (a,b,c)
a= 70; b= 70; c= 70;

newseeds= ones(Nseeds,3);
 
optimize= 1;
iter= 1;
while optimize
    say=['iteration # ',num2str(iter)];
    disp(say)
    
for s= 1:Nseeds
    % Shift dose matrix to center on seed, s
    delta_i= a-seeds(s,1);   % index shift in x   
    delta_j= b-seeds(s,2);   % index shift in y
    delta_k= c-seeds(s,3);   % index shift in z
    % Dose matrix centered on seed location
    %centered= circshift(Dose, [-delta_i,-delta_j,-delta_k]);
    T_half= 59.4*24;         % I-125 half life
    Decay= log(2)/T_half;    % I-125 decay constant
%     centered= circshift(DNist99_OLD/(Decay*100), [-delta_i,-delta_j,-delta_k]);
    centered= circshift(DNist99/(Decay*100), [-delta_i,-delta_j,-delta_k]);
%     centered= circshift(SUA_DOSE*.4, [-delta_i,-delta_j,-delta_k]);
    
    % Dose [Gy]
    D= D+centered(1:xpts,1:ypts,1:zpts);
end


[f]= ObjectiveFunction(pts, D, target, urethra, rectum, normal);

ff= cell(zpts,1);
dd= ff;
for i= 1:zpts
    ff{i,1}= f(:,:,i);
    dd{i,1}= D(:,:,i);
end

T= zeros(size(seeds,1),3);         %Displacement Vector
for k= 1:pts(3)     % z voxels
for j= 1:pts(2)     % y voxels
for i= 1:pts(1) 	% x voxels

   % Compute distance between voxel(i,j,k) and the seeds
   r= [(seeds(:,1)-i)*0.1, (seeds(:,2)-j)*0.1, (seeds(:,3)-k)*0.1];
   r= [(seeds(:,1)-i)*0.1, (seeds(:,2)-j)*0.1, (seeds(:,3)-k)*0.1];   
   % Unit vectors between seeds and voxel(i,j,k)
   factor= sqrt(sum(r.^2,2));
       unit(:,1)= r(:,1)./(factor.^3);
       unit(:,2)= r(:,2)./(factor.^3);
       unit(:,3)= r(:,3)./(factor.^3);
   % Voxel located at seeds' position    
       if any(isnan(unit))
           unit(isnan(unit))=0;
       end
   %%%%%% TEST %%%%%%
%    showx(i,j,k)= f(i,j,k)*unit(1,1);
%    showy(i,j,k)= f(i,j,k)*unit(1,2);
%    showz(i,j,k)= f(i,j,k)*unit(1,3);
%    
%    unitx(i,j,k)= unit(1,1);
%    unity(i,j,k)= unit(1,2);
%    unitz(i,j,k)= unit(1,3);
   %%%%%%%%%%%%%%%%%%
   
   T= T + f(i,j,k)*unit;
end
end
end
%T(:,1)= T(:,1)/dr(1);
%T(:,2)= T(:,2)/dr(2);
%T(:,3)= T(:,3)/dr(3);

% for i= 1:zpts
%     xx{i}= showx(:,:,i);
%     %xxx(i)= sum(showx(:,:,i));
%     yy{i}= showy(:,:,i);
%     zz{i}= showz(:,:,i); 
%     ux{i}= unitx(:,:,i);
%     uy{i}= unity(:,:,i);
%     uz{i}= unitz(:,:,i);
% end
% 
% showx(:,:,1);
% sum(showx(:,:,5))

%--- Move needles and seeds;
k= 1E-7;     % Gain factor

newseeds= ones(Nseeds,3);

%--- Move seeds in x direction
dx= k*T(:,1); 
newseeds(:,1)= seeds(:,1)+round(dx);
%--- Move seeds in y direction
dy= k*T(:,2);
newseeds(:,2)= seeds(:,2)+round(dy);
%--- Move seeds in z direction
dz= k*T(:,3);
newseeds(:,3)= seeds(:,3)+round(dz);

% newseeds(1,:)=newseeds(2,:);
% newseeds(3,:)=newseeds(2,:);
% %--- Make sure seed positions don't overlap
% for i= 1:Nseeds
%     overlap= find(newseeds(i,1)==newseeds(:,1) & newseeds(i,2)==newseeds(:,2) & newseeds(i,3)==newseeds(:,3));
%     same=overlap(:)==i;
%     overlap(same)=0;
%     % Loop if any seeds overlap
%     if any(overlap)
%         overlap(same)=i;
%         % Which needle is closest to shared grid point
%         rr(:,1)= abs(abs(newseeds(overlap,1))-abs(seeds(overlap,1)+dx(overlap))); 
%         rr(:,2)= abs(abs(newseeds(overlap,2))-abs(seeds(overlap,2)+dy(overlap)));
%         rr(:,3)= abs(abs(newseeds(overlap,3))-abs(seeds(overlap,3)+dz(overlap)));
%         RR= sqrt(rr(:,1).^2 + rr(:,2).^2 + rr(:,3).^2);
%         % Closest needle stays at grid point
%         minimum= find(RR==min(RR));
%         overlap(minimum(1))= []; % Take out of overlaped category
%         rr((minimum(1)),:)= [];
%         RR(minimum(1))= [];
%         while ~isempty(overlap)   % While there are still overlaped needles
%             maximum= find(RR==max(RR));
%             if min(XX) >= min(YY)       
%             ind2= overlap(find(YY==min(YY),1));
%             open= 1;
%             while open > 0.5
%                 % If open move needle to adjacent X position
%                 ind3= (newneedles(ind2,1)-open==newneedles(:,1)) & (newneedles(ind2,2)==newneedles(:,2));
%                 if ~any(ind3)
%                     newneedles(ind2,1)= newneedles(ind2,1)-open;
%                     open= 0;
%                 else
%                     ind4= (newneedles(ind2,1)+open==newneedles(:,1)) & (newneedles(ind2,2)==newneedles(:,2));
%                     if ~any(ind4)
%                         newneedles(ind2,1)= newneedles(ind2,1)+open;
%                         open=0;
%                     elseif isempty(overlap)
%                         open=0;
%                     else
%                         % If both adjacent positions are not open move to
%                         % adjacent + 1
%                         open=open+1;
%                     end
%                 end
%                 overlap(YY==min(YY))=[];
%                 XX(YY==min(YY))=[];
%                 YY(YY==min(YY))=[];
%             end
%             
%         else
%             ind2= overlap(find(XX==min(XX),1));
%             open= 1;
%             while open > 0.5
%                 ind3= (newneedles(ind2,1)==newneedles(:,1)) & (newneedles(ind2,2)-open==newneedles(:,2));
%                 if ~any(ind3)
%                     newneedles(ind2,2)= newneedles(ind2,2)-open;
%                     open= 0;
%                 else
%                     ind4= (newneedles(ind2,1)==newneedles(:,1)) & (newneedles(ind2,2)+open==newneedles(:,2));
%                     if ~any(ind4)
%                         newneedles(ind2,2)= newneedles(ind2,2)+open;
%                         open=0;
%                     elseif isempty(overlap)
%                         open=0;
%                     else
%                         open=open+1;
%                     end
%                 end
%                 overlap(XX==min(XX))=[];
%                 YY(XX==min(XX))=[];
%                 XX(XX==min(XX))=[];                
%             end
%         end
%         end
%     end
% 
% 
% end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if newseeds == seeds
    optimize= 0;
    disp('Seeds dont move')
end

if iter == 1
    optimize= 0;
    disp('Reached the last iteration ')
end

% seeds=newseeds;
iter= iter+1;
figure
scatter3(seeds(:,1)*.1-.05,seeds(:,2)*.1-.05,seeds(:,3)*.1-.05,'fill')
end