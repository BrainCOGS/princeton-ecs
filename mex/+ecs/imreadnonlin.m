%% Load a TIFF movie from disk and apply nonlinear motion correction on-the-fly
function movie = imreadnonlin( inputPath, mcorr, gridUpsample )
  
  %% Default arguments
  if nargin < 3
    gridUpsample        = [4 4];
  elseif numel(gridUpsample) < 2
    gridUpsample        = [gridUpsample gridUpsample];
  end
  
  %%
  nRows                 = mcorr.inputSize(1);
  nCols                 = mcorr.inputSize(2);
  nFrames               = mcorr.inputSize(end);
  gridSize              = [numel(mcorr.yCenter), numel(mcorr.xCenter)] .* gridUpsample;
  patchXShifts          = upsamplePatchShifts(mcorr.xShifts, gridSize);
  patchYShifts          = upsamplePatchShifts(mcorr.xShifts, gridSize);
  
  xCenter               = [1, round(linspace(mcorr.xCenter(1), mcorr.xCenter(end), gridSize(2))), nCols];
  yCenter               = [1, round(linspace(mcorr.yCenter(1), mcorr.yCenter(end), gridSize(1))), nRows]';
  patchNx               = diff(xCenter);
  patchNy               = diff(yCenter);
  
  
  %% Read input movie
  if ischar(inputPath)
    movie               = cv.imreadx(inputPath, mcorr.rigid.xShifts, mcorr.rigid.yShifts);
  else
    movie               = inputPath;
    movie               = cv.imtranslatex(movie, mcorr.rigid.xShifts(:,end), mcorr.rigid.yShifts(:,end));
  end
  
  %%
%   profile on
  tic
  corrected             = cell(1,1,nFrames);
  for iFrame = 1:nFrames
    %%
    xLoc                = bsxfun(@plus, patchXShifts(:,:,iFrame), xCenter);
    yLoc                = bsxfun(@plus, patchYShifts(:,:,iFrame), yCenter);
%     idx                 = knnsearch([yLoc(:),xLoc(:)], queryGrid);

    %% 
    for iX = 1:numel(xCenter)-1
      for iY = 1:numel(yCenter)-1
        %% Bounding coordinate rectangle
        patchX          = xLoc(iX:iX+1,iY:iY+1);
        patchY          = yLoc(iX:iX+1,iY:iY+1);
        boundX          = [ceil(min(patchX(:))), floor(max(patchX(:)))];
        boundY          = [ceil(min(patchY(:))), floor(max(patchY(:)))];
        
        %%
        x21             = patchX(:,2) - patchX(:,1);
        x21             = (x21(1) + x21(2)) / 2 / patchNx(iX);
        y21             = patchY(:,2) - patchY(:,1);
        y21             = (y21(1) + y21(2)) / 2 / patchNy(iY);
        x31             = patchX(2,:) - patchX(1,:);
        x31             = (x31(1) + x31(2)) / 2 / patchNx(iX);
        y31             = patchY(2,:) - patchY(1,:);
        y31             = (y31(1) + y31(2)) / 2 / patchNy(iY);
        
        %% All points inside rectangle
        Px              = boundX(1):boundX(2);
        Py              = boundY(1):boundY(2);
        Px              = Px - ( patchX(1,1) + x21*(0:patchNx(iX)-1) );
        Py              = Py - ( patchY(1,1) + y31*(0:patchNy(iX)-1) );
        
%         [Px,Py]         = meshgrid(boundX(1):boundX(2), boundY(1):boundY(2));
        
        %%
%         [x21, x31; y21, y31]
       
      end
    end

    
    %%
    %{
    for iRow = 1:nRows
      for iCol = 1:nCols-1
        a = movie(iRow,iCol,iFrame);
        a = movie(iRow,iCol+1,iFrame);
      end
    end
    %}
  end
  toc
%   profile viewer
  
end

%---------------------------------------------------------------------------------------------------
function shifts = upsamplePatchShifts(origShifts, gridSize)
  
  numFrames                 = size(origShifts,3);
  shifts                    = nan([gridSize + 2, numFrames], 'like', origShifts);
  shifts(2:end-1,2:end-1,:) = imresize( origShifts, gridSize, 'bicubic' );
  shifts(2:end-1,1,:)       = shifts(2:end-1,2,:);
  shifts(2:end-1,end,:)     = shifts(2:end-1,end-1,:);
  shifts(1,2:end-1,:)       = shifts(2,2:end-1,:);
  shifts(end,2:end-1,:)     = shifts(end-1,2:end-1,:);
  shifts(1,[1 end],:)       = shifts(2,[1 end],:);
  shifts(end,[1 end],:)     = shifts(end-1,[1 end],:);
  
end
