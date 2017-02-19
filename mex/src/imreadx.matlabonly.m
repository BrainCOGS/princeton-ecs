% IMREADX    Loads the given image stack into memory, applying row/column shifts (rigid translation) to each frame.
%
%  Usage syntax:
%    [image, stats, median] = imreadx( inputPath, [xShift = []], [yShift = []]               ...
%                                    , [xScale = 1], [yScale = 1], [maxNumFrames = inf]      ...
%                                    );
%
%  This is the pure Matlab version which is slower and has reduced functionality, but at least it
%  works without a customized OpenCV.
%
%  Author:   Sue Ann Koay (koay@princeton.edu)
%

function movie = imreadx(inputPath, xShifts, yShifts, xScale, yScale, maxNumFrames, varargin)
  
  if ~isempty(varargin)
    error('imreadx:arguments', 'Additional cv.imreadx() options not available in pure Matlab version.');
  end
  
  %% Input check
  if ischar(inputPath)
    inputPath         = {inputPath};
  end
  if nargin < 3
    xShifts           = [];
    yShifts           = [];
  end
  if nargin < 5
    xScale            = 1;
  end
  if nargin < 6
    yScale            = 1;
  end
  doTranslate         = ~isempty(xShifts) || ~isempty(yShifts);
  if doTranslate && (isempty(xShifts) ~= isempty(yShifts))
    error('imreadx:arguments', 'If xShifts is provided yShifts must be as well, and vice versa.');
  end
  if doTranslate && ~any(xShifts(:,end) ~= 0) && ~any(yShifts(:,end) ~= 0)
    doTranslate       = false;
  end
  if xScale ~= 1 || yScale ~= 1
    error('imreadx:arguments', 'xScale/yScale not supported yet.');
  end
  
  %% Read out subsets of frames
  frameOffset         = 0;
  frameSkip           = 0;
  if isempty(maxNumFrames)
    maxNumFrames      = inf;
  elseif numel(maxNumFrames) > 1
    frameOffset       = maxNumFrames(1);
    frameSkip         = maxNumFrames(2);
    if numel(maxNumFrames) > 2
      maxNumFrames    = maxNumFrames(3);
    else
      maxNumFrames    = inf;
    end
  end
  
  
  %% Data format
  info                = ecs.imfinfox(inputPath);
  if info.bitsPerSample <= 32
    dataType          = 'single';
  else
    dataType          = 'double';
  end
  info.fileFrames     = arrayfun(@(x) ceil((x - frameOffset) / (1 + frameSkip)), info.fileFrames);
  info.frames         = min(sum(info.fileFrames), maxNumFrames);
  
  %% Preallocate output
  if numel(inputPath) > 1
    movie             = zeros([info.width, info.height, info.frames], dataType);
  end
  
  %% Loop through input files
  numFrames           = 0;
  for iFile = 1:numel(inputPath)
    %% TIFF reader
    fileFrames        = min( info.fileFrames(iFile), info.frames - numFrames );
    img               = zeros(info.height, info.width, fileFrames, dataType);
    source            = Tiff(inputPath{iFile}, 'r');
    for iSkip = 1:frameOffset
      source.nextDirectory();
    end
      
    %% Get requested frames
    img(:,:,1)        = source.read();
    for iFrame = 2:fileFrames
      for iSkip = 0:frameSkip
        source.nextDirectory();
      end
      img(:,:,iFrame) = source.read();
    end
    source.close();

    
    %% Apply motion correction if so desired
    iFrame            = numFrames + (1:fileFrames);
    if doTranslate
      img             = cv.imtranslatex(img, xShifts(iFrame,end), yShifts(iFrame,end));
    end
    
    %% Store in output tensor
    if numel(inputPath) > 1
      movie(:,:,iFrame) = img;
    else
      movie           = img;
    end
    numFrames         = numFrames + fileFrames;
    if numFrames >= info.frames
      break;
    end
  end
  
end
