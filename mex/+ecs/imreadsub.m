%% IMREADSUB      Loads multiple image stacks into memory, applying motion correction and downsampling
function [movie, binnedMovie, varargout] = imreadsub(imageFiles, motionCorr, frameGrouping, cropping, varargin)

  % Default arguments
  if nargin < 3
    frameGrouping = 1;
  end
  if nargin < 4
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
  
  % Option to crop border containing motion correction artifacts (some frames with no data)
  if isempty(cropping)
    frameSize     = size(motionCorr(1).reference);
  else
    frameSize     = cropping.selectSize;
  end

  % Option to store only a subset of pixels (creates a 2D pixel-by-time matrix)
  addGrouping     = [];
  if iscell(frameGrouping)
    pixelIndex    = frameGrouping{2};
    remainIndex   = frameGrouping{3};
    if numel(frameGrouping) > 3
      addGrouping = frameGrouping{4};
    end
    frameGrouping = frameGrouping{1};
  else
    pixelIndex    = [];
  end
  
  
  % Compute movie size
  info            = ecs.imfinfox(imageFiles);
  dataType        = class(motionCorr(1).reference);
  totalFrames     = sum(ceil(info.fileFrames / frameGrouping));
  inputSize       = [frameSize, totalFrames];
  if isempty(pixelIndex)
    movieSize     = inputSize;
  else
    movieSize     = [numel(pixelIndex) + 1, totalFrames];
  end
  if verbose
    dataFcn       = str2func(dataType);
    movieBytes    = prod(movieSize) * numel(typecast(dataFcn(0), 'int8'));
    sizeStr       = arrayfun(@(x) sprintf('%d',x), movieSize, 'UniformOutput', false);
    fprintf ( '----  Allocating memory for %s pixels of type %s:  %s\n'             ...
            , strjoin(sizeStr, ' x '), dataType, formatSIPrefix(movieBytes, 'B')    ...
            );
  end
  
  
  % Preallocate output
  if numel(imageFiles) > 1 || ~isempty(pixelIndex)
    movie         = zeros(movieSize, dataType);
    if ~isempty(addGrouping)
      binnedMovie = zeros([frameSize, ceil(totalFrames/addGrouping)], dataType);
    end
  end
  
  numFrames       = 0;
  numBinned       = 0;
  numLeftover     = 0;
  for iFile = 1:numel(imageFiles)
    % Read in the image and apply motion correction shifts
    img           = cv.imreadx(imageFiles{iFile}, motionCorr(iFile).xShifts(:,end), motionCorr(iFile).yShifts(:,end), varargin{:});
    
    % Rebin if so desired
    if ~isempty(frameGrouping) && frameGrouping > 1
      img         = rebin(img, frameGrouping, 3);
    end
  
    % Crop border if so requested
    if ~isempty(cropping)
      img         = rectangularSubset(img, cropping.selectMask, cropping.selectSize, 1);
    end
    
    % Additional rebinning if specified
    if ~isempty(addGrouping)
      binned      = zeros(size(img,1), size(img,2), 0);
      if numLeftover > 0
        index     = addGrouping - numLeftover + 1:size(img,3);
        if ~isempty(index)
          binned  = (sum(img(:,:,1:index(1)-1), 3) + leftover) / (index(1) - 1 + numLeftover);
        end
      else
        index     = 1:size(img,3);
      end
      
      if isempty(index)
        numLeftover = numLeftover + size(img,3);
        leftover  = leftover + sum(img,3);
      else
        numLeftover = rem(index(end) - index(1) + 1, addGrouping);
        index     = index(1:numel(index) - numLeftover);
        binned    = cat(3, binned, rebin(img(:,:,index), addGrouping, 3));
        leftover  = sum(img(:,:,index(end)+1:end), 3);
      end
    end
    
    
    imgFrames     = size(img,3);
    range         = numFrames + (1:imgFrames);
    if ~isempty(addGrouping)
      binFrames   = size(binned,3);
      bRange      = numBinned + (1:binFrames);
    end
    
    if ~isempty(pixelIndex)
      % Store only subset of pixels
      img         = reshape(img, [], imgFrames);
      movie(1:end-1,range)        = img(pixelIndex,:);
      movie(end    ,range)        = mean(img(remainIndex,:), 1);
      if ~isempty(addGrouping)
        binnedMovie(:,:,bRange)   = binned;
      end
      
    elseif numel(imageFiles) == 1
      % If there is only one input file, avoid allocation overhead
      movie                       = img;
      if ~isempty(addGrouping)
        binnedMovie               = binned;
      end
      
    else
      % Full frame is stored if pixel indices are not specified
      movie(:,:,range)            = img;
      if ~isempty(addGrouping)
        binnedMovie(:,:,bRange)   = binned;
      end
    end
    
    % Increment frame counts
    numFrames     = numFrames + imgFrames;
    if ~isempty(addGrouping)
      numBinned   = numBinned + binFrames;
    end
  end
  
  
  % Don't forget last frame in rebinned movie
  if numLeftover > 0
    binnedMovie(:,:,numBinned+1)  = leftover / numLeftover;
  end
  
  
  % Output original movie size
  if nargout == 3
    varargout     = {inputSize};
  else
    varargout     = num2cell(inputSize);
  end
  
end
