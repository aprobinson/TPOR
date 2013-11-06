
clear all;
tic
anisoratio1=zeros(249,4,13);

load ratio30;    anisoratio1(:,:,1)=ratio; % Si,Sj,Sk,ratio
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio60;    anisoratio1(:,:,2)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio90;    anisoratio1(:,:,3)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio120;   anisoratio1(:,:,4)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio150;   anisoratio1(:,:,5)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio180;   anisoratio1(:,:,6)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio210;   anisoratio1(:,:,7)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio240;   anisoratio1(:,:,8)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio270;   anisoratio1(:,:,9)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio300;   anisoratio1(:,:,10)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio330;   anisoratio1(:,:,11)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load ratio360;   anisoratio1(:,:,12)=ratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));
load isoratio;   anisoratio1(:,:,13)=isoratio;
%fprintf('%f %f %f\n',ratio(1,1,1),ratio(1,1,2),ratio(1,1,3));

   
    save anisoratio1 anisoratio1;
    
    toc