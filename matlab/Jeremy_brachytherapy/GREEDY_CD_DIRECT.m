%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% TWO-DIMENSIONAL                                          %
%   CD-DRIVE DIRECTIONAL SEED PLACEMENT for BETTER PLANS   %
%      (with Dose-Update):                                 %
%   "testing a new method for brachytherapy optimization"  %
%                                                          %
%   written by: jeremy a roberts (c) 4/2008                %
%   based on the work of: sua yoo, vibha chaswal, et al.   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This program uses tissue-specific adjoint currents to    %
% establish a treatment plan.  The optimization routine    %
% employs the greedy method with norm of the so-called     %
% current displacement as the greedy criterion.  The file  %
% is set to run the old greedy method for easy comparison. %
% All the 'cases' are 2-d slices of a patient tumor derived%
% from data used in a 3-d version of the older methods     %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all; clf; clc;
tic
%for pat = 3:2:11
cwg = 1; 
pat = 11; 
sd = 1;   % pat=patient slice, sd=seed profile(1=sm,2=big,3=aapm)
Dp = 145;  
f = 2;           % prescribed dose; seed strength factor
Tcov = 98;                  % target coverage percent
usegrid = 1;                % enforce treatment grid 1=y,0=n

%---- load data files ---%

switch (pat==pat)
    case (pat==3)
        load pat_case/patSl3 
    case pat==5
        load pat_case/patSl5 
    case pat==7
        load pat_case/patSl7 
    case pat==9
        load pat_case/patSl9  
    case pat==11
        load pat_case/patSl11, 
    otherwise   
        disp(['case doesn''t exist! pat=',num2str(pat)])
        return
end
if (0)
    greedy2dfct(Dp,Tcov,usegrid,f,sd,pat); % call greedy w/ relevant parameters
end
load seed_files/asDdose asDdose % asymmetric seed dose profile
asDdose = f*asDdose;
switch (sd==sd)
    case sd==1
        load seed_files/sDdosesm               % modified ("slimmer") seed dose profile
        load seed_files/sDcursm
        disp('using slim seed profile')
    case sd==2
        load seed_files/sDdose                 % contains seed dose profile
        load seed_files/sDcur
        disp('using broad seed profile')
    otherwise
        load seed_files/sDdoseAAPM             % central plane from AAPM seed profile
        load seed_files/sDcursm
        disp('using center plane AAPM profile')        
end

disp('TWO-DIMENSIONAL CD-BASED GREEDY HEURISTIC w/ DOSE-UPDATE')

%---- calculate tissue current ----%
tcur(:,1) = u(:,1);     % the x-coordinates
tcur(:,2) = u(:,2);     % the y-coordinates
mx = max(u(:,1));       % max x dimension [mm]
my = max(u(:,2));       % max y dimension [mm]
sx = max(seed(:,1));    % max x dim [mm] for seed
sy = max(seed(:,2));    % max y dim [mm] for seed
uc = abs(u); rc=abs(r); tc = abs(t); nc=abs(n);
rr = sqrt(mean((r(:,3).^2 + r(:,4).^2)));
uu = sqrt(mean((u(:,3).^2 + u(:,4).^2)));
nn = sqrt(mean((n(:,3).^2 + n(:,4).^2))); 
minrms = min([1/rr 1/uu 1/nn]);
lala = [1/rr 1/uu 1/nn]/minrms; 
wr=lala(1); wu=lala(2); wn=lala(3); wt = 0;
rv=rvol; uv=uvol; nv=nvol;
rvol = 1; uvol = 1; nvol = 1;
%----- current as defined for normed-cd as greedy criterion -----%

tcur(:,3) = 1*( (wr*rc(:,3)/rvol + wu*uc(:,3)/uvol + wn*nc(:,3)/nvol)+wt*tc(:,3)/tvol )  ;
tcur(:,4) = 1*( (wr*rc(:,4)/rvol + wu*uc(:,4)/uvol + wn*nc(:,4)/nvol)+wt*tc(:,4)/tvol )  ;
tcurX = zeros(my,mx); % initiate the 2-d grid of currents
tcurY = zeros(my,mx); % for both x and y direction
c = 0;
while c < mx*my
    c = c+1;
    tcurX(tcur(c,2),tcur(c,1)) = abs(tcur(c,3));
    tcurY(tcur(c,2),tcur(c,1)) = abs(tcur(c,4));
end

%----- current as needed for cd -----%

wr = 1; wn = 1; wu = 1; wt = 0;
tcurB(:,1) = ( (wr*r(:,3)/rv + wu*u(:,3)/uv + wn*n(:,3)/nv)) - (wt*t(:,3)/1 )  ;
tcurB(:,2) = ( (wr*r(:,4)/rv + wu*u(:,4)/uv + wn*n(:,4)/nv)) - (wt*t(:,4)/1 )  ;
tcurBX = zeros(my,mx); tcurBY = zeros(my,mx);
c = 0;
while c < mx*my
    c = c+1;
    tcurBX(tcur(c,2),tcur(c,1)) = (tcurB(c,1));
    tcurBY(tcur(c,2),tcur(c,1)) = (tcurB(c,2));
end
DX = tcurBX; DY = tcurBY; avgD = mean(mean(tcurBX.^2+tcurBY.^2));
%------ initialize 2d current for seed -----%

mxs = max(seed(:,1)); mys = max(seed(:,2));  
scurX = zeros(mys,mxs); scurY = zeros(mys,mxs); scur = zeros(mys,mxs);
c = 0; 
while c < mxs*mys
    c = c+1;
    scurX(seed(c,2),seed(c,1)) = abs(seed(c,3));
    scurY(seed(c,2),seed(c,1)) = abs(seed(c,4));
end
scur = sqrt(scurX.^2 + scurY.^2); % seed ||D||

%------ initialize dose, X, and seed2d ------%

dose = zeros(my,mx);        % current dose profile
Xa = zeros(my,mx);           % used seed placement
Xi = zeros(my,mx);
grid2d = zeros(my,mx);  

if (usegrid) 
    grid2d(1:5:end,1:5:end)=1;  
    seed2d = tGeo.*grid2d;  % treatment grid, 5mm sq intervals
else
    seed2d = tGeo;          % possible seed placements
end
sDdose = sDdose*f;          % seed dose profile

%---- find the coord's of minimum (within target region) ----%

% CD is the L2 norm of the latest current matrix
CD = (tcurX.^2+tcurY.^2).^(1/2) .* seed2d;    % seed2d marks possible loc's
[yy xx v] = find(CD);
CDlst = sortrows([yy xx v],3); 
curX = -tcurX; curY = -tcurY;

%----- set the first seed location ----%

ii = my+1; fi = my-1;
ij = mx+1; fj = mx-1;
Si = CDlst(1,1); Sj = CDlst(1,2);  %Si=25;Sj=25;
Ii= 76-Si;	Fi= Ii+fi; Ij= 81-Sj;	Fj= Ij+fj; 
if ( (DX(Si,Sj)^2+DY(Si,Sj)^2) == avgD*1.8)
 %   DX(Si,Sj) = -1; DY(Si,Sj) = 1;
 DX(Si,Sj), DY(Si,Sj)
    if (DX(Si,Sj) > 0)
        if (DY(Si,Sj) > 0)
            disp('quad 1')
            theta = atan(DX(Si,Sj)/DY(Si,Sj));
        else
            disp('quad 2')
            theta = pi+atan(DX(Si,Sj)/DY(Si,Sj));
        end
    else
        if (DY(Si,Sj) < 0)
            disp('quad 3')
            theta = -pi+atan(DX(Si,Sj)/DY(Si,Sj));
        else
            disp('quad 4')
            theta = atan(DX(Si,Sj)/DY(Si,Sj));
        end
    end
    disp(['theta = ',num2str(theta*180/pi)]);
    asd = rotate_image(theta,asDdose); 
    [aym axm] = size(asd); 
    ayi = ceil((aym-149)/2)+1; ayf = floor(aym-(aym-149)/2);
    asd = asd(ayi:ayf,ayi:ayf); 
    aIi = 76-Si; aFi = Ii+fi; aIj = 81-Sj; aFj = Ij+fj;
    Xa(Si,Sj) = 1; seed2d(Si,Sj) = 0; 
    dose(:,:) = dose(:,:) + asd(aIi:aFi,aIj:Fj);
else
    Xi(Si,Sj) = 1; seed2d(Si,Sj) = 0;
    dose(:,:) = dose(:,:) + sDdose(Ii:Fi,Ij:Fj);
end
figure(1),surf(dose)
%return
%---- print the solution space following first seed ----%
V100 = sum(sum((tGeo.*(dose>Dp))))/sum(nonzeros(tGeo));
a = 1; numS = sum(nonzeros(Xa+Xi));   % coef for dose-update

while (V100 < Tcov/100 && numS < 30)
    CD = (tcurX.^2+tcurY.^2).^(1/2) .* seed2d .* dose.^a/Dp^3; 
    DX = -tcurBX; DY = -tcurBY;
    [yy xx v] = find(CD);
    CDlst = sortrows([yy xx v],3); 
    %return
    Si = CDlst(1,1); Sj = CDlst(1,2);
    Ii= 76-Si;	Fi= Ii+fi; Ij= 81-Sj;	Fj= Ij+fj; 
    if ( (DX(Si,Sj)^2+DY(Si,Sj)^2) > 1.2*avgD )

        DX(Si,Sj), DY(Si,Sj)
        if (DX(Si,Sj) < 0)
            if (DY(Si,Sj) > 0)
                disp('quad 1')
                theta = atan(DX(Si,Sj)/DY(Si,Sj));
            else
                disp('quad 2')
                theta = pi+atan(DX(Si,Sj)/DY(Si,Sj));
            end
        else
            if (DY(Si,Sj) < 0)
                disp('quad 3')
                theta = -pi+atan(DX(Si,Sj)/DY(Si,Sj));
            else
                disp('quad 4')
                theta = atan(DX(Si,Sj)/DY(Si,Sj));
            end
        end
        disp(['theta = ',num2str(theta*180/pi)]);
        asd = rotate_image(theta,asDdose); 
        [aym axm] = size(asd); 
        ayi = ceil((aym-149)/2)+1; ayf = floor(aym-(aym-149)/2);
        asd = asd(ayi:ayf,ayi:ayf); 
        aIi = 76-Si; aFi = Ii+fi; aIj = 81-Sj; aFj = Ij+fj;
        Xa(Si,Sj) = 1; seed2d(Si,Sj) = 0; 
        dose(:,:) = dose(:,:) + asd(aIi:aFi,aIj:Fj);
    else
        Xi(Si,Sj) = 1; seed2d(Si,Sj) = 0;
        dose(:,:) = dose(:,:) + sDdose(Ii:Fi,Ij:Fj);
    end
    V100 = sum(sum((tGeo.*(dose>Dp))))/sum(nonzeros(tGeo));
    numS = sum(nonzeros(Xi+Xa));
end
doseB = dose;
[C,h] = contourf(dose,[145 145 360 360]);
h = findobj('Type','patch');
set(h,'LineWidth',2)
clabel(C,h)
title('Dose contour [Gy]')
xlabel('X [mm]')
ylabel('Y [mm]')

save solnCDdirgreedy dose seed2d Xa Xi

%---- general evaluation parameters and print dvh's ----%

disp('CURRENT-DISPLACEMENT-DRIVEN GREEDY')
X = Xa+Xi;
[cn dnr] = mkeval2d(dose,X,seed2d,pat,1);  % returns cn and dnr                                          
%dose = dosecalc(X,0,sd,pat,f);
[ta ur re no yt yu yr yn] = drawDVH(dose,pat);  
figure(4)
%return

if (cwg)
    X2 = X;             % save CD seed placement
    load solnCDgreedy;  % saved by GDY function run w/ same problem parameters
    X1 = X;             % set GDY seed placement
    disp('ADJOINT-RATIO-BASED GREEDY')
    dose = dosecalc(X,0,sd,pat,f);
    [cn2 dnr2] = mkeval2d(dose,X,seed2d,pat,0);
    [ta2 ur2 re2 no2 yt2 yu2 yr2 yn2] = drawDVH(dose,pat); 
end

set(plot(yt,ta,'--'),'Color','r','LineWidth',2)
hold on
axis([0 500 0 100])
set(plot(yu,ur,'--'),'Color',[1 0.7 0.2],'LineWidth',2);
set(plot(yr,re,'--'),'Color',[0.1 0.9 0.1],'LineWidth',2);
set(plot(yn,no,'--'),'Color',[0.3 0.3 0.9],'LineWidth',2);
if (cwg)
    set(plot(yt2,ta2,'-'),'Color','r','LineWidth',2);
    set(plot(yu2,ur2,'-'),'Color',[1 0.7 0.2],'LineWidth',2);
    set(plot(yr2,re2,'-'),'Color',[0.1 0.9 0.1],'LineWidth',2);
    set(plot(yn2,no2,'-'),'Color',[0.3 0.3 0.9],'LineWidth',2);
end
plot(Dp,0:1.5:100,'k.','linewidth',6);
plot(0:2:500,Tcov,'k--','linewidth',6);
plot(360,0:1.5:100,'k--','linewidth',1);
%title(['DVH :: CD(dnr=',num2str(dnr,2),' cn=',num2str(cn,2),') GDY(dnr=',num2str(dnr2,2),' cn=',num2str(cn2,2),')']);
title(['S',num2str(pat), ' with f = ',num2str(f)]);
xlabel('Dose [Gy]','fontsize',20);
ylabel('volume [%]','fontsize',20);
legend('Target','Urethra','Rectum','Normal');
hold off
    fig1 = ['CDgrid_pat',num2str(pat),'_f',num2str(f)];
    %saveas(figure(4),[fig1,'.eps'],'eps')
   % saveas(figure(4),[fig1,'.jpg'],'jpg')

%---- plot final seed locations ----%
if (cwg)
    [xxiso yyiso] = find(Xi); 
    [xxdir yydir]  = find(Xa);
    [xx1 yy1] = find(X1); % cd w/ dose update
    figure(5)
    colormap([1 1 1])
    contourf(tGeo+1*rGeo,[1 1 2 2])
    hold on
    plot(yy1,xx1,'kx',yyiso,xxiso,'ro',yydir,xxdir,'go')
    axis([0 mx 0 my])
    xlabel('x [mm]')
    ylabel('y [mm]')
    legend('CD-Only Iso','CD Mixed-Iso','CD Mixed-Dir')
    fig5 = ['CDgrid_pat',num2str(pat),'_f',num2str(f),'_pos'];
   % saveas(figure(5),[fig5,'.eps'],'eps')
   % saveas(figure(5),[fig5,'.jpg'],'jpg')
    hold off
    %end
end
toc


