%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% mkadflROI.m - patient %%%%%
%%%%% (c)suayoo - May 2003   %%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% make adflux ROI
% load mask3D: target, urethra, rectum, margin, normal, seed3D
% load foDose: dose from one 1mCi 125-I seed at (70,70,15)

clear all;

tic

load mask3D;	%target, urethra, rectum, margin, normal, seed3D
load foDose;	%dose: 139x139x29

[II JJ KK]= size(target);
   
adflTa=zeros(60,55,KK);		numTa= 0;
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

if (target(i,j,k)==1)
	numTa= numTa+1;   
   adflTa(:,:,:)= adflTa(:,:,:)+foDose(Ii:Fi,Ij:Fj,Ik:Fk);   
elseif (urethra(i,j,k)==1)
   numUr= numUr+1;
   adflUr(:,:,:)= adflUr(:,:,:)+foDose(Ii:Fi,Ij:Fj,Ik:Fk);   
elseif (rectum(i,j,k)==1)
   numRe= numRe+1;
   adflRe(:,:,:)= adflRe(:,:,:)+foDose(Ii:Fi,Ij:Fj,Ik:Fk);   
elseif (margin(i,j,k)==1)
   numMa= numMa+1;
   adflMa(:,:,:)= adflMa(:,:,:)+foDose(Ii:Fi,Ij:Fj,Ik:Fk);   
else
   numNo= numNo+1;
   adflNo(:,:,:)= adflNo(:,:,:)+foDose(Ii:Fi,Ij:Fj,Ik:Fk);   
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

toc