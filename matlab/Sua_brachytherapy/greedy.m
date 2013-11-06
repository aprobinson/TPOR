%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% greedy.m - patient %%%%%
%%%%% (c)suayoo - Mar, 2003 %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;

tic

Dp= 145;		str= 0.4;	Wur=1; Wre=1; Wmata=0; Wmacs=1; Wta=1;
load mask3D;	load numROI;	[a1 a2 KK]= size(target);	
load foDose;	foDose= foDose*str;		%dose[Gy]
needle2D= zeros(60,55);		needle2D(sum(seed3D(:,:,:),3) >0)= 1;% sum along all three dimensions of the matrix
clear ('urethra','rectum','margin','normal','a1','a2');

%%%%%%%%%%%%%%% mkRatio %%%%%%%%%%%%%%%
load adflROI;
ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
%ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa+adflNo)./(Wmata*adflMa+Wta*adflTa);
totS= sum(nonzeros(seed3D));
ratioValue= ratioValue.*seed3D;
ratio= zeros(totS, 4);	%(:,1)=Si;	(:,2)=Sj;		(:,3)=Sk;		(:,4)=ratio;
s= 0;
for Sk= 1:KK		
   for Sj= 5:5:55		
      for Si= 5:5:60		
         if (ratioValue(Si,Sj,Sk)~=0)	
            s= s+1;		
            ratio(s,1)= Si;	ratio(s,2)= Sj;	ratio(s,3)= Sk;
            ratio(s,4)= ratioValue(Si,Sj,Sk);
            %fprintf('%i %i %i: %f\n',Si,Sj,Sk,ratioValue(Si,Sj,Sk));
         end	
      end
   end
end
save ratio ratio;
clear ('adflTa','adflUr','adflRe','adflMao','adflNo');
clear ('ratioValue','ratio','totS','s');

%%%%%%%%%%%%%%%%% mkRanking %%%%%%%%%%%%%%%%%
load ratio;			%(:,1)=Si;	(:,2)=Sj;	(:,3)=Sk;	(:,4)=ratio;
[totS,a1]= size(ratio);		clear ('a1');
ranking= zeros(totS, 4);	%(:,1)=ranking;	(:,2)=Si;	(:,3)=Sj;	(:,4)=Sk;
for s= 1:totS
   [a1,score]= min(ratio(:,4));		ranking(s,1)= s;
   Si= ratio(score,1);	ranking(s,2)= Si;
   Sj= ratio(score,2);	ranking(s,3)= Sj;
   Sk= ratio(score,3);	ranking(s,4)= Sk;
   ratio(score,4)= ratio(score,4)*1000;
end
save ranking ranking;
clear ('ratio','ranking','a1','a2','score','s');

%%%%%%%%%%%%%%%%% start %%%%%%%%%%%%%%%%%
load ranking;	%(:,1)=ranking;	(:,2)=Si;	(:,3)=Sj;	(:,4)=Sk;
totS= sum(nonzeros(seed3D));	totN= sum(nonzeros(needle2D));
%[minN,maxN]= f_Nrange(numTa);		%determine the range of num needles
[minN]= f_Nrange(numTa);
[minCiso1]= f_Ciso1(seed3D,target,foDose);
numTrial= 0;

%--------------------%
%--------------------%
for goalN= minN:30		%for1
%--------------------%
%--------------------%   
	%fprintf('goalN=%d\n',goalN);   
   
   %-----------------------------%
	%-----------------------------%
   for Ciso1= 0.001:0.001:minCiso1	%for2
   %-----------------------------%
   %-----------------------------%
	%fprintf('Ciso1=%f of %f\n',Ciso1,minCiso1);
   	dose= zeros(60,55,KK);	X= zeros(60,55,KK);	
      Needle= zeros(60,55);      rankS= 1;
      
      %1st seed
      Si= ranking(rankS,2);		Ii= 71-Si;	Fi= Ii+59;	Ni= Si;
      Sj= ranking(rankS,3);		Ij= 71-Sj;	Fj= Ij+54; 	Nj= Sj;
      Sk= ranking(rankS,4);		Ik= 16-Sk;	Fk= Ik+KK-1;      
      X(Si,Sj,Sk)= 1;		numS= 1;		%one seed implanted 
      Needle(Ni,Nj)= 1;		numN= 1;		%one needle used
      dose(:,:,:)= foDose(Ii:Fi, Ij:Fj, Ik:Fk);
      numV100= sum(nonzeros(dose.*target)>=Dp);
      
      %fprintf('\t numN \t numS \t Si \t Sj \t Sk \n');
      %fprintf('\t %4d \t %4d \t %2d \t %2d \t %2d \n',...
      %   numN, numS, Si, Sj, Sk);   
      
      %1st step: 
      %---------------------%
      while ( numN < goalN )	%while1
      %---------------------%
         rankS= rankS+1;			% check the next ranked seed
         
         if (rankS > totS)		%if1	
            %fprintf('Ciso1: rankS > totS. So, break~ \n');	
            break;	%update Ciso1, escape from while1, goto if4 then backto for2
         end  	%if1 				
         Si= ranking(rankS,2);		Ii= 71-Si;	Fi= Ii+59; 
         Sj= ranking(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;   
         Sk= ranking(rankS,4);		Ik= 16-Sk;	Fk= Ik+KK-1; 
         %if(Ciso1 == 0.001)
          %  fprintf('\t%i %i %i: %f\n',Si,Sj,Sk,dose(Si,Sj,Sk));
         %end
         if ( dose(Si,Sj,Sk) < Dp*Ciso1*numS )	%if2
            %check Needles: store (Si,Sj) into (Ni,Nj)      
            if ( Needle(Si,Sj)~=1 )	% if3: if (Si,Sj) is in a new needle
               Ni= Si;	Nj= Sj;	Needle(Ni,Nj)= 1;		numN= numN+1;
            end	%if3
            dose(:,:,:)= dose(:,:,:) + foDose(Ii:Fi, Ij:Fj, Ik:Fk);
            numS= numS+1;
            %fprintf('\t %4d \t %4d \t %2d \ut %2d \t %2d \t %5.4f \n', ...
            %   numN, numS, Si, Sj, Sk, Ciso1);
            numV100= sum(nonzeros((dose.*target)>=Dp));
            X(Si,Sj,Sk)= 1;
            ranking(rankS,1)= totS*2 + numS;
            rankS= 0;            
         end		%if2: if ( dose(Si,Sj,Sk) < Dp*Ciso1*numS )	
         
      end		%while1: while ( numN < goalN )         
      
      %save X and dose for resetting 
      XCiso1= X;	doseCiso1= dose;		
      numSCiso1= numS;	NeedleCiso1= Needle;
      
      %2nd step:
      if ( numN == goalN )	%if4: if Ciso1 was sucessful         
         
         %-------------------------%
         %-------------------------%
         for Ciso2= 1.02:0.02:1.08	%for3
         %-------------------------%
         %-------------------------%
         
         	while ( numV100 < (numTa*0.98) )	%while2
            	rankS= rankS+1;
               
               if (rankS > totS)		%if5
                  %-----	fprintf('Ciso2: rankS > totS. So, break~ \n');		
                  break;	%update Ciso2, escape from while2, goto if7 then backto for3
               end	%if5
               Si= ranking(rankS,2);		Ii= 71-Si;	Fi= Ii+59; 
               Sj= ranking(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;   
               Sk= ranking(rankS,4);		Ik= 16-Sk;	Fk= Ik+KK-1;      
               
               % if the prefered seed position is in one of the all needles                        
               if ( Needle(Si,Sj)==1 & X(Si,Sj,Sk)==0 & ...
                     dose(Si,Sj,Sk) < Dp*Ciso2)		%if6
                  dose(:,:,:)= dose(:,:,:) + foDose(Ii:Fi, Ij:Fj, Ik:Fk);
                  X(Si,Sj,Sk)= 1;		numS= numS+1;
                  numV100= sum(nonzeros(dose.*target>=Dp));
                  %-----fprintf('\t %4d \t %4d \t %2d \t %2d \t %2d \t %4.3f\n', ...
                  %-----   numN, numS, Si, Sj, Sk, Ciso2);
                  ranking(rankS,1)= totS*2 + numS;     
                  rankS= 0;            
               end   %if6
               
            end	%while2: while ( numV100 < (numTa*0.96) )
            %-----fprintf('end of While\n');
            
            if (numV100 < (numTa*0.98))	%if7
               %if Ciso2 was not sucessful
               %reset data for another Ciso2
               rankS= 0;		numV100= 0;		
         		numS= numSCiso1;	X= XCiso1;	dose= doseCiso1;
         	end	%if7
         
         	%-------------------------%
         	if (numV100 >= numTa*0.98)	%if8: if Ciso2 was sucessful
         	%-------------------------%
            	%check more precise Ciso2 value
            	%use the current goalN, Ciso1 and newCiso2
            	%run greedy and check f_obj and save all
               
               %--------------------------------------%
               %--------------------------------------%
               for newCiso2= Ciso2:-0.001:(Ciso2-0.019)	%for4
               %--------------------------------------%
               %--------------------------------------%
               
               	%----- fprintf('newCiso2=%4.3f\n',newCiso2);              		
               	rankS= 0;       
               	dose= doseCiso1;		X= XCiso1;		
               	numS= numSCiso1;		Needle= NeedleCiso1;
               	numV100= sum(nonzeros((dose.*target)>=Dp));
                  
                  while ( numV100 < numTa*0.98 ) %while3                     
                     rankS= rankS+1;
                     
                  	if (rankS > totS)	%if9: if newCiso2 is not sucessful
                        break;	%no more try for newCiso2
                        %escape from while3, 
                        %then face another break in if11, escape from for4 (newCiso2),  
                        %then face another break in if8, escape from for3 (Ciso2),
                        %then face another break in if13, escape from for2 (Ciso1)
                        %then face another break in if14, escape from for1 (goalN)
                  	end	%if9
                  	Si= ranking(rankS,2);		Ii= 71-Si;	Fi= Ii+59; 
                  	Sj= ranking(rankS,3);		Ij= 71-Sj;	Fj= Ij+54;   
                  	Sk= ranking(rankS,4);		Ik= 16-Sk;	Fk= Ik+KK-1;      
                  	if (Needle(Si,Sj)==1 & X(Si,Sj,Sk)==0 & ...
                  	      dose(Si,Sj,Sk)<Dp*newCiso2 )	%if10
                  	   dose(:,:,:)= dose(:,:,:) + foDose(Ii:Fi,Ij:Fj,Ik:Fk);                    
                  	   numV100= sum(nonzeros((dose.*target)>=Dp));   
                  	   numS= numS+1;	
                  	   ranking(rankS,1)= totS*2 + numS;		rankS= 0;
                  	end	%if10
                     
                  end 	%while3
                  
               	if (numV100 < numTa*0.98)	%if11
               	   break;	%if newCiso2 was not sucessful                  
               	end		%if11
                  
                  if (numV100 >= numTa*0.98) %if12: if newCiso2 was sucessful                  
                	  numTrial= numTrial+1;		 
                	  %-----fprintf('numTrial= %d\n',numTrial);
                	  %-----fprintf('numV100=%4.3f\n',numV100);
                	  trial(numTrial,:)= [goalN,Ciso1,newCiso2,numV100];
                	  %reset data for a new Ciso2
                	  rankS= 0;		numV100= 0;		
                	  numS= numSCiso1;	X= XCiso1;	dose= doseCiso1;                       
                  end	%if12
                  
               end	%for4: for newCiso2= Ciso2:-0.001:(Ciso2-0.009)
               
            	break;	%no more try for Ciso2
             
            end	%if8: if (numV100 >= numTa*0.96)	
             
         end		%for3: Ciso2= 1.00:0.01:1.10              
      end		%if4: if (numN == goalN)                       
      numN= 0;
      
      if (numTrial > 0) %if13
         break;   %no more tray for Ciso1
      end      %if13
      
   end		%for2: for Ciso1= 0.01:0.001:minCiso1 
   
   %if (numTrial > 0 & goalN>=maxN)
   if (numTrial > 0)	%if14
      break;	%no more try for goalN
   end	%if14
   
end	%for1: for goalN= minN:maxN

toc
goalNF= trial(numTrial,1);		
Ciso1F= trial(numTrial,2);		Ciso2F= trial(numTrial,3);		
numV100F= trial(numTrial,4);	fractionV= numV100F/numTa;
fprintf('solution: goalN=%d, Ciso1=%4.3f, Ciso2=%4.3f, numV100=%4.3f\n', ...   
   goalNF,Ciso1F,Ciso2F,fractionV);
[dose, X]= f_greedy(goalNF, Ciso1F, Ciso2F);
save packSol dose X;

