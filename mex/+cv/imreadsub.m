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
  movie           = zeros([frameSize, 0], 'like', motionCorr(1).reference);
  for iFile = 1:numel(imageFiles)
    % Read in the image and apply motion correction shifts
    img           = cv.imreadx(imageFiles{iFile}, motionCorr(iFile).xShifts(:,end), motionCorr(iFile).yShifts(:,end), varargin{:});
    img           = rebin(img, frameGrouping, 3);
  
    % Crop border if so requested
    if ~isempty(cropping)
      img         = rectangularSubset(img, cropping.selectMask, cropping.selectSize, 1);
    end
    
    movie(:,:,end + (1:size(img,3)))  = img;
  end
  
end
