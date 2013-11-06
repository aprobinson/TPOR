%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% mkadflROI_2301.m - patient %%%%%
%%%%% (c)suayoo - May 2003   %%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% make adflux ROI
% load mask3D: target, urethra, rectum, margin, normal, seed3D
% load isoseed: dose from one 1mCi 125-I seed at (70,70,15)

clear all;

%tic

load mask3D;	%target, urethra, rectum, margin, normal, seed3D
load isoseed;	%dose: 139x139x29

[II JJ KK]= size(target); % target size assigned to variables II,JJ,KK 60x55x11
   
adflTa=zeros(60,55,KK);		numTa= 0; % defining variables with certain initial value = 0 
adflUr=zeros(60,55,KK);		numUr= 0;
adflRe=zeros(60,55,KK);		numRe= 0;
adflMa=zeros(60,55,KK);		numMa= 0;
adflNo=zeros(60,55,KK);		numNo= 0;

% make adflROI(Si,Sj,Sk);
for k= 1:KK
Ik= 16-k;		Fk= Ik+KK-1;   
for j= 1:55
Ij= 71-j;		Fj= Ij+54;   
for i= 1:60
Ii= 71-i;		Fi= Ii+59;   

% adflTa: in this iterative matrix addition operation the seed is placed at
% all the target voxels and its contribution to the dose at all the other
% voxels keeps adding up whenever a seed is added to a target voxel, 
% this way you get the adjoint flux in the target region, same procedure for all other ROIs
% Ii,Ij,Ik are so chosen so that the selected point (i,j,k) should fall on
% the isoseed central point i.e. 70,70,15
if (target(i,j,k)==1)
    %fprintf('first target voxel is %d %d %d\n',i,j,k);
	numTa= numTa+1;   
   adflTa(:,:,:)= adflTa(:,:,:)+isoseed(Ii:Fi,Ij:Fj,Ik:Fk);  % matrix addition operation
   %fprintf('adflux is : %d\n',adflTa(i,j,k));
  % fprintf('dose from fwd matrix : %d\n', isoseed(Ii,Ij,Ik));
elseif (urethra(i,j,k)==1)
   numUr= numUr+1;
   adflUr(:,:,:)= adflUr(:,:,:)+isoseed(Ii:Fi,Ij:Fj,Ik:Fk);   
elseif (rectum(i,j,k)==1)
   numRe= numRe+1;
   adflRe(:,:,:)= adflRe(:,:,:)+isoseed(Ii:Fi,Ij:Fj,Ik:Fk);   
elseif (margin(i,j,k)==1)
   numMa= numMa+1;
   adflMa(:,:,:)= adflMa(:,:,:)+isoseed(Ii:Fi,Ij:Fj,Ik:Fk);   
else
   numNo= numNo+1;
   adflNo(:,:,:)= adflNo(:,:,:)+isoseed(Ii:Fi,Ij:Fj,Ik:Fk);   
end

end
end
end

adflTa= adflTa/numTa;		adflUr= adflUr/numUr;
adflRe= adflRe/numRe;		adflMa= adflMa/numMa;
adflNo= adflNo/numNo;

totS= sum(nonzeros(seed3D));

save adflROI adflTa adflUr adflRe adflMa adflNo;
save numROI numTa numUr numRe numMa numNo totS;

%toc
tic
%%%%%%%%%%%%%%% mkRatio %%%%%%%%%%%%%%%added from greedy

load adflROI; 

Wur=0.5;Wre=1;Wta=1;Wmacs=0.5;Wmata=0;
%Wur=1;Wre=1;Wta=1;Wmacs=1;Wmata=0;
ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
totS= sum(nonzeros(seed3D));
ratioValue= ratioValue.*seed3D;
isoratio= zeros(totS, 4);	%(:,1)=Si;	(:,2)=Sj;		(:,3)=Sk;		(:,4)=ratio;
s= 0;
for Sk= 1:KK		
   for Sj= 5:5:55		
      for Si= 5:5:60		
         if (ratioValue(Si,Sj,Sk)~=0)	
            s= s+1;		
            isoratio(s,1)= Si;	isoratio(s,2)= Sj;	isoratio(s,3)= Sk;
            isoratio(s,4)= ratioValue(Si,Sj,Sk);
            %fprintf('%f ',isoratio(s,4));
         end	
      end
   end
end
save isoratio isoratio;
clear ('adflTa','adflUr','adflRe','adflMao','adflNo');
clear ('ratioValue','isoratio','totS','s');

%%%%%%%%%%%%%%%%% mkRanking %%%%%%%%%%%%%%%%%
load isoratio;			%(:,1)=Si;	(:,2)=Sj;	(:,3)=Sk;	(:,4)=ratio;
[totS,a1]= size(isoratio);		clear ('a1');
isoranking= zeros(totS, 4);	%(:,1)=ranking;	(:,2)=Si;	(:,3)=Sj;	(:,4)=Sk;
for s= 1:totS
   [a1,score]= min(isoratio(:,4));		
   isoranking(s,1)= s;
   Si= isoratio(score,1);	isoranking(s,2)= Si;
   Sj= isoratio(score,2);	isoranking(s,3)= Sj;
   Sk= isoratio(score,3);	isoranking(s,4)= Sk;
   isoratio(score,4)= isoratio(score,4)*1000;
end
save isoranking isoranking;
toc
%clear ('ratio','ranking','a1','a2','score','s');