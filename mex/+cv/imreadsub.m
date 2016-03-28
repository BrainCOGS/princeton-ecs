%% IMREADS    Loads multiple image stacks into memory, applying motion correction and downsampling
function movie = imreadsub(imageFiles, motionCorr, frameGrouping, cropping, varargin)
  
  % Option to crop border containing motion correction artifacts (some frames with no data)
  if nargin < 4
    cropping      = [];
  end
  
  
  if isempty(cropping)
    frameSize     = size(motionCorr(1).reference);
  else
    frameSize     = cropping.selectSize;
  end
  
  % Preallocate output
  info            = cv.imfinfox(imageFiles);
  totalFrames     = sum(ceil(info.fileFrames / frameGrouping));
  movie           = zeros([frameSize, totalFrames], 'like', motionCorr(1).reference);
  numFrames       = 0;
  
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
    
    movie(:,:,numFrames + (1:size(img,3)))  = img;
    numFrames     = numFrames + size(img,3);
  end
  
end
