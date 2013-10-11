
function [newseeds, newneedles] = SeedRepositioning(k, T, seeds, needles, num_ndl)

Nseeds= length(seeds);
Nneedles= length(needles);
newseeds= ones(Nseeds,3);
newneedles= zeros(Nneedles,2);
dx= zeros(Nneedles,1);
dy= dx;
mm= 0;

for i= 1:Nneedles
    mn= mm+num_ndl(i);
%--- Move needles in x direction (in voxels)
    dx(i,1)= k*sum(T(mm+1:mn,1))/num_ndl(i); 
    newseeds(mm+1:mn,1,1)= seeds(mm+1:mn,1,1)+round(dx(i,1));
    newneedles(i,1)= needles(i,1)+round(dx(i,1));
%--- Move needles in y direction (in voxels)
    dy(i,1)= k*sum(T(mm+1:mn,2))/num_ndl(i);
    newseeds(mm+1:mn,2,1)= seeds(mm+1:mn,2,1)+round(dy(i,1));
    newneedles(i,2)= needles(i,2)+round(dy(i,1));    
    mm= mn;
end   

%--- Move seeds in z direction
dz= k*T(:,3);
newseeds(:,3)= seeds(:,3)+round(dz);

%--- Make sure needle positions don't overlap
for i= 1:Nneedles-1
    % Indices of needles that overlap in newneedles
    n_overlap= find((newneedles(i,1)==newneedles(:,1)) & (newneedles(i,2)==newneedles(:,2)));
    same=n_overlap(:)==i;
    n_overlap(same)=0;
    % Loop if any needles overlap
    if any(n_overlap)
        n_overlap(same)=i;
        % Which needle is closest to shared grid point
        XX= abs(abs(newneedles(n_overlap,1))-abs(needles(n_overlap,1)+dx(n_overlap))); 
        YY= abs(abs(newneedles(n_overlap,2))-abs(needles(n_overlap,2)+dy(n_overlap)));
        RR= XX.^2 + YY.^2;
        % Closest needle stays at grid point
        n_min= find(RR==min(RR));
        n_overlap(n_min(1))= []; % Take out of overlaped category
        XX(n_min(1))= [];     
        YY(n_min(1))= [];
        while ~isempty(n_overlap)   % While there are still overlaped needles
        if min(XX) >= min(YY)       
            ind2= n_overlap(find(YY==min(YY),1));
            open= 1;
            while open > 0.5
                % If open move needle to adjacent X position
                ind3= (newneedles(ind2,1)-open==newneedles(:,1)) & (newneedles(ind2,2)==newneedles(:,2));
                if ~any(ind3)
                    newneedles(ind2,1)= newneedles(ind2,1)-open;
                    open= 0;
                else
                    ind4= (newneedles(ind2,1)+open==newneedles(:,1)) & (newneedles(ind2,2)==newneedles(:,2));
                    if ~any(ind4)
                        newneedles(ind2,1)= newneedles(ind2,1)+open;
                        open=0;
                    elseif isempty(n_overlap)
                        open=0;
                    else
                        % If both adjacent positions are not open move to
                        % adjacent + 1
                        open=open+1;
                    end
                end
                n_overlap(YY==min(YY))=[];
                XX(YY==min(YY))=[];
                YY(YY==min(YY))=[];
            end
            
        else
            ind2= n_overlap(find(XX==min(XX),1));
            open= 1;
            while open > 0.5
                ind3= (newneedles(ind2,1)==newneedles(:,1)) & (newneedles(ind2,2)-open==newneedles(:,2));
                if ~any(ind3)
                    newneedles(ind2,2)= newneedles(ind2,2)-open;
                    open= 0;
                else
                    ind4= (newneedles(ind2,1)==newneedles(:,1)) & (newneedles(ind2,2)+open==newneedles(:,2));
                    if ~any(ind4)
                        newneedles(ind2,2)= newneedles(ind2,2)+open;
                        open=0;
                    elseif isempty(n_overlap)
                        open=0;
                    else
                        open=open+1;
                    end
                end
                n_overlap(XX==min(XX))=[];
                YY(XX==min(XX))=[];
                XX(XX==min(XX))=[];                
            end
        end
        end
    end

end

%--- Make sure seeds don't overlap
mm=0;
for i= 1:Nneedles
    mn= mm + num_ndl(i);
    newseeds(mm+1:mn,1)= newneedles(i,1);
    newseeds(mm+1:mn,2)= newneedles(i,2);   
    for ii= mm+1:mn
    % Indices of seeds that overlap in newseeds
    s_overlap= find((newseeds(ii,3)==newseeds(mm+1:mn,3)));
    same=s_overlap(:)==ii-mm;
    s_overlap(same)=0;
    % Loop if any seeds in the needle overlap
    if any(s_overlap)
        s_overlap(same)=i;
        % Which seed is closest to shared grid point
        ZZ= abs(abs(newseeds(s_overlap,1))-abs(seeds(s_overlap,1)+dz(s_overlap))); 
        % Closest seed stays at grid point 
        s_min= find(ZZ==min(ZZ));
        s_overlap(s_min(1))= []; % Take out of overlaped category
        ZZ(s_min(1))= [];
        while ~isempty(s_overlap)   % While there are still overlaped seeds      
            ind2= s_overlap(find(ZZ==min(ZZ),1));
            open= 1;
            while open > 0.5
                % If open move seed to adjacent X position
                ind3= (newseeds(ind2,3)-open==newseeds(:,3));
                if ~any(ind3)
                    newseeds(ind2,3)= newseeds(ind2,3)-open;
                    open= 0;
                else
                    ind4= (newseeds(ind2,3)+open==newseeds(:,3));
                    if ~any(ind4)
                        newseeds(ind2,3)= newseeds(ind2,3)+open;
                        open=0;
                    else
                        % If both adjacent positions are not open move to
                        % adjacent + 1
                        open=open+1;
                    end
                end
                s_overlap(ZZ==min(ZZ))=[];
                ZZ(ZZ==min(ZZ))=[];
            end
        end
    end
    end
    
    mm=mn;
end