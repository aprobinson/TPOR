
% program that makes supermatrix containing:
% all directional seed orientations
% creates and stores the ROI adjoint flux data for all orientations
% creates and stores ratio matrices for all oprientations


clear all;

load mask3D; % target urethra rectum normal

load anisoseed;

%tic

numUr = sum(nonzeros(urethra));
numTa = sum(nonzeros(target));
numRe = sum(nonzeros(rectum));
numMa = sum(nonzeros(margin));
numNo = sum(nonzeros(normal));

for angle = 30:30:360
    
dirdose = zeros(139,139,29); 
adflTa = zeros(60,55,11);
adflUr = zeros(60,55,11);
adflRe = zeros(60,55,11);
adflMa = zeros(60,55,11);
adflNo = zeros(60,55,11);
    
    anisoseedrotated = ['anisoseed', num2str(angle)];
    
    adflROIangle =['adflROI', num2str(angle)];
    
    ratioangle = ['ratio',num2str(angle)];
    
    rankingangle= ['ranking',num2str(angle)];
    
    for k =1:29
       dirdose(:,:,k)=dirdose(:,:,k)+imrotate(anisoseed(:,:,k), angle, 'bilinear', 'crop'); 
    end
    
    save(anisoseedrotated, 'dirdose'); % directional dose kernel rotated by specified orientation is stored under the name anisoseedrotated30, anisoseedrotated60...
        
for k= 1:11
Ik= 16-k;		Fk= Ik+11-1;   
for j= 1:55
Ij= 71-j;		Fj= Ij+54;   
for i= 1:60
Ii= 71-i;		Fi= Ii+59;  

doseUr = sum(sum(sum(urethra.*dirdose(Ii:Fi, Ij:Fj, Ik:Fk))));

adflUr(i,j,k) = doseUr;

doseTa = sum(sum(sum(target.*dirdose(Ii:Fi, Ij:Fj, Ik:Fk))));

adflTa(i,j,k) = doseTa;

doseRe = sum(sum(sum(rectum.*dirdose(Ii:Fi, Ij:Fj, Ik:Fk))));

adflRe(i,j,k) = doseRe;

doseMa = sum(sum(sum(margin.*dirdose(Ii:Fi, Ij:Fj, Ik:Fk))));

adflMa(i,j,k) = doseMa;

doseNo = sum(sum(sum(normal.*dirdose(Ii:Fi, Ij:Fj, Ik:Fk))));

adflNo(i,j,k) = doseNo;

end 
end
end

adflUr = adflUr/numUr; 
adflTa = adflTa/numTa;
adflRe = adflRe/numRe; 
adflMa = adflMa/numMa; 
adflNo = adflNo/numNo;

 save (adflROIangle,'adflUr', 'adflTa','adflRe','adflMa','adflNo');
 
 tic
 load(adflROIangle,'adflUr', 'adflTa','adflRe','adflMa','adflNo');
 %%%%%%%%%%%%%% make ratio %%%%%%%%%%%%%%%
 
 
Wur=1;Wre=1;Wta=1;Wmacs=1;Wmata=0;

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);

totS= sum(nonzeros(seed3D));
ratioValue= ratioValue.*seed3D;
ratio= zeros(totS, 4);	%(:,1)=Si;	(:,2)=Sj;		(:,3)=Sk;		(:,4)=ratio30;
s= 0;
for Sk= 1:11		
   for Sj= 5:5:55		
      for Si= 5:5:60		
         if (ratioValue(Si,Sj,Sk)~=0)	
            s= s+1;		
            ratio(s,1)= Si;	ratio(s,2)= Sj;	ratio(s,3)= Sk;
            ratio(s,4)= ratioValue(Si,Sj,Sk);
         end	
      end
   end
end
save (ratioangle, 'ratio');
clear ('adflTa','adflUr','adflRe','adflMa','adflNo');
clear ('ratioValue','totS','s');

%%%%%%%%%%%%%%%%% mkRanking %%%%%%%%%%%%%%%%%

[totS,a1]= size(ratio);		clear ('a1');
ranking= zeros(totS, 4);	%(:,1)=ranking;	(:,2)=Si;	(:,3)=Sj;	(:,4)=Sk;
for s= 1:totS
   [a1,score]= min(ratio(:,4));		ranking(s,1)= s;
   Si= ratio(score,1);	ranking(s,2)= Si;
   Sj= ratio(score,2);	ranking(s,3)= Sj;
   Sk= ratio(score,3);	ranking(s,4)= Sk;
   ratio(score,4)= ratio(score,4)*1000;
end
save (rankingangle, 'ranking');

fprintf('done');

toc

clear ('ratio','ranking','a1','a2','score','s');
 
end


%toc

