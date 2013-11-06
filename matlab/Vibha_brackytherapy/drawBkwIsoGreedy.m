%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% drawBkwIsoGreedy.m - patient %%%%%
%%%%% (c)suayoo - May 2003 %%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all

load packSol_doseupdate;
%dose(i,j,k) & X(Si,Sj,Sk)
load mask3D;
load rankinganiso;

[II,JJ,KK]= size(dose);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% isodose lines %%%%%%%%%%%%%%%%%

Dp= 145;		D200= Dp*2; 		D150= Dp*1.50; 	
D100=Dp;   	D80= Dp*0.80;  D125= Dp*1.25;  	

for numFig= 1:KK
   figure(numFig), 
   % imagesc(-X(:,:,numFig)'),colormap(gray);      
   % contour is the plot of isolines of a data or function
   % cTa is contour data, hTa is the graphical handle
   
   [cTA,hTA]=contour(target(:,:,numFig)',1,'k:');		
   
   set(hTA,'linewidth',3);    % (obj, propertyname, value) set the linewidth of the target contour
   
   set(gca,'YDir','revers'); % gca:get current axes; command to reverse the order of increasing values on y-axis. Usually it increases from bottom to top or front to back.
   
   hold on;
   
   [cRE,hRE]=contour(rectum(:,:,numFig)',1,'k:');  set(hRE,'linewidth',3);   
   
   hold on;
   
   for Sj= 5:5:55
      for Si= 5:5:60
         if (X(Si,Sj,numFig) == 1)  % numFig is same as the number of slices originally.
            
            [m,n]=find(rankinganiso(:,2)==Si & rankinganiso(:,3)==Sj & rankinganiso(:,4)==numFig);
            matrixtype=rankinganiso(m,5);

 % command to place text(char,string) at specific location in a figure         
 % fontweight can be bold(b) or normal ,check colorSpec for color 'k' is black that corresponds to [0,0,0] RGB, rotation for rotating symbol                                                                                                                      
                
 
                   
   if matrixtype==1
      text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -30); 
      elseif (matrixtype==2)          
        text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -60);          
        elseif (matrixtype==3)       
         text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -90);    
            elseif (matrixtype==4)     
                text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -120); 
                  elseif (matrixtype==5)         
                      text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -150); 
                        elseif (matrixtype==6)          
                           text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -180); 
                              elseif (matrixtype==7)         
                                  text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -210); 
                                    elseif (matrixtype==8)          
                                        text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -240); 
                                          elseif (matrixtype==9)         
                                              text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -270); 
                                                elseif (matrixtype==10)          
                                                    text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -300); 
                                                      elseif (matrixtype==11)          
                                                          text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -330); 
                                                            elseif (matrixtype==12)          
                                                                text((Si-1),Sj,'D','fontsize',16,'fontweight','b','color','r','rotation', -360); 
                                                                 elseif (matrixtype==13)          
                                                                  text((Si-1),Sj,'X','fontsize',16,'fontweight','b','color','b');  % isoseed matrix
                                                                            
                                                            
            end
            
            hold on;
         end
      end
   end      
   clear ('cTA','hTA','cRE','hRE');
   
   [c200,h200]= contour(dose(:,:,numFig)',[D200 D200],'-');
   set(h200,'linewidth',1.3,'Color','m');
   % set(h200,'linewidth',2,'Color',[0 0 0]);	
   [c150,h150]= contour(dose(:,:,numFig)',[D150 D150],'-');
   set(h150,'linewidth',1.3,'Color','r');
   % set(h150,'linewidth',2,'Color',[1 0.7 0.2]);
   %set(h150,'linewidth',2,'Color',[0 0 0]);
   [c100,h100]= contour(dose(:,:,numFig)',[D100 D100],'-');
   set(h100,'linewidth',3,'Color','g');
   % set(h100,'linewidth',2,'Color',[0.1 0.9 0.1]);
   %set(h100,'linewidth',2,'Color',[0 0 0]);
   [c80,h80]= contour(dose(:,:,numFig)',[D80 D80],'-');
   set(h80,'linewidth',2,'Color','c');
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
