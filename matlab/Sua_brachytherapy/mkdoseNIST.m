%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%% mkDoseNist99.m - Oct, 2001 %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% NIST changed the dose rate constant from 0.88 to 0.98 for I6711
% make dose data 139x139x29 for 1mCi of 125I - 6711 
% doserate = Sk U * ^ cGy/hr.U * Aniso * Radial / r^2 (eq.13 p228)

% for 125I - 6711
% air-kerma strength Sk (p218) --> 1.270 U/mCi
% dose rate constant ^ (p219) --> 0.98 cGy/hr.U
% anisotropy function Aniso (p228)
% r(cm)   1      2      3      4      5      6      7    
%       0.944  0.936  0.893  0.887  0.884  0.880  0.901          
% radial dose function Radial (p223)
% r(cm)  0.5    1.0    1.5    2.0    2.5    3.0    3.5    
%        1.04   1.00   0.926  0.832  0.731  0.632  0.541  
% r(cm)  4.0    4.5    5.0    5.5    6.0    6.5    7.0   
%        0.463  0.397  0.344  0.300  0.264  0.233  0.204

% interpolation equation 
% y= [y1(x2-x) - y2(x1-x)]/(x2-x1)

clear all
close all

Sk= 1.270;			% air-kerma strength [1mCi * 1.270 U/mCi]
DRconst= 0.98;		% dose rate constant [cGy/hr.U]
DNist99= zeros(139,139,29);		% [cGy/hr]
An= [0.944 0.936 0.893 0.887 0.884 0.880 0.901];	
% radial= [1.04 1.00 0.926 0.832 0.731 0.632 0.541 ...
%      0.463 0.397 0.344 0.300 0.264 0.233 0.204];
% radial dose function coefficient
a0= 1.01376;	  a1= 1.22747e-1;	  a2= -1.73025e-1;
a3= 4.02378e-2;	  a4= -3.85227e-3;   a5= 1.34283e-4;
% radial= a0 + a1r + a2r^2 + a3r^3 + a4r^4 + a5r^5;

a= 70;   b= 70;   c= 15; 	%(a,b,c) the middle pixel

for k= 1:29		% k= 0.5cm
for j= 1:139		% j= 0.1cm
for i= 1:139 	% i= 0.1cm
   
   % compute distance between a pixel and the middle point
   r= ((0.1*(i-a))^2 + (0.1*(j-b))^2 + (0.5*(k-c))^2)^0.5;
   if r==0	
      r= 0.1;	      
   end   
   
   % radial dose function
   radial= a0 + a1*r + a2*r^2 + a3*r^3 + a4*r^4 + a5*r^5;
   
   % anisotropy function
   index= fix(r);		% round to the nearest integer toward zero
   if index > 0 & index < 7		% interpolation
      m= index;		n= index+1;
      aniso= (An(m)*(n-r) - An(n)*(m-r))/(n-m); 
   elseif index == 0
      m= 1;	n= 2;
      aniso= (An(m)*(n-r) - An(n)*(m-r))/(n-m);      
   elseif index > 9
      aniso= 0;
   elseif index > 7
      m= 6;	n= 7;
      aniso= (An(m)*(n-r) - An(n)*(m-r))/(n-m);         
   end
   
	DNist99(i,j,k)= 0.4*Sk*DRconst*radial*aniso/(r^2); 
         
end      
end
end


save DoseNist99 DNist99;
