%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%  mkSeedDose.m - Oct, 2001  %%%%%
%%%%%   Modified April, 2013     %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% make dose 139x139x29 matrix data for 0.4 mCi I-125 seed
% dose rate [cGy/hr] = 0.4 [mCi] * Sk [U/mCi] * ^ [cGy/hr.U] * Aniso * Radial / r^2
% dose [Gy] = dose rate [cGy/hr] / ( decay constant [1/hr] * 100 )
% air-kerma strength Sk --> 1.270 U/mCi

% interpolation equation y= [y1(x2-x) - y2(x1-x)]/(x2-x1)

clear all
close all

Sk= 1.270;			% air-kerma strength [1.270 U/mCi]
T= 59.4*24;         % I-125 half life
Decay= log(2)/T;     % I-125 decay constant
Dose= zeros(139,139,29);		% [Gy]

% Dose rate constant [cGy/hr.U]
Amersham_6702_lam=  1.036;
Amersham_6711_lam=  0.965;
Best_2301_lam=      1.018;
Bebing_I25_lam=     1.012;
Imagyn_12501_lam=   0.940;

% Anisotropic function maxtrix [an(r);r(cm)]
Amersham_6702_an= [0.986 0.960 0.952 0.951 0.954 0.954; 0.5 1 2 3 4 5];	
Amersham_6711_an= [0.973 0.944 0.941 0.942 0.943 0.944; 0.5 1 2 3 4 5];	
Best_2301_an= [0.945 0.987 0.968 0.971 0.969 0.991 0.969; 1 2 3 4 5 6 7];
Bebing_I25_an= [1.122 0.968 0.939 0.939 0.938 0.94 0.941 0.949; 0.25 0.5 1 2 3 4 5 6];
Imagyn_12501_an= [0.867 0.886 0.894 0.897 0.879; 1 2 3 4 7];


% radial dose function coefficient
% g(r)= a0 + a1r + a2r^2 + a3r^3 + a4r^4 + a5r^5;
Amersham_6702_g= [7.3273E-01 5.7680E-01 -3.8203E-01 8.3724E-02 -8.0137E-03 2.8226E-04];
Amersham_6711_g= [6.3528E-01 7.4559E-01 -4.6613E-01 1.0144E-01 -9.6765E-03 3.3968E-04];
Best_2301_g=     [7.8284E-01 5.3266E-01 -3.8977E-01 8.8727E-02 -8.6634E-03 3.0869E-04];
Bebing_I25_g=    [6.7538E-01 6.8259E-01 -4.4061E-01 9.6630E-02 -9.2488E-03 3.2528E-04];
Imagyn_12501_g=  [7.2177E-01 6.6334E-01 -4.7030E-01 1.0766E-01 -1.0558E-02 3.7683E-04];

% Set the desired seed
DRconst= Amersham_6711_lam;		% Dose rate constant [cGy/hr.U]
An=      Amersham_6711_an(1,:); % Anisotropic function, an(d)
d=       Amersham_6711_an(2,:); % Reference distance, d, for an(d) [cm]
g=       Amersham_6711_g;       % radial dose function constants

a= 70;   b= 70;   c= 15;    % (a,b,c) the seed position
dx= 0.1; dy= 0.1; dz= 0.5;  % Distance between grid points [cm]

for k= 1:29		% z distance from seed (Δz= k*dz) [cm]
for j= 1:139    % y distance from seed (Δy= j*dy) [cm]
for i= 1:139 	% x distance from seed (Δx= i*dx) [cm]

   % Compute distance between a pixel and the middle point
   r= ((dx*(i-a))^2 + (dy*(j-b))^2 + (dz*(k-c))^2)^0.5;
   % For the middle point (ie: the pixel where the seed is located)
   if r==0	
      r= 0.1;	      
   end   
   
   % **** Radial Dose Function ****
   radial= g(1) + g(2)*r + g(3)*r^2 + g(4)*r^3 + g(5)*r^4 + g(6)*r^5;
   % ****
   
   % **** Anisotropy Function ****
   L=length(d);         % Length of reference distance vector

   % linear interpolation between 2 reference points
   if r <= d(1)
       n=1;
       aniso=An(n)+(An(n+1)-An(n))*(r-d(n))/(d(n+1)-d(n));
   elseif r > d(L)
       n=L-1;
       aniso=An(n)+(An(n+1)-An(n))*(r-d(n))/(d(n+1)-d(n));
   else
       m=max(d(d<r));       % Max reference distance value less than r
       n=find(d==m,1);      % Index of m in the reference distance vector
       aniso=An(n)+(An(n+1)-An(n))*(r-d(n))/(d(n+1)-d(n));
   end
   % ****

   Dose(i,j,k)= 0.4*Sk*DRconst*radial*aniso/(r^2*Decay*100);
         
end      
end
end
save DoseMatrix Dose;
display('Dose Matrix Calculated')