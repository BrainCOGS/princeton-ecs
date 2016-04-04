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

function movie = imreadx(inputPath, xShift, yShift, xScale, yScale, maxNumFrames, varargin)
  
  if ~isempty(varargin)
    error('imreadx:arguments', 'Additional cv.imreadx() options not available in pure Matlab version.');
  end
  
  % Input check
  if ischar(inputPath)
    inputPath         = {inputPath};
  end
  
  % Data format
  info                = ecs.imfinfox(inputPath);
  switch Tiff.SampleFormat.(info.sampleFormat)
    case Tiff.SampleFormat.UInt
      dataType        = sprintf('uint%d', info.bitsPerSample);
    case Tiff.SampleFormat.Int
      dataType        = sprintf('int%d', info.bitsPerSample);
    otherwise
      if info.bitsPerSample <= 32
        dataType      = 'single';
      else
        dataType      = 'double';
      end
  end
  
  
  if numel(inputPath) > 1
    movie             = zeros([info.width, info.height, info.frames], dataType);
  end
  numFrames           = 0;
  for iFile = 1:numel(inputPath)
    source            = Tiff(chunk{iFile}, 'r');
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

    if iFrame ~= fileFrames(iFile)
      error('parimread:Tiff', 'Inconsistent number of frames %d read by Tiff vs. expected count %d.', iFrame, fileFrames(iFile));
    end

    img               = cv.imtranslatex(img, xShifts{iFile}, yShifts{iFile});
    if numel(inputPath) > 1
      movie(:,:,numFrames + (1:size(img,3)))  = img;
    else
      movie           = img;
    end
    numFrames         = numFrames + size(img,3);
  end
  
end
