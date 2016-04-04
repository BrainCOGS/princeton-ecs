%% PARIMREAD    Loads multiple image stacks into memory and in parallel, applying motion correction and downsampling
function movie = parimread(imageFiles, motionCorr, frameGrouping, cropping, numParallel, varargin)
  
  % Default to no parallization if just one file
  if numel(imageFiles) < 2
    movie         = ecs.imreadsub(imageFiles, motionCorr, frameGrouping, cropping, varargin{:});
    return;
  end
  
  
  % Option to crop border containing motion correction artifacts (some frames with no data)
  if nargin < 3
    frameGrouping = 1;
  end
  if nargin < 4
    cropping      = [];
  end
  if nargin < 5
    numParallel   = [];
  end
  if ischar(imageFiles)
    imageFiles    = {imageFiles};
  end

  
  % Start parallel pool
  pool            = gcp('nocreate');
  if isempty(pool)
    pool          = parpool();
  end
  if isempty(numParallel)
    numParallel   = pool.NumWorkers;
  end
  
  
  if isempty(cropping)
    frameSize     = size(motionCorr(1).reference);
  else
    frameSize     = cropping.selectSize;
  end
  
  % Preallocate output
  info            = ecs.imfinfox(imageFiles);
  dataType        = class(motionCorr(1).reference);
  totalFrames     = sum(ceil(info.fileFrames / frameGrouping));
  if numel(imageFiles) > 1
    movie         = zeros([frameSize, totalFrames], dataType);
  end
  
  % Decide chunking
  fileChunk       = chunkIndices(numel(imageFiles), numParallel, @floor);
  numFrames       = 0;
  for iChunk = 1:numel(fileChunk) - 1
    indices       = fileChunk(iChunk):fileChunk(iChunk + 1) - 1;
    chunk         = imageFiles(indices);
    xShifts       = cell(size(indices));
    yShifts       = cell(size(indices));
    for iFile = 1:numel(indices)
      xShifts{iFile}  = motionCorr(indices(iFile)).xShifts(:,end);
      yShifts{iFile}  = motionCorr(indices(iFile)).yShifts(:,end);
    end
    subMovie      = cell(size(chunk));
    
    parfor iFile = 1:numel(chunk)
      % Read in the image and apply motion correction shifts
      img         = cv.imreadx(chunk{iFile}, xShifts{iFile}, yShifts{iFile}, varargin{:});

      % Rebin if so desired
      if ~isempty(frameGrouping) && frameGrouping > 1
        img       = rebin(img, frameGrouping, 3);
      end

      % Crop border if so requested
      if ~isempty(cropping)
        img       = rectangularSubset(img, cropping.selectMask, cropping.selectSize, 1);
      end
      
      subMovie{iFile} = img;
    end

    % Store in output location
    for iFile = 1:numel(chunk)
      movie(:,:,numFrames + (1:size(subMovie{iFile},3)))  = subMovie{iFile};
      numFrames   = numFrames + size(subMovie{iFile},3);
    end
  end
  
end
