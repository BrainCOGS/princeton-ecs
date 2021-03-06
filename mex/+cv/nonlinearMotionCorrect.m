function mcorr = nonlinearMotionCorrect(inputPath, maxShift, maxIter, stopBelowShift, medianRebin, frameSkip, patchSize, numPatches, maxShiftDifference, smoothness)
     
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
  if nargin < 6 || isempty(frameSkip)
    frameSkip           = [0 0];
  end
  if nargin < 7
%     patchSize           = [255 255];
%     patchSize           = [171 171];
%     patchSize           = [151 171];
    patchSize           = [151 151];
%     patchSize           = [127 127];
%     patchSize           = [101 101];
  elseif numel(patchSize) < 2
    patchSize           = [patchSize patchSize];
  end
  if nargin < 8
%     numPatches          = [5 5];
%     numPatches          = [6 5];
    numPatches          = [6 6];
%     numPatches          = [7 7];
%     numPatches          = [8 8];
%     numPatches          = [10 10];
  elseif numel(patchSize) < 2
    numPatches          = [numPatches numPatches];
  end
  if nargin < 9
    maxShiftDifference  = 1.5;
  end
  if nargin < 10
    smoothness          = 0.5;
  end
  
  
  %% Read input movie
  if ischar(inputPath)
    movie               = cv.imreadx(inputPath, [], [], 1, 1, frameSkip);
  else
    movie               = inputPath;
    if any(frameSkip ~= 0)
      movie             = movie(:,:,1 + frameSkip(1): 1 + frameSkip(2):end);
    end
  end
  
  %% Apply a log transform so as to be less sensitive to large differences in dynamic range (e.g. activity)
  offset                = min(movie(:)) - 1;
  movie                 = log(movie - offset);
  

  %% Rigid motion correction
  mcorr                 = struct();
  mcorr.rigid           = cv.motionCorrect(movie, maxShift(1), maxIter(1), false, stopBelowShift, nan, medianRebin);
  movie                 = cv.imtranslatex(movie, mcorr.rigid.xShifts(:,end), mcorr.rigid.yShifts(:,end));
  mcorr.rigid.cropping  = getMovieCropping(mcorr.rigid, true, sum(isnan(movie),3) > 0);
  mcorr.rigid.params.frameSkip  = frameSkip;
  
  
  %% Setup overlapping patches for rigid sub-correction
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
  
  %% Slice up movie into chunks
  moviePatch            = cell(numPatches);
  for iRow = 1:numPatches(1)
    for iCol = 1:numPatches(2)
      moviePatch{iRow,iCol} = movie( patchSpan{1}(:,iRow), patchSpan{2}(:,iCol), : );
      if sum(~isfinite(moviePatch{iRow,iCol}(:))) > 0
        error('nonlinearMotionCorrect:input', 'Non-finite values encountered in input movie.');
      end
    end
  end
  clear movie;
  
  %% Iterate over rigid correction steps
  patchCorr             = cell(size(moviePatch));
  refPatch              = cell(numPatches);
  reference             = mcorr.rigid.reference;
  for iIter = 1:maxIter(2)

    %% Create reference image for each patch
    for iRow = 1:numPatches(1)
      for iCol = 1:numPatches(2)
        refPatch{iRow,iCol}   = reference( patchSpan{1}(:,iRow), patchSpan{2}(:,iCol) );
      end
    end
    
    %% Obtain rigid motion correction per patch
    parfor iPatch = 1:numel(patchCorr)
      patchCorr{iPatch} = cv.motionCorrect( {moviePatch{iPatch},refPatch{iPatch}}, maxShift(2), 1, false, 0, nan, medianRebin, [0 0], false);
      corrected         = cv.imtranslatex(moviePatch{iPatch}, patchCorr{iPatch}.xShifts(:,end), patchCorr{iPatch}.yShifts(:,end));
      corrected         = rebin(corrected, medianRebin, 3);
      refPatch{iPatch}  = median(corrected, 3, 'omitnan');
    end
    
    %% Compose whole-field reference as the median across overlapping patches
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
  
  %% Compute "goodness-of-fit" for correlational metric 
  patchXShifts          = single(reshape( accumfun(1, @(x) x.xShifts(:,end)', patchCorr), [numPatches,numFrames] ));
  patchYShifts          = single(reshape( accumfun(1, @(x) x.yShifts(:,end)', patchCorr), [numPatches,numFrames] ));
  shiftGOF              = repmat({nan(1,numFrames)}, prod(numPatches), 1);
  if any(strcmp(mcorr.rigid.metric.name, {'sqDiffNormed','squaredDifference'}))
    localOptimum        = @imregionalmin;
    optimum             = @min;
  else
    localOptimum        = @imregionalmax;
    optimum             = @max;
  end
  
  parfor iPatch = 1:numel(patchCorr)
    patchMetric         = patchCorr{iPatch}.metric.values;
    for iFrame = 1:numFrames
      %%
      metric            = patchMetric(:,:,iFrame);
      metric            = metric(localOptimum(metric));
      [best,iBest]      = optimum(metric);
%       competitors       = optimum( metric([1:iBest-1,iBest+1:end]) );
%       if ~isempty(competitors)
%         competitors     = 1 - competitors/best;
%       competitors       = 1 - metric([1:iBest-1,iBest+1:end])/best;
      competitors       = 1 - metric([1:iBest-1,iBest+1:end])/best;
      shiftGOF{iPatch}(iFrame)    ...
                        = sum(competitors.^(-10)).^(-1/10);
%                         = 1./sum(1./competitors);
%       end
    end
  end
  shiftGOF              = reshape(cat(1, shiftGOF{:}), size(patchXShifts));
  shiftGOF(~isfinite(shiftGOF)) = 1;
  
  %% Weighted contributions
  contribWeight         = zeros(size(shiftGOF));
  contribWeight(2:end,:,:)    = contribWeight(2:end,:,:)   + shiftGOF(1:end-1,:,:);      % N
  contribWeight(1:end-1,:,:)  = contribWeight(1:end-1,:,:) + shiftGOF(2:end,:,:);        % S
  contribWeight(:,2:end,:)    = contribWeight(:,2:end,:)   + shiftGOF(:,1:end-1,:);      % E
  contribWeight(:,1:end-1,:)  = contribWeight(:,1:end-1,:) + shiftGOF(:,2:end,:);        % W
  
  shiftGOF              = num2cell(shiftGOF,3);
  selfMetric            = cellfun(@(x,y) (1-smoothness) * bsxfun(@times, x.metric.values, 1+y), patchCorr, shiftGOF, 'UniformOutput', false);
  otherMetric           = cellfun(@(x,y)    smoothness  * bsxfun(@times, x.metric.values,   y), patchCorr, shiftGOF, 'UniformOutput', false);
  

  %% Reduce difference between neighboring patch shifts if necessary
  for iRow = 1:numPatches(1)
    for iCol = 1:numPatches(2)
      %% Compute modified metric as a weighted sum with neighboring patch metrics
      metric            = zeros(size(selfMetric{iRow,iCol}));
      if iRow > 1                                             % N
        metric          = metric + otherMetric{iRow-1,iCol};
      end
      if iRow < size(patchCorr,1)                             % S
        metric          = metric + otherMetric{iRow+1,iCol};
      end
      if iCol < size(patchCorr,2)                             % E
        metric          = metric + otherMetric{iRow,iCol+1};
      end
      if iCol > 1                                             % W
        metric          = metric + otherMetric{iRow,iCol-1};
      end
      metric            = selfMetric{iRow,iCol} + bsxfun(@rdivide, metric, contribWeight(iRow,iCol,:));
      

      %% Locate global optimum, but prohibiting solutions at the borders
      metric([1 end],:,:) = nan;
      metric(:,[1 end],:) = nan;

      metricSize        = size(metric);
      metric            = reshape(metric,[],numFrames);
      [~,iOptim]        = optimum(metric, [], 1);
      [yOptim, xOptim]  = ind2sub(butlast(metricSize), iOptim);
      lnMetric          = log(metric);


      %% Use 1D Gaussian interpolation in each direction to locate maximum
      ln10                = nan(size(iOptim));
      sel                 = find( xOptim > 1 );
      ln10(sel)           = lnMetric(sub2ind(metricSize, yOptim(sel), xOptim(sel) - 1, sel));
      
      ln11                = lnMetric(sub2ind(metricSize, yOptim, xOptim, 1:numFrames));
      
      ln12                = nan(size(iOptim));
      sel                 = find( xOptim < metricSize(2) );
      ln12(sel)           = lnMetric(sub2ind(metricSize, yOptim(sel), xOptim(sel) + 1, sel));
      
%       ln01                = nan(size(iOptim));
      sel                 = find( yOptim > 1 );
      ln01(sel)           = lnMetric(sub2ind(metricSize, yOptim(sel) - 1, xOptim(sel), sel));

      ln21                = nan(size(iOptim));
      sel                 = find( yOptim < metricSize(1) );
      ln21(sel)           = lnMetric(sub2ind(metricSize, yOptim(sel) + 1, xOptim(sel), sel));

      %%
      xPeak               = ( ln10 - ln12 ) ./ ( 2 * ln10 - 4 * ln11 + 2 * ln12 );
      yPeak               = ( ln01 - ln21 ) ./ ( 2 * ln01 - 4 * ln11 + 2 * ln21 );

      
      %%
      xPeak(isnan(xPeak)) = 0;
      yPeak(isnan(yPeak)) = 0;
      patchXShifts(iRow,iCol,:) = -( xOptim - ceil(metricSize(2)/2) + xPeak );
      patchYShifts(iRow,iCol,:) = -( yOptim - ceil(metricSize(1)/2) + yPeak );
    end
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
    keyboard
    error('nonlinearMotionCorrect:relativeShifts', '%d/%d patches (%dx%d pixels) had shifts that would invert their x/y positional order.', nBadPatches, numel(patchCorr), patchSize(1), patchSize(2));
  end
  
  
  %% Create output structure in the same format as cv.imreadx()
  mcorr.rigid.reference = exp(mcorr.rigid.reference) + offset;
  mcorr.xShifts         = patchXShifts;
  mcorr.yShifts         = patchYShifts;
  mcorr.inputSize       = inputSize;
  mcorr.method          = 'cv.nonlinearMotionCorrect';
  mcorr.params          = patchCorr{1}.params;
  mcorr.params.frameSkip            = frameSkip;
  mcorr.params.emptyValue           = cellfun(@(x) x.params.emptyValue, patchCorr);
  mcorr.params.patchSize            = patchSize;
  mcorr.params.numPatches           = numPatches;
  mcorr.params.maxShiftDifference   = maxShiftDifference;
  mcorr.params.smoothness           = smoothness;
  mcorr.metric          = [];           % HACK: not stored
  mcorr.reference       = exp(reference) + offset;
  mcorr.xCenter         = patchCenter{2};
  mcorr.yCenter         = patchCenter{1};

end

%---------------------------------------------------------------------------------------------------
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
