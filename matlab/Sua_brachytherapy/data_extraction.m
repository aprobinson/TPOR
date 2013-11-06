clear all;
org = fopen('organ_mesh-SUA','w');
template = fopen('needle_template-SUA','w');
adjoint = fopen('adjoint_data-SUA','w');
seed = fopen('I6711-SUA','w');
load mask3D;
load adflROI;
load foDose;
[x y z] = size(target);
[xs ys zs] = size(foDose);
target_vol = sum(nonzeros(target));
margin_vol = sum(nonzeros(margin));
urethra_vol = sum(nonzeros(urethra));
rectum_vol = sum(nonzeros(rectum));
normal_vol = sum(nonzeros(normal));

%organ mesh file
fprintf(org,'xpts: %i\n',x);
fprintf(org,'dx(organ): 0.1\n');
fprintf(org,'ypts: %i\n',y);
fprintf(org,'dy(organ): 0.1\n');
fprintf(org,'zpts: %i\n',z);
fprintf(org,'dz(organ): 0.5\n');
fprintf(org,'target: %i\n',target_vol);

%needle template mesh file
fprintf(template,'xpts: %i\n',x);
fprintf(template,'dx(organ): 0.1\n');
fprintf(template,'ypts: %i\n',y);
fprintf(template,'dy(organ): 0.1\n');
fprintf(template,'zpts: %i\n',z);
fprintf(template,'dz(organ): 0.5\n');

%adjoint data file
fprintf(adjoint,'xpts: %i\n',x);
fprintf(adjoint,'dx(organ): 0.1\n');
fprintf(adjoint,'ypts: %i\n',y);
fprintf(adjoint,'dy(organ): 0.1\n');
fprintf(adjoint,'zpts: %i\n',z);
fprintf(adjoint,'dz(organ): 0.5\n');
fprintf(adjoint,'target: %i\n',target_vol);

%seed data file
fprintf(seed,'xpts: %i\n',xs);
fprintf(seed,'xseedcent: %i\n',(xs-1)/2);
fprintf(seed,'dx(seed): 0.1\n');
fprintf(seed,'ypts: %i\n',ys);
fprintf(seed,'yseedcent: %i\n',(ys-1)/2);
fprintf(seed,'dy(seed): 0.1\n');
fprintf(seed,'zpts: %i\n',zs);
fprintf(seed,'zseedcent: %i\n',(zs-1)/2);
fprintf(seed,'dz(seed): 0.5\n');

for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(org,'%i ',target(i,j,k));
        end
        fprintf(org,'\n');
    end
    fprintf(org,'\n');
end

fprintf(org,'urethra: %i\n',urethra_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(org,'%i ',urethra(i,j,k));
        end
        fprintf(org,'\n');
    end
    fprintf(org,'\n');
end

fprintf(org,'rectum: %i\n',rectum_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(org,'%i ',rectum(i,j,k));
        end
        fprintf(org,'\n');
    end
    fprintf(org,'\n');
end

fprintf(org,'normal: %i\n',normal_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(org,'%i ',normal(i,j,k));
        end
        fprintf(org,'\n');
    end
    fprintf(org,'\n');
end

fprintf(org,'margin: %i\n',margin_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(org,'%i ',margin(i,j,k));
        end
        fprintf(org,'\n');
    end
    fprintf(org,'\n');
end

fclose(org);

for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(template,'%i ',seed3D(i,j,k));
        end
        fprintf(template,'\n');
    end
    fprintf(template,'\n');
end

fclose(template);

for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(adjoint,'%1.12e ',adflTa(i,j,k));
        end
        fprintf(adjoint,'\n');
    end
    fprintf(adjoint,'\n');
end

fprintf(adjoint,'urethra: %i\n',urethra_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(adjoint,'%1.12e ',adflUr(i,j,k));
        end
        fprintf(adjoint,'\n');
    end
    fprintf(adjoint,'\n');
end

fprintf(adjoint,'rectum: %i\n',rectum_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(adjoint,'%1.12e ',adflRe(i,j,k));
        end
        fprintf(adjoint,'\n');
    end
    fprintf(adjoint,'\n');
end

fprintf(adjoint,'normal: %i\n',normal_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(adjoint,'%1.12e ',adflNo(i,j,k));
        end
        fprintf(adjoint,'\n');
    end
    fprintf(adjoint,'\n');
end

fprintf(adjoint,'margin: %i\n',margin_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(adjoint,'%1.12e ',adflMa(i,j,k));
        end
        fprintf(adjoint,'\n');
    end
    fprintf(adjoint,'\n');
end

fclose(adjoint);

for k= 1:zs
    for j= 1:ys
        for i= 1:xs
            fprintf(seed,'%1.12e ',foDose(i,j,k));
        end
        fprintf(seed,'\n');
    end
    fprintf(seed,'\n');
end

fclose(seed);