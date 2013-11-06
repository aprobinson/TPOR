clear all;

Wur=1;Wre=1;Wta=1;Wmacs=1;Wmata=0;

load adflROI30

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI60

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI90

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI120

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI150

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI180

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI210

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI240

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI270

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI300

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI330

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI360

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));

load adflROI

Wur=0.5;Wre=1;Wta=1;Wmacs=0.5;Wmata=0;

ratioValue= (Wur*adflUr+Wre*adflRe+Wmacs*adflMa)./(Wmata*adflMa+Wta*adflTa);
fprintf('%f %f %f\n',ratioValue(1,1,1),ratioValue(2,1,1),ratioValue(3,1,1));
