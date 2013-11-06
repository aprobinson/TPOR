clear all;
load adflROI;
[x y z] = size(adflUr);
fprintf('%f %f %f\n',x,y,z)
surface((0.05:.1:5.45),(0.05:.1:5.95),adflUr(:,:,5));