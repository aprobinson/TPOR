clear all;
org = fopen('organ_mesh-SUA','w');
load mask3D;
[x y z] = size(target);
target_vol = sum(nonzeros(target));
margin_vol = sum(nonzeros(margin));
urethra_vol = sum(nonzeros(urethra));
rectum_vol = sum(nonzeros(rectum));
normal_vol = sum(nonzeros(normal));
fprintf(org,'xpts: %i\n',x);
fprintf(org,'dx(organ): 0.1\n');
fprintf(org,'ypts: %i\n',y);
fprintf(org,'dy(organ): 0.1\n');
fprintf(org,'zpts: %i\n',z);
fprintf(org,'dz(organ): 0.5\n');
fprintf(org,'target: %i\n',target_vol);

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