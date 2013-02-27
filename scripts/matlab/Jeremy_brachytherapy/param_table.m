%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  parameter table generator for LaTeX  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% tot--> max* min* mean*, where *=Ta,Ur,Re,No
% T--> V100ta V150ta DNR CN
% U--> U125 V360Gy D90ur
% R--> R80 V90Gy D90re
% N--> D90no
% +--> number seeds
clc
for xx = 3:2:11
    
file1 = ['gyparams',num2str(xx)];
load(file1)
j = floor(xx/2);
maxTaGY(j)=maxTa; maxUrGY(j)=maxUr; maxReGY(j)=maxRe; maxNoGY(j)=maxNo;
minTaGY(j)=minTa; minUrGY(j)=minUr; minReGY(j)=minRe; minNoGY(j)=minNo;
meanTaGY(j)=meanTa; meanUrGY(j)=meanUr; meanReGY(j)=meanRe; meanNoGY(j)=meanNo;
V100taGY(j) = V100ta; V150taGY(j) = V150ta; DNRGY(j) = DNRta; CNGY(j) = CN;
U125GY(j) = U125; V360GY(j) = V360Gy; D90urGY(j) = D90ur;
R80GY(j) = R80; V90GY(j) = V90Gy; D90reGY(j) = D90re; D90noGY(j) = D90no;
numSGY(j) = numSeed;
file2 = ['cdparams',num2str(xx)];
load(file2)
j = floor(xx/2);
maxTaCD(j)=maxTa; maxUrCD(j)=maxUr; maxReCD(j)=maxRe; maxNoCD(j)=maxNo;
minTaCD(j)=minTa; minUrCD(j)=minUr; minReCD(j)=minRe; minNoCD(j)=minNo;
meanTaCD(j)=meanTa; meanUrCD(j)=meanUr; meanReCD(j)=meanRe; meanNoCD(j)=meanNo;
V100taCD(j) = V100ta; V150taCD(j) = V150ta; DNRCD(j) = DNRta; CNCD(j) = CN;
U125CD(j) = U125; V360CD(j) = V360Gy; D90urCD(j) = D90ur;
R80CD(j) = R80; V90CD(j) = V90Gy; D90reCD(j) = D90re; D90noCD(j) = D90no;
numSCD(j) = numSeed;
end


%%% begin writing table

wfid = 1;
fprintf(wfid,' \\begin{table} \n');
fprintf(wfid,' \\begin{center} \n');
fprintf(wfid,' \\begin{tabular*}{0.90\\textwidth}{@{\\extracolsep{\\fill}} crrrrrr } \n');
fprintf(wfid,' \\toprule \n')
fprintf(wfid,'		 & &  {\\bf S3} &  {\\bf S5} &  {\\bf S7}  &  {\\bf S9}  & {\\bf S11} \\\\ \n')

fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  {\\sc Target} & & & & & &  \\\\ \n')
fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  $V_{100}$(\\%%) & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',V100taCD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',V100taGY)
fprintf(wfid,'  $V_{150}$(\\%%) & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',V150taCD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',V150taGY)
fprintf(wfid,'  $CN     $        & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',CNCD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',CNGY)
fprintf(wfid,'  $DNR    $        & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',DNRCD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',DNRGY)

fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  {\\sc Urethra} & & & & & &  \\\\ \n')
fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  $V_{125,ur}$(\\%%) & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',U125CD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',U125GY)
fprintf(wfid,'  $V_{360,ur}$(\\%%) & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',V360CD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',V360GY)
fprintf(wfid,'  $D_{90,ur}$(Gy)    & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',D90urCD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',D90urGY)

fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  {\\sc Rectum} & & & & & &  \\\\ \n')
fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  $V_{80,re}$(\\%%) & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',R80CD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',R80GY)
fprintf(wfid,'  $V_{90,re}$(\\%%) & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',V90CD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',V90GY)
fprintf(wfid,'  $D_{90,re}$(Gy)   & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',D90reCD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',D90reGY)
   
fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  {\\sc Normal Tissue} & & & & & &  \\\\ \n')
fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  $D_{90,no}$(Gy) & {\\it CD} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',D90noCD)
fprintf(wfid,'                 & {\\it GY} &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f &  %-6.2f \\\\ \n',D90noGY)

fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  {\\sc Qualitative} & & & & & &  \\\\ \n')
fprintf(wfid,'  \\midrule \n')
fprintf(wfid,'  $Seeds$      & {\\it CD} &  %-6.0f &  %-6.0f &  %-6.0f &  %-6.0f &  %-6.0f \\\\ \n',numSCD)
fprintf(wfid,'                 & {\\it GY} &  %-6.0f &  %-6.0f &  %-6.0f &  %-6.0f &  %-6.0f \\\\ \n',numSGY)
         
fprintf(wfid,'\\bottomrule \n')
fprintf(wfid,' \\end{tabular*} \n')
fprintf(wfid,' \\end{center} \n')
fprintf(wfid,' \\caption{For f = %-1.0f, without grid.} \n',f)
fprintf(wfid,' \\label{tbl:mtxdims} \n')
fprintf(wfid,'\\end{table} \n')