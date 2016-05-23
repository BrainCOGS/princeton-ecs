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
  
  % Input check
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
  if xScale ~= 1 || yScale ~= 1
    error('imreadx:arguments', 'xScale/yScale not supported yet.');
  end
  
  
  % Data format
  info                = ecs.imfinfox(inputPath);
  if info.bitsPerSample <= 32
    dataType          = 'single';
  else
    dataType          = 'double';
  end
  
  
  if numel(inputPath) > 1
    movie             = zeros([info.width, info.height, info.frames], dataType);
  end
  numFrames           = 0;
  for iFile = 1:numel(inputPath)
    source            = Tiff(inputPath{iFile}, 'r');
    img               = zeros(info.height, info.width, info.fileFrames(iFile), dataType);
    
    iFrame            = 0;
    while true
      iFrame          = iFrame + 1;
      img(:,:,iFrame) = source.read();
      if source.lastDirectory()
        break;
      end
      source.nextDirectory();
    end
    source.close();

    if iFrame ~= info.fileFrames(iFile)
      error('parimread:Tiff', 'Inconsistent number of frames %d read by Tiff vs. expected count %d.', iFrame, info.fileFrames(iFile));
    end

    
    iFrame            = numFrames + (1:size(img,3));
    if doTranslate
      img             = cv.imtranslatex(img, xShifts(iFrame,end), yShifts(iFrame,end));
    end
    if numel(inputPath) > 1
      movie(:,:,iFrame) = img;
    else
      movie           = img;
    end
    numFrames         = numFrames + size(img,3);
  end
  
end
