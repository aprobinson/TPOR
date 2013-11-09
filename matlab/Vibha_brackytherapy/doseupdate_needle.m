%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%doseupdate.m - patient %%%%%
%%%%% (c)vibhachaswal - Jan, 2006%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 

%%%%%%FOR DIRECTIONAL SEED OPTIMIZATION
%%%%% WITH dose
%%%%%%% WIth Needle Limiting Function
%%%%% works under the main criteria of 98% coverage
%%%%%% ranking update scheme

clear all;

tic

Dp= 145;		str= 0.43;	

Wur=1; Wre=1; Wmata=0; Wmacs=1; Wta=1;

load mask3D;	load numROI;	[a1 a2 KK]= size(target);	

load isoseed;       % isotropic seed kernel

load supermatrix;	% all orientations of directional seed kernel
clear ('urethra','rectum','margin','normal','a1','a2');

numta = sum(nonzeros(target));

load anisoratio1; % 249x4x13 
                  % all 13 dose kernels, 12 directional dose kernel orientations 1 isoseed
                  %(:,1)=Si   (:,2)=Sj    (:,3)=Sk    (:,4)=ratio from directional seed
load isoratio;    % 249x4    

[finalaniso] = make_finalaniso(anisoratio1,isoratio);  % ratiomatrix with variance cut-off to include isoseed
 
[rankinganiso] = make_rankinganiso(finalaniso,seed3D);  % rankings generated from "finalaniso" matrix

%%%%%%%%%%% greedy heuristic starts

dose= zeros(60,55,KK);	X= zeros(60,55,KK);	 Needle= zeros(60,55); 

load rankinganiso;	%(:,1)=rankinganiso;	(:,2)=Si;	(:,3)=Sj;	(:,4)=Sk; (:,5)=matrixtype
totS= sum(nonzeros(seed3D));	

rankS=1;

      %1st seed
      Si= rankinganiso(rankS,2);		Ii= 71-Si;	Fi= Ii+59;	Ni= Si;
      Sj= rankinganiso(rankS,3);		Ij= 71-Sj;	Fj= Ij+54; 	Nj= Sj;
      Sk= rankinganiso(rankS,4);		Ik= 16-Sk;	Fk= Ik+KK-1;      
      
      
      X(Si,Sj,Sk)= 1;		numS= 1;		%one seed implanted 
      Needle(Ni,Nj)= 1;		numN= 1;		%one needle used
      
      matrixtype=rankinganiso(rankS,5);   
      
%      [plugmatrix] = select_dosematrix(matrixtype);      
      
plugmatrix=zeros(139,139,29);
      
   if matrixtype==13
          plugmatrix=isoseed;
      else
      index = matrixtype*29;
     
      plugmatrix = supermatrix(:,:,index-28:index);      % supermatrix has data for all orientations 29 slices per orientation;
  end
  
      plugmatrix=plugmatrix*str;
            
       dose(:,:,:)= plugmatrix(Ii:Fi, Ij:Fj, Ik:Fk);    
       
       numV100ta= sum(nonzeros((dose.*target)>=Dp));       
             
      if (numV100ta > 0.98*numta)
          
      break;
      
  end
      fprintf('\t %4d \t %4d \t %2d \t %2d \t %2d \t %d \t %f \t %d\n', numN, numS, Si, Sj, Sk, rankS, numV100ta, matrixtype);    
%             
%       clear('rankinganiso');
  
while (numV100ta < 0.98*numta) % MAIN if LOOP     
         
 [rankinganiso_new] = make_rankinganiso_new(finalaniso, seed3D, dose, numS);
   
      rankS= 1;			% check the next ranked seed                             
            
         Si= rankinganiso_new(rankS,2);		Ii= 71-Si;	Fi= Ii+59; 
         Sj= rankinganiso_new(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;   
         Sk= rankinganiso_new(rankS,4);		Ik= 16-Sk;	Fk= Ik+KK-1;               

         while X(Si,Sj,Sk)==1
             rankS=rankS+1;
             Si= rankinganiso_new(rankS,2);		Ii= 71-Si;	Fi= Ii+59; 
             Sj= rankinganiso_new(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;   
             Sk= rankinganiso_new(rankS,4);		Ik= 16-Sk;	Fk= Ik+KK-1;  
         end
         
         %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

         if(Needle(Si,Sj)==1)&&(X(Si,Sj,Sk)~=1)  % if1: Main if loop for needle (if same needle no change)
            
             %fprintf('\n Seed directly in old needle \t %d \t %d\t %d\n', Si,Sj,Sk);
              
             numN = numN;             
             X(Si,Sj,Sk)=1;            numS =numS+1;    
             matrixtype=rankinganiso_new(rankS,5);  
             
         elseif (Needle(Si,Sj)==0)  % if1: (cont.) if new needle penalize and rerank
    
             rankingforN=rankinganiso_new; % rankS, Si,Sj,Sk, matrixtype, ratio
             s=0;
              
             while(s==0)	% WHILE2: equate s=1 when a needle gets selected  is 0 till Needle(Si,Sj)==0    	
              
                 Penalty = (((30^2+(numN+1)^2)/(30^2-(numN+1)^2))^(1/2))*(0.5);
              
                 rankingforN(rankS,6)=Penalty.*rankingforN(rankS,6);  % Update ratios in column 5 with penalty
              
              
                 if(rankingforN((rankS+1),6)>=rankingforN(rankS,6))  %if2: if penalized-ratio doesn't increase beyond next rank no need to rerank
                      
                     matrixtype=rankingforN(rankS,5);    
     
                     break;  % Since co-ordinates selected for Needle and seed break out of while loop
               
                 elseif (rankingforN((rankS+1),6)<rankingforN(rankS,6)) % if2 cont..
                           
                     rankS = rankS+1;
             
                     Si= rankingforN(rankS,2);		Ii= 71-Si;	Fi= Ii+59; 
                     Sj= rankingforN(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;   
                     Sk= rankingforN(rankS,4);		Ik= 16-Sk;	Fk= Ik+KK-1;    
                     matrixtype=rankingforN(rankS,5);  
         
                     if(Needle(Si,Sj)==1)&&(X(Si,Sj,Sk)~=1)
                         break;
                     end
                     
                     s=0; % Go back and repenalize
                 end % if2 ends
             end % WHILE2 ends              
      
             if(Needle(Si,Sj)==1)        
                               
                 % fprintf('flows \n');
                 numN = numN;  
                 X(Si,Sj,Sk)=1;            numS = numS+1;             
             elseif(Needle(Si,Sj)~=1)
                 Needle(Si,Sj)=1;          numN = numN+1;
                 X(Si,Sj,Sk)=1;            numS = numS+1;   
             end
         end % if1 ends
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%       
            
%    [plugmatrix] = select_dosematrix(matrixtype);     
plugmatrix=zeros(139,139,29);
if matrixtype==13
    plugmatrix=isoseed;
else
    index = matrixtype*29;
     
    plugmatrix = supermatrix(:,:,index-28:index);      % supermatrix has data for all orientations 29 slices per orientation;
end

plugmatrix=plugmatrix*str;
           
dose(:,:,:)= dose(:,:,:)+ plugmatrix(Ii:Fi, Ij:Fj, Ik:Fk);
            
numV100ta= sum(nonzeros((dose.*target)>=Dp));       

fprintf('\t %4d \t %4d \t %2d \t %2d \t %2d \t %d \t %f \t %d \n', numN, numS, Si, Sj, Sk, rankS, numV100ta, matrixtype); 
%       
%       clear('rankinganiso_new');
      
if (numV100ta >= 0.98*numta)
          
    break;
      
end       

end 
save packSol_doseupdate dose X;

toc