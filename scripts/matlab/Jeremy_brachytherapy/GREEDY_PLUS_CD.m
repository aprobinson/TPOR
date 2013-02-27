%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% TWO-DIMENSIONAL                                          %
%   GREEDY HEURISTIC with DOSE-UPDATE                      %
%      + CURRENT-DISPLACEMENT on all seeds (ie 'nudge a')  %
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
%for f = 1:6
%    for pat = 3:2:11
cwg = 1; 
pat = 5; 
sd = 1;   % pat=patient slice, sd=seed profile(1=sm,2=big,3=aapm)
Dp = 145;                   % prescribed dose
f = 1;                      % seed strength factor
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

% ---- now the greedy will be called; X, seed2d must be saved (they are) and
% ---- loaded as needed.

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
disp('PREVIOUS RESULTS modified via CURRENT DISPLACEMENT')

%---- calculate tissue current ----%

tcur(:,1) = u(:,1);     % the x-coordinates
tcur(:,2) = u(:,2);     % the y-coordinates
mx = max(u(:,1));       % max x dimension [mm]
my = max(u(:,2));       % max y dimension [mm]
sx = floor(max(seed(:,1))/2)+1; 
sy = floor(max(seed(:,2))/2)+1; 

%--starting over with weights set to unity
wr = 1; wn = 1; wu = 1; wt = 1;
%%% current[ xx yy xc yc ] ---------------- a 'ratio' can be defined
tcur(:,3) = ( (wr*r(:,3)/rvol + wu*u(:,3)/uvol + wn*n(:,3)/nvol)) - (wt*t(:,3)/tvol )  ;
tcur(:,4) = ( (wr*r(:,4)/rvol + wu*u(:,4)/uvol + wn*n(:,4)/nvol)) - (wt*t(:,4)/tvol )  ;
%tcur(:,3) = ( (wr*r(:,3)/rvol + wu*u(:,3)/uvol + wn*n(:,3)/nvol)) ./ (-(wt*t(:,3) )).^1  ;
%tcur(:,4) = ( (wr*r(:,4)/rvol + wu*u(:,4)/uvol + wn*n(:,4)/nvol)) ./ (-(wt*t(:,4) )).^1  ;

tcurX = zeros(my,mx); % initiate the 2-d grid of currents
tcurY = zeros(my,mx); % for both x and y direction

c = 0;
while c < mx*my
    c = c+1;
    tcurX(tcur(c,2),tcur(c,1)) = (tcur(c,3));
    tcurY(tcur(c,2),tcur(c,1)) = (tcur(c,4));
end

tcmx = max(max(sqrt( tcurX.^2 + tcurY.^2 )));
%    figure(1),mesh(tcurX),figure(2),mesh(tcurY),return
%------ set 2d current for a single seed -----%

mxs = max(seed(:,1)); mys = max(seed(:,2));  
scurX = zeros(mys,mxs); scurY = zeros(mys,mxs);
c = 0; 
while c < mxs*mys
    c = c+1;
    scurX(seed(c,2),seed(c,1)) = -seed(c,3); % neg because cur utility for adjoint
    scurY(seed(c,2),seed(c,1)) = -seed(c,4);
end
sdmx = max(max(sqrt( scurX.^2 + scurY.^2 )));

%figure(1),contourf(scurX(25:end-25,25:end-25)),figure(2),contourf(scurY(25:end-25,25:end-25)),return
%------ load dose, X, and seed2d ------%

load soln2dGreedy dose X seed2d

%----calculate current displacement -----%

[yy xx] = find(X);  % locations of seeds with current-displacement
numS = length(xx);
sdloc = [yy xx zeros(numS,1) zeros(numS,1) zeros(numS,1)];

%---- perform displacement ----%
it = 0; Di = 0; 
%b = 1/sqrt( sdmx^2 + (1/tcmx)^2 );
%sdmx = 1;  % setting these to unity allows for investigation
m1 = mean(mean(scurX.^2+ scurY.^2));
m2 = mean(mean(tcurX.^2+ tcurY.^2));
a  = sqrt(1/(sqrt(m2/m1)*m1));
%a = 25;
cof = 1;
%tcmx = 1; cof = 0;  % of just D due to seeds or tissue
b = tcmx/sdmx/a;   % otherwise a = 25
k = a/(tcmx);

while (it < 3)
    it = it + 1;
    DX =  -cof*tcurX; DY =  -cof*tcurY;   % DX = -J^tot_x and DY = -J^tot_y 
    % calc total current-displacement map
    %disp('seedloc'),sdloc
    for ss = 1:numS
        fi = my-1; fj = mx-1;
        Si = sdloc(ss,1); Ii= sy-Si; Fi= Ii+my-1;
        Sj = sdloc(ss,2); Ij= sx-Sj; Fj= Ij+mx-1; 
        DX = (DX + 1*f*b*scurX(Ii:Fi,Ij:Fj)); % DX = DX + b*J^s_i->j,x
        DY = (DY + 1*f*b*scurY(Ii:Fi,Ij:Fj)); % DY = DY + b*J^s_i->j,y
    end
    %figure(8), contourf( sqrt( DX.^2 + DY.^2 ), 10 ),pause(1)
    %figure(8),contourf(DX,15),figure(9),contourf(DY,15),figure(10),contourf(sqrt(DY.^2+DX.^2),15)
    % calc displacement for each seed
    for ss = 1:numS
        Si = sdloc(ss,1); Ii= sy-Si; Fi= Ii+my-1;
        Sj = sdloc(ss,2); Ij= sx-Sj; Fj= Ij+mx-1; 
        sdloc(ss,3) = DY(Si,Sj)-scurX(80,75)*b;    % Dx
        sdloc(ss,4) = DX(Si,Sj)-scurY(80,75)*b;    % Dy
        sdloc(ss,5) = sqrt(sdloc(ss,3)^2 + sdloc(ss,4)^2); % |D|_2
    end
    
    % sort coordinates by displacement magnitude
    sdloc = sortrows(sdloc,5); sdloc = sdloc(end:-1:1,:);
    %sdloc(1:end,3:5)*k
    Df = sum(sdloc(1:end,5));
    %disp(['total displacement = ',num2str(sum(sdloc(:,5))),' it = ',num2str(it)])
    if (Di) disp(['ratio of Df/Di = ',num2str(Df/Di)]); end
    Di = Df;
    
    %--apply displacements--%
    num = numS; %number of seeds to be displaced during each iteration
    for n = 1:num
        if( tGeo(sdloc(n,1) + round(k*sdloc(n,3)/it),sdloc(n,2) + round(k*sdloc(n,4)/it)) )
            sdloc(n,1) = sdloc(n,1) + sign(sdloc(n,3))*round(k*abs(sdloc(n,3))/it); %k*sd(:,3)/it
            sdloc(n,2) = sdloc(n,2) + sign(sdloc(n,4))*round(k*abs(sdloc(n,4))/it); %k*sd(:,4)/it
        end
    end
    %return
end
X = zeros(my,mx);
for it = 1:length(sdloc(:,1))
    X(sdloc(it,1),sdloc(it,2))=1;
end
    
dose = dosecalc(X,0,sd,pat,f);

%---- pretty dose plot -----%
figure(1), [C,h] = contourf(dose,[145 145 360 360]);
h = findobj('Type','patch');
set(h,'LineWidth',2)
clabel(C,h)
title('Dose contour [Gy]'),xlabel('X [mm]'),ylabel('Y [mm]')

save solnNudgeA dose seed2d X

%---- general evaluation parameters and print dvh's ----%

disp('CURRENT-DISPLACEMENT-DRIVEN GREEDY')
[cn dnr] = mkeval2d(dose,X,seed2d,pat,1);  % returns cn and dnr                                          
dose = dosecalc(X,0,sd,pat,f);
[ta ur re no yt yu yr yn] = drawDVH(dose,pat);  
%figure(4)
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
    fig1 = ['GYwCDpat',num2str(pat),'_f',num2str(f)];
    saveas(figure(1),[fig1,'.eps'],'eps')
    saveas(figure(1),[fig1,'.jpg'],'jpg')
hold off

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
    fig5 = ['GYwCDpat',num2str(pat),'_f',num2str(f),'_pos'];
    %saveas(figure(5),[fig5,'.eps'],'eps')
    %saveas(figure(5),[fig5,'.jpg'],'jpg')
    hold off
end

%end
%end
toc


