%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% f_ciso1.m - patient %%%%%
%%%%% (c)suayoo - Mar 2003 %%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% determine Ciso1 range
% function [minCiso1]= f_ciso1(seed3D,target,foDose)

function [minCiso1]= f_ciso1(seed3D,target,foDose)

Dp= 145;
[II JJ KK]= size(target);
numTa= sum(nonzeros(target));
foDose(foDose>Dp)= Dp;

numS= 0;
for Sk= 1:KK
Ik= 16-Sk;		Fk= Ik+KK-1;   
for Sj= 5:5:55
Ij= 71-Sj;		Fj= Ij+54;   
for Si= 5:5:60
Ii= 71-Si;		Fi= Ii+59;   

if (seed3D(Si,Sj,Sk)==1)
   %fprintf('%i-%i %i-%i %i-%i\n',Ik,Fk,Ij,Fj,Ii,Fi);
   numS= numS+1;
   taDose(numS)= sum(nonzeros(foDose(Ii:Fi,Ij:Fj,Ik:Fk).*target(:,:,:))) ... 
      /numTa;	
    %fprintf('Ciso1tmp: %f\n',taDose(numS)/Dp);
end 	      
 
end
end
end

aveD= taDose/Dp;
minCiso1= min(aveD); 

return
