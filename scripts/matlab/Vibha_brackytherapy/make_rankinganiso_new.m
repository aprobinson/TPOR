function [rankinganiso_new] = make_rankinganiso_new(finalaniso, seed3D, dose, numS);

Dp = 145;
c = finalaniso;         %load c;	%(:,1)=Si;	(:,2)=Sj;	(:,3)=Sk;	(:,4)=ratio; (:,5)=matrixtype
totS= sum(nonzeros(seed3D));

[II,JJ,KK] = size(seed3D);   

j=0;
for Sk=1:KK
    for Si=5:5:60
        for Sj=5:5:55         
          
              for i=1:totS                  
          if ((c(i,1)==Si) && (c(i,2)==Sj) && (c(i,3)==Sk))
                  j=j+1;
              else 
                  j=0;
              end  
              
           if (j~=0)
              c(i,1)=Si;
              c(i,2)=Sj; 
              c(i,3)=Sk;
              if dose(Si,Sj,Sk)~=0
              c(i,4)=c(i,4)*dose(Si,Sj,Sk)/Dp;  % update ratio by dose-sensitivity
                           
              %c(i,4)=c(i,4)*(((Dp-dose(Si,Sj,Sk))^2)^0.5)/Dp;  % update ratio by dose-sensitivity

          else
               c(i,4)=c(i,4);
           end
              c(i,5)=c(i,5);
              
   %%%%%%%%%trying truncation %%%%           
%            if (dose(Si,Sj,Sk)>=11.1012) % this is the value in voxel 5mm (68.3681Gy) far off where the dose falls off to very small, values 1cm (19.7284), 1.5cm (11.1012) far also tried
%            c(i,4)=c(i,4)*(dose(Si,Sj,Sk)/Dp);
%         elseif (dose(Si,Sj,Sk)<11.1012)
%            c(i,4)=c(i,4);
%             end
   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%         
            end      
         end               
      end
  end
end

save c c;

clear ('Si','Sj','Sk', 'dose');

[totS,a1]= size(c);		clear ('a1');

rankinganiso_new= zeros(totS, 6);	%(:,1)=ranking;	(:,2)=Si;	(:,3)=Sj;	(:,4)=Sk;   (:,5)=matrixtype
for s= 1:totS
   [a1,score]= min(c(:,4));		
                                rankinganiso_new(s,1)= s;
   Si= c(score,1);	            rankinganiso_new(s,2)= Si;
   Sj= c(score,2);	            rankinganiso_new(s,3)= Sj;
   Sk= c(score,3);	            rankinganiso_new(s,4)= Sk;
   matrixtype=c(score,5);       rankinganiso_new(s,5)= matrixtype;
   minratio=c(score,4);         rankinganiso_new(s,6)= minratio;
   c(score,4)= c(score,4)*1000;
   if(numS == 0)
    fprintf('%f\n',minratio);
   end
end

save rankinganiso_new rankinganiso_new;
save matrixtype matrixtype;
clear ('finalaniso','a1','a2','score','s','c');