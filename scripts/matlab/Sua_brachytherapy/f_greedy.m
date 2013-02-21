%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% f_greedy.m - patient %%%%%
%%%%% (c)suayoo - Mar, 2003 %%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function [dose,X]= f_greedy(goalN, Ciso1, Ciso2);
% run greedy for the final solution

function [dose,X]= f_greedy(goalN,Ciso1,Ciso2)

Dp= 145;		str= 0.4;
load ranking;	%(:,1)=ranking;		(:,2)=Si;		(:,3)=Sj;		(:,4)=Sk;
load foDose;		foDose= foDose*str;
load numROI;		load mask3D;		
clear ('urethra','rectum','margin','normal');
[x,y,z_dim]= size(target);
numV100= 0;		
dose= zeros(60,55,z_dim);
X= zeros(60,55,z_dim);	Needle= zeros(60,55);

rankS= 1;		
Si= ranking(rankS,2);		Ii= 71-Si;	Fi= Ii+59;
Sj= ranking(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;
Sk= ranking(rankS,4);		Ik= 16-Sk;	Fk= Ik+z_dim-1;
Ni= Si;	Nj= Sj;	
X(Si,Sj,Sk)= 1;			numS= 1;	
Needle(Ni,Nj)= 1;		numN= 1;

dose(:,:,:)= foDose(Ii:Fi, Ij:Fj, Ik:Fk);
numV100= sum(nonzeros(dose.*target >= Dp));
fprintf('\t numN \t numS \t Si \t Sj \t Sk \n');
fprintf('\t %4d \t %4d \t %2d \t %2d \t %2d \n',...
   numN, numS, Si, Sj, Sk);

ranking(rankS,1)= totS*2 + numS;

%----- 1st step: Ciso1
while ( numN < goalN )
   rankS= rankS + 1;
   Si= ranking(rankS,2);		Ii= 71-Si;	Fi= Ii+59; 
   Sj= ranking(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;   
   Sk= ranking(rankS,4);		Ik= 16-Sk;	Fk= Ik+z_dim-1;   
   
   if ( dose(Si,Sj,Sk) < Dp*Ciso1*(numS) )	
      
      if ( Needle(Si,Sj)~=1 )
         Ni= Si;	Nj= Sj;	Needle(Ni,Nj)= 1;	numN= numN+1;
      end
      
      dose(:,:,:)= dose(:,:,:) + foDose(Ii:Fi, Ij:Fj, Ik:Fk);
      numS= numS+1;
      fprintf('\t %4d \t %4d \t %2d \t %2d \t %2d \n', ...
         numN, numS, Si, Sj, Sk);
      taDose= dose.*target;               
      numV100= sum(nonzeros(taDose)>=Dp);
      X(Si,Sj,Sk)= 1;
      ranking(rankS,1)= totS*2 + numS;
      rankS= 0;            
   end   
end

%%%%% 2nd step - Ciso2
while ( numV100 < (numTa*0.98) )
   rankS= rankS+1;
   Si= ranking(rankS,2);		Ii= 71-Si;	Fi= Ii+59; 
   Sj= ranking(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;   
   Sk= ranking(rankS,4);		Ik= 16-Sk;	Fk= Ik+z_dim-1;   
   
   if ( Needle(Si,Sj)==1 & X(Si,Sj,Sk)==0 & ...
         dose(Si,Sj,Sk) < Dp*Ciso2)		       dose(:,:,:)= dose(:,:,:) + foDose(Ii:Fi, Ij:Fj, Ik:Fk);
      numS= numS+1;
      fprintf('\t %4d \t %4d \t %2d \t %2d \t %2d \n', ...
         numN, numS, Si, Sj, Sk);
      tadose(:,:,:)= dose(:,:,:).*target(:,:,:);               
      numV100= sum(sum(sum(tadose(:,:,:)>=Dp)));
      X(Si,Sj,Sk)= 1;
      ranking(rankS,1)= totS*2 + numS;
      rankS= 0;            
   end   
end
save packSol dose X
return
