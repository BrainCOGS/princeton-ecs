function mc = nonlinearMotionCorrect(movie, cropping, maxShift, maxIter, stopBelowShift, patchSize, numPatches)
     
  %% Default arguments
  if nargin < 5
    stopBelowShift      = 0;
  end
  if nargin < 6
    patchSize           = [128 128];
%     patchSize           = [100 100];
  elseif numel(patchSize) < 2
    patchSize           = [patchSize patchSize];
  end
  if nargin < 7
    numPatches          = [10 10];
  elseif numel(patchSize) < 2
    numPatches          = [numPatches numPatches];
  end
  

  %% 
  inputSize             = size(movie);
  numFrames             = inputSize(end);
  patchSpan             = cell(size(patchSize));
  for iDim = 1:numel(patchSize)
    startRange          = [cropping.range(iDim,1), cropping.range(iDim,2) - patchSize(iDim) + 1];
    patchStart          = linspace(startRange(1), startRange(2), numPatches(iDim));
    patchStart          = min(max(round(patchStart), startRange(1)), startRange(2));
    patchSpan{iDim}     = bsxfun(@plus, (0:patchSize(iDim)-1)', patchStart);
  end
  
  %%
  moviePatch            = cell(numPatches);
  for iRow = 1:numPatches(1)
    for iCol = 1:numPatches(2)
      moviePatch{iRow,iCol} = movie( patchSpan{1}(:,iRow), patchSpan{2}(:,iCol), : );
    end
  end
  
  %%
  tic
  patchCorr             = cell(size(moviePatch));
  parfor iPatch = 1:numel(patchCorr)
    patchCorr{iPatch}   = cv.motionCorrect( moviePatch{iPatch}, maxShift, maxIter, false, stopBelowShift, nan, 10, [0 0], false );
  end
  toc
  patchCorr             = reshape([patchCorr{:}], size(patchCorr));

  %%
  patchXShifts          = single(reshape( accumfun(1, @(x) x.xShifts(:,end)', patchCorr), [numPatches,numFrames] ));
  patchYShifts          = single(reshape( accumfun(1, @(x) x.yShifts(:,end)', patchCorr), [numPatches,numFrames] ));
  pixelXShifts          = imresize(patchXShifts, inputSize(1:end-1), 'bicubic');
  pixelYShifts          = imresize(patchYShifts, inputSize(1:end-1), 'bicubic');
  [xGrid, yGrid]        = meshgrid(1:inputSize(2), 1:inputSize(1));
  
  %%
  clear moviePatch;
  frameDims             = 1:numel(inputSize)-1;
  movieFrame            = num2cell(movie, frameDims);
  pixelXShifts          = num2cell(bsxfun(@plus, pixelXShifts, xGrid), frameDims);
  pixelYShifts          = num2cell(bsxfun(@plus, pixelYShifts, yGrid), frameDims);
  
  %%
  tic
  corrected             = cell(size(movieFrame));
  parfor iFrame = 1:numFrames
    %%
    xLoc                = double( xGrid + pixelXShifts(:,:,iFrame) );
    yLoc                = double( yGrid + pixelYShifts(:,:,iFrame) );
    corrected{iFrame}   = griddata(xLoc, yLoc, double(movie(:,:,iFrame)), xGrid, yGrid);
  end
  corrected             = cat(3, corrected{:});
  toc
  
end
