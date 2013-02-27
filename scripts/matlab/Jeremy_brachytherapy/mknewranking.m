function newranking = mknewranking(ratioValue, seed2d, dose, Dp);

[II JJ]=size(seed2d);

totS= sum(nonzeros(seed2d));

newratio= zeros(totS, 3);	%(:,1)=Si; (:,2)=Sj; (:,3)=ratio;
s= 0;
for Sj= 1:JJ %5:5:JJ		
      for Si= 1:II %5:5:II		
         if (ratioValue(Si,Sj)~=0)	
            s= s+1;		
            newratio(s,1)= Si;	newratio(s,2)= Sj;
            newratio(s,3)= ratioValue(Si,Sj).*(dose(Si,Sj)/Dp);
         end	
      end
end
%save newratio newratio;

temp = newratio;			%(:,1)=Si;	(:,2)=Sj; (:,3)=ratio;
[totS,a1]= size(temp);		clear ('a1');
newranking= zeros(totS, 4);	%(:,1)=ranking;	(:,2)=Si;	(:,3)=Sj;	(:,4)=Sk;

for s= 1:totS
    
   [a1,score]= min(temp(:,3));		
   
                            newranking(s,1)= s;
   Si= temp(score,1);	    newranking(s,2)= Si;
   Sj= temp(score,2);	    newranking(s,3)= Sj;
   ratio= temp(score,3);    newranking(s,4)= ratio;
   temp(score,3)= temp(score,3)*1000;
   
end

return
%save newranking newranking;
%clear ('temp','newratio','a1','a2','score','s');
