

function [finalaniso] = make_finalaniso(anisoratio1,isoratio);

Sk=12;
Sj=4;

%for Si=1:249
%     varmatrix(Si,1)=var(anisoratio1(Si,Sj,1:Sk));   % matrix having variance of directional seed ratios 
%     fprintf('%f \n',varmatrix(Si,1));
% end
% 
% clear('Sj','Sk');
% save varmatrix varmatrix;

Sj=4;

 for Si=1:249 
        
       finalaniso(Si,1)= anisoratio1(Si,1);
       finalaniso(Si,2)= anisoratio1(Si,2);
       finalaniso(Si,3)= anisoratio1(Si,3); 
    
%    if (varmatrix(Si,1)<0.65)     %  0.3456, 0.2701 NOTE: variance= (0.65 = 0.44*var_cut) is the value used for ABS_abstract submission
%                                           %  0.7353 is the 50% of the average variance
%                                           %  = 0.50*1.4706
%                                           
%        finalaniso(Si,4)=isoratio(Si,4);
%        finalaniso(Si,5)=13;
        %fprintf('%f\n',finalaniso(Si,4));
%        
%    else
     for Sk=1:13
           
            if anisoratio1(Si,Sj,Sk)==min(anisoratio1(Si,Sj,1:13)); 
        
                finalaniso(Si,4)=min(anisoratio1(Si,Sj,1:13));
                finalaniso(Si,5)=Sk; %fprintf('%f\n',finalaniso(Si,4));
     
            end
     end
  %  end
 end
  %%%%%%%%%%%var_cut loop
% var_cut = max(nonzeros(varmatrix));
% 
  %%%%%%%%%%%%%%%%%%%%%%%%%           
         
   save finalaniso finalaniso; 
     clear ('Si', 'Sj', 'Sk','varmatrix', 'var_cut');
