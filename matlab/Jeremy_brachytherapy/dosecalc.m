function dose2d = dosecalc(X,sd,s,pat,f)
% 'dosecalc(X)' takes a given 2-d seed placement and
% returns the dose map for function-defined seed
% dose profile. It also prints relevant evaluation
% parameters.
%   X == 2-d grid of seed placement
%  sd == vector of seed coordinates
%   s == seed profile, 1=slim, 2=broad, 3=aapm
%   f == seed strength factor

switch (s==s)
    case s==1
        load seed_files/sDdosesm               % modified ("slimmer") seed dose profile
    case s==2
        load seed_files/sDdose                 % contains seed dose profile
    otherwise
        load seed_files/sDdoseAAPM             % central plane from AAPM seed profile
end

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


Dp = 145;     % prescribed dose
sDdose = sDdose*f;
if (sum(nonzeros(X)~=0))
    [ym xm] = size(X); 
    numS = sum(nonzeros(X));
    dose2d = zeros(ym,xm);
    [yy xx] = find(X); % puts coords in one vector
    sd = [yy xx];
else 
    [ym xm] = size(tGeo); 
    numS = length(sd(:,1));
    dose2d = zeros(ym,xm);
end

ii = ym+1; fi = ym-1;
ij = xm+1; fj = xm-1;
for ss = 1:numS
    Si = sd(ss,1); Ii= 76-Si;	Fi= Ii+fi;
    Sj = sd(ss,2); Ij= 81-Sj;	Fj= Ij+fj; 
    dose2d(:,:) = dose2d(:,:) + sDdose(Ii:Fi,Ij:Fj);
end

return