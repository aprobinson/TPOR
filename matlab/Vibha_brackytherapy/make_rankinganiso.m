
function [rankinganiso] = make_rankinganiso(finalaniso, seed3D);

c = finalaniso; 
%load c;			%(:,1)=Si;	(:,2)=Sj;	(:,3)=Sk;	(:,4)=ratio; (:,5)=matrixtype
totS= sum(nonzeros(seed3D));
[totS,a1]= size(c);		clear ('a1');
rankinganiso= zeros(totS, 6);	%(:,1)=ranking;	(:,2)=Si;	(:,3)=Sj;	(:,4)=Sk;   (:,5)=matrixtype
for s= 1:totS
   [a1,score]= min(c(:,4));		rankinganiso(s,1)= s;
   Si= c(score,1);	rankinganiso(s,2)= Si;
   Sj= c(score,2);	rankinganiso(s,3)= Sj;
   Sk= c(score,3);	rankinganiso(s,4)= Sk;
   matrixtype=c(score,5); rankinganiso(s,5)=matrixtype;
   minratio=c(score,4); rankinganiso(s,6)=minratio;
   c(score,4)= c(score,4)*1000;
end
save rankinganiso rankinganiso;
save matrixtype matrixtype;
clear ('finalaniso','a1','a2','score','s');
