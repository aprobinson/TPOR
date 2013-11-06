%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% mkMask.m - Nov, 2001 %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% read filenameSc#.tif file - 480x640x3
% make maskfile...

clear all

num=480*640*3;

Pname= input('patient name: ','s');
numSc= input('number of slices: ');
mask= zeros(640,480,numSc);

for slice= 1:numSc
   txtSc= num2str(slice);
   filename= [Pname,'Sc',txtSc,'.tif']; 
   fprintf('%s\n',filename);   
  
   tifImg= double(imread(filename,'tif'));
   % 480x640x3
   % 1)red: prostate
   % 2)green: urethra
   % 3)blue: rectum
   
   for x= 1:640
      for y= 1:480            
         
         if (tifImg(y,x,1)~=0)	% tumor= 1
            mask(x,y,slice)= 1;
         elseif (tifImg(y,x,2)~=0) 	% urethra= 2
            mask(x,y,slice)= 2;               
         elseif (tifImg(y,x,3)~=0)	% rectum= 3
            mask(x,y,slice)= 3;
         end        
                  
      end		% end of x 640
   end   		% end of y 480
   
   clear ('tifImg');
end			% end of slice
