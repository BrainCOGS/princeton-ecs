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
  
  xCenter               = [1, linspace(mcorr.xCenter(1), mcorr.xCenter(end), gridSize(2)), nCols];
  yCenter               = [1, linspace(mcorr.yCenter(1), mcorr.yCenter(end), gridSize(1)), nRows]';
  [xQuery,yQuery]       = meshgrid( 1:nCols, 1:nRows );
  queryGrid             = [xQuery(:), yQuery(:)];
  
  
  %% Read input movie
  if ischar(inputPath)
    movie               = cv.imreadx(inputPath, mcorr.rigid.xShifts, mcorr.rigid.yShifts);
  else
    movie               = inputPath;
    movie               = cv.imtranslatex(movie, mcorr.rigid.xShifts(:,end), mcorr.rigid.yShifts(:,end));
  end
  
  %%
  tic
  corrected             = cell(1,1,nFrames);
  for iFrame = 1:nFrames
    %%
    xLoc                = bsxfun(@plus, patchXShifts(:,:,iFrame), xCenter);
    yLoc                = bsxfun(@plus, patchYShifts(:,:,iFrame), yCenter);
%     idx                 = knnsearch([yLoc(:),xLoc(:)], queryGrid);
    
    for iRow = 1:nRows
      for iCol = 1:nCols-1
        a = movie(iRow,iCol,iFrame);
        a = movie(iRow,iCol+1,iFrame);
      end
    end
  end
  toc
  
end

%---------------------------------------------------------------------------------------------------
function shifts = upsamplePatchShifts(origShifts, gridSize)
  
  numFrames                 = size(origShifts,3);
  shifts                    = nan([gridSize + 2, numFrames]);
  shifts(2:end-1,2:end-1,:) = imresize( origShifts, gridSize, 'bicubic' );
  shifts(2:end-1,1,:)       = shifts(2:end-1,2,:);
  shifts(2:end-1,end,:)     = shifts(2:end-1,end-1,:);
  shifts(1,2:end-1,:)       = shifts(2,2:end-1,:);
  shifts(end,2:end-1,:)     = shifts(end-1,2:end-1,:);
  shifts(1,[1 end],:)       = shifts(2,[1 end],:);
  shifts(end,[1 end],:)     = shifts(end-1,[1 end],:);
  
end
