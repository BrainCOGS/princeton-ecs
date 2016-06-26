%%
function [frameCorr, fileCorr] = getMotionCorrection(inputFiles, recompute, globalRegistration, varargin)
  
  % Default arguments
  if nargin < 2
    recompute   = false;
  end
  if nargin < 3
    globalRegistration  = true;
  end
  if nargin < 4
    varargin    = {30, 5, false, 0.3};
  end
  forceLoad     = false;
  if ischar(recompute)
    if strcmpi(recompute, 'never')
      forceLoad = true;
      recompute = false;
    elseif strcmpi(recompute, 'none')
      varargin{1} = 0;
      varargin{2} = 1;
      recompute = false;
    else
      error('getMotionCorrection:arguments', 'recompute must be either true, false, or the string ''none'', or ''never'' to require that motion correction info already exist.');
    end
  end

  % Parallel pool preferences
  parSettings                   = parallel.Settings;
  origAutoCreate                = parSettings.Pool.AutoCreate;
  parSettings.Pool.AutoCreate   = false;

  

  % Check for existing correction files
  frameCorr     = cell(size(inputFiles));
  corrPath      = cell(size(inputFiles));
  for iFile = 1:numel(inputFiles)
    [dir,name]  = parsePath(inputFiles{iFile});
    corrPath{iFile}     = fullfile(dir, [name '.mcorr.mat']);

    if ~recompute && exist(corrPath{iFile}, 'file')
      mcorr     = load(corrPath{iFile});
      frameCorr{iFile}  = mcorr.mcorr;
    elseif forceLoad
      error('getMotionCorrection:nodata', 'Motion correction information file "%s" not found.', corrPath{iFile});
    end
  end

  % Compute remaining correction factors
  iCompute      = find(cellfun(@isempty, frameCorr));
  if ~isempty(iCompute)
    fprintf(' ... motion correcting');
    
    newCorr     = cell(size(iCompute));
    corrInput   = inputFiles(iCompute);
    corrPath    = corrPath(iCompute);
    parfor iFile = 1:numel(iCompute)
      startTime = tic;
      mcorr     = cv.motionCorrect(corrInput{iFile}, varargin{:});
      fprintf(' (%.3g min)', toc(startTime)/60);
      
      output    = corrPath{iFile};
      parsave(output, mcorr);
      newCorr{iFile}    = mcorr;
    end
    frameCorr(iCompute) = newCorr;
  end  
  
  
  % Stitch together separately corrected stacks, but only if nontrivial corrections were requested
  % for any one constituent file (handle special case where motion correction should be turned off)
  frameCorr     = [frameCorr{:}];
  params        = [frameCorr.params];
  if numel(inputFiles) > 1 && any([params.maxShift] ~= 0)
    refImage    = cat(3, frameCorr.reference);
    fileCorr    = cv.motionCorrect(refImage, varargin{1:min(4,end)});
    if globalRegistration
      for iFile = 1:numel(inputFiles)
        frameCorr(iFile).xShifts(:,end+1) = frameCorr(iFile).xShifts(:,end) + fileCorr.xShifts(iFile, end);
        frameCorr(iFile).yShifts(:,end+1) = frameCorr(iFile).yShifts(:,end) + fileCorr.yShifts(iFile, end);
      end
    end
  else
    fileCorr    = frameCorr(1);
    fileCorr.xShifts    = zeros(numel(inputFiles), 1);
    fileCorr.yShifts    = zeros(numel(inputFiles), 1);
    fileCorr.reference  = mean(cat(3, frameCorr.reference), 3);
  end

  % Restore parallel pool settings
  parSettings.Pool.AutoCreate   = origAutoCreate;
  
end
