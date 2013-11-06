

filename= 'baron.img';		KK= 13;

num= 640*480*KK; % dimensions from the header file...pixel size 0.1816mm
fid= fopen(filename,'r');%opens file for reading
F= fread(fid, [num 1], 'char');% reads binary data and stores into a matrix F
ROI= reshape(F,640,480,KK);% reurns 640*480*kk matrix ROI with elements taken column wise from F
status= fclose(fid);
clear ('F');

targetSc= zeros(640,480,KK); %defining matrices
urethraSc= zeros(640,480,KK);
rectumSc= zeros(640,480,KK);
marginSc= zeros(640,480,KK);
target= zeros(60,55,KK);
urethra=zeros(60,55,KK);
rectum= zeros(60,55,KK);
margin= zeros(60,55,KK);

  startX= 112;	startY= 127;
   numX= 330;		numY= 303;   
   tempTa(:,:)= ...
      targetSc(startX:startX+numX-1,startY:startY+numY-1,sc);
   tempUr(:,:)= ...
      urethraSc(startX:startX+numX-1,startY:startY+numY-1,sc);
   tempRe(:,:)= ...
      rectumSc(startX:startX+numX-1,startY:startY+numY-1,sc);
   tempMa(:,:)= ...
      marginSc(startX:startX+numX-1,startY:startY+numY-1,sc);
   
   imagesc(targetSc(:,:,5)); colormap('gray'); % i gave
