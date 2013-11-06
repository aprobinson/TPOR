%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% drawBkwIsoGreedy.m - patient %%%%%
%%%%% (c)suayoo - May 2003 %%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all

load packSol;
%dose(i,j,k) & X(Si,Sj,Sk)
load mask3D;

[II,JJ,KK]= size(dose);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% isodose lines 
Dp= 145;		D200= Dp*2; 		D150= Dp*1.50; 	
D100=Dp;   	D80= Dp*0.80;  D125= Dp*1.25;  	

for numFig= 1:KK
   figure(numFig), 
   % imagesc(-X(:,:,numFig)'),colormap(gray);      
   [cTA,hTA]=contour(target(:,:,numFig)',1,'k:');		set(hTA,'linewidth',3);   
   set(gca,'YDir','revers');
   hold on;
   [cRE,hRE]=contour(rectum(:,:,numFig)',1,'k:');  set(hRE,'linewidth',3);   
   hold on;
   for Sj= 5:5:55
      for Si= 5:5:60
         if (X(Si,Sj,numFig) == 1)
            text((Si-1),Sj,'X','fontsize',16,'fontweight','b','color','k');
            hold on;
         end
      end
   end      
   clear ('cTA','hTA','cRE','hRE');
   
   [c200,h200]= contour(dose(:,:,numFig)',[D200 D200],'-');
   set(h200,'linewidth',1,'Color','k');
   % set(h200,'linewidth',2,'Color',[0 0 0]);	
   [c150,h150]= contour(dose(:,:,numFig)',[D150 D150],'-');
   set(h150,'linewidth',1,'Color','k');
   % set(h150,'linewidth',2,'Color',[1 0.7 0.2]);
   %set(h150,'linewidth',2,'Color',[0 0 0]);
   [c100,h100]= contour(dose(:,:,numFig)',[D100 D100],'-');
   set(h100,'linewidth',3,'Color','k');
   % set(h100,'linewidth',2,'Color',[0.1 0.9 0.1]);
   %set(h100,'linewidth',2,'Color',[0 0 0]);
   [c80,h80]= contour(dose(:,:,numFig)',[D80 D80],'-');
   set(h80,'linewidth',1,'Color','k');
   % set(h80,'linewidth',2,'Color',[0.3 0.3 0.9]);   
   %set(h80,'linewidth',2,'Color',[0 0 0]);   
   numFigTxt= num2str(numFig);
   T= title(['Isodose sc=',numFigTxt]);
   %set(gca,'fontsize',16);
   %set(T,'Color',[.5 .5 .5]);
   %set(gca,'XColor',[.5 .5 .5],'YColor',[.5 .5 .5]);
   grid on;		hold off;

   clear ('c80','h80','c100','h100','c150','h150','c200','h200');   
end
