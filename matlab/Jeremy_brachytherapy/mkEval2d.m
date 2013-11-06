function [CN, DNRta, DVHre, DVHno, DVHtot] = mkEval2d(dose,X,seed2d,pat,run)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% mkEval2d.m                           %
% (c) J. Roberts                       %
% a routine for calculating evaluation %
% parameters for the 2-d Greedy With   %
% Current Displacement algorithm.      %
% based on V.C. and S.Y.'s files       %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

switch (pat==pat)
    case (pat==3)
        load pat_case/patSl3 tGeo uGeo rGeo nGeo
    case pat==5
        load pat_case/patSl5 tGeo uGeo rGeo nGeo
    case pat==7
        load pat_case/patSl7 tGeo uGeo rGeo nGeo
    case pat==9
        load pat_case/patSl9 tGeo uGeo rGeo nGeo
    case pat==11
        load pat_case/patSl11 tGeo uGeo rGeo nGeo
    otherwise   
        return
end

[II JJ]= size(tGeo);

Dp= 145;		% prescribed dose

wfid= 1;%fopen('evaGreedy.out','w');
fprintf(wfid,'           Target     Urethra   Rectum    Normal    Total\n');

doseTa= dose.*tGeo;		doseUr= dose.*uGeo;
doseRe= dose.*rGeo;		doseNo= dose.*nGeo;
% re-arrange dose matrix
numTa= 0; numUr= 0; numRe= 0; numNo= 0; numTot= II*JJ;

targetD  = nonzeros(doseTa);	numTa= sum(targetD>0);		clear ('doseTa');
urethraD = nonzeros(doseUr); 	numUr= sum(urethraD>0);		clear ('doseUr');
rectumD  = nonzeros(doseRe);	numRe= sum(rectumD>0);		clear ('doseRe');
normalD  = nonzeros(doseNo);	numNo= sum(normalD>0);		clear ('doseNo');
totalD   = nonzeros(dose);

% numb of pixels in each ROI
fprintf(wfid,' #pixel    %-9d %-9d %-9d %-9d %-9d \n', ...
   numTa, numUr, numRe, numNo, numTot);

% minDose
minTa= min(targetD);		minUr= min(urethraD);
minRe= min(rectumD);		minNo= min(normalD);
minTot= min(totalD);
fprintf(wfid,' minDose   %-9.3f %-9.3f %-9.3f %-9.3f %-9.3f \n', ...
   minTa, minUr, minRe, minNo, minTot);

% maxDose
maxTa= max(targetD);		maxUr= max(urethraD);
maxRe= max(rectumD);		maxNo= max(normalD);
maxTot= max(totalD);
fprintf(wfid,' maxDose   %-9.3f %-9.3f %-9.3f %-9.3f %-9.3f \n', ...
   maxTa, maxUr, maxRe, maxNo, maxTot);

% totDose & meanDose
totTa= sum(targetD);	totUr= sum(urethraD);
totRe= sum(rectumD);	totNo= sum(normalD); 	totTot= sum(totalD);
meanTa= totTa/numTa;	meanUr= totUr/numUr;
meanRe= totRe/numRe;	meanNo= totNo/numNo;	meanTot= totTot/numTot;
%fprintf(wfid,' totDose   %-4.3e %-4.3e %-4.3e %-4.3e %-4.3e \n', ...
%   totTu, totUr, totRe, totNo, totTot);
fprintf(wfid,' meanDose  %-9.3f %-9.3f %-9.3f %-9.3f %-9.3f \n', ...
   meanTa, meanUr, meanRe, meanNo, meanTot);

%%%%%% seed
fprintf(wfid,'\n SEED\n');
%% numS
numS= (sum(sum(seed2d)));
fprintf(wfid,' avail. Seed= %-4d\t',numS);
numSeed= (sum(sum(X)));
fprintf(wfid,' used Seed= %-4d\t',numSeed);

%%%%%% target
fprintf(wfid,'\n TARGET \n');
% V90, V100, V150 and DNR(=V150/V100)
% volume receiving at least 90%Dp, 100%Dp, 150%Dp
xTa= 0:maxTa/200:(maxTa+maxTa/200);	% x-axis: dose[Gy]
D1= hist(targetD,xTa);		D2= fliplr(D1);
D3= cumsum(D2);				D4= fliplr(D3);
DVHta= 100*D4/max(D4);					% y-axis: dose-volume Hist
clear ('D1','D2','D3','D4');
i= 1;	while (i < 202 & xTa(i) < 0.9*Dp),    i= i+1;	end
V90ta= DVHta(i);	fprintf(wfid,' V90= %-4.2f\t',V90ta);
V100ta= sum(nonzeros(dose.*tGeo >= Dp))/numTa*100;
fprintf(wfid,' V100= %-4.2f\t',V100ta);
%i= 1;	while (i < 202 & xTa(i) < Dp), 			i= i+1;	end
%V100ta= DVHta(i);	fprintf(wfid,' V100= %-4.2f\t',V100ta);
i= 1;	while (i < 202 & xTa(i) < 1.5*Dp),    i= i+1;	end
V150ta= DVHta(i);	fprintf(wfid,' V150= %-4.2f\n',V150ta);
% D50, D100: min dose covering 90%, and 100% of volume
j= 1;	while (j < 202 & DVHta(j) >= 10), 		 j= j+1;	end
D10ta= xTa(j);		fprintf(wfid,' D10= %-4.2fGy\t',D10ta);
j= 1;	while (j < 202 & DVHta(j) >= 50), 		 j= j+1;	end
D50ta= xTa(j);		fprintf(wfid,' D50= %-4.2fGy\t',D50ta);
j= 1;	while (j < 202 & DVHta(j) >= 90),    j= j+1;	end
D90ta= xTa(j);		fprintf(wfid,' D90= %-4.2fGy\t',D90ta);
j= 1;	while (j < 202 & DVHta(j) >= 100),    j= j+1;	end
D100ta= xTa(j);		fprintf(wfid,' D100= %-4.2fGy\n',D100ta);
%j= 1;	while (j < 202 & DVHtu(j) >= 99.9),    j= j+1;	end
%D99tu= xTu(j);		fprintf(wfid,' D99.9= %-4.2fGy\t',D99tu);

DNRta= V150ta/V100ta;		fprintf(wfid,' DNR= %-4.2f\t\t',DNRta);

%%%%% total 
% V90, V100, V150 and DNR(=V150/V100)
% volume receiving at least 90%Dp, 100%Dp, 150%Dp
xTot= 0:maxTot/200:(maxTot+maxTot/200);	% x-axis: dose[Gy]
D1= hist(totalD,xTot);		D2= fliplr(D1);
D3= cumsum(D2);					D4= fliplr(D3);
DVHtot= 100*D4/max(D4);					% y-axis: dose-volume Hist
clear ('D1','D2','D3','D4');
i= 1;	while (i < 202 & xTot(i) < 0.9*Dp ),    i= i+1;	end
V90tot= DVHtot(i);
i= 1;	while (i < 202 & xTot(i) < Dp ),  		 i= i+1;	end
V100tot= DVHtot(i);
i= 1;	while (i < 202 & xTot(i) < 1.5*Dp),    i= i+1;	end
V150tot= DVHtot(i);
DNRtot= V150tot/V100tot;
% D90, D100: min dose covering 90%, and 100% of volume
j= 1;	while (j < 202 & DVHtot(j) >= 100),    j= j+1;	end
D100tot= xTot(j);
j= 1;	while (j < 202 & DVHtot(j) >= 90 ), 	 j= j+1;	end
D90tot= xTot(j);

% conformation number CN= V100tu/volTu * V100tu/V100tot;
CN= (V100ta/100*numTa)/numTa*(V100ta/100*numTa)/(V100tot/100*numTot);
fprintf(wfid,' CN= %-4.2f\n',CN);

%%%%%% Urethra
fprintf(wfid,'\n URETHRA\n');
% Upper bound: 125%= 145*1.25= 181.25Gy
% how mucn volume receives more than Upper bound
xUr= 0:maxUr/200:(maxUr+maxUr/200);	% x-axis: dose[Gy]
D1= hist(urethraD,xUr);		D2= fliplr(D1);
D3= cumsum(D2);				D4= fliplr(D3);
DVHur= 100*D4/max(D4);					% y-axis: dose-volume Hist
clear ('D1','D2','D3','D4');
% V125: volume receiving 125% of Dp
%i= 1;	while (i < 202 & xUr(i) <= 1.25*Dp),    i= i+1;	end
%V125ur= DVHur(i);	fprintf(wfid,' V125ur(V over 1.25 of Dp=181.250Gy)= %4.3f\n',V125ur);
% D3: min dose covering 3% of volume
%j= 1;	while (j < 202 & DVHur(j) >= 3),    j= j+1;	end
%D3ur= xUr(j);		fprintf(wfid,' D3ur= %4.3f\n',D3ur);
%U125: urethra volume receiving dose greater than 135% of Dp
i= 1;	while (i < 202 & xUr(i) < 1.25*Dp),		i= i+1;	end 
U125= DVHur(i);		fprintf(wfid,' U125(V over 1.25*Dp)= %-4.2f\t',U125); 
% V360: urethra volume receving the dose greater than 360Gy
i= 1;	while (i < 202 & xUr(i) < 360),	i= i+1;	end
V360Gy= DVHur(i);		fprintf(wfid,' V360Gy(V over 360Gy)= %4.2f\n',V360Gy);
%D10ur, D50ur, D90ur
j= 1;	while (j < 202 & DVHur(j) >= 10),		j= j+1;	end
D10ur= xUr(j);		fprintf(wfid,' D10ur= %-4.2f\t',D10ur);
j= 1;	while (j < 202 & DVHur(j) >= 50),		j= j+1;	end
D50ur= xUr(j);		fprintf(wfid,' D50ur= %-4.2f\t',D50ur);
j= 1;	while (j < 202 & DVHur(j) >= 90),		j= j+1;	end
D90ur= xUr(j);		fprintf(wfid,' D90ur= %-4.2f\n',D90ur);

%%%%%% Rectum
fprintf(wfid,'\n RECTUM\n');
% Upper bound: 80%= 145*0.8= 116Gy
% how mucn volume receives more than Upper bound
xRe= 0:maxRe/200:(maxRe+maxRe/200);	% x-axis: dose[Gy]
D1= hist(rectumD,xRe);		D2= fliplr(D1);
D3= cumsum(D2);				D4= fliplr(D3);
DVHre= 100*D4/max(D4);					% y-axis: dose-volume Hist
clear ('D1','D2','D3','D4');
% R80: volume receiving 80% of Dp
i= 1;	while (i < 202 & xRe(i) < 0.8*Dp),    i= i+1;	end
R80= DVHre(i);		fprintf(wfid,' R80(V over 0.8*Dp)= %-4.2f\t',R80);
% D3: min dose covering 3% of volume
%j= 1;	while (j < 202 & DVHre(j) >= 3),    j= j+1;	end
%D3re= xRe(j);	fprintf(wfid,' D3re= %4.3f\n',D3re);
% V90: rectum volume receiving the dose greater than 90Gy
i= 1;	while (i < 202 & xRe(i) < 90),	i= i+1;	end
V90Gy= DVHre(i);	fprintf(wfid,' V90Gy(V over 90Gy)= %-4.2f\n',V90Gy);
%D10re, D50re, D90re
j= 1;	while (j < 202 & DVHre(j) >= 10),		j= j+1;	end
D10re= xRe(j);		fprintf(wfid,' D10re= %-4.2f\t',D10re);
j= 1;	while (j < 202 & DVHre(j) >= 50),		j= j+1;	end
D50re= xRe(j);		fprintf(wfid,' D50re= %-4.2f\t',D50re);
j= 1;	while (j < 202 & DVHre(j) >= 90),		j= j+1;	end
D90re= xRe(j);		fprintf(wfid,' D90re= %-4.2f\n',D90re);

%%%%%%% normal
fprintf(wfid,'\n NORMAL\n');
% Upper bound: 120% Lower bound: 80%
% how mucn volume receives more than Upper bound
xNo= 0:maxNo/200:(maxNo+maxNo/200);	% x-axis: dose[Gy]
D1= hist(normalD,xNo);		D2= fliplr(D1);
D3= cumsum(D2);				D4= fliplr(D3);
DVHno= 100*D4/max(D4);					% y-axis: dose-volume Hist
clear ('D1','D2','D3','D4');
%D10no, D50no, D90no
j= 1;	while (j < 202 & DVHno(j) >= 10),		j= j+1;	end
D10no= xNo(j);		fprintf(wfid,' D10no= %-4.2f\t',D10no);
j= 1;	while (j < 202 & DVHno(j) >= 50),		j= j+1;	end
D50no= xNo(j);		fprintf(wfid,' D50no= %-4.2f\t',D50no);
j= 1;	while (j < 202 & DVHno(j) >= 90),		j= j+1;	end
D90no= xNo(j);		fprintf(wfid,' D90no= %-4.2f\n',D90no);

if (~run)
    file = ['gyparams',num2str(pat)]
    save(file)
else
    file = ['cdparams',num2str(pat)]
    save(file)
end

return