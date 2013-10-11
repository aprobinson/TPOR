
function [newseeds, newneedles] = Overlap(seeds, newseeds, needles, newneedles, num_ndl, mm, n, d_seeds, d_needle)

%--- Make sure needle positions don't overlap

% Indices of needles that overlap in newneedles
n_overlap= find((newneedles(n,1)==newneedles(:,1)) & (newneedles(n,2)==newneedles(:,2)));
same=n_overlap(:)==n;
n_overlap(same)=[];
% Loop if any needles overlap
if any(n_overlap)
    % Which needle is closest to shared grid point
    XX= abs(abs(newneedles(n,1))-abs(needles(n,1)+d_needle(1))); 
    YY= abs(abs(newneedles(n,2))-abs(needles(n,2)+d_needle(2)));
    % Move needle to next closest grid point 
    open= 1;
    if XX >= YY      
        while open > 0.5
            % If open move needle to adjacent X position
            ind1= (newneedles(n,1)-open==newneedles(:,1)) & (newneedles(n,2)==newneedles(:,2));
            if ~any(ind1)
                newneedles(n,1)= newneedles(n,1)-open;
                open= 0;
            else
                ind2= (newneedles(n,1)+open==newneedles(:,1)) & (newneedles(n,2)==newneedles(:,2));
                if ~any(ind2)
                    newneedles(n,1)= newneedles(n,1)+open;
                    open=0;
                else
                    open=open+1;
                end
            end
        end

    else
        while open > 0.5
            ind1= (newneedles(n,1)==newneedles(:,1)) & (newneedles(n,2)-open==newneedles(:,2));
            if ~any(ind1)
                newneedles(n,2)= newneedles(n,2)-open;
                open= 0;
            else
                ind2= (newneedles(n,1)==newneedles(:,1)) & (newneedles(n,2)+open==newneedles(:,2));
                if ~any(ind2)
                    newneedles(n,2)= newneedles(n,2)+open;
                    open=0;
                else
                    open=open+1;
                end
            end                
        end
    end
end


%--- Make sure seeds don't overlap
mn= mm + num_ndl(n);
newseeds(mm+1:mn,1)= newneedles(n,1);
newseeds(mm+1:mn,2)= newneedles(n,2);

for ii= mm+1:mn
% Indices of seeds that overlap in newseeds
s_overlap= find((newseeds(ii,3)==newseeds(mm+1:mn,3)));
same=s_overlap(:)==ii-mm;
s_overlap(same)=0;
% Loop if any seeds in the needle overlap
if any(s_overlap)
    s_overlap(same)=ii-mm;
%     A= d_seeds(s_overlap);
%     B= seeds(s_overlap+mm,3);
%     C= newseeds(s_overlap+mm,3);
%     DD= s_overlap+mm;
    % Which seed is closest to shared grid point
    ZZ= abs(abs(newseeds(s_overlap+mm,3))-abs(seeds(s_overlap+mm,3)+d_seeds(s_overlap))); 
    % Closest seed stays at grid point 
    s_overlap(ZZ==min(ZZ))= []; % Take out of overlaped category
    ZZ(ZZ==min(ZZ))= [];
    while ~isempty(s_overlap)   % While there are still overlaped seeds      
        ind2= s_overlap(find(ZZ==min(ZZ),1));
        open= 1;
        while open > 0.5
            % If open move seed to adjacent Z position
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