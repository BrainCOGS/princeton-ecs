function mcorr = nonlinearMotionCorrect(inputPath, maxShift, maxIter, stopBelowShift, medianRebin, patchSize, numPatches, maxShiftDifference, smoothness)
     
  %% Default arguments
  if nargin < 2
    maxShift            = [15 15];
  elseif numel(maxShift) < 2
    maxShift            = [maxShift, maxShift];
  end
  if nargin < 3
    maxIter             = [5 2];
  elseif numel(maxIter) < 2
    maxIter(end+1)      = 2;
  end
  if nargin < 4
    stopBelowShift      = 0;
  end
  if nargin < 5
    medianRebin         = 10;
  end
  if nargin < 6
%     patchSize           = [129 171];
%     patchSize           = [255 255];
    patchSize           = [151 151];
%     patchSize           = [127 127];
%     patchSize           = [101 101];
  elseif numel(patchSize) < 2
    patchSize           = [patchSize patchSize];
  end
  if nargin < 7
%     numPatches          = [3 3];
    numPatches          = [6 6];
%     numPatches          = [7 7];
%     numPatches          = [8 8];
%     numPatches          = [10 10];
  elseif numel(patchSize) < 2
    numPatches          = [numPatches numPatches];
  end
  if nargin < 8
    maxShiftDifference  = 1;
  end
  if nargin < 9
    smoothness          = 0.5;
  end
  
  
  %% Read input movie
  if ischar(inputPath)
    movie               = cv.imreadx(inputPath);
  else
    movie               = inputPath;
  end

  %% Rigid motion correction
  mcorr                 = struct();
  mcorr.rigid           = cv.motionCorrect(movie, maxShift(1), maxIter(1), false, stopBelowShift, nan, medianRebin);
  movie                 = cv.imtranslatex(movie, mcorr.rigid.xShifts(:,end), mcorr.rigid.yShifts(:,end));
  mcorr.rigid.cropping  = getMovieCropping(mcorr.rigid);
  
  
  %% 
  inputSize             = size(movie);
  numFrames             = inputSize(end);
  patchSpan             = cell(size(patchSize));
  patchCenter           = cell(size(patchSize));
  for iDim = 1:numel(patchSize)
    startRange          = [mcorr.rigid.cropping.range(iDim,1), mcorr.rigid.cropping.range(iDim,2) - patchSize(iDim) + 1];
    patchStart          = linspace(startRange(1), startRange(2), numPatches(iDim));
    patchStart          = min(max(round(patchStart), startRange(1)), startRange(2));
    patchSpan{iDim}     = bsxfun(@plus, (0:patchSize(iDim)-1)', patchStart);
    patchCenter{iDim}   = ( patchSpan{iDim}(1,:) + patchSpan{iDim}(end,:) ) / 2;
  end
  [patchX, patchY]      = meshgrid(patchCenter{2}, patchCenter{1});
  
  %%
  moviePatch            = cell(numPatches);
  for iRow = 1:numPatches(1)
    for iCol = 1:numPatches(2)
      moviePatch{iRow,iCol} = movie( patchSpan{1}(:,iRow), patchSpan{2}(:,iCol), : );
    end
  end
  clear movie;
  
  %%
  patchCorr             = cell(size(moviePatch));
  refPatch              = cell(numPatches);
  reference             = mcorr.rigid.reference;
  for iIter = 1:maxIter(2)

    %%
    for iRow = 1:numPatches(1)
      for iCol = 1:numPatches(2)
        refPatch{iRow,iCol}   = reference( patchSpan{1}(:,iRow), patchSpan{2}(:,iCol) );
      end
    end
    
    %%
    parfor iPatch = 1:numel(patchCorr)
      patchCorr{iPatch} = cv.motionCorrect( {moviePatch{iPatch},refPatch{iPatch}}, maxShift(2), 1, false, 0, nan, medianRebin, [0 0], false);
      corrected         = cv.imtranslatex(moviePatch{iPatch}, patchCorr{iPatch}.xShifts(:,end), patchCorr{iPatch}.yShifts(:,end));
      corrected         = rebin(corrected, medianRebin, 3);
      refPatch{iPatch}  = median(corrected, 3, 'omitnan');
    end
    
    %%
    reference           = nan([size(mcorr.rigid.reference),size(patchCorr)], 'like', reference);
    for iRow = 1:numPatches(1)
      for iCol = 1:numPatches(2)
        reference(patchSpan{1}(:,iRow), patchSpan{2}(:,iCol),iRow,iCol)   ...
                        = refPatch{iRow,iCol};
      end
    end
    reference           = median(reference(:,:,:), 3, 'omitnan');
    reference(isnan(reference))           ... OpenCV algorithms typically cannot handle NaNs
                        = mcorr.rigid.params.emptyValue;
    
  end
  
  
  %%
  patchXShifts          = single(reshape( accumfun(1, @(x) x.xShifts(:,end)', patchCorr), [numPatches,numFrames] ));
  patchYShifts          = single(reshape( accumfun(1, @(x) x.yShifts(:,end)', patchCorr), [numPatches,numFrames] ));
  minXDiff              = minNeighborDifference(patchXShifts);
  minYDiff              = minNeighborDifference(patchYShifts);
  tooDifferent          = minXDiff > maxShiftDifference | minYDiff > maxShiftDifference;
  index                 = find(tooDifferent(:));
  
  %%
  for iDiff = 1:numel(index)
    %%
    [iRow,iCol,iFrame]  = ind2sub(size(tooDifferent), index(iDiff));
    metric              = patchCorr{iRow,iCol}.metric.values(:,:,iFrame);
    if iRow > 1                                             % N
      metric(:,:,end+1) = patchCorr{iRow-1,iCol}.metric.values(:,:,iFrame);
    end
    if iRow < size(patchCorr,1)                             % S
      metric(:,:,end+1) = patchCorr{iRow+1,iCol}.metric.values(:,:,iFrame);
    end
    if iCol < size(patchCorr,2)                             % E
      metric(:,:,end+1) = patchCorr{iRow,iCol+1}.metric.values(:,:,iFrame);
    end
    if iCol > 1                                             % W
      metric(:,:,end+1) = patchCorr{iRow,iCol-1}.metric.values(:,:,iFrame);
    end
    
    %%
    comboMetric         = (1 - smoothness)*metric(:,:,1) + smoothness*mean(metric(:,:,2:end), 3);
    [~,iMax]            = max(comboMetric(:));
    [yMax, xMax]        = ind2sub(size(comboMetric), iMax);
    
    %% 1D Gaussian interpolation in each direction
    if xMax > 1
      ln10              = log(comboMetric(yMax, xMax - 1));
    else
      ln10              = nan;
    end
    ln11                = log(comboMetric(yMax, xMax));
    if xMax < size(comboMetric,2)
      ln12              = log(comboMetric(yMax, xMax + 1));
    else
      ln12              = nan;
    end
    if yMax > 1
      ln01              = log(comboMetric(yMax - 1, xMax));
    else
      ln01              = nan;
    end
    if yMax < size(comboMetric,1)
      ln21              = log(comboMetric(yMax + 1, xMax));
    else
      ln21              = nan;
    end
    xPeak               = ( ln10 - ln12 ) / ( 2 * ln10 - 4 * ln11 + 2 * ln12 );
    yPeak               = ( ln01 - ln21 ) / ( 2 * ln01 - 4 * ln11 + 2 * ln21 );
    if isnan(xPeak)
      xPeak             = 0;
    end
    if isnan(yPeak)
      yPeak             = 0;
    end
    
    %%
%     patchCorr{iRow,iCol}.metric.values(:,:,iFrame)  = comboMetric;
    patchXShifts(iRow,iCol,iFrame)  = -( xMax - ceil(size(comboMetric,2)/2) + xPeak );
    patchYShifts(iRow,iCol,iFrame)  = -( yMax - ceil(size(comboMetric,1)/2) + yPeak );
  end

  
  %% Ensure that the order of patches are preserved
  badShift              = false(size(patchCorr));
  for iFrame = 1:numFrames
    badXShift           = checkRelativeShifts( patchXShifts(:,:,iFrame) + patchX, 2 );
    badYShift           = checkRelativeShifts( patchYShifts(:,:,iFrame) + patchY, 1 );
    badShift            = badShift | badXShift | badYShift;
  end
  
  nBadPatches           = sum(badShift(:));
  if nBadPatches > 0
    error('nonlinearMotionCorrect:relativeShifts', '%d/%d patches (%dx%d pixels) had shifts that would invert their x/y positional order.', nBadPatches, numel(patchCorr), patchSize(1), patchSize(2));
  end
  
  
  %%
  mcorr.xShifts         = patchXShifts;
  mcorr.yShifts         = patchYShifts;
  mcorr.inputSize       = inputSize;
  mcorr.method          = 'ecs.nonlinearMotionCorrect';
  mcorr.params          = patchCorr{1}.params;
  mcorr.params.emptyValue = cellfun(@(x) x.params.emptyValue, patchCorr);
  mcorr.params.patchSize= patchSize;
  mcorr.metric          = [];           % HACK: not stored
  mcorr.reference       = reference;
  mcorr.xCenter         = patchCenter{2};
  mcorr.yCenter         = patchCenter{1};

end

%%
function badShift = checkRelativeShifts(location, dim)
  
  locDiff               = diff(location, 1, dim);
  badShift              = locDiff <= 0;
  switch dim
    case 1
      badShift(end+1,:) = false;
      badShift(2:end,:) = badShift(2:end,:) | badShift(1:end-1,:);
    case 2
      badShift(:,end+1) = false;
      badShift(:,2:end) = badShift(:,2:end) | badShift(:,1:end-1);
    otherwise
      error('nonlinearMotionCorrect:checkRelativeShifts', 'Unexpected data dimension %d.', dim);
  end

end
