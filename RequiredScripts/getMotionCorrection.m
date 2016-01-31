%%
function [frameCorr, fileCorr] = getMotionCorrection(inputFiles, recompute, varargin)
  
  if nargin < 2
    recompute   = false;
  end
  if nargin < 3
    varargin    = {10, 1, false, 0.1};
  end
  

  frameCorr      = struct([]);
  isFirst       = true;
  for iFile = 1:numel(inputFiles)
    [dir,name]  = parsePath(inputFiles{iFile});
    corrPath    = fullfile(dir, [name '.mcorr.mat']);

    if ~recompute && exist(corrPath, 'file')
      load(corrPath);
    else
      if isFirst
        isFirst = false;
        fprintf(' ... motion correcting');
      end
      startTime = tic;
      mcorr     = cv.motionCorrect(inputFiles{iFile}, varargin{:});
      fprintf(' (%.3g min)', toc(startTime)/60);
      save(corrPath, 'mcorr');
    end

    if isempty(frameCorr)
      frameCorr         = mcorr;
    else
      frameCorr(end+1)  = mcorr;
    end
  end

  % Stitch together separately corrected stacks 
  if numel(inputFiles) > 1
    refImage    = cat(3, frameCorr.reference);
    fileCorr    = cv.motionCorrect(refImage, varargin{1:min(4,end)});
    if nargout < 2
      for iFile = 1:numel(inputFiles)
        frameCorr(iFile).xShifts(:,end+1) = frameCorr(iFile).xShifts(:,end) + fileCorr.xShifts(iFile, end);
        frameCorr(iFile).yShifts(:,end+1) = frameCorr(iFile).yShifts(:,end) + fileCorr.yShifts(iFile, end);
      end
    end
  else
    fileCorr    = frameCorr;
    fileCorr.xShifts  = zeros(numel(inputFiles), 1);
    fileCorr.yShifts  = zeros(numel(inputFiles), 1);
  end
    
end