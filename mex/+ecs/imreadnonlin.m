%% Load a TIFF movie from disk and apply nonlinear motion correction on-the-fly
function movie = imreadnonlin( inputPath, mcorr, doParallel, gridUpsample )
  
  %% Default arguments
  if nargin < 3
    doParallel          = ~isempty(gcp('nocreate'));
  end
  if nargin < 4
%     gridUpsample        = [4 4];
    gridUpsample        = [2 2];
%     gridUpsample        = [1 1];
  elseif numel(gridUpsample) < 2
    gridUpsample        = [gridUpsample gridUpsample];
  end
  
  %% Upsample patch-based shifts
  nRows                 = mcorr.inputSize(1);
  nCols                 = mcorr.inputSize(2);
  nFrames               = mcorr.inputSize(end);
  gridSize              = [numel(mcorr.yCenter), numel(mcorr.xCenter)] .* gridUpsample;
  
  xShifts               = mcorr.xShifts;
  yShifts               = mcorr.yShifts;
%   xShifts               = imgaussfilt(mcorr.xShifts, 0.5);
%   yShifts               = imgaussfilt(mcorr.yShifts, 0.5);
  patchXShifts          = upsamplePatchShifts(xShifts, gridSize);
  patchYShifts          = upsamplePatchShifts(yShifts, gridSize);

  %% The image warping is defined using the (upsampled) centers of the patches
  xCenter               = [1, round(linspace(mcorr.xCenter(1), mcorr.xCenter(end), gridSize(2))), nCols];
  yCenter               = [1, round(linspace(mcorr.yCenter(1), mcorr.yCenter(end), gridSize(1))), nRows]';
  patchXLoc             = bsxfun(@plus, patchXShifts, xCenter);
  patchYLoc             = bsxfun(@plus, patchYShifts, yCenter);
  
  
  %% Read input movie
  if ischar(inputPath)
    movie               = cv.imreadx(inputPath, mcorr.rigid.xShifts, mcorr.rigid.yShifts);
  else
    movie               = inputPath;
    movie               = cv.imtranslatex(movie, mcorr.rigid.xShifts(:,end), mcorr.rigid.yShifts(:,end));
  end

  %% The rest of this is performed using MEX code for speed
  if doParallel
    pool                = gcp('nocreate');
    
    %% Parallelize application of shifts by chunking the movie into subsets of frames
    frameChunks         = round(linspace(1, nFrames+1, pool.NumWorkers + 1));
    chunkSize           = diff(frameChunks);
    movie               = mat2cell(movie, nRows, nCols, chunkSize);
    patchXLoc           = mat2cell(patchXLoc, numel(yCenter), numel(xCenter), chunkSize);
    patchYLoc           = mat2cell(patchYLoc, numel(yCenter), numel(xCenter), chunkSize);
    parfor iChunk = 1:numel(movie)
      movie{iChunk}     = ecs.barycentricMeshWarp(movie{iChunk}, xCenter, yCenter, patchXLoc{iChunk}, patchYLoc{iChunk});
    end
    movie               = cat(3, movie{:});
    
  else
    movie               = ecs.barycentricMeshWarp(movie, xCenter, yCenter, patchXLoc, patchYLoc);
  end
  
end

%---------------------------------------------------------------------------------------------------
function shifts = upsamplePatchShifts(origShifts, gridSize)
  
  %% Bi-cubic interpolation between patch centers
  numFrames                 = size(origShifts,3);
  shifts                    = nan([gridSize + 2, numFrames], 'like', origShifts);
  shifts(2:end-1,2:end-1,:) = imresize( origShifts, gridSize, 'bicubic' );
  
  %% Assume constant extrapolations up to the borders of the movie
  shifts(2:end-1,1,:)       = shifts(2:end-1,2,:);
  shifts(2:end-1,end,:)     = shifts(2:end-1,end-1,:);
  shifts(1,2:end-1,:)       = shifts(2,2:end-1,:);
  shifts(end,2:end-1,:)     = shifts(end-1,2:end-1,:);
  shifts(1,[1 end],:)       = shifts(2,[1 end],:);
  shifts(end,[1 end],:)     = shifts(end-1,[1 end],:);
  
end
