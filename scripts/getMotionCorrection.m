%% Run motion correction over a given set of input files, including global registration across files
function [frameCorr, fileCorr] = getMotionCorrection(inputFiles, recompute, globalRegistration, varargin)
  
  %% Default arguments
  if nargin < 2
    recompute                   = false;
  end
  if nargin < 3
    globalRegistration          = true;
  end
  if nargin < 4
    varargin                    = {30, 5, false, 0.3};
  end
  
  forceLoad                     = false;
  if ischar(recompute)
    if strcmpi(recompute, 'never')
      forceLoad                 = true;
      recompute                 = false;
    elseif strcmpi(recompute, 'none')
      varargin{1}               = 0;
      varargin{2}               = 1;
      recompute                 = false;
    else
      error('getMotionCorrection:arguments', 'recompute must be either true, false, or the string ''none'', or ''never'' to require that motion correction info already exist.');
    end
  end
  
  globalComputation             = nargout > 1 || isequal(globalRegistration,true);
  if ischar(globalRegistration)
    if strcmpi(globalRegistration, 'off')
      globalRegistration        = false;
      globalComputation         = false;
    else
      error('getMotionCorrection:arguments', 'globalRegistration must be either true, false, or the string ''off'' to prohibit computation of global shifts.');
    end
  end
  
  % If more than one maxShift is specified, then this must be nonlinear motion correction
  doNonlinear                   = numel(varargin{1}) > 1;
  

  %% Parallel pool preferences
  parSettings                   = parallel.Settings;
  origAutoCreate                = parSettings.Pool.AutoCreate;
  parSettings.Pool.AutoCreate   = false;

  
  %% Check for existing correction files
  frameCorr                     = cell(size(inputFiles));
  corrPath                      = cell(size(inputFiles));
  for iFile = 1:numel(inputFiles)
    [dir,name]                  = fileparts(inputFiles{iFile});
    corrPath{iFile}             = fullfile(dir, [name '.mcorr.mat']);

    if ~recompute && exist(corrPath{iFile}, 'file')
      mcorr                     = load(corrPath{iFile});
      frameCorr{iFile}          = mcorr.mcorr;
      doNonlinear               = isfield(frameCorr{iFile}, 'rigid');
    elseif forceLoad
      error('getMotionCorrection:nodata', 'Motion correction information file "%s" not found.', corrPath{iFile});
    end
  end

  %% Compute remaining correction factors
  iCompute                      = find(cellfun(@isempty, frameCorr));
  if ~isempty(iCompute)
    newCorr                     = cell(size(iCompute));
    corrInput                   = inputFiles(iCompute);
    corrPath                    = corrPath(iCompute);
    
    if doNonlinear
      fprintf(' ... correcting nonlinear motion ');
      
      %% Parallel processing for nonlinear motion correction is within the correction code
      for iFile = 1:numel(iCompute)
        startTime               = tic;
        mcorr                   = cv.nonlinearMotionCorrect(corrInput{iFile}, varargin{:});
        fprintf(' (%.3g min)', toc(startTime)/60);

        output                  = corrPath{iFile};
        parallelSave(output, mcorr);
        newCorr{iFile}          = mcorr;
      end
    
    else
      fprintf(' ... correcting rigid motion ');
      
      %% Can use parallel processing for rigid motion correction
      parfor iFile = 1:numel(iCompute)
        startTime               = tic;
        mcorr                   = cv.motionCorrect(corrInput{iFile}, varargin{:});
        fprintf(' (%.3g min)', toc(startTime)/60);

        output                  = corrPath{iFile};
        parallelSave(output, mcorr);
        newCorr{iFile}          = mcorr;
      end
    end
    frameCorr(iCompute)         = newCorr;
  end  
  
  
  %% Global registration
  % Stitch together separately corrected stacks, but only if nontrivial corrections were requested
  % for any one constituent file (handle special case where motion correction should be turned off)
  frameCorr                     = [frameCorr{:}];
  params                        = [frameCorr.params];
  if globalComputation && numel(inputFiles) > 1 && any([params.maxShift] ~= 0)
    refImage                    = cat(3, frameCorr.reference);
    
    if doNonlinear              % HACK these parameters are hard-coded 
      %% If constituent files are nonlinearly corrected, the global registration should also be and inherit their parameters
      params                    = [frameCorr.params];
      if ~isequaln(params.patchSize) || ~isequaln(params.numPatches) || ~isequaln(params.maxShiftDifference) || ~isequaln(params.smoothness)
        error('getMotionCorrection:nonlinear', 'Inconsistent parameters used for nonlinear motion correction per file, cannot compute global shifts.');
      end
      params                    = params(1);
      fileCorr                  = cv.nonlinearMotionCorrect ( refImage, [30 10], [5 1], 0.3, 1, [0 0]         ...
                                                            , params.patchSize, params.numPatches             ...
                                                            , params.maxShiftDifference, params.smoothness    ...
                                                            );

      %% Separately add rigid and per-patch shifts; note that changes to rigid shifts also affect the patches displacements
      for iFile = 1:numel(inputFiles)
        rigidDx                                   = fileCorr.rigid.xShifts(iFile,end);
        rigidDy                                   = fileCorr.rigid.yShifts(iFile,end);
        frameCorr(iFile).rigid.xShifts(:,end+1)   = frameCorr(iFile).rigid.xShifts(:,end) + rigidDx;
        frameCorr(iFile).rigid.yShifts(:,end+1)   = frameCorr(iFile).rigid.yShifts(:,end) + rigidDy;
        
        %% Centers for the original shifts are relocated, we interpolate back in order to shift them by the global amount
        patchScale                                = [ numel(frameCorr(iFile).yCenter) / ( frameCorr(iFile).yCenter(end) - frameCorr(iFile).yCenter(1) + 1 )   ...
                                                    , numel(frameCorr(iFile).xCenter) / ( frameCorr(iFile).xCenter(end) - frameCorr(iFile).xCenter(1) + 1 )   ...
                                                    ];
        rigidShift                                = [rigidDy, rigidDx] .* patchScale;
        frameCorr(iFile).xShifts                  = relocatePatchShifts(frameCorr(iFile).xShifts, rigidShift);
        frameCorr(iFile).yShifts                  = relocatePatchShifts(frameCorr(iFile).yShifts, rigidShift);
        frameCorr(iFile).xShifts                  = bsxfun(@plus, frameCorr(iFile).xShifts, fileCorr.xShifts(:,:,iFile));
        frameCorr(iFile).yShifts                  = bsxfun(@plus, frameCorr(iFile).yShifts, fileCorr.yShifts(:,:,iFile));
      end
        
    else
      %% For rigid motion correction the global shifts are added to the per-file shifts
      fileCorr                  = cv.motionCorrect(refImage, varargin{1:min(4,end)});
      if globalRegistration
        for iFile = 1:numel(inputFiles)
          frameCorr(iFile).xShifts(:,end+1)       = frameCorr(iFile).xShifts(:,end) + fileCorr.xShifts(iFile, end);
          frameCorr(iFile).yShifts(:,end+1)       = frameCorr(iFile).yShifts(:,end) + fileCorr.yShifts(iFile, end);
        end
      end
    end
    
  elseif nargout > 1
    %% No global registration
    fileCorr                    = frameCorr(1);
    fileCorr.xShifts            = zeros(numel(inputFiles), 1);
    fileCorr.yShifts            = zeros(numel(inputFiles), 1);
    fileCorr.reference          = mean(cat(3, frameCorr.reference), 3);
  end

  %% Restore parallel pool settings
  parSettings.Pool.AutoCreate   = origAutoCreate;
  
end

%---------------------------------------------------------------------------------------------------
function shifts = relocatePatchShifts(origShifts, relocation)
  
  %% Sanity check to prevent too large shifts
  if any(abs(relocation) > 1)
    error('getMotionCorrection:relocatePatchShifts', 'Patch-level shifts for rigid global registration exceeds 1, there is probably something wrong.');
  end
  
  %% Pad data to account for missing borders
  shifts                    = origShifts;
  if relocation(1) ~= 0
    shifts                  = padarray(shifts, [1 0 0], 'replicate', 'both');
  end
  if relocation(2) ~= 0
    shifts                  = padarray(shifts, [0 1 0], 'replicate', 'both');
  end
  
  %% Translate the original shifts by the desired amount
  shifts                    = cv.imtranslatex(shifts, relocation(2), relocation(1));
  if relocation(1) ~= 0
    shifts                  = shifts(2:end-1,:,:);
  end
  if relocation(2) ~= 0
    shifts                  = shifts(:,2:end-1,:);
  end
  
  %% Sanity check
  if any(~isfinite(shifts(:)))
    error('getMotionCorrection:relocatePatchShifts', 'Non-finite values encountered in patch-level shifts.');
  end
  
end
