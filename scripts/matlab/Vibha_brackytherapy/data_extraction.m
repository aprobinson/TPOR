clear all;
org = fopen('organ_mesh-VIBHA','w');
template = fopen('needle_template-VIBHA','w');
isoadjoint = fopen('adjoint_data_iso-VIBHA','w');
anisoadjoint = fopen('adjoint_data_aniso-VIBHA','w');
seediso = fopen('I2301iso-VIBHA','w');
seedaniso = fopen('I2301aniso-VIBHA','w');
load mask3D;
load adflROI;
load isoseed;

[x y z] = size(target);
[xs ys zs] = size(isoseed);

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

%needle template mesh file
fprintf(template,'xpts: %i\n',x);
fprintf(template,'dx(organ): 0.1\n');
fprintf(template,'ypts: %i\n',y);
fprintf(template,'dy(organ): 0.1\n');
fprintf(template,'zpts: %i\n',z);
fprintf(template,'dz(organ): 0.5\n');

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

%iso adjoint data file
fprintf(isoadjoint,'xpts: %i\n',x);
fprintf(isoadjoint,'dx(organ): 0.1\n');
fprintf(isoadjoint,'ypts: %i\n',y);
fprintf(isoadjoint,'dy(organ): 0.1\n');
fprintf(isoadjoint,'zpts: %i\n',z);
fprintf(isoadjoint,'dz(organ): 0.5\n');
fprintf(isoadjoint,'target: %i\n',target_vol);

for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(isoadjoint,'%1.12e ',adflTa(i,j,k));
        end
        fprintf(isoadjoint,'\n');
    end
    fprintf(isoadjoint,'\n');
end

fprintf(isoadjoint,'urethra: %i\n',urethra_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(isoadjoint,'%1.12e ',adflUr(i,j,k));
        end
        fprintf(isoadjoint,'\n');
    end
    fprintf(isoadjoint,'\n');
end

fprintf(isoadjoint,'rectum: %i\n',rectum_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(isoadjoint,'%1.12e ',adflRe(i,j,k));
        end
        fprintf(isoadjoint,'\n');
    end
    fprintf(isoadjoint,'\n');
end

fprintf(isoadjoint,'normal: %i\n',normal_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(isoadjoint,'%1.12e ',adflNo(i,j,k));
        end
        fprintf(isoadjoint,'\n');
    end
    fprintf(isoadjoint,'\n');
end

fprintf(isoadjoint,'margin: %i\n',margin_vol);
for k= 1:z
    for j= 1:y
        for i= 1:x
            fprintf(isoadjoint,'%1.12e ',adflMa(i,j,k));
        end
        fprintf(isoadjoint,'\n');
    end
    fprintf(isoadjoint,'\n');
end

fclose(isoadjoint);

%aniso adjoint data file
fprintf(anisoadjoint,'xpts: %i\n',x);
fprintf(anisoadjoint,'dx(organ): 0.1\n');
fprintf(anisoadjoint,'ypts: %i\n',y);
fprintf(anisoadjoint,'dy(organ): 0.1\n');
fprintf(anisoadjoint,'zpts: %i\n',z);
fprintf(anisoadjoint,'dz(organ): 0.5\n');
fprintf(anisoadjoint,'angles: 12\n');
fprintf(anisoadjoint,'dtheta: 30\n\n');

for l= 1:12
    clear adflTa adflUr adflRe adflRe adflMa adflNo;
    adfil = ['adflROI',num2str(30*l)];
    load(adfil); 
    fprintf(anisoadjoint,'angle %i data\n',l);
    fprintf(anisoadjoint,'target: %i\n',target_vol);
    for k= 1:z
        for j= 1:y
            for i= 1:x
                fprintf(anisoadjoint,'%1.12e ',adflTa(i,j,k));
            end
            fprintf(anisoadjoint,'\n');
        end
        fprintf(anisoadjoint,'\n');
    end

    fprintf(anisoadjoint,'urethra: %i\n',urethra_vol);
    for k= 1:z
        for j= 1:y
            for i= 1:x
                fprintf(anisoadjoint,'%1.12e ',adflUr(i,j,k));
            end
            fprintf(anisoadjoint,'\n');
        end
        fprintf(anisoadjoint,'\n');
    end

    fprintf(anisoadjoint,'rectum: %i\n',rectum_vol);
    for k= 1:z
        for j= 1:y
            for i= 1:x
                fprintf(anisoadjoint,'%1.12e ',adflRe(i,j,k));
            end
            fprintf(anisoadjoint,'\n');
        end
        fprintf(anisoadjoint,'\n');
    end

    fprintf(anisoadjoint,'normal: %i\n',normal_vol);
    for k= 1:z
        for j= 1:y
            for i= 1:x
                fprintf(anisoadjoint,'%1.12e ',adflNo(i,j,k));
            end
            fprintf(anisoadjoint,'\n');
        end
        fprintf(anisoadjoint,'\n');
    end

    fprintf(anisoadjoint,'margin: %i\n',margin_vol);
    for k= 1:z
        for j= 1:y
            for i= 1:x
                fprintf(anisoadjoint,'%1.12e ',adflMa(i,j,k));
            end
            fprintf(anisoadjoint,'\n');
        end
        fprintf(anisoadjoint,'\n');
    end
end

fclose(anisoadjoint);

%iso seed data file
fprintf(seediso,'xpts: %i\n',xs);
fprintf(seediso,'xseedcent: %i\n',(xs-1)/2);
fprintf(seediso,'dx(seed): 0.1\n');
fprintf(seediso,'ypts: %i\n',ys);
fprintf(seediso,'yseedcent: %i\n',(ys-1)/2);
fprintf(seediso,'dy(seed): 0.1\n');
fprintf(seediso,'zpts: %i\n',zs);
fprintf(seediso,'zseedcent: %i\n',(zs-1)/2);
fprintf(seediso,'dz(seed): 0.5\n');

for k= 1:zs
    for j= 1:ys
        for i= 1:xs
            fprintf(seediso,'%1.12e ',isoseed(i,j,k));
        end
        fprintf(seediso,'\n');
    end
    fprintf(seediso,'\n');
end

fclose(seediso);

%aniso seed data file
fprintf(seedaniso,'xpts: %i\n',xs);
fprintf(seedaniso,'xseedcent: %i\n',(xs-1)/2);
fprintf(seedaniso,'dx(seed): 0.1\n');
fprintf(seedaniso,'ypts: %i\n',ys);
fprintf(seedaniso,'yseedcent: %i\n',(ys-1)/2);
fprintf(seedaniso,'dy(seed): 0.1\n');
fprintf(seedaniso,'zpts: %i\n',zs);
fprintf(seedaniso,'zseedcent: %i\n',(zs-1)/2);
fprintf(seedaniso,'dz(seed): 0.5\n');
fprintf(seedaniso,'angles: 12\n');
fprintf(seedaniso,'dtheta: 30\n\n');

for l= 1:12
    clear dirdose;
    sfil = ['anisoseed',num2str(30*l)];
    load(sfil);
    fprintf(seedaniso,'angle %i data\n',l);
    for k= 1:zs
        for j= 1:ys
            for i= 1:xs
                fprintf(seedaniso,'%1.12e ',dirdose(i,j,k));
            end
            fprintf(seedaniso,'\n');
        end
        fprintf(seedaniso,'\n');
    end
end

fclose(seedaniso);
    

