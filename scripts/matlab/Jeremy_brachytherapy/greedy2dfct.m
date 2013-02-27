function y = greedy2dfct(Dp,Tcov,usegrid,factor,sd,pat)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% TWO-DIMENSIONAL GREEDY HEURISTIC w/ DOSE-UPDATE          %
%   code by: jeremy a roberts     04/2008                  %
%   based on the work of: sua yoo, vibha chaswal, et al.   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This version is a function called by another method for  %
% comparison.  As input are the various problem parameters,%
% namely the use of grid, seed strength factor, seed type, %
% and patient slice number.                                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
disp('TWO-DIMENSIONAL GREEDY HEURISTIC w/ DOSE-UPDATE')
tic

%---- load data files ---%

switch (pat==pat)
    case (pat==3)
        load pat_case/patSl3 ta ur re no *vol *Geo
        disp('patient - slice 3')
    case pat==5
        load pat_case/patSl5 ta ur re no *vol *Geo
        disp('patient - slice 5')  
    case pat==7
        load pat_case/patSl7 ta ur re no *vol *Geo
        disp('patient - slice 7')
    case pat==9
        load pat_case/patSl9 ta ur re no *vol *Geo
        disp('patient - slice 9')   
    case pat==11
        load pat_case/patSl11 ta ur re no *vol *Geo
        disp('patient - slice 11')
    otherwise   
        disp(['case doesn''t exist! pat=',num2str(pat)])
        return
end

switch (sd==sd)
    case sd==1
        load seed_files/sDdosesm               % modified ("slimmer") seed dose profile
        disp('using slim seed profile')
    case sd==2
        load seed_files/sDdose                 % contains seed dose profile
        disp('using broad seed profile')
    otherwise
        load seed_files/sDdoseAAPM             % central plane from AAPM seed profile
        disp('using center plane AAPM profile')
end

%---- initiate various arrays ----%

[ym xm] = size(ta);         % [num rows =y, num cols =x]
dose2d = zeros(ym,xm);      % current dose profile
X = zeros(ym,xm);           % used seed placement
grid2d = zeros(ym,xm);  
if (usegrid) 
    grid2d(1:5:end,1:5:end)=1;  
    seed2d = tGeo.*grid2d;  % treatment grid, 5mm sq intervals
else
    seed2d = tGeo;          % possible seed placements
end
sDdose = sDdose*factor;     % seed dose profile 

%---- generate adjoint ratio ----%
ratioVal =  ((1*ur/uvol + 1.0*re/rvol + no/nvol)./(ta/tvol)) .* seed2d ;
ratio = zeros(sum(nonzeros(seed2d)), 3);

% the search space is limited to the target region
s = 0;
for Sj= 1:xm		
    for Si= 1:ym		
        if (ratioVal(Si,Sj)~=0)	
            s = s+1;		
            ratio(s,1)= Si; ratio(s,2)= Sj;
            ratio(s,3)= ratioVal(Si,Sj);
         end	
   end
end

%---- make ranking ----%
ranking = sortrows(ratio,3);
ranking = [[1:s]' ranking]; % [orig rank x y val]

%---- start greedy ----%
rankS = 1;
fi = ym-1; fj = xm-1;

% first seed %
Si= ranking(rankS,2);		Ii= 76-Si;	Fi= Ii+fi; %76,74,81,79
Sj= ranking(rankS,3);		Ij= 81-Sj;	Fj= Ij+fj;  
X(Si,Sj)= 1;	
dose2d(:,:,:) = dose2d(:,:,:) + sDdose(Ii:Fi,Ij:Fj);
doseLst = zeros(2800,1);
count = 0;
for i = 1:ym
    for j = 1:xm
        if (tGeo(i,j)*dose2d(i,j)~=0)
          count = count+1;
          doseLst(count)=dose2d(i,j);
        end
    end
end    
V100 = 0;
numS = 1;

%% subsequent seeds %%
while (V100 < Tcov/100 & sum(nonzeros(X)) < 30)
    ranking = mknewranking(ratioVal,X,dose2d,Dp);
    Si= ranking(1,2);		Ii= 76-Si;	Fi= Ii+fi; 
    Sj= ranking(1,3);		Ij= 81-Sj;	Fj= Ij+fj;  
    X(Si,Sj)= 1;	
    dose2d(:,:,:)= dose2d(:,:,:)+sDdose(Ii:Fi,Ij:Fj);
    V100 = sum(sum((tGeo.*(dose2d>Dp))))/sum(nonzeros(tGeo));
end
dose = dose2d;
% take me out!
%X = zeros(55,60); X(25,15)=1;X(27,18)=1;
%----
save soln2dGreedy dose seed2d X
toc
y = 0;
return


