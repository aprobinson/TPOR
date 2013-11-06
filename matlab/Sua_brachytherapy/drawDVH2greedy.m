%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% drawDVH2greedy.m - patient %%%%%
%%%%% (c)suayoo - Mar, 2003 %%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%draw black & white DVH for the greedy result

clear all;

load packSol;
load mask3D;
[II,JJ,KK]= size(target);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% DVHs
dTA= dose.*target;
dUR= dose.*urethra;
dRE= dose.*rectum;
dNO= dose.*(~target & ~urethra & ~rectum);

% Target
x= max(nonzeros(dTA));
y= 0:x/200:(x+x/200);
D1= nonzeros(dTA);		D2= hist(D1,y);		D3= fliplr(D2);			
D4= cumsum(D3);			D5= fliplr(D4);
figNum= input('type the figure number: ');
figure(figNum);
ta= plot(y,100*D5/max(D5),'-');
set(ta,'Color','k','LineWidth',2);
axis([0 300 0 100]);
hold on
clear ('x','y','D1','D2','D3','D4','D5','ta');

% Urethra
x= max(nonzeros(dUR));
y= 0:x/200:(x+x/200);
D1= nonzeros(dUR);		D2= hist(D1,y);		D3= fliplr(D2);
D4= cumsum(D3);			D5= fliplr(D4);
ur= plot(y,100*D5/max(D5),'--');
set(ur,'Color','k','LineWidth',3);
%set(ur,'Color','k','LineWidth',4);
%set(ur,'Color',[1 0.7 0.2],'LineWidth',2);
clear ('x','y','D1','D2','D3','D4','D5','ur');

% Rectum
x= max(nonzeros(dRE));
y= 0:x/200:(x+x/200);
D1= nonzeros(dRE);		D2= hist(D1,y);		D3= fliplr(D2);
D4= cumsum(D3);			D5= fliplr(D4);
%re= plot(y,100*D5/max(D5),'--');
%set(re,'Color','k','LineWidth',2);
%set(re,'Color',[0.1 0.9 0.1],'LineWidth',2);
re= plot(y,100*D5/max(D5),':');
set(re,'Color','k','LineWidth',4);
clear ('x','y','D1','D2','D3','D4','D5','re');

% Normal
x= max(nonzeros(dNO));
y= 0:x/200:(x+x/200);
D1= nonzeros(dNO);		D2= hist(D1,y);			D3= fliplr(D2);
D4= cumsum(D3);			D5= fliplr(D4);
%nor= plot(y,100*D5/max(D5),'--');
% set(nor,'Color','k','LineWidth',2);
%set(nor,'Color',[0.3 0.3 0.9],'LineWidth',2);
nor= plot(y,100*D5/max(D5),'-.');
set(nor,'Color','k','LineWidth',3);


clear ('x','y','D1','D2','D3','D4','D5','nor');


Dp= 145;
V= [0:1.5:100];
plot(Dp,V,'k.','linewidth',6);

T= title(['DVH']);
xlabel('Dose [Gy]','fontsize',20);
ylabel('volume [%]','fontsize',20);
L= legend('Target','Urethra','Rectum','Normal');
%set(L,'Color',[.5 .5 .5]);
%set(T,'Color',[.2 .2 .2]);
set(T,'FontSize',20,'FontWeight','b');
%set(gca,'XColor',[.2 .2 .2],'YColor',[.2 .2 .2]);
set(gca,'FontSize',16,'FontWeight','b');
% grid on
clear ('dTA','dUR','dRE','dNO','sub');
hold off
