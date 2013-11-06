%% mkMask_updated.m
% Updated: 3/15/2013

% --------------------------- Overview ------------------------------------
%
% This Matlab script reads in a '.img' file and allows the user to define
% specific regions of interest, corresponding to organs, for each vertical
% slice. It then reads through the defined regions of interest and creates
% a coarser voxel mesh that can be used by the Greedy Heuristic.
%
% ------------------------- End Overview ----------------------------------

% --------------------------- Bug List ------------------------------------
%
% The old version of this file parses some specific regions as 5x5 (25 pix),
% 5x6 (30 pix), 6x5 (30 pix), and 6x6 (36 pix). All of these had very
% similar weighting parameters to determine which organ the new voxel grain
% will be.
%
% --------------------------- End Bug List --------------------------------

% Initial file read and file constraints

filename  = 'baron.img';
pixelX    = 640;            % Number of pixels in the x direction
pixelY    = 480;            % Number of pixels in the y direction
numSlices = 13;             % Number of slices (images) in z direction

% Voxel size definitions (i.e. output matrix dimensions)
voxelX    = 66;
voxelY    = 61;

fid = fopen(filename,'r');                  % Open selected file
F   = fread(fid);                           % Read into single column vector
ROI = reshape(F,pixelX,pixelY,numSlices);  % Construct matrix from F
status = fclose(fid);                       % Close the file
clear ('F');                                % Deallocate file colum vector

targetSc = zeros(pixelX,pixelY,numSlices);     % Used for target mask
urethraSc = zeros(pixelX,pixelY,numSlices);    % Used for urethra mask
rectumSc = zeros(pixelX,pixelY,numSlices);     % Used for rectum mask
marginSc = zeros(pixelX,pixelY,numSlices);     % Used for margin mask

target  = zeros(voxelX,voxelY,numSlices);     % Returned target matrix
urethra = zeros(voxelX,voxelY,numSlices);     % Returned urethra matrix
rectum  = zeros(voxelX,voxelY,numSlices);     % Returned rectum matrix
margin  = zeros(voxelX,voxelY,numSlices);     % Returned margin matrix

% Begin construction of masks for each individual slice

for i = 1:numSlices
    
    % Initialize the image of the current slice
    figure(i), imagesc(ROI(:,:,i)');
    colormap(gray);	hold on;
    txtSc= int2str(i);
    
    % Carry over the contours from the previous slice to better inform the
    % user of their previous choices.
    if (i > 1)
        [c, h]= contour(targetSc(:,:,i-1)',1,'w');
        [c, h]= contour(marginSc(:,:,i-1)',1,'w');
        [c, h]= contour(rectumSc(:,:,i-1)',1,'w');
    end
    
    % Display image in figure(sc) and prompt the user to construct the
    % specified masks.
    fprintf('Current Slice: #%d\n',i);
    
    % Prompt the user to construct the target mask
    fprintf('Current Countour: TargetSc#%d\n',i); %
    temp(:,:)= roipoly;	targetSc(:,:,i)= double(temp(:,:)');
    [c, h]= contour(targetSc(:,:,i)',1,'r');
    
    % Prompt the user to construct the urethra mask
    fprintf('Current Countour: UrethraSc#%d\n',i);
    temp(:,:)= roipoly;	urethraSc(:,:,i)= double(temp(:,:)');
    [c, h]= contour(urethraSc(:,:,i)',1,'y');
    
    % Prompt the user to construct the rectum mask
    fprintf('Current Countour: RectumSc#%d\n',i);
    temp(:,:)= roipoly;	rectumSc(:,:,i)= double(temp(:,:)');
    [c, h]= contour(rectumSc(:,:,i)',1,'g');
    
    % Prompt the user to construct the margin mask
    fprintf('Current Countour: MarginSc#%d\n',i);
    temp(:,:)= roipoly;	marginSc(:,:,i)= double(temp(:,:)');
    clear ('temp');
    
    % Clean up any overlapping regions of the masks such that each position
    % exists at most in one mask.
    
    % Margin Cleanup -> Margin = MarginTotal - Target - Rectum;
    temp= marginSc(:,:,i) - targetSc(:,:,i) - rectumSc(:,:,i);
    marginSc(:,:,i)= (temp==1);
    clear temp;
    
    % Target Cleanup (1) -> Target = TargetTotal - Urethra;
    temp= targetSc(:,:,i) - urethraSc(:,:,i);
    targetSc(:,:,i)= temp;
    clear temp;
    
    % Target Cleanup (2) -> Target = Target(1) - Overlapping Rectum;
    temp= ((targetSc(:,:,i) + rectumSc(:,:,i))==2);
    targetSc(:,:,i)= targetSc(:,:,i) - temp;
    clear temp;
    
    % Clean masks have now been created, but must be resized in order to
    % fit the voxel parameters of the problem. This will be done by reading
    % through preset regions of field of view and parameterizing them based
    % on how many cells from each mask lie therein.
    %
    % Example:
    % A = [ 1 1 2 3;
    %       1 1 3 3;
    %       2 1 2 4;
    %       2 3 4 4 ]
    %
    % If this is designed to be parameterized into a 2x2 matrix B, the
    % following matrix B would be constructed:
    %
    % B = [ 1 3;
    %       2 4 ]
    %
    % Note that the actual weighting scheme is slightly different and can
    % be seen below for a typical 5x5 (25 pixel) array.
    %
    % Classification  |  Condition
    % ----------------|-------------
    % Urethra         | numUrethra > 7
    % Rectum          | numRectum  > 10
    % Margin          | numMargin  > 10
    % Target          | numTarget  > 12
    
    % Initialize a more narrow FOV (Field of View) that will still capture
    % the organs/features that are desired.
    % field-of-view (FOV) 330x305xKK
    
    xPosStart= 112;	  % Intiial X Position
    yPosStart= 127;   % Initial Y Position
    
    widthX = 330;	  % Width (pixels) in X
    widthY = 305;     % Width (pixels) in Y
    
    % Define temporary arrays in our new FOV from the previous ROI masks
    tempTa(:,:)= targetSc(xPosStart:xPosStart+widthX-1,yPosStart:yPosStart+widthY-1,i);
    tempUr(:,:)= urethraSc(xPosStart:xPosStart+widthX-1,yPosStart:yPosStart+widthY-1,i);
    tempRe(:,:)= rectumSc(xPosStart:xPosStart+widthX-1,yPosStart:yPosStart+widthY-1,i);
    tempMa(:,:)= marginSc(xPosStart:xPosStart+widthX-1,yPosStart:yPosStart+widthY-1,i);
    
    for y = 1:voxelY

        for x = 1:voxelX
            
            % Tally the number of each ROI in the selected 5x5 region
            numTarget  = 0;	
            numUrethra = 0;	
            numRectum  = 0;	
            numMargin  = 0;
            grain      = 5;
            
            % Search in the 5x5 (25 pixel) region space which will then
            % be counted as a single voxel
            
            currentY = ((y-1)*grain) + 1;
            currentX = ((x-1)*grain) + 1;
            
            fineX = currentX + 4;    % Ensure that the upper left corner	
            fineY = currentY + 4;    % is in the correct position
            
            for j = currentY:fineY
                for ii = currentX:fineX
                    
                    % Check to see what is filling that region
                    if tempUr(ii,j)~=0
                        numUrethra = numUrethra + 1;
                        
                    elseif tempRe(ii,j)~=0
                        numRectum= numRectum+1;
                        
                    elseif tempTa(ii,j)~=0
                        numTarget= numTarget+1;
                        
                    elseif tempMa(ii,j)~=0
                        numMargin= numMargin+1;
                    end
                    
                end
            end
            
            % Determine the ROI to classify the current voxel as
            if numUrethra > 7
                urethra(x,y,i) = true;     % Set as Urethra
                target(x,y,i)  = false;
                rectum(x,y,i)  = false;		
                margin(x,y,i)  = false;
                
            elseif numRectum > 10
                urethra(x,y,i) = false;	
                target(x,y,i)  = false;
                rectum(x,y,i)  = true;     % Set as Rectum
                margin(x,y,i)  = false;
                
            elseif numMargin > 10
                urethra(x,y,i) = false;	
                target(x,y,i)  = false;
                rectum(x,y,i)  = false;		
                margin(x,y,i)  = true;     % Set as Margin
                
            elseif numTarget > 12
                urethra(x,y,i) = false;	
                target(x,y,i)  = true;     % Set as Target
                rectum(x,y,i)  = false;		
                margin(x,y,i)  = false;
            end
            
        end       % Close x = 1:voxelX
    end       % Close y = 1:voxelY 
    
    hold off;
    close;
    
end    % Completed all of the slices in the image

% Save matrix file in the requested output format
save mask3D2 target urethra rectum margin
