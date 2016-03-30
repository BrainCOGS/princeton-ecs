%% IMREADSUB      Loads multiple image stacks into memory, applying motion correction and downsampling
function movie = imreadsub(imageFiles, motionCorr, frameGrouping, cropping, varargin)
  
  % Option to crop border containing motion correction artifacts (some frames with no data)
  if nargin < 3
    frameGrouping = 1;
  end
  if nargin < 4
    cropping      = [];
  end
  if ischar(imageFiles)
    imageFiles    = {imageFiles};
  end
  
  % Option to use Matlab-only code for reading in files
  matlabOnly      = isempty(which('cv.imreadx'));
  if matlabOnly && ~isempty(varargin)
    error('imreadsub:arguments', 'Additional cv.imreadx() options not available because cv.imreadx() was not found.');
  end
  
  
  if isempty(cropping)
    frameSize     = size(motionCorr(1).reference);
  else
    frameSize     = cropping.selectSize;
  end
  
  % Preallocate output
  info            = ecs.imfinfox(imageFiles);
  totalFrames     = sum(ceil(info.fileFrames / frameGrouping));
  movie           = zeros([frameSize, totalFrames], 'like', motionCorr(1).reference);
  numFrames       = 0;
  
  for iFile = 1:numel(imageFiles)
    % Read in the image and apply motion correction shifts
    if matlabOnly
      img         = zeros(info.height, info.width, info.fileFrames(iFile));
      iFrame      = 0;
      source      = Tiff(imageFiles{iFile}, 'r');
      while true
        iFrame    = iFrame + 1;
        img(:,:,iFrame) = source.read();
        if source.lastDirectory()
          break;
        end
        source.nextDirectory();
      end
      source.close();
      
      if iFrame ~= info.fileFrames(iFile)
        error('imreadsub:Tiff', 'Inconsistent number of frames %d read by Tiff vs. expected count %d.', iFrame, info.fileFrames(iFile));
      end
      
      img         = cv.imtranslatex(img, motionCorr(iFile).xShifts(:,end), motionCorr(iFile).yShifts(:,end));
    else
      img         = cv.imreadx(imageFiles{iFile}, motionCorr(iFile).xShifts(:,end), motionCorr(iFile).yShifts(:,end), varargin{:});
    end
    
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
