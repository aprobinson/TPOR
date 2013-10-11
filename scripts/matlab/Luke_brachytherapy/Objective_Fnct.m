function [f] = Objective_Fnct(pts, Dose, target, urethra, rectum, normal)

xpts= pts(1);
ypts= pts(2);
zpts= pts(3);

%---Target Objective Function, f_T
f_T= zeros(xpts,ypts,zpts,Nneedles); %Target objective function
L_T= 180;            % Left dose boundary
Gra_L_T= -40;        % Slope to left boundary
R_T= 255;            % Right dose boundary
Gra_R_T= -8;         % Slope to right boundary

%--- Urethra Objective Function, f_U
f_U= zeros(xpts,ypts,zpts,Nneedles); %Urethra objective function
L_U= 95;            % Left dose boundary
Gra_L_U= -40;        % Slope to left boundary
R_U= 100;            % Right dose boundary
Gra_R_U= -40;         % Slope to right boundary

%--- Rectum Objective Function, f_R
f_R= zeros(xpts,ypts,zpts,Nneedles); %Rectum objective function
R_R= 80;            % Right dose boundary
Gra_R_R= -40;        % Slope to right boundary

%--- Normal Tissue Objective Function, f_N
f_N= zeros(xpts,ypts,zpts,Nneedles);
R_N= 145;            % Right dose boundary
Gra_R_N= -40;        % Slope to right boundary


% f_T for underdoseCalculate Dose
TT= (Dose < L_T) & (target > 0.5);
f_T(TT)= Gra_L_T*(Dose(TT)-L_T);

% f_T for overdose
TT= (Dose > R_T) & (target > 0.5);
f_T(TT)= Gra_R_T*(Dose(TT)-R_T);


% f_U for underdose
U= (Dose < L_U) & (urethra > 0.5);
f_U(U)= Gra_L_U*(Dose(U)-L_U);

% f_U for overdose
U= (Dose > R_U) & (urethra > 0.5);
f_U(U)= Gra_R_U*(Dose(U)-R_U);


% f_R for overdose
R= (Dose > R_R) & (rectum > 0.5);
f_R(R)= Gra_R_R*(Dose(R)-R_R);


% f_N for overdose
N= (Dose > R_N) & (normal > 0.5);
f_N(N)= Gra_R_N*(Dose(N)-R_N);

%--- Total Objective Function, f
f= zeros(xpts,ypts,zpts,Nneedles);
f_needle= f_T + f_U + f_R + f_N;

% for kk=1:zpts
%  figure
%  surf(f_T(:,:,kk))
% end