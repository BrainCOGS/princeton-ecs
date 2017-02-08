function mc = nonlinearMotionCorrect ( inputPath, maxShift, maxIter, displayProgress  ... maxShift = [rigid,patch]
                                     , varargin                                       ... unused but retained for compatibility with cv.motionCorrect()
                                     )
     
  %% Default arguments
  if nargin < 4
    displayProgress     = false;
  end

  %% Load data
  if ischar(inputPath)
    movie               = cv.imreadx(inputPath);
  else
    movie               = inputPath;
  end

  %% Convert to options for online_motion_correction_patches()
  [nRows,nCols,nFrames] = size(movie);
  options               = NoRMCorreSetParms ( 'd1'          , nRows               ... 
                                            , 'd2'          , nCols               ... 
                                            , 'grid_size'   , [64,64]             ... size of non-overlapping portion of the grid in each direction (x-y-z)
                                            , 'overlap_pre' , 32                  ... size of overlapping region in each direction before upsampling
                                            , 'mot_uf'      , 4                   ... upsampling factor for smoothing and refinement of motion field
                                            , 'overlap_post', 32                  ... size of overlapping region in each direction after upsampling
                                            , 'bin_width'   , 100                 ... length of bin over which the registered frames are averaged to update the template
                                            , 'max_shift'   , maxShift(1)         ... maximum allowed shift for rigid translation
                                            , 'max_dev'     , maxShift(2)         ... maximum deviation of each patch from estimated rigid translation
                                            , 'us_fac'      , 50                  ... upsampling factor for subpixel registration
                                            , 'iter'        , maxIter             ... number of times to go over the dataset
                                            , 'use_parallel', true                ... use parfor when breaking each frame into patches
                                            , 'plot_flag'   , displayProgress     ...
                                            );
  

  %% Run nonlinear registration
  mc                    = struct();
  [corrected, mc.shifts, mc.reference, mc.params]      ...
                        = normcorre_batch(movie, options);
  
                      
  %% Create output structure in the same format as cv.motionCorrect()
  mc.inputSize          = size(movie);
  mc.method             = 'ecs.nonlinearMotionCorrect';
  mc.params.maxIter     = maxIter;

end
