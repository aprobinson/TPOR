%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% mkMask.m - Sep, 2003 %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 

filename= 'baron.img';		KK= 13;

num= 640*480*KK; % dimensions from the header file...pixel size 0.1816mm
fid= fopen(filename,'r');%opens file for reading
F= fread(fid, [num 1], 'char');% reads binary data and stores into a matrix F
ROI= reshape(F,640,480,KK);% reurns 640*480*kk matrix ROI with elements taken column wise from F
status= fclose(fid);
clear ('F');

targetSc= zeros(640,480,KK); 
urethraSc= zeros(640,480,KK);
rectumSc= zeros(640,480,KK);
marginSc= zeros(640,480,KK);
target= zeros(66,61,KK);
urethra=zeros(66,61,KK);
rectum= zeros(66,61,KK);
margin= zeros(66,61,KK);


for sc= 1:KK	%for1
   figure(sc), imagesc(ROI(:,:,sc)');
   colormap(gray);	hold on;
   txtSc= int2str(sc);
   
   if (sc > 1)
      [c, h]= contour(targetSc(:,:,sc-1)',1,'w'); % puts the previous contours on the current slide so that...
      [c, h]= contour(marginSc(:,:,sc-1)',1,'w'); %... estimation of ROI to be contoured becomes easy for the current step
      [c, h]= contour(rectumSc(:,:,sc-1)',1,'w'); 			        
   end
   % contouring current slide
   fprintf('Slice#%d\n',sc); 
   fprintf('Contour targetSc#%d\n',sc); % 
   temp(:,:)= roipoly;	targetSc(:,:,sc)= double(temp(:,:)'); % selects polygon area of interest in the current image and assigns it to targetsc matrix
   [c, h]= contour(targetSc(:,:,sc)',1,'r'); % returns contour matrix C and handle h to the graphic obj
   fprintf('Contour urethraSc#%d\n',sc);
   temp(:,:)= roipoly;	urethraSc(:,:,sc)= double(temp(:,:)');   
   [c, h]= contour(urethraSc(:,:,sc)',1,'y');
   fprintf('Contour rectumSc#%d\n',sc);
   temp(:,:)= roipoly;	rectumSc(:,:,sc)= double(temp(:,:)');
   [c, h]= contour(rectumSc(:,:,sc)',1,'g');   
   fprintf('Contour marginSc#%d\n',sc);
   temp(:,:)= roipoly;	marginSc(:,:,sc)= double(temp(:,:)');
   clear ('temp');
   
   %margin
   temp= marginSc(:,:,sc) - targetSc(:,:,sc) - rectumSc(:,:,sc);
   marginSc(:,:,sc)= (temp==1);
   clear temp;
   
   %target
   temp= targetSc(:,:,sc) - urethraSc(:,:,sc);
   targetSc(:,:,sc)= temp;
   clear temp;
   
   %rectum
   temp= ((targetSc(:,:,sc) + rectumSc(:,:,sc))==2);
   targetSc(:,:,sc)= targetSc(:,:,sc) - temp;
   clear temp;   
   
   % field-of-view (FOV) 330x303xKK
   startX= 112;	startY= 127;
   numX= 330;		numY= 305;   
   tempTa(:,:)= ...
      targetSc(startX:startX+numX-1,startY:startY+numY-1,sc);
   tempUr(:,:)= ...
      urethraSc(startX:startX+numX-1,startY:startY+numY-1,sc);
   tempRe(:,:)= ...
      rectumSc(startX:startX+numX-1,startY:startY+numY-1,sc);
   tempMa(:,:)= ...
      marginSc(startX:startX+numX-1,startY:startY+numY-1,sc);
   
   imagesc(targetSc(:,:,5)); colormap('gray'); % i gave
   
   %final image
   Ij= 1;
   for y= 1:61	%for2
   Ii= 1;
   for x= 1:66		%for 2
      
      %count num of RIO
   	numTa= 0;	numUr= 0;	numRe= 0;	numMa= 0;
   
   %5x6
   if (fix(x/2)~=x/2 & fix(y/2)~=y/2)	%x:odd, y:odd
      %counts ROIs in 5x6 (30 pixels)
      Fi= Ii+4;	Fj= Ij+5;	  
      for j= Ij:Fj
      for i= Ii:Fi
      	if tempUr(i,j)~=0
         	numUr= numUr+1;
         elseif tempRe(i,j)~=0
            numRe= numRe+1;
         elseif tempTa(i,j)~=0
            numTa= numTa+1;
         elseif tempMa(i,j)~=0
            numMa= numMa+1;
         end
      end
      end      
      Ii= Fi+1;      
      %determine ROI      
      if numUr > 9
         urethra(x,y,sc)= (0==0);	target(x,y,sc)= (0~=0);
         rectum(x,y,sc)= (0~=0);		margin(x,y,sc)= (0~=0);
      elseif numRe > 12 
         rectum(x,y,sc)= (0==0);		urethra(x,y,sc)= (0~=0);
         target(x,y,sc)= (0~=0);		margin(x,y,sc)= (0~=0);
      elseif numMa > 12
         margin(x,y,sc)= (0==0);		urethra(x,y,sc)= (0~=0);
         target(x,y,sc)= (0~=0);		rectum(x,y,sc)= (0~=0);
      elseif numTa > 15
         target(x,y,sc)= (0==0);		margin(x,y,sc)= (0~=0);
         urethra(x,y,sc)= (0~=0);	rectum(x,y,sc)= (0~=0);         
      end
   %end of 5x6
   
   %6x6
	elseif (fix(x/2)==x/2 & fix(y/2)~=y/2)	%x:even, y:odd
   	%counts ROIs in 6x6 (36 pixels)
      Fi= Ii+5;	Fj= Ij+5;
      for j= Ij:Fj
      for i= Ii:Fi
      	if tempUr(i,j)~=0
         	numUr= numUr+1;
         elseif tempRe(i,j)~=0
            numRe= numRe+1;
         elseif tempTa(i,j)~=0
            numTa= numTa+1;
         elseif tempMa(i,j)~=0
            numMa= numMa+1;
         end
      end
      end      
      Ii= Fi+1;      
      %determine ROI      
      if numUr > 11
         urethra(x,y,sc)= (0==0);	target(x,y,sc)= (0~=0);
         rectum(x,y,sc)= (0~=0);		margin(x,y,sc)= (0~=0);
      elseif numRe > 15 
         rectum(x,y,sc)= (0==0);		urethra(x,y,sc)= (0~=0);
         target(x,y,sc)= (0~=0);		margin(x,y,sc)= (0~=0);
      elseif numMa > 15
         margin(x,y,sc)= (0==0);		urethra(x,y,sc)= (0~=0);
         target(x,y,sc)= (0~=0);		rectum(x,y,sc)= (0~=0);
      elseif numTa > 18
         target(x,y,sc)= (0==0);		margin(x,y,sc)= (0~=0);
         urethra(x,y,sc)= (0~=0);	rectum(x,y,sc)= (0~=0);         
      end
		%end of 6x6
   
   %5x5
	elseif (fix(x/2)~=x/2 & fix(y/2)==y/2)	%x:odd, y:even
   	%counts ROIs in 5x5 (25 pixels)
      Fi= Ii+4;	Fj= Ij+4;
      for j= Ij:Fj
      for i= Ii:Fi
      	if tempUr(i,j)~=0
         	numUr= numUr+1;
         elseif tempRe(i,j)~=0
            numRe= numRe+1;
         elseif tempTa(i,j)~=0
            numTa= numTa+1;
         elseif tempMa(i,j)~=0
            numMa= numMa+1;
         end
      end
      end      
      Ii= Fi+1;      
      %determine ROI      
      if numUr > 7
         urethra(x,y,sc)= (0==0);	target(x,y,sc)= (0~=0);
         rectum(x,y,sc)= (0~=0);		margin(x,y,sc)= (0~=0);
      elseif numRe > 10
         rectum(x,y,sc)= (0==0);		urethra(x,y,sc)= (0~=0);
         target(x,y,sc)= (0~=0);		margin(x,y,sc)= (0~=0);
      elseif numMa > 10
         margin(x,y,sc)= (0==0);		urethra(x,y,sc)= (0~=0);
         target(x,y,sc)= (0~=0);		rectum(x,y,sc)= (0~=0);
      elseif numTa > 12
         target(x,y,sc)= (0==0);		margin(x,y,sc)= (0~=0);
         urethra(x,y,sc)= (0~=0);	rectum(x,y,sc)= (0~=0);         
      end
   %end of 5x5
   
   %6x5
	elseif (fix(x/2)==x/2 & fix(y/2)==y/2)	%x:even, y:even
   	%counts ROIs in 6x5 (30 pixels)
      Fi= Ii+5;	Fj= Ij+4;
      for j= Ij:Fj
      for i= Ii:Fi
      	if tempUr(i,j)~=0
         	numUr= numUr+1;
   		elseif tempRe(i,j)~=0
            numRe= numRe+1;
         elseif tempTa(i,j)~=0
            numTa= numTa+1;
         elseif tempMa(i,j)~=0
            numMa= numMa+1;
         end
      end
      end      
      Ii= Fi+1;      
      %determine ROI      
      if numUr > 9
         urethra(x,y,sc)= (0==0);	target(x,y,sc)= (0~=0);
         rectum(x,y,sc)= (0~=0);		margin(x,y,sc)= (0~=0);
      elseif numRe > 12
         rectum(x,y,sc)= (0==0);		urethra(x,y,sc)= (0~=0);
         target(x,y,sc)= (0~=0);		margin(x,y,sc)= (0~=0);
      elseif numMa > 12
         margin(x,y,sc)= (0==0);		urethra(x,y,sc)= (0~=0);
         target(x,y,sc)= (0~=0);		rectum(x,y,sc)= (0~=0);
      elseif numTa > 15
         target(x,y,sc)= (0==0);		margin(x,y,sc)= (0~=0);
         urethra(x,y,sc)= (0~=0);	rectum(x,y,sc)= (0~=0);         
      end
   %end of 6x5      
	end	%end of if
   
end	%for3
Ij= Fj+1;
end	%for2
clear ('tempTa','tempUr','tempRe','tempMa');
hold off;	close;
end	%for1

save mask3D2 target urethra rectum margin

%save FOV ROI 
%saveName= ['tempROI',txtSc];
%save (saveName,'tempTa','tempUr','tempRe','tempMa');
%loadName= ['tempROI',txtSc];
%load (loadName);