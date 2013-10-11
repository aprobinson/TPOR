close all
clear all



%%%%%   LOAD DATA   %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%
load Input

fid2= fopen('needle_template-SUA');    %Opens Sua's needle template file
dim= textscan(fid2, '%s %n',6, 'delimiter', ':');    %read dimensions in header

%---Template Data
template= zeros(pts(1),pts(2),pts(3)/5);        %[0 1] matrix of target organ
%template_c= cell(pts(3),1);

%---Read target data from organ_mesh-SUA file
for i= 1:pts(3)/5
   data= textscan(fid2,repmat('%n',1,pts(1)),pts(2),'CollectOutput',1);
   template(:,:,i)= permute(data{1},[2 1 3]);
   %template_c{i}= data{1};
end
%---Possible x and y compnents of needle locations
Ntemplate= sum(sum(template(:,:,5)));
[u,v]= find(template(:,:,5));

% Loop through different gain factors
for loop= 1:10
% Display loop #
say=['loop # ',num2str(loop)];
disp(say)
 
rng(511)
%%%%%   PLACE SEEDS   %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%
%---Random needle placement
Nneedles= 14;    %number of initial needles

% Note this will sometimes give zero or repeat so needles are in same
% location
rnd1= round(rand(Nneedles,1)*Ntemplate+1);
needles= [u(rnd1) v(rnd1)];


%---Find the number of initial seeds
Nseeds= 0;       
for i= 1:Nneedles
    [o]= find(template(needles(i,1),needles(i,2),:));
    Nseeds= Nseeds+length(o);
end

%---Initial seed placement
%NOTE: there are mutiple seeds for each x and y poisiton of a needle
seeds= ones(Nseeds,3);

num_ndl=zeros(Nneedles,1);
mm= 0;
for i= 1:Nneedles
    [o]= find(template(needles(i,1),needles(i,2),:));    %z position along needle i
    mn= mm+length(o);                            
    seeds(mm+1:mn,1)= seeds(mm+1:mn,1)*needles(i,1);     %x position
    seeds(mm+1:mn,2)= seeds(mm+1:mn,2)*needles(i,2);     %y position
    seeds(mm+1:mn,3)= seeds(mm+1:mn,3).*o;               %z poistion
    num_ndl(i)=length(o);                                %number of seeds in the needle
    mm= mn;
end

%%%%%   OPTIMIZE SEEDS   %%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
D= zeros(pts(1),pts(2),pts(3),Nneedles);    %dose-rate matrix
%dose= zeros(pts(1),pts(2),pts(3)); 

% Calculate dose

%---Use SeedDose.m to calculate dose by how many voxels away 
%   from the seed in the x y z direction 
load DoseMatrix
load DoseNist99
load suadose
% Seed position in dose rate matrix (a,b,c)
a= 70; b= 70; c= 70;

optimized=1;
iter=0;
iter_end=1000;
while optimized
%  newseeds= ones(Nseeds,3);
%  newneedles= zeros(Nneedles,2); 
 
 iter= iter+1;
if iter==100
    optimized= 0;
end
% Display # of iterations
say=['iteration # ',num2str(iter)];
disp(say)

mm=0;
for n= 1:Nneedles
    mn= mm+num_ndl(n);
    
    for s= mm+1:mn
        % Shift dose matrix to center on seed, s
        delta_i= a-seeds(s,1);   % index shift in x   
        delta_j= b-seeds(s,2);   % index shift in y
        delta_k= c-seeds(s,3);   % index shift in z
        % Dose matrix centered on seed location
        %centered= circshift(Dose, [-delta_i,-delta_j,-delta_k]);
        T_half= 59.4*24;         % I-125 half life
        Decay= log(2)/T_half;    % I-125 decay constant
        centered= circshift(DNist99/(Decay*100), [-delta_i,-delta_j,-delta_k]);
        %centered= circshift(SUA_DOSE/100, [-delta_i,-delta_j,-delta_k]);
        
        % Dose [Gy]
        D(:,:,:,n)= D(:,:,:,n)+centered(1:pts(1),1:pts(2),1:pts(3));
    end
%    dose= dose+D(:,:,:,n);
    mm= mn;

end




mm=0;
newseeds=seeds;
newneedles=needles;

%--- Loop to move needles individually
for n= 1:Nneedles
    mn= mm+num_ndl(n);

    dose= sum(D,4);   
    %--- Objective Function, f
    [f]= ObjectiveFunction(pts, dose, target, urethra, rectum, normal);

    if f==0
        optimized=0;
        disp('all perameters met')
    end
    
    %--- Displacement Vector, T
    [T]= DisplacementVector(pts, seeds(mm+1:mn,:), f);

    % for kk=1:pts(3)
    %   figure
    %   surf(showx(:,:,kk))
    % end

    %--- Move needles and seeds;
    k= 1E-8*(7+loop/2);     % Gain factor

    [d_seeds, d_needle]= Seed_Repo(k, T);
    
%     if any(d_seeds)
%         d_seeds 
%         d_needle
%     end

    newseeds(mm+1:mn,:)=newseeds(mm+1:mn,:)+d_seeds;
    newneedles(n,:)=newneedles(n,:)+d_needle;
    
%     hold all 
%     figure
%     scatter3(newseeds(:,1),newseeds(:,2),newseeds(:,3),'fill')
    
    %--- Make sure seeds and needles don't overlap
    [newseeds, newneedles]= Overlap(seeds, newseeds, needles, newneedles, num_ndl, mm, n, d_seeds, d_needle);
    
    %--- Recalculate the Dose from that needle
    D(:,:,:,n)=0;
        for s= mm+1:mn
            % Shift dose matrix to center on seed, s
            delta_i= a-newseeds(s,1);   % index shift in x   
            delta_j= b-newseeds(s,2);   % index shift in y
            delta_k= c-newseeds(s,3);   % index shift in z
            % Dose matrix centered on seed location
            %centered= circshift(Dose, [-delta_i,-delta_j,-delta_k]);
            T_half= 59.4*24;         % I-125 half life
            Decay= log(2)/T_half;    % I-125 decay constant
            centered= circshift(DNist99/(Decay*100), [-delta_i,-delta_j,-delta_k]);
            %centered= circshift(SUA_DOSE/100, [-delta_i,-delta_j,-delta_k]);

            % Dose [Gy]
            D(:,:,:,n)= D(:,:,:,n)+centered(1:pts(1),1:pts(2),1:pts(3));
        end
    max(max(D(:,:,:,n)));
    mm=mn;
end





%--- If seeds don't move inbetween iterations then end loop
if newseeds==seeds
    optimized= 0;
    disp('seeds dont move')
end

% Set seed positions to the new positions
needles=newneedles;
seeds=newseeds;
% Seeds (needle) leaves grid in x direction
outx= find((seeds(:,1) > pts(1)) | (seeds(:,1) < 0));
if any(outx)
seeds(outx,:)=[];
end
outx_n= find((needles(:,1) > pts(1)) | (needles(:,1) < 0));
if ~isempty(outx_n)
needles(outx_n,:)= [];
Nneedles= size(needles,1);
num_ndl(outx_n)=[];
end

% Seeds (needle) leaves grid in y direction
outy= find((seeds(:,2) > pts(2)) | (seeds(:,2) < 0));
if any(outy)
seeds(outy,:)= [];
end
outy_n= find((needles(:,2) > pts(2)) | (needles(:,2) < 0));
if any(outy_n)
needles(outy_n,:)= [];
Nneedles= size(needles,1);
num_ndl(outy_n,:)=[];
end

% Seeds leaves grid in z direction
outz= (seeds(:,3) > pts(3)) | (seeds(:,3) <= 0);
if ~isempty(outz)
seeds(outz,:)= [];
mm=0;
for i= 1:Nneedles
    mn= mm + num_ndl(i);
    num_ndl(i)= num_ndl(i) - sum(outz(mm+1:mn));
    mm=mn;
end
end
empty= (num_ndl==0);
num_ndl(empty)= [];
needles(empty,:)= [];
Nneedles= size(needles,1);
Nseeds= size(seeds,1);

if Nneedles<3
    optimized=0;
    disp('Not enough needles')
end
%hold all
figure
scatter3(seeds(:,1),seeds(:,2),seeds(:,3),'fill')
hold all

TDose= 145;                % [Gy] target dose
ind= target > 0.5;
Dtarget= dose(ind);  % Dose to the target

Percent100= sum(Dtarget > TDose)/length(Dtarget)
Percent200= sum(Dtarget > TDose*2)/length(Dtarget)

%figure
%bar(Dtarget)

if Nseeds<4
    optimized=0;
    disp('Not enough seeds')
end
end

% TDose= 145;             % [Gy] target dose
% ind= target > 0.5;
% Dtarget= dose(ind)/TDose;  % Fraction of target dose to the target
end
