% IMREADSUB      Loads multiple image stacks into memory, applying motion correction and downsampling
%
%  Inputs
% ========
%   imageFiles       : Cell array of .tif files to read.
%   motionCorr       : Struct array that is 1-1 with the list of imageFiles, as produced by
%                      cv.motionCorrect or cv.nonlinearMotionCorrect. The format of this will
%                      determine whether linear or nonlinear motion correction is applied to the
%                      loaded data.
%   frameGrouping    : Either an integer specifying the number of frames to group together via
%                      averaging to produce the output movie, or a cell array 
%                         {frameGrouping, pixelIndex, avgIndex, [addGrouping]}
%                      with the interpretation that frameGrouping is used to group frames together
%                      for the output, pixelIndex restricts the output movie to the given pixel
%                      indices, avgIndex adds a pixel to the end of the output movie that is the
%                      average over the listed pixels, and (optional) addGrouping generates a second
%                      output binnedMovie with a total temporal grouping factor of 
%                      frameGrouping * addGrouping. N.B. Pixel indices are relative to the cropped
%                      movie (see cropping input argument), and a single non-positive number (as
%                      opposed to a list of pixels) for avgIndex will average across all pixels in
%                      the cropped input movie. 
%   cropping         : Struct for info on how to crop the borders of the movie e.g. as produced by
%                      getMovieCropping() in order to remove areas where one or more frames have no
%                      info due to applied translations for motion correction. If empty, no cropping
%                      is applied.
%   varargin         : Additional arguments for cv.imreadx(), after the motion correction yShifts.
%
%  Outputs
% =========
%   movie            : A temporally downsampled and optionally spatially cropped movie from the
%                      concatenation of imageFiles. To produce this, frames are grouped
%                      (frameGrouping input number into a single output frame) and averaged within
%                      groups, i.e. downsampled by a factor of frameGrouping.
%   binnedMovie      : Optional and produced only in case of cell array frameGrouping input with
%                      addGrouping specified, in which case this is like movie but further
%                      downsampled by the addGrouping factor.
%   inputSize        : Original size of all input files.
%   info             : Struct with parameters used in computing the movie output.
%
% Author  :  Sue Ann Koay (koay@princeton.edu)
%
function [movie, binnedMovie, inputSize, info] = imreadsub(imageFiles, motionCorr, frameGrouping, cropping, varargin)

  %% Default arguments
  if nargin < 2
    motionCorr    = [];
  elseif iscell(motionCorr)
    doParallel    = motionCorr{2};
    motionCorr    = motionCorr{1};
  else
    doParallel    = ~isempty(gcp('nocreate'));
  end
  if nargin < 3 || isempty(frameGrouping)
    frameGrouping = 1;
  end
  if nargin < 4 || isempty(cropping)
    cropping      = [];
  end
  if ischar(imageFiles)
    imageFiles    = {imageFiles};
  end
  if ~isempty(varargin) && isequal(lower(varargin{1}), 'verbose')
    verbose       = true;
    varargin(1)   = [];
  else
    verbose       = false;
  end
  
  if isempty(varargin)
    varargin      = {1, 1, []};
  elseif numel(varargin) == 1
    varargin      = [varargin, {1, []}];
  elseif numel(varargin) == 2
    varargin      = [varargin, {[]}];
  end
  
  
  %% Option to crop border containing motion correction artifacts (some frames with no data)
  info            = cv.imfinfox(imageFiles);
  if ~isempty(cropping)
    frameSize     = cropping.selectSize;
  elseif isempty(motionCorr)
    frameSize     = [info.height, info.width];
  else
    frameSize     = size(motionCorr(1).reference);
  end

  %% Option to store only a subset of pixels (creates a 2D pixel-by-time matrix)
  addGrouping     = [];
  if iscell(frameGrouping)
    pixelIndex    = frameGrouping{2};
    avgIndex      = frameGrouping{3};
    averageAll    = numel(avgIndex) == 1 && avgIndex <= 0;
    if numel(frameGrouping) > 3
      addGrouping = frameGrouping{4};
    end
    frameGrouping = frameGrouping{1};
  else
    pixelIndex    = [];
    avgIndex      = [];
  end
  storePixels     = ~isempty(pixelIndex) || ~isempty(avgIndex);
  
  
  %% Compute movie size
  if isempty(motionCorr)
    dataType      = sprintf('%s%d', lower(info.sampleFormat), info.bitsPerSample);
    frameSkip     = [0 0];
  else
    frameSkip     = motionCorr(1).params.frameSkip;
    dataType      = class(motionCorr(1).reference);
  end
  if isempty(frameSkip)
    frameSkip     = [0 0];
  end
  varargin{3}     = frameSkip;
  
  %% Adjust number of frames for skips and binning
  info.fileFrames = arrayfun(@(x) ceil((x - frameSkip(1)) / (1 + frameSkip(2))), info.fileFrames);
  totalFrames     = ceil(sum(info.fileFrames) / frameGrouping);
  inputSize       = [frameSize, totalFrames];
  if storePixels
    movieSize     = [numel(pixelIndex) + 1, totalFrames];
  else
    movieSize     = inputSize;
  end
  
  
  %% Report number of required bytes
  if verbose
    dataFcn       = str2func(dataType);
    movieBytes    = prod(movieSize) * numel(typecast(dataFcn(0), 'int8'));
    sizeStr       = arrayfun(@(x) sprintf('%d',x), movieSize, 'UniformOutput', false);
    fprintf ( '----  Allocating memory for %s pixels of type %s:  %s\n'             ...
            , strjoin(sizeStr, ' x '), dataType, formatSIPrefix(movieBytes, 'B')    ...
            );
  end
  
  
  %% Preallocate output
  if numel(imageFiles) > 1 || storePixels
    movie         = zeros(movieSize, dataType);
  end
  if isempty(addGrouping)
    binnedMovie   = [];
  else
    binnedMovie   = zeros([frameSize, ceil(sum(info.fileFrames) / frameGrouping / addGrouping)], dataType);
  end

  info.nHasData   = zeros(frameSize);
  info.nonlinearMotionCorr      = false;

  
  %% Loop sequentially over input files and collect chunks of frames
  out             = struct('nFrames', {0}, 'nLeft', {0}, 'leftover', {[]});
  sub             = struct('nFrames', {0}, 'nLeft', {0}, 'leftover', {[]});
  if verbose
    fprintf('      Processing file        ');
  end
  for iFile = 1:numel(imageFiles)
    if verbose
      fprintf('\b\b\b\b\b\b\b%3d/%-3d', iFile, numel(imageFiles));
    end
  
    %% Read in the image and apply motion correction shifts
    if isempty(motionCorr)
      img         = cv.imreadx(imageFiles{iFile}, [], [], varargin{:});
    elseif isfield(motionCorr(iFile), 'rigid')
      img         = cv.imreadnonlin(imageFiles{iFile}, motionCorr(iFile), frameSkip, doParallel);
      info.nonlinearMotionCorr  = true;
    else
      img         = cv.imreadx(imageFiles{iFile}, motionCorr(iFile).xShifts(:,end), motionCorr(iFile).yShifts(:,end), varargin{:});
    end
    
    
    %% Crop border if so requested
    if ~isempty(cropping)
      img         = rectangularSubset(img, cropping.selectMask, cropping.selectSize, 1);
    end
    
    % Aggregate the number of frames with valid data per pixel
    info.nHasData = info.nHasData + sum(isfinite(img), 3);
    
    
    %% Rebin if so desired
    if ~isempty(frameGrouping) && frameGrouping > 1
      [img, range, out]       ...
                  = onTheFlyRebin(img, frameGrouping, out);
    else
      range       = out.nFrames + (1:size(img,3));
      out.nFrames = out.nFrames + size(img,3);
    end
  
    % Additional rebinning if specified
    if ~isempty(addGrouping)
      [binned, bRange, sub]   ...
                  = onTheFlyRebin(img, addGrouping, sub);
      binnedMovie(:,:,bRange)     = binned;
    end

    %% Store in the target chunk, optionally as a list of pixels
    if storePixels
      % Store only subset of pixels
      img         = reshape(img, [], size(img,3));
      movie(1:end-1,range)        = img(pixelIndex,:);
      if averageAll
        movie(end  ,range)        = mean(img(:,:), 1);
      else
        movie(end  ,range)        = mean(img(avgIndex,:), 1);
      end
      
    elseif numel(imageFiles) == 1
      % If there is only one input file, avoid allocation overhead
      movie                       = img;
      
    else
      % Full frame is stored if pixel indices are not specified
      movie(:,:,range)            = img;
    end
  end
  
  %% Don't forget last frame in rebinned movies
  if verbose
    fprintf(' ... finalizing.');
  end
  if out.nLeft > 0
    if storePixels
      img         = reshape(out.leftover, [], 1);
      movie(1:end-1,out.nFrames+1)  = img(pixelIndex,:);
      if averageAll
        movie(end  ,out.nFrames+1)  = mean(img(:,:), 1);
      else
        movie(end  ,out.nFrames+1)  = mean(img(avgIndex,:), 1);
      end
    else
      movie(:,:,out.nFrames+1)      = out.leftover / out.nLeft;
    end
  end
  if sub.nLeft > 0
    binnedMovie(:,:,sub.nFrames+1)  = sub.leftover / sub.nLeft;
  end
  
end


%---------------------------------------------------------------------------------------------------
function [binned, binRange, info] = onTheFlyRebin(img, binsPerGroup, info)
  
  % Preallocate output
  binned          = zeros(size(img,1), size(img,2), 0);
  
  % Get range of indices excluding those owed to a leftover bin
  if info.nLeft > 0
    index         = binsPerGroup - info.nLeft + 1:size(img,3);
    if ~isempty(index)
      binned      = (sum(img(:,:,1:index(1)-1), 3) + info.leftover) / (index(1) - 1 + info.nLeft);
    end
  else
    index         = 1:size(img,3);
  end

  % Collect rebinned movie and leftover frames
  if isempty(index)
    info.nLeft    = info.nLeft + size(img,3);
    info.leftover = info.leftover + sum(img,3);
  elseif index(end) - index(1) + 1 < binsPerGroup
    info.nLeft    = index(end) - index(1) + 1;
    info.leftover = sum(img, 3);
  else
    info.nLeft    = rem(index(end) - index(1) + 1, binsPerGroup);
    index         = index(1:numel(index) - info.nLeft);
    binned        = cat(3, binned, rebin(img(:,:,index), binsPerGroup, 3));
    info.leftover = sum(img(:,:,index(end)+1:end), 3);
  end
  
  % Output range in which to store binned movie
  binRange        = info.nFrames + (1:size(binned,3));
  info.nFrames    = info.nFrames + size(binned,3);
    
end
