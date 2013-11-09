close all
clear all

%%%%%   LOAD DATA   %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%
xpts= 33;  %Number of x mesh points
ypts= 33;  %Number of y mesh points
zpts= 33;  %Number of z mesh points
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

%---Needle placement
Nneedles= 2;    %number of initial needles
%needles= [28 28];
needles= [17 14; 17 20];

z= [11 17 20 26];
Nseeds= length(z)*Nneedles;
seeds= zeros(Nseeds,3);
num_ndl= zeros(1,Nneedles);

for i= 1:Nneedles
    n= length(z)*i;
    num_ndl(i)= length(z);
    m= n-length(z)+1;
    seeds(m:n,1,1)= needles(i,1);
    seeds(m:n,2,1)= needles(i,2);
    seeds(m:n,3,1)= z;
end

D=zeros(xpts,ypts,zpts);    %dose-rate matrix

% Calculate dose

%---Use SeedDose.m to calculate dose by how many voxels away 
%   from the seed in the x y z direction 
load DoseMatrix
load DoseNist99
load suadose
% Seed position in dose rate matrix (a,b,c)
a= 70; b= 70; c= 70;

newseeds= ones(Nseeds,3);
newneedles= zeros(Nneedles,2); 
 
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
    centered= circshift(DNist99/(Decay*100), [-delta_i,-delta_j,-delta_k]);
    %centered= circshift(SUA_DOSE*.4), [-delta_i,-delta_j,-delta_k]);
    testing= DNist99(60:80,60:80,70)/(Decay*100);
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

check= f(28,25:31,6);
D(28,28,1:11);
check2= f(28,28,1:11);

T= zeros(size(seeds,1),3);         %Displacement Vector
for k= 1:pts(3)     % z voxels
for j= 1:pts(2)     % y voxels
for i= 1:pts(1) 	% x voxels

   % Compute distance between voxel(i,j,k) and the seeds
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

[newseeds, newneedles]= SeedRepositioning(k, T, seeds, needles, num_ndl);
% newseeds= ones(Nseeds,3);
% newneedles= zeros(Nneedles,2);
% dx= zeros(Nneedles(1));
% dy= dx;
% mm= 0;
% 
% for i= 1:Nneedles
%     mn= mm+num_ndl(i);
% %--- Move needles in x direction (in voxels)
%     dx(i,1)= k*sum(T(mm+1:mn,1))/num_ndl(i); 
%     newseeds(mm+1:mn,1,1)= seeds(mm+1:mn,1,1)+round(dx(i,1));
%     newneedles(i,1)= needles(i,1)+round(dx(i,1));
% %--- Move needles in y direction (in voxels)
%     dy(i,1)= k*sum(T(mm+1:mn,2))/num_ndl(i);
%     newseeds(mm+1:mn,2,1)= seeds(mm+1:mn,2,1)+round(dy(i,1));
%     newneedles(i,2)= needles(i,2)+round(dy(i,1));    
%     mm= mn;
% end   
% 
% %--- Move seeds in z direction
% dz= k*T(:,3);
% newseeds(:,3)= seeds(:,3)+round(dz);
% %newneedles(1,:)= newneedles(2,:);
% %--- Make sure needle positions don't overlap
% for i= 1:Nneedles-1
%     % Indices of needles that overlap in newneedles
%     n_overlap= find((newneedles(i,1)==newneedles(:,1)) & (newneedles(i,2)==newneedles(:,2)));
%     same=n_overlap(:)==i;
%     n_overlap(same)=0;
%     % Loop if any needles overlap
%     if any(n_overlap)
%         n_overlap(same)=i;
%         % Which needle is closest to shared grid point
%         XX= abs(abs(newneedles(n_overlap,1))-abs(needles(n_overlap,1)+dx(n_overlap))); 
%         YY= abs(abs(newneedles(n_overlap,2))-abs(needles(n_overlap,2)+dy(n_overlap)));
%         RR= XX.^2 + YY.^2;
%         % Closest needle stays at grid point
%         n_minimum= find(RR==min(RR));
%         n_overlap(n_minimum(1))= []; % Take out of overlaped category
%         XX(n_minimum(1))= [];     
%         YY(n_minimum(1))= [];
%         while ~isempty(n_overlap)   % While there are still overlaped needles
%         if min(XX) >= min(YY)       
%             ind2= n_overlap(find(YY==min(YY),1));
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
%                     elseif isempty(n_overlap)
%                         open=0;
%                     else
%                         % If both adjacent positions are not open move to
%                         % adjacent + 1
%                         open=open+1;
%                     end
%                 end
%                 n_overlap(YY==min(YY))=[];
%                 XX(YY==min(YY))=[];
%                 YY(YY==min(YY))=[];
%             end
%             
%         else
%             ind2= n_overlap(find(XX==min(XX),1));
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
%                     elseif isempty(n_overlap)
%                         open=0;
%                     else
%                         open=open+1;
%                     end
%                 end
%                 n_overlap(XX==min(XX))=[];
%                 YY(XX==min(XX))=[];
%                 XX(XX==min(XX))=[];                
%             end
%         end
%         end
%     end
% end
% 
% %newseeds(1,:)=newseeds(2,:);
% %--- Make sure seeds don't overlap
% mm=0;
% for i= 1:Nneedles
%     mn= mm + num_ndl(i);
%     newseeds(mm+1:mn,1)= newneedles(i,1);
%     newseeds(mm+1:mn,2)= newneedles(i,2);
%     
%     for ii= mm+1:mn
%     % Indices of seeds that overlap in newseeds
%     s_overlap= find((newseeds(ii,3)==newseeds(mm+1:mn,3)));
%     same=s_overlap(:)==ii-mm;
%     s_overlap(same)=0;
%     % Loop if any seeds in the needle overlap
%     if any(s_overlap)
%         s_overlap(same)=ii;
%         % Which seed is closest to shared grid point
%         ZZ= abs(abs(newseeds(s_overlap,1))-abs(seeds(s_overlap,1)+dz(s_overlap))); 
%         % Closest seed stays at grid point 
%         s_minimum= find(ZZ==min(ZZ));
%         s_overlap(s_minimum(1))= []; % Take out of overlaped category
%         ZZ(s_minimum)= [];
%         while ~isempty(s_overlap)   % While there are still overlaped seeds      
%             ind2= s_overlap(find(ZZ==min(ZZ),1));
%             open= 1;
%             while open > 0.5
%                 % If open move seed to adjacent X position
%                 ind3= (newseeds(ind2,3)-open==newseeds(:,3));
%                 if ~any(ind3)
%                     newseeds(ind2,3)= newseeds(ind2,3)-open;
%                     open= 0;
%                 else
%                     ind4= (newseeds(ind2,3)+open==newseeds(:,3));
%                     if ~any(ind4)
%                         newseeds(ind2,3)= newseeds(ind2,3)+open;
%                         open=0;
%                     else
%                         % If both adjacent positions are not open move to
%                         % adjacent + 1
%                         open=open+1;
%                     end
%                 end
%                 s_overlap(ZZ==min(ZZ))=[];
%                 ZZ(ZZ==min(ZZ))=[];
%             end
%         end
%     end
%     end
%     
%     mm=mn;
% end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if newseeds == seeds
    optimize= 0;
    disp('Seeds dont move')
end

if Nneedles < 2
    optimize= 0;
    disp('Not enough needles')
end

if Nseeds < 2
    optimize= 0;
    disp('Not enough seeds')
end

if iter == 5
    optimize= 0;
    disp('Reached the last iteration ')
end

seeds=newseeds;
needles=newneedles;
iter= iter+1;
figure
scatter3(seeds(:,1)*.1,seeds(:,2)*.1,seeds(:,3)*.1,'fill')
end




