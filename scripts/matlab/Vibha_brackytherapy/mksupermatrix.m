
clear all;

load anisoseed;

s=1;
for angle = 30:30:360
    
dirdose = zeros(139,139,29); 
    
    anisoseedrotated = ['anisoseed', num2str(angle)];

    for k =1:29
       dirdose(:,:,k)=dirdose(:,:,k)+imrotate(anisoseed(:,:,k), angle, 'bilinear', 'crop'); 
    end
    
    supermatrix(:,:,s:s+28) = dirdose;
    s=s+29;
    
    save(anisoseedrotated, 'dirdose'); % directional dose kernel rotated by specified orientation is stored under the name anisoseedrotated30, anisoseedrotated60...
end

save supermatrix supermatrix;