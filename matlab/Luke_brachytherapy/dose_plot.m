
load('DoseMatrix.mat')
load('DoseNist99.mat')
load('suadose.mat')

T= 59.4*24;         % I-125 half life
Decay= log(2)/T;     % I-125 decay constant

Dose1(:,1)=0.4*SUA_DOSE(71:139,70,15);
Dose2(:,1)=DNist99(71:139,70,70)/(Decay*100);
Dose3(:,1)=Dose(71:139,70,15);
for i=1:69
    r(i)=0.1*(i);
end

%%%% TG-43 Eq. (11) Dose calculation
rTG=[0.5 1 1.5 2.0 3.0 4.0 5.0 6.0 7.0];
DoseTG=[3.937 .911 .368 .186 .0643 .0284 .0134 .00688 .00506];
Dose4=DoseTG*.508/(Decay*100);
figure
hold all
plot(r,Dose1,'green')
plot(r,Dose2,'blue')
% plot(r,Dose3)
plot(rTG,Dose4,'red')

