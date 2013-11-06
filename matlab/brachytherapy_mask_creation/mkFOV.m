%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% mkFOV.m - Nov, 2001 %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% read ROI.mat - 640x480x12
% get tumor, urethra, rectum
% resize the ROIs for field-of-view - 60x55x12

clear all

Pname= input('patient name: ','s');
numSc= input('number of slice: ');

matName= [Pname,'ROI'];

load (matName);

num= 640*480*numSc;

startX= 112;
startY= 127;


for slice= 1:numSc
   
   for j= 1:303		% 5.5cm= 303pixels
      for i= 1:330		% 6cm= 330pixels
         tempTu(i,j,slice)= ...
            tumor(startX-1+i,startY-1+j,slice);         
         tempUr(i,j,slice)= ...
            urethra(startX-1+i,startY-1+j,slice);            
         tempRe(i,j,slice)= ...
            rectum(startX-1+i,startY-1+j,slice);               
      end      
   end
      
end

clear ('tumor','urethra','rectum');
% tempTu= 330x303x13
% tempUr= 330x303x13
% tempRe= 330x303x13

FOVtu= zeros(60,55,numSc);
FOVur= zeros(60,55,numSc);
FOVre= zeros(60,55,numSc);

for sc= 1:numSc
Ij= 1;
for y= 1:55		
Ii= 1;	
for x= 1:60
   
   % count num of ROI
   numTu= 0; numUr= 0; numRe= 0;  
   
   if (fix(x/2)~=x/2 & fix(y/2)~=y/2 & y~=55)	% x:odd & y:odd 
      
      % counts ROIs in 5x5= 25 pixels
      Fj= Ij+4; Fi= Ii+4;
      for j= Ij:Fj
      for i= Ii:Fi         
         if tempUr(i,j,sc)~= 0 
            numUr= numUr+1;
         elseif tempRe(i,j,sc)~= 0
            numRe= numRe+1;
         elseif tempTu(i,j,sc)~= 0
            numTu= numTu+1;
         end         
      end		% EO 'for j= Ij:Fj'
      end 	% EO 'for i= Ii:Fi'
      Ii= Fi+1;
      
      % determine ROI --> modify it
      if numUr > 7
         FOVur(x,y,sc)= (0==0);		% logical value: true
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      elseif numRe > 10         
         FOVre(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
      elseif numTu >12
         FOVtu(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      end
      
   elseif (fix(x/2)~=x/2 & fix(y/2)==y/2 & y~=55)		% x:odd & y:even 
      
      % counts ROIs in 5x6= 30 pixels
      Fj= Ij+5; Fi= Ii+4;
      for j= Ij:Fj
      for i= Ii:Fi         
         if tempUr(i,j,sc)~= 0 
            numUr= numUr+1;
         elseif tempRe(i,j,sc)~= 0
            numRe= numRe+1;
         elseif tempTu(i,j,sc)~= 0
            numTu= numTu+1;
         end         
      end		% EO 'for j= Ij:Fj'
      end 	% EO 'for i= Ii:Fi'
      Ii= Fi+1;
      
      % determine ROI --> modify it
      if numUr > 9
         FOVur(x,y,sc)= (0==0);		% logical value: true
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      elseif numRe > 12
         FOVre(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
      elseif numTu >15
         FOVtu(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      end
      
   elseif (fix(x/2)==x/2 & fix(y/2)~=y/2 & y~=55)		% x:even & y:odd
      
      % counts ROIs in 6x5= 30 pixels
      Fj= Ij+4; Fi= Ii+5;
      for j= Ij:Fj
      for i= Ii:Fi         
         if tempUr(i,j,sc)~= 0 
            numUr= numUr+1;
         elseif tempRe(i,j,sc)~= 0
            numRe= numRe+1;
         elseif tempTu(i,j,sc)~= 0
            numTu= numTu+1;
         end         
      end		% EO 'for j= Ij:Fj'
      end 	% EO 'for i= Ii:Fi'
      Ii= Fi+1;
      
      % determine ROI --> modify it
      if numUr > 9
         FOVur(x,y,sc)= (0==0);		% logical value: true
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      elseif numRe > 12
         FOVre(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
      elseif numTu >15
         FOVtu(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      end     
     
   elseif (fix(x/2)==x/2 & fix(y/2)==y/2 & y~=55)		% x=even & y=even
      
      % counts ROIs in 6x6= 36 pixels
      Fj= Ij+5; Fi= Ii+5;
      for j= Ij:Fj
      for i= Ii:Fi         
         if tempUr(i,j,sc)~= 0 
            numUr= numUr+1;
         elseif tempRe(i,j,sc)~= 0
            numRe= numRe+1;
         elseif tempTu(i,j,sc)~= 0
            numTu= numTu+1;
         end         
      end		% EO 'for j= Ij:Fj'
      end 	% EO 'for i= Ii:Fi'
      Ii= Fi+1;
      
      % determine ROI --> modify it
      if numUr > 11
         FOVur(x,y,sc)= (0==0);		% logical value: true
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      elseif numRe > 14
         FOVre(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
      elseif numTu >18
         FOVtu(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      end     
      
   elseif (y==55 & fix(x/2)~=x/2)		% x:odd & y==55
      
      % counts ROIs in 5x6= 30 pixels
      Fj= Ij+5; Fi= Ii+4;
      for j= Ij:Fj
      for i= Ii:Fi         
         if tempUr(i,j,sc)~= 0 
            numUr= numUr+1;
         elseif tempRe(i,j,sc)~= 0
            numRe= numRe+1;
         elseif tempTu(i,j,sc)~= 0
            numTu= numTu+1;
         end         
      end		% EO 'for j= Ij:Fj'
      end 	% EO 'for i= Ii:Fi'
      Ii= Fi+1;
      
      % determine ROI --> modify it
      if numUr > 9
         FOVur(x,y,sc)= (0==0);		% logical value: true
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      elseif numRe > 12
         FOVre(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
      elseif numTu >15
         FOVtu(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      end
      
   elseif (y==55 & fix(x/2)==x/2)		% x:even & y==55      
      
      % counts ROIs in 6x6= 36 pixels
      Fj= Ij+5; Fi= Ii+5;
      for j= Ij:Fj
      for i= Ii:Fi         
         if tempUr(i,j,sc)~= 0 
            numUr= numUr+1;
         elseif tempRe(i,j,sc)~= 0
            numRe= numRe+1;
         elseif tempTu(i,j,sc)~= 0
            numTu= numTu+1;
         end         
      end		% EO 'for j= Ij:Fj'
      end 	% EO 'for i= Ii:Fi'
      Ii= Fi+1;
      
      % determine ROI --> modify it
      if numUr > 11
         FOVur(x,y,sc)= (0==0);		% logical value: true
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      elseif numRe > 14
         FOVre(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVtu(x,y,sc)= (0~=0);		% logical value: false
      elseif numTu >18
         FOVtu(x,y,sc)= (0==0);		% logical value: true
         FOVur(x,y,sc)= (0~=0);		% logical value: false
         FOVre(x,y,sc)= (0~=0);		% logical value: false
      end     
      
   end             
   
end		% EO 'for x= 1:60'
Ij= Fj+1;

   
end		% EO 'for y= 1:55'

end		% EO 'for sc= 1:numSc'