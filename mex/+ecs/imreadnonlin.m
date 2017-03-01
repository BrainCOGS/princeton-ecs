%% Load a TIFF movie from disk and apply nonlinear motion correction on-the-fly
%
% movie is the nonlinearly corrected output.
% rigid is corrected only up to whole-frame translations.
%
function [movie, rigid] = imreadnonlin( inputPath, mcorr, frameSkip, doParallel, gridUpsample )
  
  %% Default arguments
  if nargin < 3 || isempty(frameSkip)
    frameSkip           = [0 0];
  end
  if nargin < 4
    doParallel          = ~isempty(gcp('nocreate'));
  end
  if nargin < 5
%     gridUpsample        = [4 4];
    gridUpsample        = [2 2];
%     gridUpsample        = [1 1];
  elseif numel(gridUpsample) < 2
    gridUpsample        = [gridUpsample gridUpsample];
  end
  clearRigid            = nargout < 2;
  
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
  
  if any(~isfinite(patchXShifts(:))) || any(~isfinite(patchYShifts(:)))
    error('imreadnonlin:input', 'Non-finite shifts encountered for one or more patch centers.');
  end
  

  %% The image warping is defined using the (upsampled) centers of the patches
  xCenter               = [1, round(linspace(mcorr.xCenter(1), mcorr.xCenter(end), gridSize(2))), nCols];
  yCenter               = [1, round(linspace(mcorr.yCenter(1), mcorr.yCenter(end), gridSize(1))), nRows]';
  patchXLoc             = bsxfun(@plus, patchXShifts, xCenter);
  patchYLoc             = bsxfun(@plus, patchYShifts, yCenter);
  
  
  %% Read input movie
  if ischar(inputPath)
    rigid               = cv.imreadx(inputPath, mcorr.rigid.xShifts, mcorr.rigid.yShifts, 1, 1, frameSkip);
  else
    rigid               = inputPath;
    if any(frameSkip ~= 0)
      rigid             = rigid(:,:,1 + frameSkip(1): 1 + frameSkip(2):end);
    end
    rigid               = cv.imtranslatex(rigid, mcorr.rigid.xShifts(:,end), mcorr.rigid.yShifts(:,end));
  end
  
  patchXLoc             = cast(patchXLoc, 'like', rigid);
  patchYLoc             = cast(patchYLoc, 'like', rigid);
  

  %% The rest of this is performed using MEX code for speed
  if doParallel
    pool                = gcp('nocreate');
    
    %% Parallelize application of shifts by chunking the movie into subsets of frames
    frameChunks         = round(linspace(1, nFrames+1, pool.NumWorkers + 1));
    chunkSize           = diff(frameChunks);
    movie               = mat2cell(rigid, nRows, nCols, chunkSize);
    if clearRigid
      clear rigid;
    end
    
    patchXLoc           = mat2cell(patchXLoc, numel(yCenter), numel(xCenter), chunkSize);
    patchYLoc           = mat2cell(patchYLoc, numel(yCenter), numel(xCenter), chunkSize);
    parfor iChunk = 1:numel(movie)
      movie{iChunk}     = ecs.barycentricMeshWarp(movie{iChunk}, xCenter, yCenter, patchXLoc{iChunk}, patchYLoc{iChunk});
    end
    movie               = cat(3, movie{:});
    
  else
    movie               = ecs.barycentricMeshWarp(rigid, xCenter, yCenter, patchXLoc, patchYLoc);
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
