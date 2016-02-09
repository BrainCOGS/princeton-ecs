%% IMREADS    Loads the given image stack into memory, applying row/column shifts (rigid translation) to each frame.
function [img, selectMask, selectSize, xRange, yRange] = imreads(imageFiles, motionCorrection, doCrop, varargin)
  
  % Option to crop border containing motion correction artifacts (some frames with no data)
  if nargin < 3
    doCrop      = false;
  end
  
  
  if iscellstr(imageFiles)
    % Aggregate shifts for multiple inputs
    if numel(imageFiles) ~= numel(motionCorrection)
      error('imreads:arguments', 'The same number of image files and motion correction structures must be provided.');
    end
    
    xShifts     = zeros(0,1);
    yShifts     = zeros(0,1);
    for iCorr = 1:numel(motionCorrection)
      xShifts(end + (1:size(motionCorrection(iCorr).xShifts,1)), :)  ...
                = motionCorrection(iCorr).xShifts(:,end);
      yShifts(end + (1:size(motionCorrection(iCorr).yShifts,1)), :)  ...
                = motionCorrection(iCorr).yShifts(:,end);
    end
    
  elseif numel(motionCorrection) ~= 1
    error('imreads:arguments', 'The same number of image files and motion correction structures must be provided.');
    
  else
    % Take last iteration shifts for a single input
    xShifts     = motionCorrection.xShifts(:,end);
    yShifts     = motionCorrection.yShifts(:,end);
  end
  
  
  % Read in the image and apply motion correction shifts
  img           = cv.imreadx(imageFiles, xShifts, yShifts, varargin{:});
  
  % Crop border if so requested
  if doCrop
    hasData     = ~isnan(sum(img, 3));
    rowData     = any(hasData, 2);
    colData     = any(hasData, 1);
    xRange      = [find(colData, 1, 'first'), find(colData, 1, 'last')];
    yRange      = [find(rowData, 1, 'first'), find(rowData, 1, 'last')];
    [selectMask, selectSize]      ...
                = rectangularMask(size(img), xRange, yRange);
    img         = rectangularSubset(img, selectMask, selectSize, 1);
%     boundary  = ceil([max(abs(yShifts)), max(abs(xShifts))]);
%     img       = img(1+boundary(1):end-boundary(1), 1+boundary(2):end-boundary(2), :);
  end
  
end
