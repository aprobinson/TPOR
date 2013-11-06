%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% showUS.m - Sep, 2000 %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% function showUS(filename,z_dim);

function showUS(filename,z_dim);

num= 640*480*z_dim;

fid= fopen(filename,'r');
F= fread(fid,[num 1],'char');
ROI= reshape(F,640,480,z_dim);
status= fclose(fid);

for fig= 1:3
   figure(fig),
   for pos= 1:4
      sc= 4*(fig-1)+pos;
      subplot(2,2,pos),imagesc(ROI(:,:,sc)');
      colormap(gray);
      txtsc= int2str(sc);
      sub= strcat(filename,' sc#',txtsc);
      title(sub);
   end      
end

if z_dim > 12   
   sc= z_dim;
   figure(4),
   imagesc(ROI(:,:,sc)');	   
   colormap(gray);
   txtsc= int2str(sc);   
   subplot(2,2,1),imagesc(ROI(:,:,sc)');
   sub= strcat(filename,' sc#',txtsc);
   title(sub);   
   txtsc= int2str(sc+1);
   subplot(2,2,2),imagesc(ROI(:,:,sc+1)');
   sub= strcat(filename,' sc#',txtsc);
   title(sub);
end
