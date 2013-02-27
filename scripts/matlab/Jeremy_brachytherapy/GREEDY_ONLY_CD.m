%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% TWO-DIMENSIONAL                                          %
%   GREEDY HEURISTIC + CURRENT DISPLACEMENT                %
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
pat = 5; 
sd = 1;   % pat=patient slice, sd=seed profile(1=sm,2=big,3=aapm)
Dp = 145;                   % prescribed dose; 
f = 2;                      % seed strength factor
Tcov = 98;                  % target coverage percent
usegrid = 0;                % enforce treatment grid 1=y,0=n

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
if (cwg)
    greedy2dfct(Dp,Tcov,usegrid,f,sd,pat); % call greedy w/ relevant parameters
end
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
u = abs(u); r=abs(r); t = abs(t); n=abs(n);
%wr = rvol; wu = 1*uvol; wn = nvol; wt = tvol; %wr = 1; wu = 5; wn = 1.0; wt = 5;
%wr = 5000; wu = 1450; wn = 13000.0; wt = -0; %wr = 1; wu = 5; wn = 1.0; wt = 5; 5000,1450,13000
%wr = 7.5; wu = 300; wn = 0.5; wt = -0;
%wr = 3.5; wu = 1; wn = 9; wt = -0;
%wr = 1; wu = 1; wn = 0.2; wt = -0;
rr = sqrt(mean((r(:,3).^2 + r(:,4).^2)));
uu = sqrt(mean((u(:,3).^2 + u(:,4).^2)));
nn = sqrt(mean((n(:,3).^2 + n(:,4).^2))); 
%  rr = (mean((r(:,3).^2 + r(:,4).^2)));
%  uu = (mean((u(:,3).^2 + u(:,4).^2)));
%  nn = (mean((n(:,3).^2 + n(:,4).^2)));
minrms = min([1/rr 1/uu 1/nn]);
lala = [1/rr 1/uu 1/nn]/minrms; 
wr=lala(1); wu=lala(2); wn=lala(3); wt = 0;
%wr = 1/2; wu = 1; wn = 1/6; wt = -0;
rvol = 1; uvol = 1; nvol = 1;
%%% current[ xx yy xc yc ] ---------------- a 'ratio' can be defined
tcur(:,3) = 1*( (wr*r(:,3)/rvol + wu*u(:,3)/uvol + wn*n(:,3)/nvol)+wt*t(:,3)/tvol )  ;
tcur(:,4) = 1*( (wr*r(:,4)/rvol + wu*u(:,4)/uvol + wn*n(:,4)/nvol)+wt*t(:,4)/tvol )  ;
%tcur(:,3) = ( (wr*r(:,3)/rvol + wu*u(:,3)/uvol + wn*n(:,3)/nvol)) .* (abs(wt*t(:,3) )).^1  ;
%tcur(:,4) = ( (wr*r(:,4)/rvol + wu*u(:,4)/uvol + wn*n(:,4)/nvol)) .* (abs(wt*t(:,4) )).^1  ;

tcurX = zeros(my,mx); % initiate the 2-d grid of currents
tcurY = zeros(my,mx); % for both x and y direction

c = 0;
while c < mx*my
    c = c+1;
    tcurX(tcur(c,2),tcur(c,1)) = abs(tcur(c,3));
    tcurY(tcur(c,2),tcur(c,1)) = abs(tcur(c,4));
end

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
X = zeros(my,mx);           % used seed placement
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
curX = tcurX; curY = tcurY;

%----- set the first seed location ----%

Si = CDlst(1,1); Sj = CDlst(1,2);  
X(Si,Sj) = 1; seed2d(Si,Sj) = 0;

%---- print the solution space following first seed ----%

dose = dosecalc(X,0,sd,pat,f);
V100 = sum(sum((tGeo.*(dose>Dp))))/sum(nonzeros(tGeo));
a = 1;    % coef for dose-update

%---- cycle through for rest of seeds ----%

while (V100 < Tcov/100 && sum(nonzeros(X)) < 30)
    CD = (tcurX.^2+tcurY.^2).^(1/2) .* seed2d .* dose.^a/Dp^3; 
    %figure(1), contourf(CD/sum(sum(CD)),10), figure(2), contourf(dose,[145 145 360 360]), pause(.3)
    [yy xx v] = find(CD);
    CDlst = sortrows([yy xx v],3); 
    Si = CDlst(1,1); Sj = CDlst(1,2);
    X(Si,Sj) = 1; seed2d(Si,Sj) = 0;
    figure(3),contour(X)
    dose = dosecalc(X,0,sd,pat,f);
    V100 = sum(sum((tGeo.*(dose>Dp))))/sum(nonzeros(tGeo));
end
dose = dosecalc(X,0,sd,pat,f);
CD = (curX.^2+curY.^2).^(1/2) .* seed2d .* dose.^a/Dp^3;    % seed2d marks possible loc's
%figure(1), contourf(CD/sum(sum(CD)),10), figure(2), contourf(dose,[145 145 360 360]), pause(1)

[C,h] = contourf(dose,[145 145 360 360]);
h = findobj('Type','patch');
set(h,'LineWidth',2)
clabel(C,h)
title('Dose contour [Gy]')
xlabel('X [mm]')
ylabel('Y [mm]')

save solnCDgreedy dose seed2d X

%---- general evaluation parameters and print dvh's ----%

disp('CURRENT-DISPLACEMENT-DRIVEN GREEDY')
[cn dnr] = mkeval2d(dose,X,seed2d,pat,1);  % returns cn and dnr                                          
dose = dosecalc(X,0,sd,pat,f);
[ta ur re no yt yu yr yn] = drawDVH(dose,pat);  
figure(4)
%return

if (cwg)
    X2 = X;             % save CD seed placement
    load soln2dGreedy;  % saved by GDY function run w/ same problem parameters
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
    [xx1 yy1] = find(X1); [xx2 yy2] = find(X2);
    figure(5)
    colormap([1 1 1])
    contourf(tGeo+1*rGeo,[1 1 2 2])
    hold on
    plot(yy1,xx1,'x',yy2,xx2,'o')
    axis([0 mx 0 my])
    xlabel('x [mm]')
    ylabel('y [mm]')
    legend('Greedy','Current-displacement')
    fig5 = ['CDgrid_pat',num2str(pat),'_f',num2str(f),'_pos'];
   % saveas(figure(5),[fig5,'.eps'],'eps')
   % saveas(figure(5),[fig5,'.jpg'],'jpg')
    hold off
    %end
end
toc


