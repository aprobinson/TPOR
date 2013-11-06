function [ta,ur,re,no,yt,yu,yr,yn] = drawDVH(dose,pat)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% drawDVH1greedy.m - patientA %%%%%
%%%%% (c)suayoo - Mar, 2003 %%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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


[II,JJ]= size(tGeo);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% DVHs
dTA= dose.*tGeo; dUR= dose.*uGeo;
dRE= dose.*rGeo; dNO= dose.*nGeo;

% Target
xt= max(nonzeros(dTA));  yt= 0:xt/200:(xt+xt/200);
D1= nonzeros(dTA);		D2= hist(D1,yt);		D3= fliplr(D2);			
D4= cumsum(D3);			D5= fliplr(D4);
ta = 100*D5/max(D5);
%ta= plot(y,100*D5/max(D5),'-');
%axis([0 500 0 100]);
%hold on
%set(ta,'Color','r','LineWidth',2);

% Urethra
xu= max(nonzeros(dUR));  yu= 0:xu/200:(xu+xu/200);
D1= nonzeros(dUR);		D2= hist(D1,yu);		D3= fliplr(D2);
D4= cumsum(D3);			D5= fliplr(D4);
ur = 100*D5/max(D5);
%ur= plot(y,100*D5/max(D5),'-');
%set(ur,'Color','k','LineWidth',4);

% Rectum
xr= max(nonzeros(dRE));  yr= 0:xr/200:(xr+xr/200);
D1= nonzeros(dRE);		D2= hist(D1,yr);		D3= fliplr(D2);
D4= cumsum(D3);			D5= fliplr(D4);
re = 100*D5/max(D5);
%re= plot(y,100*D5/max(D5),'-');
%set(re,'Color','k','LineWidth',2);
%set(re,'Color',[0.1 0.9 0.1],'LineWidth',2);

% Normal
xn= max(nonzeros(dNO));  yn= 0:xn/200:(xn+xn/200);
D1= nonzeros(dNO);		D2= hist(D1,yn);			D3= fliplr(D2);
D4= cumsum(D3);			D5= fliplr(D4);
no = 100*D5/max(D5);
%no = plot(y,100*D5/max(D5),'-');
% set(nor,'Color','k','LineWidth',2);
%set(no,'Color',[0.3 0.3 0.9],'LineWidth',2);

return

